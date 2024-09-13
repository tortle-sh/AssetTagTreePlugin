// Copyright 2024 tortle-sh. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "UObject/Object.h"
#include "ASTT_AssetManager.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTAGTREERUNTIME_API UASTT_AssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	virtual void OnAssetRegistryFilesLoaded() override;

	void InitializePackages() const;
};
