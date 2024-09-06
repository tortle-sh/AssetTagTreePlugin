// Copyright (c) 2024 tortle-sh All Rights Reserved

#pragma once

// #include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "AssetTagTreeRuntimeModule.h"

class FAssetTagTreeRuntimeModule : public IAssetTagTreeRuntime
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
