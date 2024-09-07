// Copyright (c) 2024 tortle-sh All Rights Reserved


#include "AssetTagTreeNode.h"

#include "AssetTagTreePCH.h"
#include "NativeGameplayTags.h"

UAssetTagTreeNode::UAssetTagTreeNode()
{
	NodeTag = FGameplayTag::RequestGameplayTag("AssetTagTree");
}

UAssetTagTreeNode::UAssetTagTreeNode(FGameplayTag Tag)
	: NodeTag(Tag)
{
}

bool UAssetTagTreeNode::IsLeaf() const
{
	return Children.Num() == 0;
}

TArray<UObject*> UAssetTagTreeNode::FindObjectsByTag(const FGameplayTag& Tag) const
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
		if (Tag.MatchesTag(Child->NodeTag))
		{
			return Child->FindObjectsByTag(Tag);
		}
	}

	return {};
}

TArray<UObject*> UAssetTagTreeNode::FindAllObjectsByTags(const FGameplayTagContainer& Tags)
{
	if (!Tags.HasTag(NodeTag))
	{
		return {};
	}

	TArray<UObject*> Results = {};
	Results.Append(Objects);

	for (auto Child : Children)
	{
		if (Tags.HasTag(Child->NodeTag))
		{
			Results.Append(Child->FindAllObjectsByTags(Tags));
		}
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


	for (auto Child : Children)
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

		for (auto Child : Children)
		{
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

		if(CreatedTags.HasTagExact(NextTag))
		{
			continue;
		}
		
		UAssetTagTreeNode* NewChild = NewObject<UAssetTagTreeNode>();
		NewChild->SetTag(NextTag);
		this->Children.Add(NewChild);

		CreatedTags.AddTag(NextTag);
	}
}

void UAssetTagTreeNode::InsertToTag(UObject* NewAssetTagObject, const FGameplayTag& Tag)
{
	if (!Tag.MatchesTag(NodeTag))
	{
		return;
	}

	if (Tag.MatchesTagExact(NodeTag))
	{
		this->Objects.Add(NewAssetTagObject);
		return;
	}

	FGameplayTagContainer SingleTagContainer;
	SingleTagContainer.AddTag(Tag);
	CreateMissingChildren(SingleTagContainer);

	for (auto Child : this->Children)
	{
		Child->InsertToTag(NewAssetTagObject, Tag);
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
		this->Objects.Add(NewAssetTagObject);
	}

	CreateMissingChildren(Tags);

	for (auto Child : Children)
	{
		Child->InsertToTags(NewAssetTagObject, Tags);
	}
}

void UAssetTagTreeNode::RemoveObjectFromTag(UObject* OldAssetTagObject, const FGameplayTag& Tag)
{
	if (!Tag.MatchesTag(NodeTag))
	{
		return;
	}

	if (Tag.MatchesTagExact(NodeTag))
	{
		Objects.Remove(OldAssetTagObject);
		return;
	}

	for (auto Child : Children)
	{
		if (Tag.MatchesTag(Child->NodeTag))
		{
			Child->RemoveObjectFromTag(OldAssetTagObject, Tag);
			return;
		}
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
		Objects.Remove(OldAssetTagObject);
	}

	for (auto Child : Children)
	{
		Child->RemoveObjectFromTags(OldAssetTagObject, Tags);
	}
}

void UAssetTagTreeNode::RemoveNode(const FGameplayTag& Tag)
{
	if (!Tag.MatchesTag(NodeTag))
	{
		return;
	}

	for (auto Child : Children)
	{
		Child->RemoveNode(Tag);
	}

	Objects.Empty();
	Children.Empty();
	BroadcastUpdate();
	OnSubTreeUpdated.Clear();
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
			Child->CollectChildTagsOfTargetTags(TargetTags, ChildTags);
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
		ChildTags.AddTag(Child->NodeTag);
		Child->CollectChildTags(ChildTags);
	}
}

void UAssetTagTreeNode::BroadcastUpdate() const
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
		Child->BroadcastUpdates(Tags);
	}
}

void UAssetTagTreeNode::BroadcastUpdatesToChildren()
{
	for (auto Child : Children)
	{
		Child->BroadcastUpdate();
		Child->BroadcastUpdatesToChildren();
	}
}

void UAssetTagTreeNode::PrintTree()
{
	LOG_INFO("Tag: %s; Objects: %i", *NodeTag.GetTagName().ToString(), Objects.Num())

	for (auto Child : Children)
	{
		Child->PrintTree();
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

void UAssetTagTreeNode::SetTag(const FGameplayTag& Tag)
{
	this->NodeTag = Tag;
}
