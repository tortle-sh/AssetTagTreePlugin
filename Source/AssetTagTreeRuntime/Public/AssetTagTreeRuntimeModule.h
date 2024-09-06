// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(AssetTagTreeRuntime, Log, All);

class IAssetTagTreeRuntime : public IModuleInterface
{
	static IAssetTagTreeRuntime& Get()
	{
		return FModuleManager::LoadModuleChecked<IAssetTagTreeRuntime>("AssetTagTreeRuntime");
	}

	static bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("AssetTagTreeRuntime");
	}
};