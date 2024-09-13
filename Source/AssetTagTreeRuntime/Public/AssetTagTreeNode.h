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

	UPROPERTY()
	FSubTreeUpdatedDelegate OnSubTreeUpdated;

	UPROPERTY(EditAnywhere)
	FGameplayTag NodeTag;
	
	UPROPERTY(EditAnywhere)
	bool bIsRootNode = false;

	UPROPERTY(VisibleAnywhere, meta=(AssetBundles = TreeBundle))
	TArray<TWeakObjectPtr<UAssetTagTreeNode>> Children;

	UPROPERTY(VisibleAnywhere)
	TArray<TWeakObjectPtr<UObject>> Objects;

	UPROPERTY()
	UPackage* RootPackage;

	UPROPERTY()
	FString RootPackageName;

	UPROPERTY()
	FGameplayTag ParentTag;

	void SetParentTag(FGameplayTag NewParentTag);
	void CreateMissingChildren(const FGameplayTagContainer& Tags);
	bool RemoveNode(const FGameplayTag& Tag);
	void BroadcastUpdate();
	FSubTreeUpdatedDelegate &GetTreeUpdateDelegate() { return this->OnSubTreeUpdated; }

public:
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
	UFUNCTION()
	FGameplayTag GetNextTag(const FGameplayTag &SearchedTag);
	
	UFUNCTION()
	void SetTag(const FGameplayTag &Tag);
	
	UFUNCTION()
	bool IsLeaf() const;

	UFUNCTION(BlueprintCallable)
	void PrintTree();

	UFUNCTION(BlueprintCallable)
	TArray<UAssetTagTreeNode*> FindNodesByTags(const FGameplayTagContainer &Tags);
	void SetRootPackageName(const FString& PackageName);

	static FString BuildPackageObjectName(const FGameplayTag& NextTag);

	UFUNCTION()
	TArray<TWeakObjectPtr<UObject>> FindObjectsByTag(const FGameplayTag& Tag) const;

	UFUNCTION()
	TArray<TWeakObjectPtr<UObject>> FindObjectsByTags(const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable)
	FGameplayTag GetTag() const;

	UFUNCTION()
	FSubTreeUpdatedDelegate &GetOnSubTreeUpdatedDelegate();

	UFUNCTION()
	bool InsertToTag(UObject* NewAssetTagObject, const FGameplayTag& Tag);

	UFUNCTION()
	bool InsertToTags(UObject* NewAssetTagObject, const FGameplayTagContainer& Tags);

	UFUNCTION()
	bool RemoveObjectFromTag(UObject* OldAssetTagObject, const FGameplayTag& Tag);

	UFUNCTION()
	bool RemoveObjectFromTags(UObject* OldAssetTagObject, const FGameplayTagContainer& Tags);

	UFUNCTION()
	void CollectChildTagsOfTargetTags(const FGameplayTagContainer &TargetTags, FGameplayTagContainer &ChildTags);

	UFUNCTION()
	void CollectChildTags(FGameplayTagContainer &ChildTags);

	UFUNCTION()
	void BroadcastUpdates(const FGameplayTagContainer &Tags);

	UFUNCTION()
	void Persist();

	UFUNCTION()
	void PersistOnTags(const FGameplayTagContainer& Tags);

	UFUNCTION()
	void PersistAll();

	UFUNCTION()
	bool IsRootNode() const;

	UFUNCTION()
	void InitializeRootNode();

	UFUNCTION()
	void SetRootPackage(UPackage* Package);

	UFUNCTION()
	void SetIsRootNode(bool value);

	UFUNCTION()
	void InsertChildNode(UAssetTagTreeNode* Node);

	UFUNCTION()
	bool IsAncestorOf(UAssetTagTreeNode* Node) const;

	const FGameplayTag &GetParentTag() const;
	UFUNCTION()
	bool IsParentOf(UAssetTagTreeNode* Node) const;
};
