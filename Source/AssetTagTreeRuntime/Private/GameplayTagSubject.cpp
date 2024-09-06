// Copyright (c) 2024 tortle-sh All Rights Reserved

#include "GameplayTagSubject.h"
#include "AssetTagTreePCH.h"

#include "AssetTagTreeSubsystem.h"
#include "AssetTagTreeRuntimeModule.h"
#include "GameplayTagContainerUtils.h"

void FGameplayTagSubject::PreEditChange()
{
	PreChangeTagContainer.Reset();
	PreChangeTagContainer.AppendTags(TagContainer);
}

bool FGameplayTagSubject::HaveTagsChanged(FPropertyChangedEvent& PropertyChangedEvent)
{
	return PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(
		FGameplayTagSubject, TagContainer);
}

void FGameplayTagSubject::InitializeSubject()
{
	if (TagContainer.IsEmpty())
	{
		return;
	}

	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	Subsystem->InsertObjectToTags(this->Parent, TagContainer);
}

void FGameplayTagSubject::DeinitializeSubject()
{
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	Subsystem->RemoveObjectFromTags(this->Parent, this->TagContainer);
	Subsystem->NotifySubscribers(this->TagContainer, this->BroadcastStrategy);
}

void FGameplayTagSubject::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if(PropertyChangedEvent.GetPropertyName() != GET_MEMBER_NAME_CHECKED(FGameplayTagSubject, TagContainer))
	{
		return;
	}
	
	FGameplayTagContainerChangeData Data = UGameplayTagContainerUtils::GetChangeData(
		PreChangeTagContainer, TagContainer);
	UAssetTagTreeSubsystem* AssetTagTreeSubsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();

	if (Data.InsertedTags.Num() > 0)
	{
		LOG_INFO("Inserted Tags: %s", *Data.InsertedTags.ToString());
		AssetTagTreeSubsystem->InsertObjectToTags(this->Parent, Data.InsertedTags);
	}

	if (Data.RemovedTags.Num() > 0)
	{
		LOG_INFO("Removed Tags: %s", *Data.RemovedTags.ToString());
		AssetTagTreeSubsystem->RemoveObjectFromTags(this->Parent, Data.RemovedTags);
	}

	AssetTagTreeSubsystem->NotifySubscribers(Data.BroadcastTags, BroadcastStrategy);
}
