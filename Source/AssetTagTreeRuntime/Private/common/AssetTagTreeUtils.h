// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeNode.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AssetTagTreeUtils.generated.h"

USTRUCT()
struct FGameplayTagContainerChangeData
{
	GENERATED_BODY()

 	FGameplayTagContainer InsertedTags;
 	FGameplayTagContainer RemovedTags;
};

UCLASS()
class ASSETTAGTREERUNTIME_API UAssetTagTreeUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION()
	static FGameplayTagContainerChangeData GetTagContainerChangeData(
		const FGameplayTagContainer& PreUpdateContainer,
		const FGameplayTagContainer& PostUpdateContainer);

	UFUNCTION()
	static void CollectParentTags(const FGameplayTagContainer &TargetTags, FGameplayTagContainer &ParentTags);
	UFUNCTION()
	static FString BroadcastTypeToString(const EBroadcastType Broadcast);
};

