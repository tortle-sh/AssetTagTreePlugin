// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeSubsystem.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagSubject.generated.h"

struct FDefaultSubjectConfig
{
	UObject* Subject;
	ETagCollectionFlag BroadcastStrategy;
};

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
	bool bInitialized;

	void InitializeSubject(const FDefaultSubjectConfig& DefaultConfig);
	void DeinitializeSubject() const;
	void PostEditChangeProperty(const FPropertyChangedEvent& PropertyChangedEvent) const;
	void PreEditChange();

	static bool HaveTagsChanged(const FPropertyChangedEvent& PropertyChangedEvent);
};
