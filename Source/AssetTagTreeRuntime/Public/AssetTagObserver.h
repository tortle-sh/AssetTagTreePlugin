// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeConstants.h"
#include "AssetTagTreeNode.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagObserver.generated.h"

DECLARE_DYNAMIC_DELEGATE(FObservedObjectsChangedDelegate);

USTRUCT()
struct ASSETTAGTREERUNTIME_API FAssetTagObserver
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(Bitmask, BitmaskEnum = "/Script/AssetTagTreeRuntime.ETagCollectionFlag"))
	uint8 CollectObjectsFrom;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UObject> FilteredClass = UObject::StaticClass();

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

	void AddObject(const TSoftObjectPtr<UObject>& Object);
	void RemoveObject(const TSoftObjectPtr<UObject>& Object);

	void InitializeObserver() const;
	void DeinitializeObserver() const;

	void PreEditChange();
	void OnTagChanges();
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent);

	TArray<TSoftObjectPtr<UObject>> FindObservedObjects() const;
	void LoadObservedObjects();

	void OnCallbackUpdateObservedObjects(const EBroadcastType BroadcastType,
	                                     const TSoftObjectPtr<UObject>& UpdatedObject);

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
