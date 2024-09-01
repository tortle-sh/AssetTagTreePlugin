// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "AssetTagTreeContainerSubject.generated.h"

UCLASS(BlueprintType)
class ASSETTAGTREERUNTIME_API UAssetTagTreeContainerSubject : public UObject
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category=Information)
	FGameplayTagContainer TagContainer;

	UPROPERTY()
	FGameplayTagContainer PreChangeTagContainer;
	
	void InitializeSubject();
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PreEditChange(FProperty* PropertyAboutToChange) override;
};
