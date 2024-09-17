// Copyright (c) 2024 tortle-sh All Rights Reserved


#include "AssetTagTreeNode.h"

#include "AssetTagTreePCH.h"

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

TArray<TSoftObjectPtr<UObject>> UAssetTagTreeNode::FindAllObjectsByTags(const FGameplayTagContainer& Tags)
{
	if (!Tags.HasTag(NodeTag))
	{
		return {};
	}

	TArray<TSoftObjectPtr<UObject>> Results = {};
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
		this->Children.Add(NewChild);

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
			this->Objects.Add(NewAssetTagObject);
		}
	}

	CreateMissingChildren(Tags);

	for (auto Child : Children)
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
		LOG_INFO("Removed Object from %s", *GetTag().GetTagName().ToString());
		Objects.Remove(OldAssetTagObject);
	}

	for (auto Child : Children)
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

	for(auto Child : Children)
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

	for(auto Child : Children)
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
