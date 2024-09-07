// Copyright (c) 2024 tortle-sh All Rights Reserved


#include "AssetTagTreeSubsystem.h"

#include "GameplayTagContainerUtils.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(AssetTagTree, "AssetTagTree", "Root of all AssetTagTree tags");

FGameplayTagContainer UAssetTagTreeSubsystem::FilterTags(const FGameplayTagContainer& Tags)
{
	FGameplayTagContainer FilteredTags;
	FilteredTags.AppendTags(Tags);
	FilteredTags.RemoveTag(ASTT_ROOT_TAG);

	return FilteredTags;
}

void UAssetTagTreeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	
	RootNode = NewObject<UAssetTagTreeNode>();
	RootNode->PrintTree();
}

void UAssetTagTreeSubsystem::Deinitialize()
{
	Super::Deinitialize();
}

void UAssetTagTreeSubsystem::RegisterCallbackOnNodes(const FTagTreeUpdateCallbackDelegate &CallbackDelegate, const FGameplayTagContainer &Tags)
{
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	
	TArray<UAssetTagTreeNode*> Nodes = this->RootNode->FindNodesByTags(FilteredTags);
	for (auto Node : Nodes)
	{
		UE_LOG(LogTemp, Display, TEXT("added Callback to %s"), *Node->GetTag().GetTagName().ToString());
		Node->GetOnSubTreeUpdatedDelegate().Add(CallbackDelegate);
	}
}

void UAssetTagTreeSubsystem::RemoveCallbackFromNodes(const FTagTreeUpdateCallbackDelegate& CallbackDelegate,
	const FGameplayTagContainer& Tags)
{
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	
	TArray<UAssetTagTreeNode*> Nodes = this->RootNode->FindNodesByTags(FilteredTags);
	for(auto Node : Nodes)
	{
		UE_LOG(LogTemp, Display, TEXT("removed Callback from %s"), *Node->GetTag().GetTagName().ToString());
		Node->GetOnSubTreeUpdatedDelegate().Remove(CallbackDelegate);
	}
}

void UAssetTagTreeSubsystem::InsertObjectToTag(UObject* InsertedObject, const FGameplayTag& Tag)
{
	this->RootNode->InsertToTag(InsertedObject, Tag);
}

void UAssetTagTreeSubsystem::InsertObjectToTags(UObject* InsertedObject, const FGameplayTagContainer& Tags)
{
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	this->RootNode->InsertToTags(InsertedObject, FilteredTags);
}

void UAssetTagTreeSubsystem::RemoveObjectFromTag(UObject* RemovedObject, const FGameplayTag& Tag)
{
	this->RootNode->RemoveObjectFromTag(RemovedObject, Tag);
}

void UAssetTagTreeSubsystem::RemoveObjectFromTags(UObject* RemovedObject, const FGameplayTagContainer& Tags)
{
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	this->RootNode->RemoveObjectFromTags(RemovedObject, FilteredTags);
}

void UAssetTagTreeSubsystem::NotifySubscribers(FGameplayTagContainer& TargetTags, EBroadCastTagStrategy TagCollectionStrategy)
{
	TargetTags.RemoveTag(ASTT_ROOT_TAG);
	
	if(TargetTags.Num() > 0)
	{
		FGameplayTagContainer BroadcastTags;
		switch (TagCollectionStrategy) {
		case TAG_ONLY:
			BroadcastTags.AppendTags(TargetTags);
			break;
		case TAG_AND_CHILDREN:
			RootNode->CollectChildTagsOfTargetTags(TargetTags, BroadcastTags);
			break;
		case TAG_AND_PARENTS:
			BroadcastTags.AppendTags(TargetTags);
			UGameplayTagContainerUtils::CollectParentTags(TargetTags, BroadcastTags);
			break;
		case TAG_CHILDREN_AND_PARENT:
			RootNode->CollectChildTagsOfTargetTags(TargetTags, BroadcastTags);
			UGameplayTagContainerUtils::CollectParentTags(TargetTags, BroadcastTags);
			break;
		}
		
		BroadcastTags.RemoveTag(ASTT_ROOT_TAG);
		
		this->RootNode->BroadcastUpdates(BroadcastTags);
	}
}
