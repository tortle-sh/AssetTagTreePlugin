// Copyright 2024 tortle-sh. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AssetTagVersionSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTAGTREERUNTIME_API UAssetTagVersionSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static void LoadVersionAssets();
};
