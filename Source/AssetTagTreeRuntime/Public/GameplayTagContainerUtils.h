// Fill out your copyright notice in the Description page of Project Settings.

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

