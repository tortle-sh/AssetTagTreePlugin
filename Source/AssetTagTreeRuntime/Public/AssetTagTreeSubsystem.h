// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once
#include "CoreMinimal.h"
#include "AssetTagTreeConstants.h"
#include "AssetTagTreeNode.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"
#include "AssetTagTreeSubsystem.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(AssetTagTree);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(AssetTagTree_A_1)

UENUM(BlueprintType, meta=(Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum ETagCollectionFlag : uint8
{
	Self = 0 UMETA(Hidden),
	Children = 1 << 0,
	Parents = 1 << 1,
	RootNode = 1 << 2,
};
ENUM_CLASS_FLAGS(ETagCollectionFlag)


UCLASS()
class ASSETTAGTREERUNTIME_API UAssetTagTreeSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UAssetTagTreeNode*> RootNodes;

	FGameplayTagContainer RootTags;

	FGameplayTagContainer FilterTags(const FGameplayTagContainer &Tags) const;

	void AddMissingRootNodes(const FGameplayTagContainer& Tags);

public:
	TSet<TSoftObjectPtr<UObject>> FindObjects(const FGameplayTagContainer& Tags,
	 	UPARAM(meta = (BitMask, BitMaskEnum = "/Script/AssetTagTreeRuntime.ETagCollectionFlag")) const int32 CollectionFlags) const;

	UFUNCTION(BlueprintCallable)
	void RegisterCallbackOnNodes(const FCallbackDelegate& CallbackDelegate, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable)
	void RemoveCallbackFromNodes(const FCallbackDelegate& CallbackDelegate, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable)
	void AddTagsToNodeTree(const FGameplayTagContainer &Tags);

	UFUNCTION(BlueprintCallable)
	void InsertObjectToTags(UObject* InsertedObject, const FGameplayTagContainer &Tags);

	UFUNCTION(BlueprintCallable)
	void RemoveObjectFromTags(UObject* RemovedObject, const FGameplayTagContainer &Tags);
	void CollectTags(const FGameplayTagContainer& TargetTags,
		UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/AssetTagTreeRuntime.ETagCollectionFlag")) const int32 TagCollectionFlags,
		
		FGameplayTagContainer& CollectedTags);

	UFUNCTION(BlueprintCallable)
	void NotifySubscribers(const FGameplayTagContainer& TargetTags,
		UPARAM(meta=(Bitmask, BitmaskEnum = "/Script/AssetTagTreeRuntime.ETagCollectionFlag")) const int32 TagCollectionFlags);
};
