// Copyright (c) 2024 tortle-sh All Rights Reserved

#include "AssetTagObserver.h"

#include "common/AssetTagTreePCH.h"
#include "AssetTagTreeSubsystem.h"
#include "common/AssetTagTreeUtils.h"

void FAssetTagObserver::AddObject(const TSoftObjectPtr<UObject>& Object)
{
	if (!Object.Get()->GetClass()->IsChildOf(FilteredClass))
	{
		return;
	}

	int32* Entry = ObservedObjects.Find(Object);
	if (Entry == nullptr)
	{
		ObservedObjects.Emplace(Object, 1);
		return;
	}

	*Entry = *Entry + 1;
}

void FAssetTagObserver::RemoveObject(const TSoftObjectPtr<UObject>& Object)
{
	int32* Entry = ObservedObjects.Find(Object);
	if (Entry == nullptr)
	{
		return;
	}

	*Entry = *Entry - 1;
	if (*Entry < 1)
	{
		ObservedObjects.Remove(Object);
	}
}

void FAssetTagObserver::InitializeObserver() const
{
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if (!Subsystem)
	{
		LOG_ERROR("AssetTagTreeSubsystem is not initialized!");
		exit(1);
	}

	Subsystem->AddTagsToNodeTree(this->TagContainer);
	Subsystem->RegisterCallbackOnNodes(OnSubscribedNodesUpdated, this->TagContainer);

}

void FAssetTagObserver::DeinitializeObserver() const
{
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if (!Subsystem)
	{
		LOG_ERROR("AssetTagTreeSubsystem is not initialized!");
		exit(1);
	}

	Subsystem->RemoveCallbackFromNodes(this->OnSubscribedNodesUpdated, this->TagContainer);
}

bool FAssetTagObserver::Validate()
{
	// validate graph integrity to handle merge conflicts
	const uint32 InitialHash = CollectionHash;
	LoadHashesForTags();
	UpdateCollectionHash();
	if (InitialHash != CollectionHash)
	{
		LoadObservedObjects();
		return false;
	}

	return true;
}

void FAssetTagObserver::PreEditChange()
{
	this->PreChangeTagContainer.Reset();
	this->PreChangeTagContainer.AppendTags(TagContainer);
}

void FAssetTagObserver::OnTagChanges()
{
	const auto [InsertedTags, RemovedTags] = UAssetTagTreeUtils::GetTagContainerChangeData(
		this->PreChangeTagContainer, this->TagContainer);
	UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();

	if (!Subsystem)
	{
		LOG_ERROR("AssetTagTreeSubsystem is not initialized!");
		exit(1);
	}

	if (!InsertedTags.IsEmpty())
	{
		Subsystem->RegisterCallbackOnNodes(OnSubscribedNodesUpdated, InsertedTags);
	}

	if (!RemovedTags.IsEmpty())
	{
		Subsystem->RemoveCallbackFromNodes(OnSubscribedNodesUpdated, RemovedTags);
		for (auto const Tag : InsertedTags)
		{
			NodeHashes.Remove(Tag);
		}
	}

	LoadHashesForTags();
	UpdateCollectionHash();
	
	LoadObservedObjects();
}

void FAssetTagObserver::PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FAssetTagObserver, TagContainer))
	{
		OnTagChanges();
		return;
	}

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FAssetTagObserver, CollectObjectsFrom))
	{
		LoadObservedObjects();
		return;
	}

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FAssetTagObserver, FilteredClass))
	{
		LoadObservedObjects();
		return;
	}
}

void FAssetTagObserver::LoadHashesForTags()
{
	const UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if (!Subsystem)
	{
		LOG_ERROR("AssetTagTreeSubsystem is not initialized!");
		exit(1);
	}

	NodeHashes.Empty();
	Subsystem->CollectNodeHashes(this->NodeHashes, this->TagContainer, this->CollectObjectsFrom);
}

void FAssetTagObserver::UpdateCollectionHash()
{
	for (const auto Hash : NodeHashes)
	{
		CollectionHash = HashCombine(CollectionHash, Hash.Value);
	}
}

TArray<TSoftObjectPtr<UObject>> FAssetTagObserver::FindObservedObjects() const
{
	const UAssetTagTreeSubsystem* Subsystem = GEngine->GetEngineSubsystem<UAssetTagTreeSubsystem>();
	if (!Subsystem)
	{
		LOG_ERROR("AssetTagTreeSubsystem is not initialized!");
		exit(1);
	}

	return Subsystem->FindObjects(this->TagContainer, CollectObjectsFrom);
}

void FAssetTagObserver::LoadObservedObjects()
{
	ObservedObjects.Empty();
	for (const auto Object : FindObservedObjects())
	{
		AddObject(Object);
	}

	if (!OnObservedObjectsChanged.ExecuteIfBound())
	{
		LOG_WARNING("OnObservedObjectsChanged is not bound");
	}
}

void FAssetTagObserver::OnCallbackUpdateObservedObjects(
	const EBroadcastType BroadcastType,
	const TSoftObjectPtr<UObject>& UpdatedObject,
	const FGameplayTag& UpdatedTag,
	uint32 NodeHash)
{
	switch (BroadcastType)
	{
	case Removed:
		this->RemoveObject(UpdatedObject);
		break;
	case Inserted:
		this->AddObject(UpdatedObject);
		break;
	default:
		LOG_ERROR("Unknown BroadcastType");
		exit(1);
	}
	
	NodeHashes.Emplace(UpdatedTag, NodeHash);
	UpdateCollectionHash();

	if (!OnObservedObjectsChanged.ExecuteIfBound())
	{
		LOG_WARNING("OnObservedObjectsChanged is not bound");
	}
}
