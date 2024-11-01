// Copyright (c) 2024 tortle-sh All Rights Reserved


#include "AssetTagTreeNode.h"

#include "AssetTagTreeSubsystem.h"
#include "common/AssetTagTreePCH.h"
#include "common/AssetTagTreeUtils.h"

TArray<TSoftObjectPtr<UObject>> UAssetTagTreeNode::FindObjectsByTags(const FGameplayTagContainer& Tags,
                                                                     const int32 CollectObjectsFrom,
                                                                     const bool bIsRootTag)
{
	if (!Tags.HasTag(NodeTag))
	{
		return {};
	}

	TArray<TSoftObjectPtr<UObject>> Results = {};
	if(Tags.HasTagExact(NodeTag))
	{
		if(!bIsRootTag || CollectObjectsFrom & RootNode)
		{
			Results.Append(Objects);
			
			if(CollectObjectsFrom & Children)
			{
				Results.Append(FindObjectsFromChildren(true));
			}

			return Results;
		}

	}
	
	if(CollectObjectsFrom & Parents){
		Results.Append(Objects);
	}

	for (const auto Child : ChildNodes)
	{
		Results.Append(Child->FindObjectsByTags(Tags, CollectObjectsFrom));
	}

	return Results;
}

TArray<TSoftObjectPtr<UObject>> UAssetTagTreeNode::FindObjectsFromChildren(const bool bIsFirst)
{
	TArray<TSoftObjectPtr<UObject>> Results;
	if(!bIsFirst)
	{
		Results.Append(Objects);
	}
	
	for(const auto Child : ChildNodes)
	{
		Results.Append(Child->FindObjectsFromChildren());
	}

	return Results;
}

TArray<UAssetTagTreeNode*> UAssetTagTreeNode::FindAllNodesByTags(const FGameplayTagContainer& Tags)
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


	for (const auto Child : ChildNodes)
	{
		Results.Append(Child->FindAllNodesByTags(Tags));
	}

	return Results;
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

		for (const auto Child : ChildNodes)
		{
			if (!Child)
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

		if(!NextTag.MatchesTag(NodeTag))
		{
			continue;
		}

		if(NextTag.MatchesTagExact(NodeTag))
		{
			continue;
		}

		if(CreatedTags.HasTagExact(NextTag))
		{
			continue;
		}
		
		UAssetTagTreeNode* NewChild = NewObject<UAssetTagTreeNode>();
		NewChild->SetTag(NextTag);
		this->ChildNodes.Add(NewChild);

		CreatedTags.AddTag(NextTag);
	}
}

void UAssetTagTreeNode::InsertToTags(UObject* NewAssetTagObject, const FGameplayTagContainer& Tags)
{
	if (!Tags.HasTag(NodeTag))
	{
		return;
	}

	if (Tags.HasTagExact(NodeTag))
	{
		if(NewAssetTagObject)
		{
			LOG_INFO("added %s to %s", *NewAssetTagObject->GetName(), *GetTag().GetTagName().ToString());
			this->Objects.Add(NewAssetTagObject);
		}
	}

	CreateMissingChildren(Tags);

	for (const auto Child : ChildNodes)
	{
		Child->InsertToTags(NewAssetTagObject, Tags);
	}
}

void UAssetTagTreeNode::RemoveObjectFromTags(UObject* OldAssetTagObject, const FGameplayTagContainer& Tags)
{
	if (!Tags.HasTag(NodeTag))
	{
		return;
	}

	if (Tags.HasTagExact(NodeTag))
	{
		LOG_INFO("removed %s from %s", *OldAssetTagObject->GetName(), *GetTag().GetTagName().ToString());
		Objects.Remove(OldAssetTagObject);
	}

	for (const auto Child : ChildNodes)
	{
		Child->RemoveObjectFromTags(OldAssetTagObject, Tags);
	}
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
		for (const auto Child : ChildNodes)
		{
			Child->CollectChildTagsOfTargetTags(TargetTags, ChildTags);
		}
		return;
	}

	ChildTags.AddTag(NodeTag);
	CollectChildTags(ChildTags);
}

