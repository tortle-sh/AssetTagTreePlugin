// Copyright (c) 2024 tortle-sh All Rights Reserved


#include "AssetTagTreeSubsystem.h"

#include "common/AssetTagTreeUtils.h"

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

			FGameplayTag NewRootTag = ParentTags.IsEmpty() ? Tag : ParentTags.Last();
			RootTags.AddTag(NewRootTag);
			
			UAssetTagTreeNode* NewNode = NewObject<UAssetTagTreeNode>();
			NewNode->SetTag(NewRootTag);
			this->RootNodes.Add(NewNode);
			
			UE_LOG(LogTemp, Display, TEXT("added %s to root nodes"), *NewRootTag.GetTagName().ToString());
		}
	}
}

TArray<TSoftObjectPtr<UObject>> UAssetTagTreeSubsystem::FindObjects(const FGameplayTagContainer& Tags,
                                                                    const int32 CollectionFlags) const
{
	TArray<TSoftObjectPtr<UObject>> ResultObjects;
	for (const auto Node : RootNodes)
	{
		ResultObjects.Append(Node->FindObjectsByTags(Tags, CollectionFlags, true));
	}

	return ResultObjects;
}

void UAssetTagTreeSubsystem::RegisterCallbackOnNodes(const FCallbackDelegate& CallbackDelegate,
                                                     const FGameplayTagContainer& Tags)
{
	AddMissingRootNodes(Tags);
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	for (const auto Node : RootNodes)
	{
		Node->RegisterCallbackOnTags(CallbackDelegate, FilteredTags);
	}
}

void UAssetTagTreeSubsystem::RemoveCallbackFromNodes(const FCallbackDelegate& CallbackDelegate,
                                                     const FGameplayTagContainer& Tags)
{
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	for (const auto Node : RootNodes)
	{
		Node->RemoveCallbackFromTags(CallbackDelegate, FilteredTags);
	}
}

void UAssetTagTreeSubsystem::AddTagsToNodeTree(const FGameplayTagContainer& Tags)
{
	AddMissingRootNodes(Tags);
	for (const auto Node : RootNodes)
	{
		Node->InsertToTags(nullptr, Tags);
	}
}

void UAssetTagTreeSubsystem::InsertObjectToTags(UObject* InsertedObject, const FGameplayTagContainer& Tags)
{
	AddMissingRootNodes(Tags);
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	for (const auto Node : RootNodes)
	{
		Node->InsertToTags(InsertedObject, FilteredTags);
	}
}

void UAssetTagTreeSubsystem::RemoveObjectFromTags(UObject* RemovedObject, const FGameplayTagContainer& Tags)
{
	const FGameplayTagContainer FilteredTags = FilterTags(Tags);
	for (const auto Node : RootNodes)
	{
		Node->RemoveObjectFromTags(RemovedObject, FilteredTags);
	}
}

void UAssetTagTreeSubsystem::CollectTags(const FGameplayTagContainer& TargetTags, const int32 TagCollectionFlags, FGameplayTagContainer& CollectedTags)
{
	FGameplayTagContainer TargetTagsCopy;
	TargetTagsCopy.AppendTags(TargetTags);

	if(!(TagCollectionFlags & RootNode))
	{
		TargetTagsCopy.RemoveTags(RootTags);
	}
	
	CollectedTags.AppendTags(TargetTagsCopy);
		
	if(TagCollectionFlags & Parents)
	{
		UAssetTagTreeUtils::CollectParentTags(TargetTagsCopy, CollectedTags);

		if(!(TagCollectionFlags & RootNode))
		{
			CollectedTags.RemoveTags(RootTags);
		}
	}

	if(TagCollectionFlags & Children)
	{
		for(const auto Node : RootNodes)
		{
			Node->CollectChildTagsOfTargetTags(TargetTagsCopy, CollectedTags);
		}
	}
}

void UAssetTagTreeSubsystem::NotifySubscribers(
	const FGameplayTagContainer& TargetTags,
	const int32 TagCollectionFlags,
	const EBroadcastType BroadcastType,
	const TSoftObjectPtr<UObject> ModifiedObject
	)
{
	FGameplayTagContainer BroadcastTags;
	CollectTags(TargetTags, TagCollectionFlags, BroadcastTags);

	for (const auto Node : RootNodes)
	{
		Node->BroadcastUpdates(BroadcastTags, BroadcastType, ModifiedObject);
	}
}
