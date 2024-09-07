// Copyright (c) 2024 tortle-sh All Rights Reserved

#include "GameplayTagObserver.h"

#include "AssetTagTreeSubsystem.h"
#include "GameplayTagContainerUtils.h"

void FGameplayTagObserver::InitializeObserver()
{
}

void FGameplayTagObserver::DeinitializeObserver()
{
}

bool FGameplayTagObserver::HaveTagsChanged(FPropertyChangedEvent& PropertyChangedEvent)
{
	return PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(
		FGameplayTagObserver, TagContainer);
}

void FGameplayTagObserver::PreEditChange()
{
	this->PreChangeTagContainer.Reset();
	this->PreChangeTagContainer.AppendTags(TagContainer);
}

void FGameplayTagObserver::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if(!HaveTagsChanged(PropertyChangedEvent))
	{
		return;
	}

	FGameplayTagContainerChangeData Data = UGameplayTagContainerUtils::GetChangeData(
		this->PreChangeTagContainer, this->TagContainer);
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();

	if(!Subsystem)
	{
		return;
	}

	if(Data.InsertedTags.Num() > 0)
	{
		Subsystem->RegisterCallbackOnNodes(Callback, Data.InsertedTags);
	}

	if(Data.RemovedTags.Num() > 0)
	{
		Subsystem->RemoveCallbackFromNodes(Callback, Data.RemovedTags);
	}
}
