// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTagTreePCH.h"
#include "AssetTagTreeContainerSubject.h"

#include "AssetTagTreeSubsystem.h"
#include "AssetTagTreeRuntimeModule.h"
#include "GameplayTagContainerUtils.h"

void UAssetTagTreeContainerSubject::PreEditChange(FProperty* PropertyAboutToChange)
{
	UObject::PreEditChange(PropertyAboutToChange);
	if(TagContainer.IsEmpty())
	{
		return;
	}

	if(PropertyAboutToChange->GetFName() == GET_MEMBER_NAME_CHECKED(UAssetTagTreeContainerSubject, TagContainer))
	{
		PreChangeTagContainer = FGameplayTagContainer();
		PreChangeTagContainer.AppendTags(TagContainer);
	}
}

void UAssetTagTreeContainerSubject::InitializeSubject()
{
	if(TagContainer.IsEmpty())
	{
		return;
	}

	UAssetTagTreeSubsystem *Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	for( auto Tag : TagContainer)
	{
		Subsystem->InsertObjectToTag(this, Tag);
	}
}

void UAssetTagTreeContainerSubject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if(TagContainer.IsEmpty())
	{
		return;
	}

	UObject::PostEditChangeProperty(PropertyChangedEvent);

	if(PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(UAssetTagTreeContainerSubject, TagContainer))
	{
		FGameplayTagContainerChangeData Data =  UGameplayTagContainerUtils::GetChangeData(PreChangeTagContainer, TagContainer);
		UAssetTagTreeSubsystem *AssetTagTreeSubsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();

		if(Data.InsertedTags.Num() > 0)
		{
			LOG_INFO("Inserted Tags: %s", *Data.InsertedTags.ToString());
			for(auto Tag : Data.InsertedTags)
			{
				AssetTagTreeSubsystem->InsertObjectToTag(this, Tag);
			}
		}
		
		if(Data.RemovedTags.Num() > 0)
		{
			LOG_INFO("Removed Tags: %s", *Data.RemovedTags.ToString());
			for(auto Tag : Data.RemovedTags)
			{
				AssetTagTreeSubsystem->RemoveObjectFromTag(this, Tag);
			}
		}
		
		AssetTagTreeSubsystem->NotifySubscribers(Data.BroadcastTags, TAG_AND_CHILDREN);
	}
}
