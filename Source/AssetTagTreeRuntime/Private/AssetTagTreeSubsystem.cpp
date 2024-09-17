// Copyright (c) 2024 tortle-sh All Rights Reserved


#include "AssetTagTreeSubsystem.h"

#include "GameplayTagContainerUtils.h"

UE_DEFINE_GAMEPLAY_TAG_COMMENT(AssetTagTree, "AssetTagTree", "Root of all AssetTagTree tags");

FGameplayTagContainer UAssetTagTreeSubsystem::FilterTags(const FGameplayTagContainer& Tags) const
{
	FGameplayTagContainer FilteredTags;
	FilteredTags.AppendTags(Tags);
	FilteredTags.RemoveTags(RootTags);

	return FilteredTags;
}

void UAssetTagTreeSubsystem::AddMissingRootNodes(const FGameplayTagContainer& Tags)
{
	for(auto Tag : Tags)
	{
		if(!Tag.MatchesAny(this->RootTags))
		{
			TArray<FGameplayTag> ParentTags;
			Tag.ParseParentTags(ParentTags);

			FGameplayTag NewRootTag = ParentTags.IsEmpty() ? Tag : ParentTags[0];
			RootTags.AddTag(NewRootTag);
			
			UAssetTagTreeNode* NewNode = NewObject<UAssetTagTreeNode>();
			NewNode->SetTag(NewRootTag);
			this->RootNodes.Add(NewNode);
			
			UE_LOG(LogTemp, Display, TEXT("added %s to root nodes"), *NewRootTag.GetTagName().ToString());
		}
	}
}

TArray<TSoftObjectPtr<UObject>> UAssetTagTreeSubsystem::FindObjectsWithTags(const FGameplayTagContainer& Tags) const
{
	TArray<TSoftObjectPtr<UObject>> ResultObjects;
	for (auto Node : RootNodes)
	{
		ResultObjects.Append(Node->FindAllObjectsByTags(Tags));
	}

	return ResultObjects;
}

void UAssetTagTreeSubsystem::RegisterCallbackOnNodes(const FCallbackDelegate& CallbackDelegate,
                                                     const FGameplayTagContainer& Tags)
{
	AddMissingRootNodes(Tags);
	FGameplayTagContainer FilteredTags = FilterTags(Tags);
	for (auto Node : RootNodes)
	{
		Node->RegisterCallbackOnTags(CallbackDelegate, FilteredTags);
	}
}

void UAssetTagTreeSubsystem::RemoveCallbackFromNodes(const FCallbackDelegate& CallbackDelegate,
                                                     const FGameplayTagContainer& Tags)
{
	FGameplayTagContainer FilteredTags = FilterTags(Tags);
	for (auto Node : RootNodes)
	{
		Node->RemoveCallbackFromTags(CallbackDelegate, FilteredTags);
	}
}

void UAssetTagTreeSubsystem::AddTagsToNodeTree(const FGameplayTagContainer& Tags)
{
	AddMissingRootNodes(Tags);
	for (auto Node : RootNodes)
	{
		Node->InsertToTags(nullptr, Tags);
	}
}

void UAssetTagTreeSubsystem::InsertObjectToTags(UObject* InsertedObject, const FGameplayTagContainer& Tags)
{
	AddMissingRootNodes(Tags);
	FGameplayTagContainer FilteredTags = FilterTags(Tags);
	for (auto Node : RootNodes)
	{
		Node->InsertToTags(InsertedObject, FilteredTags);
	}
}

void UAssetTagTreeSubsystem::RemoveObjectFromTags(UObject* RemovedObject, const FGameplayTagContainer& Tags)
{
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	for (auto Node : RootNodes)
	{
		Node->RemoveObjectFromTags(RemovedObject, FilteredTags);
	}
}

void UAssetTagTreeSubsystem::NotifySubscribers(FGameplayTagContainer& TargetTags,
                                               EBroadCastTagStrategy TagCollectionStrategy)
{
	TargetTags.RemoveTags(RootTags);

	if (TargetTags.Num() > 0)
	{
		FGameplayTagContainer BroadcastTags;
		switch (TagCollectionStrategy)
		{
		case TAG_ONLY:
			BroadcastTags.AppendTags(TargetTags);
			break;
		case TAG_AND_CHILDREN:
			for (auto Node : RootNodes)
			{
				Node->CollectChildTagsOfTargetTags(TargetTags, BroadcastTags);
			}
			break;
		case TAG_AND_PARENTS:
			BroadcastTags.AppendTags(TargetTags);
			UGameplayTagContainerUtils::CollectParentTags(TargetTags, BroadcastTags);
			break;
		case TAG_CHILDREN_AND_PARENT:
			for (auto Node : RootNodes)
			{
				Node->CollectChildTagsOfTargetTags(TargetTags, BroadcastTags);
				UGameplayTagContainerUtils::CollectParentTags(TargetTags, BroadcastTags);
			}
			break;
		}

		BroadcastTags.RemoveTags(RootTags);

		for (auto Node : RootNodes)
		{
			Node->BroadcastUpdates(BroadcastTags);
		}
	}
}
