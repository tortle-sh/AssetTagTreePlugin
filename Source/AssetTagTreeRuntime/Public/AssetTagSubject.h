// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagSubject.generated.h"

USTRUCT(BlueprintType)
struct ASSETTAGTREERUNTIME_API FAssetTagSubject
{
	GENERATED_BODY()

	UPROPERTY()
	UObject* ParentObject;

	UPROPERTY(EditAnywhere, meta=(Bitmask, BitmaskEnum = "/Script/AssetTagTreeRuntime.ETagCollectionFlag"))
	uint8 BroadcastTo;
	
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer TagContainer;
	
	FGameplayTagContainer PreChangeTagContainer;
	
	UPROPERTY(VisibleAnywhere)
	FGuid SubjectId;

	UPROPERTY()
	bool Initialized;

	void InitializeSubject( UObject* Subject, uint8 BroadcastStrategy);
	void DeinitializeSubject() const;
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent) const;
	void PreEditChange();

	static bool HaveTagsChanged(const FPropertyChangedEvent& PropertyChangedEvent);
};
