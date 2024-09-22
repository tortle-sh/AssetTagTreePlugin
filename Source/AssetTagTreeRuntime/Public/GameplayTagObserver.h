// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeConstants.h"
#include "AssetTagTreeNode.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GameplayTagObserver.generated.h"

USTRUCT()
struct ASSETTAGTREERUNTIME_API FGameplayTagObserver
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(Bitmask, BitmaskEnum = "/Script/AssetTagTreeRuntime.ETagCollectionFlag"))
	int32 CollectObjectsFrom;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer TagContainer;
	
	UPROPERTY()
	FCallbackDelegate Callback;

	FGameplayTagContainer PreChangeTagContainer;

	void InitializeObserver() const;
	void DeinitializeObserver() const;

	void PreEditChange();
	void OnTagChanges() const;
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent) const;

	TSet<TSoftObjectPtr<UObject>> FindObservedObjects() const;
};

