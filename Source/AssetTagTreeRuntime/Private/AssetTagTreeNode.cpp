// Copyright (c) 2024 tortle-sh All Rights Reserved


#include "AssetTagTreeNode.h"

#include "AssetTagTreeConstants.h"
#include "AssetTagTreePCH.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "UObject/SavePackage.h"

bool UAssetTagTreeNode::IsLeaf() const
{
	return Children.Num() == 0;
}

TArray<TWeakObjectPtr<UObject>> UAssetTagTreeNode::FindObjectsByTag(const FGameplayTag& Tag) const
{
	if (!Tag.MatchesTag(NodeTag))
	{
		return {};
	}

	if (Tag.MatchesTagExact(NodeTag))
	{
		return this->Objects;
	}

	for (auto Child : this->Children)
	{
		if (!Child.IsValid())
		{
			continue;
		}
		if (Tag.MatchesTag(Child->NodeTag))
		{
			if (Child.IsValid())
			{
				return Child->FindObjectsByTag(Tag);
			}
		}
	}

	return {};
}

TArray<TWeakObjectPtr<UObject>> UAssetTagTreeNode::FindObjectsByTags(const FGameplayTagContainer& Tags)
{
	if (!Tags.HasTag(NodeTag))
	{
		return {};
	}

	TArray<TWeakObjectPtr<UObject>> Results = {};
	Results.Append(Objects);

	for (auto Child : Children)
	{
		if (!Child.IsValid())
		{
			continue;
		}
		if (Tags.HasTag(Child->NodeTag))
		{
			Results.Append(Child->FindObjectsByTags(Tags));
		}
	}

	return Results;
}

TArray<UAssetTagTreeNode*> UAssetTagTreeNode::FindNodesByTags(const FGameplayTagContainer& Tags)
{
	if (!Tags.HasTag(NodeTag))
	{
		return {};
	}

	TArray<UAssetTagTreeNode*> Results;

	if (Tags.HasTagExact(NodeTag))
	{
		Results.Add(this);
	}


	for (auto Child : Children)
	{
		if (!Child.IsValid())
		{
			continue;
		}
		Results.Append(Child->FindNodesByTags(Tags));
	}

	return Results;
}

void UAssetTagTreeNode::SetRootPackageName(const FString& PackageName)
{
	this->RootPackageName = PackageName;
}

FString UAssetTagTreeNode::BuildPackageObjectName(const FGameplayTag& NextTag)
{
	FString AsssetName = NextTag.GetTagName().ToString();
	AsssetName = AsssetName.Replace(TEXT("."), TEXT("_"));
	return AsssetName;
}

void UAssetTagTreeNode::SetParentTag(FGameplayTag NewParentTag)
{
	this->ParentTag = NewParentTag;
}

void UAssetTagTreeNode::CreateMissingChildren(const FGameplayTagContainer& Tags)
{
	FGameplayTagContainer MissingChildren, ExistingTags;
	MissingChildren.AppendTags(Tags);
	ExistingTags.AddTag(NodeTag);

	for (auto Tag : MissingChildren)
	{
		if (!Tag.MatchesTag(NodeTag))
		{
			continue;
		}

		for (auto Child : Children)
		{
			if (!Child.IsValid())
			{
				continue;
			}
			if (Tag.MatchesTag(Child->NodeTag))
			{
				ExistingTags.AddTag(Tag);
				break;
			}
		}
	}

	MissingChildren.RemoveTags(ExistingTags);
	FGameplayTagContainer CreatedTags;

	for (auto Tag : MissingChildren)
	{
		FGameplayTag NextTag = GetNextTag(Tag);

		if (!NextTag.MatchesTag(NodeTag))
		{
			continue;
		}

		if (NextTag.MatchesTagExact(NodeTag))
		{
			continue;
		}

		if (CreatedTags.HasTagExact(NextTag))
		{
			continue;
		}

		FString ChildAssetName = BuildPackageObjectName(NextTag);

		UAssetTagTreeNode* NewChild = NewObject<UAssetTagTreeNode>(RootPackage, *ChildAssetName,
		                                                           RF_Public | RF_Standalone);
		NewChild->SetParentTag(this->GetTag());
		NewChild->SetRootPackage(this->RootPackage);
		NewChild->SetRootPackageName(this->RootPackageName);
		NewChild->SetTag(NextTag);
		FAssetRegistryModule::AssetCreated(NewChild);
		NewChild->Persist();

		this->Children.Add(NewChild);
		CreatedTags.AddTag(NextTag);
	}
}

