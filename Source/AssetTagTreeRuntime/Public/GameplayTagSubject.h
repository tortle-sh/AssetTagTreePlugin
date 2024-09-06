// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeSubsystem.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GameplayTagSubject.generated.h"

USTRUCT(BlueprintType)
struct ASSETTAGTREERUNTIME_API FGameplayTagSubject
{
	GENERATED_BODY()

	UPROPERTY()
	UObject* Parent;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<EBroadCastTagStrategy> BroadcastStrategy;
	
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer TagContainer;
	
	FGameplayTagContainer PreChangeTagContainer;

	void InitializeSubject();
	void DeinitializeSubject();
	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent);
	void PreEditChange();

	static bool HaveTagsChanged(FPropertyChangedEvent& PropertyChangedEvent);
};
