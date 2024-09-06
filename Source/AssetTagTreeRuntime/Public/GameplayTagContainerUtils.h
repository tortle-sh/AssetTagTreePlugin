// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainerUtils.generated.h"

USTRUCT()
struct FGameplayTagContainerChangeData
{
	GENERATED_BODY()

 	FGameplayTagContainer InsertedTags;
 	FGameplayTagContainer RemovedTags;
	FGameplayTagContainer BroadcastTags;
};

UCLASS()
class ASSETTAGTREERUNTIME_API UGameplayTagContainerUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION()
	static FGameplayTagContainerChangeData GetChangeData(
		const FGameplayTagContainer& PreUpdateContainer,
		const FGameplayTagContainer& PostUpdateContainer);

	UFUNCTION()
	static void CollectParentTags(const FGameplayTagContainer &TargetTags, FGameplayTagContainer &ParentTags);
};

