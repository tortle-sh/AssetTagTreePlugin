// Copyright (c) 2024 tortle-sh All Rights Reserved

#include "AssetTagSubject.h"

#include "common/AssetTagTreePCH.h"
#include "AssetTagTreeSubsystem.h"
#include "common/AssetTagTreeUtils.h"

void FAssetTagSubject::PreEditChange()
{
	PreChangeTagContainer.Reset();
	PreChangeTagContainer.AppendTags(TagContainer);
}

bool FAssetTagSubject::HaveTagsChanged(const FPropertyChangedEvent& PropertyChangedEvent)
{
	return PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED( FAssetTagSubject, TagContainer);
}

void FAssetTagSubject::InitializeSubject(UObject* Subject, uint8 BroadcastStrategy)
{
	if (!Initialized)
	{
		ParentObject = Subject;
		BroadcastTo = BroadcastStrategy;
		TagContainer = {};
		PreChangeTagContainer = {};
		SubjectId = FGuid::NewGuid();
		this->Initialized = true;
	}
	
	if (TagContainer.IsEmpty())
	{
		return;
	}
	
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if(!Subsystem)
	{
		LOG_WARNING("AssetTagTreeSubsystem not initialized!");
		return;
	}

	Subsystem->InsertObjectToTags(this->ParentObject, TagContainer, SubjectId);
}



void FAssetTagSubject::DeinitializeSubject() const
{
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if(!Subsystem)
	{
		LOG_WARNING("AssetTagTreeSubsystem not initialized!");
		return;
	}

	Subsystem->RemoveObjectFromTags(this->ParentObject, this->TagContainer, SubjectId);
	Subsystem->NotifySubscribers(this->TagContainer, BroadcastTo, Removed, this->ParentObject);
}

void FAssetTagSubject::PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent) const
{
	if(!HaveTagsChanged(PropertyChangedEvent))
	{
		return;
	}
	
	auto [InsertedTags, RemovedTags] = UAssetTagTreeUtils::GetTagContainerChangeData(
		this->PreChangeTagContainer, this->TagContainer);
	
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if(!Subsystem)
	{
		LOG_WARNING("AssetTagTreeSubsystem not initialized!");
		return;
	}

	if (RemovedTags.Num() > 0)
	{
		Subsystem->RemoveObjectFromTags(this->ParentObject, RemovedTags, this->SubjectId);
	}
	Subsystem->NotifySubscribers(RemovedTags, BroadcastTo, Removed, this->ParentObject);
	
	if (InsertedTags.Num() > 0)
	{
		Subsystem->InsertObjectToTags(this->ParentObject, InsertedTags, this->SubjectId);
	}
	Subsystem->NotifySubscribers(InsertedTags, BroadcastTo, Inserted, this->ParentObject);
}
