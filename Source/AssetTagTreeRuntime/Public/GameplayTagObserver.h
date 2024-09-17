// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeNode.h"
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
	FCallbackDelegate Callback;

	FGameplayTagContainer PreChangeTagContainer;

	void InitializeObserver();
	void DeinitalizeObserver();

	bool HaveTagsChanged(FPropertyChangedEvent& PropertyChangedEvent);
	void PreEditChange();
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);

	TArray<TSoftObjectPtr<UObject>> FindObservedObjects() const;
};

