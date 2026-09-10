/****************************************************************************
Copyright (c) 2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once
#include <string>

#include "axmol/platform/PlatformMacros.h"

namespace ax
{

class AudioDecoder;

class AudioDecoderManager
{
public:
    static bool init();
    static void destroy();
    static AudioDecoder* createDecoder(std::string_view path);
    static void destroyDecoder(AudioDecoder* decoder);
};

}  // namespace ax
