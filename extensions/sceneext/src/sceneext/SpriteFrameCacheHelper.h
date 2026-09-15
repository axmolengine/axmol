/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

#include "axmol/platform/PlatformMacros.h"
#include "sceneext/ArmatureDefine.h"
#include "sceneext/SceneExtMacros.h"
#include <string>
#include <map>
#include <vector>

namespace ax
{
class SpriteFrame;
}

namespace ax::ext
{

/**
 *  @lua NA
 */
class SCNEXT_API SpriteFrameCacheHelper
{
public:
    static SpriteFrameCacheHelper* getInstance();

    static void destroyInstance();

public:
    /**
     *    @brief    Add sprite frame to CCSpriteFrameCache, it will save display name and it's relative image name
     */
    void addSpriteFrameFromFile(std::string_view plistPath, std::string_view imagePath);
    void removeSpriteFrameFromFile(std::string_view plistPath);

private:
    void retainSpriteFrames(std::string_view plistPath);
    void releaseSpriteFrames(std::string_view plistPath);

    SpriteFrameCacheHelper();
    ~SpriteFrameCacheHelper();

    tlx::string_map<std::vector<ax::SpriteFrame*>> _usingSpriteFrames;
    static SpriteFrameCacheHelper* _spriteFrameCacheHelper;
};

}  // namespace ax::ext
