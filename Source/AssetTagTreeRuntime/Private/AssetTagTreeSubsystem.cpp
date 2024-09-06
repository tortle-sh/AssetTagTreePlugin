// Copyright (c) 2024 tortle-sh All Rights Reserved


#include "AssetTagTreeSubsystem.h"

#include "GameplayTagContainerUtils.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(AssetTagTree, "AssetTagTree", "Root of all AssetTagTree tags");

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

void UAssetTagTreeSubsystem::InsertObjectToTag(UObject* InsertedObject, const FGameplayTag& Tag)
{
	this->RootNode->InsertToTag(InsertedObject, Tag);
}

void UAssetTagTreeSubsystem::InsertObjectToTags(UObject* InsertedObject, const FGameplayTagContainer& Tags)
{
	this->RootNode->InsertToTags(InsertedObject, Tags);
}

void UAssetTagTreeSubsystem::RemoveObjectFromTag(UObject* RemovedObject, const FGameplayTag& Tag)
{
	this->RootNode->RemoveObjectFromTag(RemovedObject, Tag);
}

void UAssetTagTreeSubsystem::RemoveObjectFromTags(UObject* RemovedObject, const FGameplayTagContainer& Tags)
{
	this->RootNode->RemoveObjectFromTags(RemovedObject, Tags);
}

void UAssetTagTreeSubsystem::NotifySubscribers(FGameplayTagContainer& TargetTags, EBroadCastTagStrategy TagCollectionStrategy)
{
	static const FGameplayTag ROOT_TAG = FGameplayTag::RequestGameplayTag("AssetTagTree");
	TargetTags.RemoveTag(ROOT_TAG);
	
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
		
		BroadcastTags.RemoveTag(ROOT_TAG);
		
		this->RootNode->BroadcastUpdates(BroadcastTags);
	}
}
