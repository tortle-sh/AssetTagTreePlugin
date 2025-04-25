// Copyright (c) 2024 tortle-sh All Rights Reserved

#include "AssetTagTreeUtils.h"

FGameplayTagContainerChangeData UAssetTagTreeUtils::GetTagContainerChangeData(
	const FGameplayTagContainer& PreUpdateContainer, const FGameplayTagContainer& PostUpdateContainer)
{
	FGameplayTagContainerChangeData Data{};

	Data.InsertedTags.AppendTags(PostUpdateContainer);
	Data.InsertedTags.RemoveTags(PreUpdateContainer);

	Data.RemovedTags.AppendTags(PreUpdateContainer);
	Data.RemovedTags.RemoveTags(PostUpdateContainer);

	return Data;
}

void UAssetTagTreeUtils::CollectParentTags(const FGameplayTagContainer& TargetTags,
                                                   FGameplayTagContainer& ParentTags)
{
	TArray<FGameplayTag> TempParentTags;
	for (auto Tag : TargetTags)
	{
		Tag.ParseParentTags(TempParentTags);
	}
	ParentTags.AppendTags(FGameplayTagContainer::CreateFromArray(TempParentTags));
}

FString UAssetTagTreeUtils::BroadcastTypeToString(const EBroadcastType Broadcast)
{
	switch (Broadcast)
	{
	case Inserted:
		return "Inserted";
	case Removed:
		return "Removed";
	default:
		return "unknown type";
	}
}
