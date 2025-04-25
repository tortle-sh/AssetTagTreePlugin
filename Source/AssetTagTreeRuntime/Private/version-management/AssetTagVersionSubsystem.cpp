// Copyright 2024 tortle-sh. All Rights Reserved


#include "version-management/AssetTagVersionSubsystem.h"

#include "AssetToolsModule.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "Factories/BlueprintFactory.h"
#include "UObject/SavePackage.h"
#include "version-management/AssetTagVersionRegistry.h"

class FContentBrowserModule;

void UAssetTagVersionSubsystem::LoadVersionAssets()
{
	// const FAssetToolsModule &AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
	// const FContentBrowserModule &ContentBrowserModule = FModuleManager::Get().LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	// IAssetRegistry &AssetRegistry = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>("AssetRegistry").Get();
	//
	// FString Name, PackageName;
	// AssetToolsModule.Get().CreateUniqueAssetName(TEXT("/Game/Generated/AssetTagVersionRegistry"), TEXT(""), PackageName, Name);
	// const FString PackagePath = FPackageName::GetLongPackagePath(PackageName);
	//
	// UPackage* Package = CreatePackage(*PackageName);
	// UBlueprintFactory* MyFactory = NewObject<UBlueprintFactory>(UBlueprintFactory::StaticClass());
	// UObject* CreatedObject = AssetToolsModule.Get().CreateAsset(Name, PackagePath, UAssetTagVersionRegistry::StaticClass(), MyFactory);
	//
	// FSavePackageArgs SaveArgs;
	// {
	// 	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	// 	SaveArgs.SaveFlags = SAVE_NoError;
	// }
	// const FString FileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());
	//
	// UPackage::Save(Package, CreatedObject, *FileName, SaveArgs);
	//
	// AssetRegistry.AssetCreated(CreatedObject);
	//
	// TArray<UObject*> Objects;
	// Objects.Add(CreatedObject);
	// ContentBrowserModule.Get().SyncBrowserToAssets(Objects);
}
