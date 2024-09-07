// Copyright (c) 2024 tortle-sh All Rights Reserved

#include "GameplayTagContainerUtils.h"

FGameplayTagContainerChangeData UGameplayTagContainerUtils::GetChangeData(
	const FGameplayTagContainer& PreUpdateContainer, const FGameplayTagContainer& PostUpdateContainer)
{
	FGameplayTagContainerChangeData Data{};
	
	Data.InsertedTags.AppendTags(PostUpdateContainer);
	Data.InsertedTags.RemoveTags(PreUpdateContainer);

	Data.RemovedTags.AppendTags(PreUpdateContainer);
	Data.RemovedTags.RemoveTags(PostUpdateContainer);

	Data.AllTags.AppendTags(Data.InsertedTags);
	Data.AllTags.AppendTags(Data.RemovedTags);

	return Data;
}

void UGameplayTagContainerUtils::CollectParentTags(const FGameplayTagContainer& TargetTags,
	FGameplayTagContainer& ParentTags)
{
	for(auto Tag : TargetTags)
	{
		ParentTags.AppendTags(Tag.GetGameplayTagParents());
	}
}
