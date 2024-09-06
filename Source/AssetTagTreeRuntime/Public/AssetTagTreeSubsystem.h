// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeNode.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"
#include "AssetTagTreeSubsystem.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(AssetTagTree);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(AssetTagTree_A_1)

UENUM()
enum EBroadCastTagStrategy : uint8
{
	TAG_ONLY,
	TAG_AND_CHILDREN,
	TAG_AND_PARENTS,
	TAG_CHILDREN_AND_PARENT
};

UCLASS()
class ASSETTAGTREERUNTIME_API UAssetTagTreeSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	UAssetTagTreeNode *RootNode;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintCallable)
	void InsertObjectToTag(UObject* InsertedObject, const FGameplayTag &Tag);

	UFUNCTION(BlueprintCallable)
	void InsertObjectToTags(UObject* InsertedObject, const FGameplayTagContainer &Tags);

	UFUNCTION(BlueprintCallable)
	void RemoveObjectFromTag(UObject* RemovedObject, const FGameplayTag &Tag);

	UFUNCTION(BlueprintCallable)
	void RemoveObjectFromTags(UObject* RemovedObject, const FGameplayTagContainer &Tags);

	UFUNCTION(BlueprintCallable)
	void NotifySubscribers(FGameplayTagContainer &TargetTags, EBroadCastTagStrategy TagCollectionStrategy);

};
