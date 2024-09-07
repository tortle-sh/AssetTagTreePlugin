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

DECLARE_DYNAMIC_DELEGATE(FTagTreeUpdateCallbackDelegate);

UCLASS()
class ASSETTAGTREERUNTIME_API UAssetTagTreeSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	UAssetTagTreeNode *RootNode;

	static FGameplayTagContainer FilterTags(const FGameplayTagContainer &Tags);

public:
	inline static const FGameplayTag ASTT_ROOT_TAG = FGameplayTag::RequestGameplayTag("AssetTagTree");
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable)
	void RegisterCallbackOnNodes(const FTagTreeUpdateCallbackDelegate& CallbackDelegate, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable)
	void RemoveCallbackFromNodes(const FTagTreeUpdateCallbackDelegate& CallbackDelegate, const FGameplayTagContainer& Tags);

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
