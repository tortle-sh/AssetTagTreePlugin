// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayTagContainerUtils.h"

FGameplayTagContainerChangeData UGameplayTagContainerUtils::GetChangeData(
	const FGameplayTagContainer& PreUpdateContainer, const FGameplayTagContainer& PostUpdateContainer)
{
	FGameplayTagContainerChangeData Data{};
	
	Data.InsertedTags.AppendTags(PostUpdateContainer);
	Data.InsertedTags.RemoveTags(PreUpdateContainer);

	Data.RemovedTags.AppendTags(PreUpdateContainer);
	Data.RemovedTags.RemoveTags(PostUpdateContainer);

	Data.BroadcastTags.AppendTags(Data.InsertedTags);
	Data.BroadcastTags.AppendTags(Data.RemovedTags);

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