void UAssetTagTreeNode::CollectChildTags(FGameplayTagContainer& ChildTags)
{
	for (const auto Child : ChildNodes)
	{
		ChildTags.AddTag(Child->NodeTag);
		Child->CollectChildTags(ChildTags);
	}
}

void UAssetTagTreeNode::BroadcastUpdate(const EBroadcastType BroadcastType, const TSoftObjectPtr<UObject>& ChangedObject) const
{
	LOG_INFO("%s Type Broadcast from: %s", *UAssetTagTreeUtils::BroadcastTypeToString(BroadcastType), *NodeTag.GetTagName().ToString());
	OnSubTreeUpdated.Broadcast(BroadcastType, ChangedObject);
}

void UAssetTagTreeNode::BroadcastUpdates(const FGameplayTagContainer& Tags, const EBroadcastType BroadcastType, const TSoftObjectPtr<UObject>& ChangedObject)
{
	if (!Tags.HasTag(NodeTag))
	{
		return;
	}

	if (Tags.HasTagExact(NodeTag))
	{
		BroadcastUpdate(BroadcastType, ChangedObject);
	}

	for (const auto Child : ChildNodes)
	{
		Child->BroadcastUpdates(Tags, BroadcastType, ChangedObject);
	}
}

void UAssetTagTreeNode::BroadcastUpdatesToChildren(const EBroadcastType BroadcastType, const TSoftObjectPtr<UObject>& ChangedObject)
{
	for (const auto Child : ChildNodes)
	{
		Child->BroadcastUpdate(BroadcastType, ChangedObject);
		Child->BroadcastUpdatesToChildren(BroadcastType, ChangedObject);
	}
}

void UAssetTagTreeNode::PrintTree()
{
	LOG_INFO("Tag: %s; Objects: %i", *NodeTag.GetTagName().ToString(), Objects.Num())

	for (const auto Child : ChildNodes)
	{
		Child->PrintTree();
	}
}

FGameplayTag UAssetTagTreeNode::GetTag() const
{
	return this->NodeTag;
}

FGameplayTag UAssetTagTreeNode::GetNextTag(const FGameplayTag& SearchedTag) const
{
	if (!SearchedTag.MatchesTag(NodeTag))
	{
		return {};
	}

	if (NodeTag.MatchesTagExact(SearchedTag))
	{
		return SearchedTag;
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

FSubTreeUpdatedDelegate& UAssetTagTreeNode::GetOnSubTreeUpdatedDelegate()
{
	return OnSubTreeUpdated;
}

void UAssetTagTreeNode::RegisterCallbackOnTags(const FCallbackDelegate& CallbackDelegate,
	const FGameplayTagContainer& Tags)
{
	if(!Tags.HasTag(NodeTag))
	{
		return;
	}

	if(Tags.HasTagExact(NodeTag))
	{
		this->OnSubTreeUpdated.Add(CallbackDelegate);
		LOG_INFO("added callback to %s", *GetTag().GetTagName().ToString());
	}

	CreateMissingChildren(Tags);

	for(const auto Child : ChildNodes)
	{
		Child->RegisterCallbackOnTags(CallbackDelegate, Tags);
	}
}

void UAssetTagTreeNode::RemoveCallbackFromTags(const FCallbackDelegate& CallbackDelegate,
	const FGameplayTagContainer& Tags)
{
	if(!Tags.HasTag(NodeTag))
	{
		return;
	}

	if(Tags.HasTagExact(NodeTag))
	{
		LOG_INFO("Removed callback from %s", *GetTag().GetTagName().ToString());
		this->OnSubTreeUpdated.Remove(CallbackDelegate);
	}

	for(const auto Child : ChildNodes)
	{
		if(Child)
		{
			Child->RemoveCallbackFromTags(CallbackDelegate, Tags);
		}
	}
}

void UAssetTagTreeNode::SetTag(const FGameplayTag& Tag)
{
	this->NodeTag = Tag;
}
