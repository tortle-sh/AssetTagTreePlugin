// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeNode.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"
#include "AssetTagTreeSubsystem.generated.h"

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
	TArray<UAssetTagTreeNode*> RootNodes;

	UPROPERTY()
	FGameplayTagContainer RootTags;

	FGameplayTagContainer FilterTags(const FGameplayTagContainer &Tags);
	const FGameplayTagContainer& GetRootTags() const;
	void FindNodesWithTags(const FGameplayTagContainer& Tags, TArray<UAssetTagTreeNode*>& Nodes);
	
public:
	UFUNCTION(BlueprintCallable)
	void RegisterCallbackOnNodes(const FTagTreeUpdateCallbackDelegate& CallbackDelegate, const FGameplayTagContainer& Tags);

	UFUNCTION(BlueprintCallable)
	void RemoveCallbackFromNodes(const FTagTreeUpdateCallbackDelegate& CallbackDelegate, const FGameplayTagContainer& Tags);

	UFUNCTION()
	void RegisterRootNode(UAssetTagTreeNode* RootNode);

	UFUNCTION()
	void RemoveRootNode(UAssetTagTreeNode* RootNode);

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

	UFUNCTION()
	static void PersistChanges(UAssetTagTreeNode* RootNode);
};