bool UAssetTagTreeNode::InsertToTag(UObject* NewAssetTagObject, const FGameplayTag& Tag)
{
	if (!Tag.MatchesTag(this->NodeTag))
	{
		return false;
	}

	if (Tag.MatchesTagExact(this->NodeTag))
	{
		this->Objects.Add(NewAssetTagObject);
		return true;
	}

	FGameplayTagContainer SingleTagContainer;
	SingleTagContainer.AddTag(Tag);
	CreateMissingChildren(SingleTagContainer);

	bool bHasChildBeenUpdated = false;
	for (auto Child : this->Children)
	{
		if (Child.IsValid())
		{
			bHasChildBeenUpdated = Child->InsertToTag(NewAssetTagObject, Tag) ? true : bHasChildBeenUpdated;
		}
	}

	return bHasChildBeenUpdated;
}

bool UAssetTagTreeNode::InsertToTags(UObject* NewAssetTagObject, const FGameplayTagContainer& Tags)
{
	if (!Tags.HasTag(NodeTag))
	{
		return false;
	}

	bool bHasSubtreeUpdated = false;

	if (Tags.HasTagExact(NodeTag))
	{
		this->Objects.Add(NewAssetTagObject);
		bHasSubtreeUpdated = true;
	}

	CreateMissingChildren(Tags);

	for (auto Child : Children)
	{
		if (Child.IsValid())
		{
			bHasSubtreeUpdated = Child->InsertToTags(NewAssetTagObject, Tags) ? true : bHasSubtreeUpdated;
		}
	}

	return bHasSubtreeUpdated;
}

bool UAssetTagTreeNode::RemoveObjectFromTag(UObject* OldAssetTagObject, const FGameplayTag& Tag)
{
	if (!Tag.MatchesTag(NodeTag))
	{
		return false;
	}

	if (Tag.MatchesTagExact(NodeTag))
	{
		Objects.Remove(OldAssetTagObject);
		return true;
	}

	for (auto Child : Children)
	{
		if (!Child.IsValid())
		{
			continue;
		}
		if (Tag.MatchesTag(Child->NodeTag))
		{
			return Child->RemoveObjectFromTag(OldAssetTagObject, Tag);
		}
	}

	return false;
}

bool UAssetTagTreeNode::RemoveObjectFromTags(UObject* OldAssetTagObject, const FGameplayTagContainer& Tags)
{
	if (!Tags.HasTag(NodeTag))
	{
		return false;
	}

	bool bHasSubtreeUpdated = false;

	if (Tags.HasTagExact(NodeTag))
	{
		Objects.Remove(OldAssetTagObject);
		bHasSubtreeUpdated = true;
	}

	for (auto Child : Children)
	{
		if (Child.IsValid())
		{
			bHasSubtreeUpdated = Child->RemoveObjectFromTags(OldAssetTagObject, Tags) ? true : bHasSubtreeUpdated;
		}
	}

	return bHasSubtreeUpdated;
}

bool UAssetTagTreeNode::RemoveNode(const FGameplayTag& Tag)
{
	if (!Tag.MatchesTag(NodeTag))
	{
		return false;
	}

	for (auto Child : Children)
	{
		if (Child.IsValid())
		{
			Child->RemoveNode(Tag);
		}
	}

	Objects.Empty();
	Children.Empty();
	BroadcastUpdate();
	OnSubTreeUpdated.Clear();

	return true;
}

void UAssetTagTreeNode::CollectChildTagsOfTargetTags(const FGameplayTagContainer& TargetTags,
                                                     FGameplayTagContainer& ChildTags)
{
	if (!TargetTags.HasTag(NodeTag))
	{
		return;
	}

	if (!TargetTags.HasTagExact(NodeTag))
	{
		for (auto Child : Children)
		{
			if (Child.IsValid())
			{
				Child->CollectChildTagsOfTargetTags(TargetTags, ChildTags);
			}
		}
		return;
	}

	ChildTags.AddTag(NodeTag);
	CollectChildTags(ChildTags);
}

void UAssetTagTreeNode::CollectChildTags(FGameplayTagContainer& ChildTags)
{
	for (auto Child : Children)
	{
		if (Child.IsValid())
		{
			ChildTags.AddTag(Child->NodeTag);
			Child->CollectChildTags(ChildTags);
		}
	}
}

void UAssetTagTreeNode::BroadcastUpdate()
{
	LOG_INFO("Broadcast from: %s", *NodeTag.GetTagName().ToString());
	OnSubTreeUpdated.Broadcast();
}

void UAssetTagTreeNode::BroadcastUpdates(const FGameplayTagContainer& Tags)
{
	if (!Tags.HasTag(NodeTag))
	{
		return;
	}

	if (Tags.HasTagExact(NodeTag))
	{
		BroadcastUpdate();
	}

	for (auto Child : Children)
	{
		if (Child.IsValid())
		{
			Child->BroadcastUpdates(Tags);
		}
	}
}

