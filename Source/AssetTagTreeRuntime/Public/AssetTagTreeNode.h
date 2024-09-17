// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeConstants.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagTreeNode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSubTreeUpdatedDelegate);
DECLARE_DYNAMIC_DELEGATE(FCallbackDelegate);

UCLASS(BlueprintType)
class ASSETTAGTREERUNTIME_API UAssetTagTreeNode : public UObject
{
	GENERATED_BODY()

	UAssetTagTreeNode();
	UAssetTagTreeNode(FGameplayTag Tag);

	UPROPERTY()
	FSubTreeUpdatedDelegate OnSubTreeUpdated;

	UPROPERTY()
	FGameplayTag NodeTag;

	UPROPERTY()
	TArray<UAssetTagTreeNode*> Children;

	UPROPERTY()
	TArray<TSoftObjectPtr<UObject>> Objects;

	UFUNCTION()
	void SetTag(const FGameplayTag &Tag);
	
	void CreateMissingChildren(const FGameplayTagContainer& Tags);
	
public:
	UFUNCTION(BlueprintCallable)
	bool IsLeaf() const;

	UFUNCTION(BlueprintCallable)
	TArray<TSoftObjectPtr<UObject>> FindAllObjectsByTags(const FGameplayTagContainer &Tags);

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
