/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#if defined(AX_ENABLE_EXT_SPINE)
#    include "axmol/axmol.h"
#    include "spine/spine.h"
#    include "spine/spine-axmol.h"
#    include "sceneext/SceneExtMacros.h"
#    include <functional>

#    if !defined(AX_SPINE_VERSION) || AX_SPINE_VERSION >= 0x030700
class SCNEXT_API SpineSkeletonDataCache
{
public:
    class SkeletonData : public ax::Object
    {
    public:
        SkeletonData(spine::SkeletonData* d, spine::AttachmentLoader* loader) : data(d), attachmentLoader(loader) {}
        ~SkeletonData()
        {
            if (data != nullptr)
                delete (data);
            if (attachmentLoader != nullptr)
                delete (attachmentLoader);
        }
        spine::AttachmentLoader* attachmentLoader;
        spine::SkeletonData* data;
    };

    static SpineSkeletonDataCache* getInstance();

    SpineSkeletonDataCache();

    SkeletonData* addData(const char* dataFile, const char* atlasFile, float scale);

    void removeData(const char* dataFile);

    void removeAllData(void);
    void removeAllUnusedData(void);

public:
    tlx::string_map<SkeletonData*> _cacheTable;
};

#    else
class SCNEXT_API SpineSkeletonDataCache
{
public:
    class SkeletonData : public ax::Object
    {
    public:
        SkeletonData(spSkeletonData* d, spAttachmentLoader* loader) : data(d), attachmentLoader(loader) {}
        ~SkeletonData()
        {
            if (data != nullptr)
                spSkeletonData_dispose(data);
            if (attachmentLoader != nullptr)
                spAttachmentLoader_dispose(attachmentLoader);
        }
        spAttachmentLoader* attachmentLoader;
        spSkeletonData* data;
    };

    SpineSkeletonDataCache();

    static SpineSkeletonDataCache* getInstance();

    SkeletonData* addData(const char* dataFile, const char* atlasFile, float scale);

    void removeData(const char* dataFile);

    void removeAllData(void);
    void removeAllUnusedData(void);

public:
    tlx::string_map<SkeletonData*> _cacheTable;
};

#    endif

#endif  // defined(AX_ENABLE_EXT_SPINE)
