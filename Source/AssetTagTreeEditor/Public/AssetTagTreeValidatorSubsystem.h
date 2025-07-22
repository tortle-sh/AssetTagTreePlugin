// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "AssetTagTreeValidatorSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class ASSETTAGTREEEDITOR_API UAssetTagTreeValidatorSubsystem : public UEditorSubsystem
{
	GENERATED_BODY()

	virtual void PostCDOContruct() override;
	virtual void PostInitProperties() override;
	virtual void PostLoad() override;
};
