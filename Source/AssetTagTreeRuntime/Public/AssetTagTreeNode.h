// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagTreeNode.generated.h"

class UCollectTagsAndChildrenStrategy;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSubTreeUpdatedDelegate);

UCLASS(BlueprintType)
class ASSETTAGTREERUNTIME_API UAssetTagTreeNode : public UObject
{
	GENERATED_BODY()

	friend UCollectTagsAndChildrenStrategy;

	UAssetTagTreeNode();
	UAssetTagTreeNode(FGameplayTag Tag);

	UPROPERTY()
	FSubTreeUpdatedDelegate OnSubTreeUpdated;

	UPROPERTY()
	FGameplayTag NodeTag;

	UPROPERTY()
	TArray<UAssetTagTreeNode*> Children;

	UPROPERTY()
	TWeakObjectPtr<UAssetTagTreeNode> Parent;

	UPROPERTY()
	TArray<UObject*> Objects;

	UFUNCTION()
	void SetTag(const FGameplayTag &Tag);
	
public:
	UFUNCTION(BlueprintCallable)
	bool IsLeaf() const;

	UFUNCTION(BlueprintCallable)
	TArray<UObject*> FindObjectsByTag(const FGameplayTag &Tag) const;

	UFUNCTION(BlueprintCallable)
	TArray<UObject*> FindAllObjectsByTags(const FGameplayTagContainer &Tags);

	UFUNCTION(BlueprintCallable)
	void InsertToTag(UObject* NewAssetTagObject, const FGameplayTag &Tag);

	UFUNCTION(BlueprintCallable)
	void InsertToTags(UObject* NewAssetTagObject, const FGameplayTagContainer &Tags);

	UFUNCTION(BlueprintCallable)
	void RemoveObjectFromTag(UObject* OldAssetTagObject, const FGameplayTag &Tag);

	UFUNCTION(BlueprintCallable)
	void RemoveObjectFromTags(UObject* OldAssetTagObject, const FGameplayTagContainer &Tags);

	UFUNCTION(BlueprintCallable)
	void RemoveNode(const FGameplayTag &Tag);

	UFUNCTION()
	void CollectChildTagsOfTargetTags(const FGameplayTagContainer &TargetTags, FGameplayTagContainer &ChildTags);

	UFUNCTION()
	void CollectChildTags(FGameplayTagContainer &ChildTags);

	UFUNCTION()
	void BroadcastUpdate() const;

	UFUNCTION()
	void BroadcastUpdates(const FGameplayTagContainer &Tags);

	UFUNCTION()
	void BroadcastUpdatesToChildren();

	UFUNCTION()
	FSubTreeUpdatedDelegate &GetTreeUpdateDelegate() { return this->OnSubTreeUpdated; }

	UFUNCTION()
	void PrintTree();

	UFUNCTION()
	FGameplayTag GetTag() const;

	UFUNCTION()
	FGameplayTag GetNextTag(const FGameplayTag &SearchedTag);
};
