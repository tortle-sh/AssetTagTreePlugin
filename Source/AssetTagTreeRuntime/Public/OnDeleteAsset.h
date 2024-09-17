// Copyright 2024 tortle-sh. All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "OnDeleteAsset.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UOnDeleteAsset : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ASSETTAGTREERUNTIME_API IOnDeleteAsset
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void OnDeleteAsset();
};
