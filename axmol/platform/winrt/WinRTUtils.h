/****************************************************************************
Copyright (c) 2010 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/platform/PlatformMacros.h"

#include <string>

#include <winrt/Windows.Foundation.h>
using namespace winrt;

namespace ax
{

bool isWindowsPhone();

Windows::Foundation::IInspectable findXamlElement(Windows::Foundation::IInspectable const& parent,
                                                  const winrt::hstring& name);
bool removeXamlElement(Windows::Foundation::IInspectable const& parent,
                       Windows::Foundation::IInspectable const& element);
bool replaceXamlElement(Windows::Foundation::IInspectable const& parent,
                        Windows::Foundation::IInspectable const& add,
                        Windows::Foundation::IInspectable const& remove);

std::string PlatformStringToString(const winrt::hstring& s);
winrt::hstring PlatformStringFromString(std::string_view s);

void AX_DLL printIPAddresses();

std::string AX_DLL getDeviceIPAddresses();

std::string computeHashForFile(const std::string& filePath);

// creates a  cache file path corresponding to given source file.
// srcFilePath - source file.
// cacheFilePath - cache file path to be used to save cache.
// return true if file already exists
bool createMappedCacheFile(/*In*/ const std::string& srcFilePath,
                           /*Out*/ std::string& cacheFilePath,
                           /*Optional*/ const std::string& ext = "");
void destroyMappedCacheFile(const std::string& key);

}  // namespace ax
