// Copyright (c) 2024 tortle-sh All Rights Reserved
#pragma once

#include "AssetTagTreeConstants.h"
#include "AssetTagTreeNode.h"
#include "AssetTagObserver.h"
#include "AssetTagSubject.h"

#include "AssetTagTreeRuntime.h"

#define LOG_INFO(FMT, ...) UE_LOG(AssetTagTreeRuntime, Display, TEXT(FMT), ##__VA_ARGS__)
#define LOG_WARNING(FMT, ...) UE_LOG(AssetTagTreeRuntime, Warning, TEXT(FMT), ##__VA_ARGS__)
#define LOG_ERROR(FMT, ...) UE_LOG(AssetTagTreeRuntime, Error, TEXT(FMT), ##__VA_ARGS__)