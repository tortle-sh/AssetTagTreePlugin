// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeConstants.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GameplayTagSubject.generated.h"

USTRUCT(BlueprintType)
struct ASSETTAGTREERUNTIME_API FGameplayTagSubject
{
	GENERATED_BODY()

	UPROPERTY()
	UObject* Parent;

	UPROPERTY(EditAnywhere, meta=(Bitmask, BitmaskEnum = "/Scripts/AssetTagTreeRuntime.ETagCollectionFlag"))
	int32 BroadcastTo;
	
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer TagContainer;
	
	FGameplayTagContainer PreChangeTagContainer;

	void InitializeSubject() const;
	void DeinitializeSubject() const;
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent) const;
	void PreEditChange();

	static bool HaveTagsChanged(const FPropertyChangedEvent& PropertyChangedEvent);
};
