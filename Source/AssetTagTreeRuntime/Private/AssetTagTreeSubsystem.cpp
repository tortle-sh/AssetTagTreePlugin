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

TArray<TSoftObjectPtr<UObject>> UAssetTagTreeSubsystem::FindObjectsWithTags(const FGameplayTagContainer& Tags) const
{
	return this->RootNode->FindAllObjectsByTags(Tags);
}

void UAssetTagTreeSubsystem::RegisterCallbackOnNodes(const FCallbackDelegate &CallbackDelegate, const FGameplayTagContainer &Tags)
{
	this->RootNode->RegisterCallbackOnTags(CallbackDelegate, FilterTags(Tags));
}

void UAssetTagTreeSubsystem::RemoveCallbackFromNodes(const FCallbackDelegate& CallbackDelegate,
	const FGameplayTagContainer& Tags)
{
	this->RootNode->RemoveCallbackFromTags(CallbackDelegate, FilterTags(Tags));
}

void UAssetTagTreeSubsystem::AddTagsToNodeTree(const FGameplayTagContainer& Tags)
{
	RootNode->InsertToTags(nullptr, Tags);
}

void UAssetTagTreeSubsystem::InsertObjectToTags(UObject* InsertedObject, const FGameplayTagContainer& Tags)
{
	this->RootNode->InsertToTags(InsertedObject, FilterTags(Tags));
}

void UAssetTagTreeSubsystem::RemoveObjectFromTags(UObject* RemovedObject, const FGameplayTagContainer& Tags)
{
	this->RootNode->RemoveObjectFromTags(RemovedObject, Tags);
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
