// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeSubsystem.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GameplayTagObserver.generated.h"

USTRUCT()
struct ASSETTAGTREERUNTIME_API FGameplayTagObserver
{
	GENERATED_BODY()

	UPROPERTY()
	UObject* Parent;

	UPROPERTY(EditAnywhere)
	FGameplayTagContainer TagContainer;
	
	UPROPERTY()
	FTagTreeUpdateCallbackDelegate Callback;

	FGameplayTagContainer PreChangeTagContainer;

	void InitializeObserver();
	void DeinitializeObserver();

	bool HaveTagsChanged(FPropertyChangedEvent& PropertyChangedEvent);
	void PreEditChange();
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
};
