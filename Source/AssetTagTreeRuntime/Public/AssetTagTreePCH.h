#pragma once

#include "AssetTagTreeNode.h"
#include "AssetTagTreeContainerObserver.h"
#include "AssetTagTreeContainerSubject.h"

#include "AssetTagTreeRuntime.h"

#define LOG_INFO(FMT, ...) UE_LOG(AssetTagTreeRuntime, Display, TEXT(FMT), ##__VA_ARGS__)
#define LOG_WARNING(FMT, ...) UE_LOG(AssetTagTreeRuntime, Warning, TEXT(FMT), ##__VA_ARGS__)
#define LOG_ERROR(FMT, ...) UE_LOG(AssetTagTreeRuntime, Error, TEXT(FMT), ##__VA_ARGS__)