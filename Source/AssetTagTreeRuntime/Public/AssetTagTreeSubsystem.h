// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AssetTagTreeNode.h"
#include "NativeGameplayTags.h"
#include "UObject/Object.h"
#include "AssetTagTreeSubsystem.generated.h"

UE_DECLARE_GAMEPLAY_TAG_EXTERN(AssetTagTree);
UE_DECLARE_GAMEPLAY_TAG_EXTERN(AssetTagTree_A_1)

UENUM()
enum EBroadCastTagStrategy
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
	
	UFUNCTION()
	void InsertObjectToTag(UObject* InsertedObject, const FGameplayTag &Tag);

	UFUNCTION()
	void RemoveObjectFromTag(UObject* RemovedObject, const FGameplayTag &Tag);

	UFUNCTION()
	void NotifySubscribers(FGameplayTagContainer &TargetTags, EBroadCastTagStrategy TagCollectionStrategy);

};
