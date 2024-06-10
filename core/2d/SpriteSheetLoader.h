/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Jason Booth
Copyright (c) 2009      Robert J Payne
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

https://axmol.dev/

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
#include "2d/SpriteFrame.h"
#include "base/Object.h"
#include "base/Value.h"
#include "base/Map.h"
#include "base/Data.h"

NS_AX_BEGIN

class Sprite;
class Texture2D;
class PolygonInfo;
class SpriteFrameCache;

/**
 * @addtogroup _2d
 * @{
 */

struct SpriteSheetFormat
{
    enum : uint32_t
    {
        PLIST  = 1,
        CUSTOM = 1000
    };
};

class SpriteSheet
{
public:
    std::string path;
    uint32_t format;
    hlookup::string_set frames;
    bool full = false;
};

class ISpriteSheetLoader
{
public:
    virtual ~ISpriteSheetLoader()                                                                           = default;
    virtual uint32_t getFormat()                                                                            = 0;
    virtual void load(std::string_view filePath, SpriteFrameCache& cache)                                   = 0;
    virtual void load(std::string_view filePath, Texture2D* texture, SpriteFrameCache& cache)               = 0;
    virtual void load(std::string_view filePath, std::string_view textureFileName, SpriteFrameCache& cache) = 0;
    virtual void load(const Data& content, Texture2D* texture, SpriteFrameCache& cache)                     = 0;
    virtual void reload(std::string_view filePath, SpriteFrameCache& cache)                                 = 0;
};

class SpriteSheetLoader : public ISpriteSheetLoader
{
public:
    /** Configures PolygonInfo class with the passed sizes + triangles */
    void initializePolygonInfo(const Vec2& textureSize,
                               const Vec2& spriteSize,
                               const std::vector<int>& vertices,
                               const std::vector<int>& verticesUV,
                               const std::vector<int>& triangleIndices,
                               PolygonInfo& polygonInfo);

    uint32_t getFormat() override                                                                            = 0;
    void load(std::string_view filePath, SpriteFrameCache& cache) override                                   = 0;
    void load(std::string_view filePath, Texture2D* texture, SpriteFrameCache& cache) override               = 0;
    void load(std::string_view filePath, std::string_view textureFileName, SpriteFrameCache& cache) override = 0;
    void load(const Data& content, Texture2D* texture, SpriteFrameCache& cache) override                     = 0;
    void reload(std::string_view filePath, SpriteFrameCache& cache) override                                 = 0;
};

// end of _2d group
/// @}

NS_AX_END
