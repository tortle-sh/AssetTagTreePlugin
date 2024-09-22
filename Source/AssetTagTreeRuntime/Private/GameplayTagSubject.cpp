// Copyright (c) 2024 tortle-sh All Rights Reserved

#include "GameplayTagSubject.h"

#include "AssetTagTreePCH.h"
#include "AssetTagTreeSubsystem.h"
#include "GameplayTagContainerUtils.h"

void FGameplayTagSubject::PreEditChange()
{
	PreChangeTagContainer.Reset();
	PreChangeTagContainer.AppendTags(TagContainer);
}

bool FGameplayTagSubject::HaveTagsChanged(const FPropertyChangedEvent& PropertyChangedEvent)
{
	return PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(
		FGameplayTagSubject, TagContainer);
}

void FGameplayTagSubject::InitializeSubject() const
{
	if (TagContainer.IsEmpty())
	{
		return;
	}
	
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if(!Subsystem)
	{
		LOG_WARNING("AssetTagTreeSubystem not initizlized!");
		return;
	}

	Subsystem->InsertObjectToTags(this->Parent, TagContainer);
}



void FGameplayTagSubject::DeinitializeSubject() const
{
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if(!Subsystem)
	{
		LOG_WARNING("AssetTagTreeSubystem not initizlized!");
		return;
	}

	Subsystem->RemoveObjectFromTags(this->Parent, this->TagContainer);
	Subsystem->NotifySubscribers(this->TagContainer, BroadcastTo);
}

void FGameplayTagSubject::PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent) const
{
	if(!HaveTagsChanged(PropertyChangedEvent))
	{
		return;
	}
	
	const auto [InsertedTags, RemovedTags, AllTags] = UGameplayTagContainerUtils::GetChangeData(
		this->PreChangeTagContainer, this->TagContainer);
	
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if(!Subsystem)
	{
		LOG_WARNING("AssetTagTreeSubystem not initizlized!");
		return;
	}

	if (InsertedTags.Num() > 0)
	{
		Subsystem->InsertObjectToTags(this->Parent, InsertedTags);
	}

	if (RemovedTags.Num() > 0)
	{
		Subsystem->RemoveObjectFromTags(this->Parent, RemovedTags);
	}

	Subsystem->NotifySubscribers(AllTags, BroadcastTo);
}
