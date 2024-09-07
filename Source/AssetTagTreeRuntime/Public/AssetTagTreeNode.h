// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagTreeNode.generated.h"

class UCollectTagsAndChildrenStrategy;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSubTreeUpdatedDelegate);

UCLASS(BlueprintType)
class ASSETTAGTREERUNTIME_API UAssetTagTreeNode : public UPrimaryDataAsset
{
	GENERATED_BODY()

	friend UCollectTagsAndChildrenStrategy;

	UAssetTagTreeNode();
	UAssetTagTreeNode(FGameplayTag Tag);

	UPROPERTY()
	FSubTreeUpdatedDelegate OnSubTreeUpdated;

	UPROPERTY(EditAnywhere)
	FGameplayTag NodeTag;

	UPROPERTY()
	TArray<UAssetTagTreeNode*> Children;

	UPROPERTY()
	TWeakObjectPtr<UAssetTagTreeNode> Parent;

	UPROPERTY()
	TArray<UObject*> Objects;

	void SetTag(const FGameplayTag &Tag);
	void CreateMissingChildren(const FGameplayTagContainer& Tags);
	void RemoveNode(const FGameplayTag &Tag);
	void BroadcastUpdate() const;
	void BroadcastUpdatesToChildren();
	FSubTreeUpdatedDelegate &GetTreeUpdateDelegate() { return this->OnSubTreeUpdated; }
	FGameplayTag GetNextTag(const FGameplayTag &SearchedTag);

public:
	UFUNCTION()
	bool IsLeaf() const;

	UFUNCTION(BlueprintCallable)
	void PrintTree();

	UFUNCTION(BlueprintCallable)
	TArray<UAssetTagTreeNode*> FindNodesByTags(const FGameplayTagContainer &Tags);
	
	UFUNCTION(BlueprintCallable)
	TArray<UObject*> FindObjectsByTag(const FGameplayTag &Tag) const;

	UFUNCTION(BlueprintCallable)
	TArray<UObject*> FindObjectsByTags(const FGameplayTagContainer &Tags);

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetTag() const;

	UFUNCTION()
	FSubTreeUpdatedDelegate &GetOnSubTreeUpdatedDelegate();

	UFUNCTION()
	void InsertToTag(UObject* NewAssetTagObject, const FGameplayTag &Tag);

	UFUNCTION()
	void InsertToTags(UObject* NewAssetTagObject, const FGameplayTagContainer &Tags);

	UFUNCTION()
	void RemoveObjectFromTag(UObject* OldAssetTagObject, const FGameplayTag &Tag);

	UFUNCTION()
	void RemoveObjectFromTags(UObject* OldAssetTagObject, const FGameplayTagContainer &Tags);

	UFUNCTION()
	void CollectChildTagsOfTargetTags(const FGameplayTagContainer &TargetTags, FGameplayTagContainer &ChildTags);

	UFUNCTION()
	void CollectChildTags(FGameplayTagContainer &ChildTags);

	UFUNCTION()
	void BroadcastUpdates(const FGameplayTagContainer &Tags);
};
