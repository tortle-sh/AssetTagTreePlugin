// Copyright 2024 tortle-sh. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagVersionRegistry.generated.h"

UCLASS()
class ASSETTAGTREERUNTIME_API UAssetTagVersionRegistry : public UPrimaryDataAsset
{
	GENERATED_BODY()
	TMap<FGameplayTag, FGuid> TagVersions;

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
