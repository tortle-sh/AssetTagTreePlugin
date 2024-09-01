// Fill out your copyright notice in the Description page of Project Settings.


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
	this->RootNode->Insert(InsertedObject, Tag);
}

void UAssetTagTreeSubsystem::RemoveObjectFromTag(UObject* RemovedObject, const FGameplayTag& Tag)
{
	this->RootNode->RemoveObject(RemovedObject, Tag);
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
