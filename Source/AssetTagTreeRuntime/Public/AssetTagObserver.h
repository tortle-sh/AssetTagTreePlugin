// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeNode.h"
#include "AssetTagTreeSubsystem.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagObserver.generated.h"

DECLARE_DYNAMIC_DELEGATE(FObservedObjectsChangedDelegate);

struct FDefaultObserverConfig
{
	ETagCollectionFlag CollectObjectsFrom;
	TSubclassOf<UObject> FilteredClass;
};

USTRUCT()
struct ASSETTAGTREERUNTIME_API FAssetTagObserver
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(Bitmask, BitmaskEnum = "/Script/AssetTagTreeRuntime.ETagCollectionFlag"))
	uint8 CollectObjectsFrom;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UObject> FilteredClass;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer TagContainer;

	UPROPERTY()
	FCallbackDelegate OnSubscribedNodesUpdated;

	UPROPERTY()
	FObservedObjectsChangedDelegate OnObservedObjectsChanged;

	/**
	 * The map tracks how often the Object is present in the map.
	 */
	UPROPERTY(VisibleAnywhere)
	TMap<TSoftObjectPtr<UObject>, int32> ObservedObjects;

	FGameplayTagContainer PreChangeTagContainer;

	UPROPERTY(VisibleAnywhere)
	TMap<FGameplayTag, uint32> NodeHashes;

	UPROPERTY(VisibleAnywhere)
	uint32 CollectionHash;
	
	UPROPERTY()
	bool bInitialized;

	void AddObject(const TSoftObjectPtr<UObject>& Object);
	void RemoveObject(const TSoftObjectPtr<UObject>& Object);

	void InitializeObserver(const FDefaultObserverConfig& DefaultConfig);
	void DeinitializeObserver() const;
	bool Validate();

	void PreEditChange();
	void OnTagChanges();
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent);
	void LoadHashesForTags();
	void UpdateCollectionHash();

	TArray<TSoftObjectPtr<UObject>> FindObservedObjects() const;
	void LoadObservedObjects();

	void OnCallbackUpdateObservedObjects(
		EBroadcastType BroadcastType,
		const TSoftObjectPtr<UObject>& UpdatedObject, const FGameplayTag& UpdatedTag, uint32 NodeHash);
	template <class T>
	TArray<TSoftObjectPtr<T>> GetObservedObjects() const;
};

template <class T>
TArray<TSoftObjectPtr<T>> FAssetTagObserver::GetObservedObjects() const
{
	static_assert(std::is_base_of_v<UObject, T>, "T is not a UObject!");
	const UObject* TemplateValidationObject = NewObject<T>(UObject::StaticClass());
	if(!TemplateValidationObject->GetClass()->IsChildOf(FilteredClass))
	{
		return {};
	}
	
	TArray<TSoftObjectPtr<UObject>> RawObjects;
	this->ObservedObjects.GetKeys(RawObjects);
	
	TArray<TSoftObjectPtr<T>> CastedObjects;
	for(const auto ObjectPtr : RawObjects)
	{
		if(ObjectPtr.Get()->GetClass()->IsChildOf(FilteredClass))
		{
			CastedObjects.Add(Cast<T>(ObjectPtr.Get()));
		}
	}

	return CastedObjects;
}
