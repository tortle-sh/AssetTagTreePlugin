// Copyright 2024 tortle-sh. All Rights Reserved

#include "version-management/AssetTagVersionRegistry.h"

#include "AssetTagTreeConstants.h"

FPrimaryAssetId UAssetTagVersionRegistry::GetPrimaryAssetId() const
{
	return FPrimaryAssetId(AssetTagVersionRegistryType, *GetName());
}
