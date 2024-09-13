// Copyright 2024 tortle-sh. All Rights Reserved


#include "ASTT_AssetManager.h"

#include "AssetTagTreeConstants.h"
#include "AssetTagTreeNode.h"
#include "UObject/SavePackage.h"

void UASTT_AssetManager::OnAssetRegistryFilesLoaded()
{
	Super::OnAssetRegistryFilesLoaded();

	LoadPrimaryAssetsWithType(RootTagType, {},
		FStreamableDelegate::CreateUObject(this, &UASTT_AssetManager::InitializePackages));
}

void UASTT_AssetManager::InitializePackages() const
{
	TArray<UObject*> Objects;
	GetPrimaryAssetObjectList(RootTagType, Objects);

	TArray<UAssetTagTreeNode*> RootNodes;
	TArray<UAssetTagTreeNode*> ChildNodes;

	for(auto Object : Objects)
	{
		if(UAssetTagTreeNode* Node = Cast<UAssetTagTreeNode>(Object))
		{
			(Node->IsRootNode() ? RootNodes : ChildNodes).Add(Node);
		}
	}

	for(auto RootNode : RootNodes)
	{
		FString PackageName = PackagePath + "/" + RootNode->GetName() + "Package";
		UPackage* Package = CreatePackage(*PackageName);
		Package->FullyLoad();
		
		RootNode->SetRootPackage(Package);
		RootNode->SetRootPackageName(PackageName);
		RootNode->InitializeRootNode();
	}

	for(auto ChildNode : ChildNodes)
	{
		for(auto RootNode : RootNodes)
		{
			RootNode->InsertChildNode(ChildNode);
			break;
		}
	}
	

	// for(auto Object : Objects)
	// {
	// 	if(UAssetTagTreeNode* RootNode = Cast<UAssetTagTreeNode>(Object))
	// 	{
	// 		
	// 		if(!RootNode->IsRootNode())
	// 		{
	// 			continue;
	// 		}
	// 		
	// 		FString PackageName = PackagePath + "/" + RootNode->GetName() + "Package";
	// 		UPackage* Package = CreatePackage(*PackageName);
	// 		Package->FullyLoad();
	// 		
	// 		UPackage* RootPackage = RootNode->GetPackage();
	// 		if(RootPackage)
	// 		{
	// 			RootPackage->FullyLoad();
	// 		}
	//
	// 		RootNode->SetRootPackage(Package);
	// 		RootNode->SetRootPackageName(PackageName);
	// 		RootNode->InitializeRootNode();
	// 	}
	// }
}
