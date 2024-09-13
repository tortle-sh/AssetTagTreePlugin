// Copyright (c) 2024 tortle-sh All Rights Reserved


#include "AssetTagTreeSubsystem.h"

#include "AssetTagTreePCH.h"
#include "GameplayTagContainerUtils.h"

FGameplayTagContainer UAssetTagTreeSubsystem::FilterTags(const FGameplayTagContainer& Tags)
{
	FGameplayTagContainer FilteredTags;
	FilteredTags.AppendTags(Tags);
	FilteredTags.RemoveTags(this->GetRootTags());

	return FilteredTags;
}

const FGameplayTagContainer& UAssetTagTreeSubsystem::GetRootTags() const
{
	return RootTags;
}

void UAssetTagTreeSubsystem::FindNodesWithTags(const FGameplayTagContainer& Tags, TArray<UAssetTagTreeNode*> &Nodes)
{
	for(auto RootNode : RootNodes)
	{
		Nodes.Append(RootNode->FindNodesByTags(Tags));
	}
}


void UAssetTagTreeSubsystem::RegisterCallbackOnNodes(const FTagTreeUpdateCallbackDelegate &CallbackDelegate, const FGameplayTagContainer &Tags)
{
	// TODO implement recursive function 
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);

	TArray<UAssetTagTreeNode*> Nodes;
	FindNodesWithTags(FilteredTags, Nodes);
	
	for (auto Node : Nodes)
	{
		UE_LOG(LogTemp, Display, TEXT("added Callback to %s"), *Node->GetTag().GetTagName().ToString());
		Node->GetOnSubTreeUpdatedDelegate().Add(CallbackDelegate);
	}
}

void UAssetTagTreeSubsystem::RemoveCallbackFromNodes(const FTagTreeUpdateCallbackDelegate& CallbackDelegate,
	const FGameplayTagContainer& Tags)
{
	// TODO implement recursive function
	
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	
	TArray<UAssetTagTreeNode*> Nodes;
	FindNodesWithTags(FilteredTags, Nodes);
	
	for(auto Node : Nodes)
	{
		UE_LOG(LogTemp, Display, TEXT("removed Callback from %s"), *Node->GetTag().GetTagName().ToString());
		Node->GetOnSubTreeUpdatedDelegate().Remove(CallbackDelegate);
	}
}

void UAssetTagTreeSubsystem::RegisterRootNode(UAssetTagTreeNode* RootNode)
{
	if(!RootNodes.Contains(RootNode))
	{
		RootNodes.Add(RootNode);
		RootTags.AddTag(RootNode->GetTag());
		LOG_INFO("registered %s", *RootNode->GetName());
	}
}

void UAssetTagTreeSubsystem::RemoveRootNode(UAssetTagTreeNode* RootNode)
{
	RootNodes.Remove(RootNode);
	RootTags.RemoveTag(RootNode->GetTag());
}

void UAssetTagTreeSubsystem::InsertObjectToTag(UObject* InsertedObject, const FGameplayTag& Tag)
{
	for(auto RootNode : RootNodes)
	{
		if(RootNode->InsertToTag(InsertedObject, Tag))
		{
		}
	}
}

void UAssetTagTreeSubsystem::InsertObjectToTags(UObject* InsertedObject, const FGameplayTagContainer& Tags)
{
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	
	for(auto RootNode : RootNodes)
	{
		if(RootNode->InsertToTags(InsertedObject, FilteredTags))
		{
		}
	}
}

void UAssetTagTreeSubsystem::RemoveObjectFromTag(UObject* RemovedObject, const FGameplayTag& Tag)
{
	
	for(auto RootNode : RootNodes)
	{
		if(RootNode->RemoveObjectFromTag(RemovedObject, Tag))
		{
		}
	}
}

void UAssetTagTreeSubsystem::RemoveObjectFromTags(UObject* RemovedObject, const FGameplayTagContainer& Tags)
{
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	
	for(auto RootNode : RootNodes)
	{
		if(RootNode->RemoveObjectFromTags(RemovedObject, FilteredTags))
		{
		}
	}
}

void UAssetTagTreeSubsystem::NotifySubscribers(FGameplayTagContainer& TargetTags, EBroadCastTagStrategy TagCollectionStrategy)
{
	TargetTags.RemoveTags(GetRootTags());
	
	if(TargetTags.Num() > 0)
	{
		FGameplayTagContainer BroadcastTags;
		switch (TagCollectionStrategy) {
		case TAG_ONLY:
			BroadcastTags.AppendTags(TargetTags);
			break;
		case TAG_AND_CHILDREN:
			for (auto RootNode : RootNodes)
			{
				RootNode->CollectChildTagsOfTargetTags(TargetTags, BroadcastTags);
			}
			break;
		case TAG_AND_PARENTS:
			BroadcastTags.AppendTags(TargetTags);
			UGameplayTagContainerUtils::CollectParentTags(TargetTags, BroadcastTags);
			break;
		case TAG_CHILDREN_AND_PARENT:
			for (auto RootNode : RootNodes)
			{
				RootNode->CollectChildTagsOfTargetTags(TargetTags, BroadcastTags);
			}
			UGameplayTagContainerUtils::CollectParentTags(TargetTags, BroadcastTags);
			break;
		}
		
		BroadcastTags.RemoveTags(GetRootTags());

		for (auto RootNode : RootNodes)
		{
			RootNode->BroadcastUpdates(BroadcastTags);
			if(BroadcastTags.HasTag(RootNode->GetTag()))
			{
				PersistChanges(RootNode);
			}
		}
	}
}

void UAssetTagTreeSubsystem::PersistChanges(UAssetTagTreeNode* RootNode)
{
	RootNode->Persist();
}
