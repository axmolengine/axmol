/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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
#ifndef __CCSPRITEFRAMECACHEHELPER_H__
#define __CCSPRITEFRAMECACHEHELPER_H__

#include "platform/PlatformMacros.h"
#include "ArmatureDefine.h"
#include "CocosStudioExport.h"
#include <string>
#include <map>
#include <vector>

NS_AX_BEGIN
class SpriteFrame;
NS_AX_END

namespace cocostudio
{

/**
 *  @js NA
 *  @lua NA
 */
class CCS_DLL SpriteFrameCacheHelper
{
public:
    /** @deprecated Use getInstance() instead */
    AX_DEPRECATED_ATTRIBUTE static SpriteFrameCacheHelper* sharedSpriteFrameCacheHelper()
    {
        return SpriteFrameCacheHelper::getInstance();
    }

    static SpriteFrameCacheHelper* getInstance();

    static void purge();

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

    hlookup::string_map<std::vector<ax::SpriteFrame*>> _usingSpriteFrames;
    static SpriteFrameCacheHelper* _spriteFrameCacheHelper;
};

}  // namespace cocostudio

#endif /*__CCSPRITEFRAMECACHEHELPER_H__*/
