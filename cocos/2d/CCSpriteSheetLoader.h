/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Jason Booth
Copyright (c) 2009      Robert J Payne
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#pragma once

#include <set>
#include <unordered_map>
#include <string>
#include "2d/CCSpriteFrame.h"
#include "base/CCRef.h"
#include "base/CCValue.h"
#include "base/CCMap.h"
#include "base/CCData.h"

NS_CC_BEGIN

class Sprite;
class Texture2D;
class PolygonInfo;
class SpriteFrameCache;

/**
 * @addtogroup _2d
 * @{
 */

class SpriteSheet
{
public:
    std::string path;
    std::string format;
    std::set<std::string> frames;
    bool full = false;
};

class ISpriteSheetLoader
{
public:
    virtual ~ISpriteSheetLoader() = default;
    virtual void Load(const std::string& filePath, SpriteFrameCache& cache) = 0;
    virtual void Load(const std::string& filePath, Texture2D* texture, SpriteFrameCache& cache) = 0;
    virtual void Load(const std::string& filePath, const std::string& textureFileName, SpriteFrameCache& cache) = 0;
    virtual void Load(const Data& content, Texture2D* texture, SpriteFrameCache& cache) = 0;
    virtual void Reload(const std::string& filePath, SpriteFrameCache& cache) = 0;
};

class SpriteSheetLoader : public ISpriteSheetLoader
{
public:
    /** Configures PolygonInfo class with the passed sizes + triangles */
    void initializePolygonInfo(const Size& textureSize,
        const Size& spriteSize,
        const std::vector<int>& vertices,
        const std::vector<int>& verticesUV,
        const std::vector<int>& triangleIndices,
        PolygonInfo& polygonInfo);

    void Load(const std::string& filePath, SpriteFrameCache& cache) override = 0;
    void Load(const std::string& filePath, Texture2D* texture, SpriteFrameCache& cache) override = 0;
    void Load(const std::string& filePath, const std::string& textureFileName, SpriteFrameCache& cache) override = 0;
    void Load(const Data& content, Texture2D* texture, SpriteFrameCache& cache) override = 0;
    void Reload(const std::string& filePath, SpriteFrameCache& cache) override = 0;
};

// end of _2d group
/// @}

NS_CC_END