void UAssetTagTreeNode::Persist()
{
	// TODO Fix Objects not being persisted
	if (this->IsRootNode())
	{
		return;
	}

	UPackage* Package = this->RootPackage;

	if (!Package)
	{
		return;
	}

	FSavePackageArgs SaveArgs;
	{
		SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
		SaveArgs.SaveFlags = SAVE_NoError;
	}

	// const FString PackageFileName = BuildFileName(this->NodeTag);
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(
		RootPackageName, FPackageName::GetAssetPackageExtension());
	const bool bSaveSuccessful = UPackage::SavePackage(Package, this, *PackageFileName, SaveArgs);
	Package->MarkPackageDirty();
	if (bSaveSuccessful)
	{
		LOG_WARNING("%s got saved into %s", *GetTag().GetTagName().ToString(), *PackageFileName);
	}
	else
	{
		LOG_ERROR("saving %s into %s failed", *GetTag().GetTagName().ToString(), *PackageFileName)
	}
}

void UAssetTagTreeNode::PersistOnTags(const FGameplayTagContainer& Tags)
{
	if (!Tags.HasTag(NodeTag))
	{
		return;
	}

	if (Tags.HasTagExact(NodeTag))
	{
		this->Persist();
	}

	for (auto Child : Children)
	{
		if (Child.IsValid())
		{
			Child->PersistOnTags(Tags);
		}
	}
}

void UAssetTagTreeNode::PersistAll()
{
	this->Persist();

	for (auto Child : Children)
	{
		if (Child.IsValid())
		{
			Child->PersistAll();
		}
	}
}

bool UAssetTagTreeNode::IsRootNode() const
{
	return bIsRootNode;
}

void UAssetTagTreeNode::InitializeRootNode()
{
	if (IsRootNode())
	{
		UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
		Subsystem->RegisterRootNode(this);
		this->PrintTree();
	}
}

void UAssetTagTreeNode::SetRootPackage(UPackage* Package)
{
	this->RootPackage = Package;
}

void UAssetTagTreeNode::SetIsRootNode(bool value)
{
	this->bIsRootNode = value;
}

void UAssetTagTreeNode::InsertChildNode(UAssetTagTreeNode* Node)
{
	if (!this->IsAncestorOf(Node))
	{
		return;
	}

	for (auto Child : Children)
	{
		// prevent node being added twice
		if (Child->GetTag().MatchesTagExact(Node->GetTag()))
		{
			return;
		}
	}

	if (this->IsParentOf(Node))
	{
		this->Children.Add(Node);
		return;
	}

	for (auto Child : Children)
	{
		Child->InsertChildNode(Node);
	}
}

bool UAssetTagTreeNode::IsAncestorOf(UAssetTagTreeNode* Node) const
{
	return Node->GetTag().MatchesTag(this->NodeTag);
}

const FGameplayTag& UAssetTagTreeNode::GetParentTag() const
{
	return this->ParentTag;
}

bool UAssetTagTreeNode::IsParentOf(UAssetTagTreeNode* Node) const
{
	return Node->GetParentTag().MatchesTagExact(this->NodeTag);
}

void UAssetTagTreeNode::PrintTree()
{
	LOG_INFO("Tag: %s; Objects: %i", *NodeTag.GetTagName().ToString(), Objects.Num())

	for (auto Child : Children)
	{
		if (Child.IsValid())
		{
			Child->PrintTree();
		}
	}
}

FGameplayTag UAssetTagTreeNode::GetTag() const
{
	return this->NodeTag;
}

FGameplayTag UAssetTagTreeNode::GetNextTag(const FGameplayTag& SearchedTag)
{
	if (!SearchedTag.MatchesTag(NodeTag))
	{
		return {};
	}

	if (SearchedTag.MatchesTagExact(NodeTag))
	{
		return NodeTag;
	}

	FString FollowingTags = SearchedTag.GetTagName().ToString().Mid(NodeTag.GetTagName().ToString().Len() + 1);

	FString NextTag = SearchedTag.GetTagName().ToString();
	int NextDotPosition = 0;
	if (FollowingTags.FindChar('.', NextDotPosition))
	{
		FollowingTags.RemoveAt(NextDotPosition, FollowingTags.Len() - NextDotPosition);
		NextTag = NodeTag.GetTagName().ToString() + "." + FollowingTags;
	}

	return FGameplayTag::RequestGameplayTag(FName(NextTag));
}

FPrimaryAssetId UAssetTagTreeNode::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(RootTagType, *GetName());
}

FSubTreeUpdatedDelegate& UAssetTagTreeNode::GetOnSubTreeUpdatedDelegate()
{
	return OnSubTreeUpdated;
}

void UAssetTagTreeNode::SetTag(const FGameplayTag& Tag)
{
	this->NodeTag = Tag;
}
