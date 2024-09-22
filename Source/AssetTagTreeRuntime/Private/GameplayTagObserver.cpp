// Copyright (c) 2024 tortle-sh All Rights Reserved

#include "GameplayTagObserver.h"

#include "AssetTagTreePCH.h"
#include "AssetTagTreeSubsystem.h"
#include "GameplayTagContainerUtils.h"

void FGameplayTagObserver::InitializeObserver() const
{
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if(!Subsystem)
	{
		LOG_WARNING("AssetTagTreeSubsystem is not initialized!");
		return;
	}
	
	Subsystem->AddTagsToNodeTree(this->TagContainer);
	Subsystem->RegisterCallbackOnNodes(Callback, this->TagContainer);
}

void FGameplayTagObserver::DeinitializeObserver() const
{
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if(!Subsystem)
	{
		LOG_WARNING("AssetTagTreeSubsystem is not initialized!");
		return;
	}
	
	Subsystem->RemoveCallbackFromNodes(this->Callback, this->TagContainer);
}

void FGameplayTagObserver::PreEditChange()
{
	this->PreChangeTagContainer.Reset();
	this->PreChangeTagContainer.AppendTags(TagContainer);
}

void FGameplayTagObserver::OnTagChanges() const
{
	const auto [InsertedTags, RemovedTags, AllTags] = UGameplayTagContainerUtils::GetChangeData(
		this->PreChangeTagContainer, this->TagContainer);
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();

	if(!Subsystem)
	{
		LOG_WARNING("AssetTagTreeSubsystem is not initialized!");
		return;
	}

	if(!InsertedTags.IsEmpty())
	{
		Subsystem->RegisterCallbackOnNodes(Callback, InsertedTags);
	}

	if(!RemovedTags.IsEmpty())
	{
		Subsystem->RemoveCallbackFromNodes(Callback, RemovedTags);
	}
}

void FGameplayTagObserver::PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent) const
{
	if(PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FGameplayTagObserver, TagContainer))
	{
		OnTagChanges();
	}

	if(PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FGameplayTagObserver, CollectObjectsFrom))
	{
		Callback.Execute();
	}
}

TSet<TSoftObjectPtr<UObject>> FGameplayTagObserver::FindObservedObjects() const
{
	const UAssetTagTreeSubsystem *Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if(!Subsystem)
	{
		LOG_WARNING("AssetTagTreeSubsystem is not initialized!");
		return {};
	}
	
	return Subsystem->FindObjects(this->TagContainer, CollectObjectsFrom);
}
