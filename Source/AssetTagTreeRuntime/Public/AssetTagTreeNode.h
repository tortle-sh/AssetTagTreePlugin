// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagTreeNode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSubTreeUpdatedDelegate);
DECLARE_DYNAMIC_DELEGATE(FCallbackDelegate);

UCLASS(BlueprintType)
class ASSETTAGTREERUNTIME_API UAssetTagTreeNode : public UObject
{
	GENERATED_BODY()

	UPROPERTY()
	FSubTreeUpdatedDelegate OnSubTreeUpdated;

	UPROPERTY()
	FGameplayTag NodeTag;

	UPROPERTY()
	TArray<UAssetTagTreeNode*> ChildNodes;

	UPROPERTY()
	TArray<TSoftObjectPtr<UObject>> Objects;

	void CreateMissingChildren(const FGameplayTagContainer& Tags);
	
public:
	UFUNCTION()
	void SetTag(const FGameplayTag &Tag);

	TSet<TSoftObjectPtr<UObject>> FindObjectsByTags(const FGameplayTagContainer& Tags, int32 CollectObjectsFrom, bool bIsRootTag = false);
	TSet<TSoftObjectPtr<UObject>> FindObjectsFromChildren(const bool bIsFirst = false);

	TArray<UAssetTagTreeNode*> FindAllNodesByTags(const FGameplayTagContainer &Tags);

	UFUNCTION(BlueprintCallable)
	void InsertToTags(UObject* NewAssetTagObject, const FGameplayTagContainer &Tags);
	
	void RemoveObjectFromTags(UObject* OldAssetTagObject, const FGameplayTagContainer &Tags);

	void CollectChildTagsOfTargetTags(const FGameplayTagContainer &TargetTags, FGameplayTagContainer &ChildTags);

	void CollectChildTags(FGameplayTagContainer &ChildTags);

	void BroadcastUpdate() const;

	void BroadcastUpdates(const FGameplayTagContainer &Tags);

	void BroadcastUpdatesToChildren();

	UFUNCTION()
	FSubTreeUpdatedDelegate &GetTreeUpdateDelegate() { return this->OnSubTreeUpdated; }

	void PrintTree();

	FGameplayTag GetTag() const;

	FGameplayTag GetNextTag(const FGameplayTag &SearchedTag) const;

	UFUNCTION()
	FSubTreeUpdatedDelegate &GetOnSubTreeUpdatedDelegate();

	UFUNCTION()
	void RegisterCallbackOnTags(const FCallbackDelegate& CallbackDelegate, const FGameplayTagContainer& Tags);

	UFUNCTION()
	void RemoveCallbackFromTags(const FCallbackDelegate& CallbackDelegate, const FGameplayTagContainer& Tags);
};
