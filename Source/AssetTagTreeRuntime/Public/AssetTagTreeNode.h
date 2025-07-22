// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagTreeNode.generated.h"

UENUM()
enum EBroadcastType
{
	Inserted,
	Removed
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
	FSubTreeUpdatedDelegate,
	EBroadcastType, BroadcastType,
	TSoftObjectPtr<UObject>, ChangedObject,
	const FGameplayTag&, ChangedTag,
	uint32,  TagHash);

DECLARE_DYNAMIC_DELEGATE_FourParams(
	FCallbackDelegate,
	const EBroadcastType, BroadcastType,
	const TSoftObjectPtr<UObject>&, ChangedObject,
	const FGameplayTag&, ChangedTag,
	uint32,  TagHash);

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

	TSet<FGuid> SubjectIds;

	uint32 NodeHash;

	void CreateMissingChildren(const FGameplayTagContainer& Tags);
	void UpdateNodeHash();

public:
	UFUNCTION()
	void SetTag(const FGameplayTag &Tag);

	TArray<TSoftObjectPtr<UObject>> FindObjectsByTags(const FGameplayTagContainer& Tags, int32 CollectObjectsFrom,
	                                                  bool bIsRootTag = false);
	TArray<TSoftObjectPtr<UObject>> FindObjectsFromChildren(const bool bIsFirst = false);

	TArray<UAssetTagTreeNode*> FindAllNodesByTags(const FGameplayTagContainer &Tags);

	UFUNCTION(BlueprintCallable)
	void InsertToTags(UObject* NewAssetTagObject, const FGameplayTagContainer& Tags, const FGuid& SubjectId);
	
	void RemoveObjectFromTags(UObject* OldAssetTagObject, const FGameplayTagContainer& Tags, const FGuid& SubjectId);

	void CollectChildTagsOfTargetTags(const FGameplayTagContainer &TargetTags, FGameplayTagContainer &ChildTags);

	void CollectChildTags(FGameplayTagContainer &ChildTags);

	void BroadcastUpdate(EBroadcastType BroadcastType, const TSoftObjectPtr<UObject>& ChangedObject) const;

	void BroadcastUpdates(const FGameplayTagContainer& Tags, EBroadcastType BroadcastType, const TSoftObjectPtr<UObject>& ChangedObject);

	void BroadcastUpdatesToChildren(EBroadcastType BroadcastType, const TSoftObjectPtr<UObject>& ChangedObject);

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

	[[nodiscard]] uint32 GetNodeHash() const
	{
		return NodeHash;
	}
};
