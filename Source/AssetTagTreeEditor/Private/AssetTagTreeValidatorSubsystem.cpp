// Fill out your copyright notice in the Description page of Project Settings.


#include "AssetTagTreeValidatorSubsystem.h"

void UAssetTagTreeValidatorSubsystem::PostCDOContruct()
{
	Super::PostCDOContruct();
	UE_LOG(LogTemp, Display, TEXT("AssetTagTreeValidator PostCDOConstruct()"));
}

void UAssetTagTreeValidatorSubsystem::PostInitProperties()
{
	Super::PostInitProperties();
	UE_LOG(LogTemp, Display, TEXT("AssetTagTreeValidator PostInitProperties()"));
}

void UAssetTagTreeValidatorSubsystem::PostLoad()
{
	Super::PostLoad();
	UE_LOG(LogTemp, Display, TEXT("AssetTagTreeValidator PostLoad()"));
}
