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
#include "SpineSkeletonDataCache.h"

#if defined(AX_ENABLE_EXT_SPINE)

#    if !defined(AX_SPINE_VERSION) || AX_SPINE_VERSION >= 0x030700
SpineSkeletonDataCache* SpineSkeletonDataCache::getInstance()
{
    static SpineSkeletonDataCache internalShared;
    return &internalShared;
}

SpineSkeletonDataCache::SpineSkeletonDataCache() {}

void SpineSkeletonDataCache::removeData(const char* dataFile)
{
    auto target = _cacheTable.find(dataFile);
    if (target != _cacheTable.end())
    {
        target->second->release();
        _cacheTable.erase(target);
    }
}

SpineSkeletonDataCache::SkeletonData* SpineSkeletonDataCache::addData(const char* dataFile,
                                                                      const char* atlasFile,
                                                                      float scale)
{
    auto target = _cacheTable.find(dataFile);
    if (target != _cacheTable.end())
    {
        return target->second;
    }

    spine::SkeletonData* skeletonData = nullptr;
    spine::AttachmentLoader* loader   = nullptr;
    bool ok                           = false;

    auto fileExtension = ax::FileUtils::getPathExtension(dataFile);
    do
    {
        auto atlas = new (__FILE__, __LINE__) spine::Atlas(atlasFile, spine::AxmolTextureLoader::getInstance());

        if (nullptr == (atlas))
            break;

        loader = new (__FILE__, __LINE__) spine::AxmolAtlasAttachmentLoader(atlas);

        int failed = 0;

        /*
        ** Atlas is used by shared attachment loader, temp json or binary should be dispose.
        ** Cache, we just need SkeletonData & atlas.
        */
        if (fileExtension == ".skel")
        {
            spine::SkeletonBinary binary(loader);

            binary.setScale(scale);
            skeletonData = binary.readSkeletonDataFile(dataFile);
            if ((!binary.getError().isEmpty()))
            {
                ++failed;
                AXLOGE("#parse spine .skel data file failed, error:{}", binary.getError().buffer());
            }
        }
        else
        {
            spine::SkeletonJson json(loader);

            json.setScale(scale);
            skeletonData = json.readSkeletonDataFile(dataFile);
            if ((!json.getError().isEmpty()))
            {
                ++failed;
                AXLOGE("#parse spine .json data file failed, error:{}", json.getError().buffer());
            }
        }

        if (failed > 0)
        {
            if (skeletonData != nullptr)
                delete (skeletonData);

            delete (atlas);
            delete (loader);
            break;
        }

        ok = true;
    } while (false);

    if (ok)
    {
        auto newData = new SkeletonData(skeletonData, loader);
        _cacheTable.emplace(dataFile, newData);
        return newData;
    }
    return nullptr;
}

void SpineSkeletonDataCache::removeAllData(void)
{
    for (auto&& e : _cacheTable)
    {
        e.second->release();
    }
    _cacheTable.clear();
}

void SpineSkeletonDataCache::removeAllUnusedData(void)
{
    auto _First = _cacheTable.begin();
    auto _Last  = _cacheTable.end();
    for (; _First != _Last;)
    {
        if ((*_First).second->getReferenceCount() == 1)
        {
            (*_First).second->release();
            _cacheTable.erase(_First++);
            continue;
        }
        ++_First;
    }
}

#    else

SpineSkeletonDataCache* SpineSkeletonDataCache::getInstance()
{
    static SpineSkeletonDataCache internalShared;
    return &internalShared;
}

SpineSkeletonDataCache::SpineSkeletonDataCache() {}

void SpineSkeletonDataCache::removeData(const char* dataFile)
{
    auto target = _cacheTable.find(dataFile);
    if (target != _cacheTable.end())
    {
        target->second->release();
        _cacheTable.erase(target);
    }
}

SpineSkeletonDataCache::SkeletonData* SpineSkeletonDataCache::addData(const char* dataFile,
                                                                      const char* atlasFile,
                                                                      float scale)
{
    auto target = _cacheTable.find(dataFile);
    if (target != _cacheTable.end())
    {
        return target->second;
    }

    spSkeletonData* skeletonData = nullptr;
    spAttachmentLoader* loader   = nullptr;
    bool ok                      = false;

    auto fileExtension = ax::FileUtils::getPathExtension(dataFile);

    do
    {
        spAtlas* atlas = spAtlas_createFromFile(atlasFile, 0);

        if (nullptr == (atlas))
            break;

        loader = (spAttachmentLoader*)Cocos2dAttachmentLoader_create(atlas);

        int failed = 0;

        /*
        ** Atlas is used by shared attachment loader, temp json or binary should be dispose.
        ** Cache, we just need SkeletonData & atlas.
        */
        if (fileExtension == ".skel")
        {
            auto binary = spSkeletonBinary_createWithLoader(loader);
            if (nullptr == binary)
            {
                spAtlas_dispose(atlas);
                break;
            }

            binary->scale = scale;
            skeletonData  = spSkeletonBinary_readSkeletonDataFile(binary, dataFile);
            if ((binary->error != nullptr))
            {
                ++failed;
                AXLOGE("#parse spine .skel data file failed, error:{}", binary->error);
            }

            spSkeletonBinary_dispose(binary);
        }
        else
        {
            spSkeletonJson* json = spSkeletonJson_createWithLoader(loader);
            if (nullptr == json)
            {
                spAtlas_dispose(atlas);
                break;
            }

            json->scale  = scale;
            skeletonData = spSkeletonJson_readSkeletonDataFile(json, dataFile);
            if ((json->error != nullptr))
            {
                ++failed;
                AXLOGE("#parse spine .json data file failed, error:{}", json->error);
            }

            spSkeletonJson_dispose(json);
        }

        if ((loader->error1 != nullptr))
        {
            ++failed;
            AXLOGE("#parse spine attachment failed, error:{}{}", loader->error1, loader->error2);
        }

        if (failed > 0)
        {
            if (skeletonData != nullptr)
                spSkeletonData_dispose(skeletonData);

            spAtlas_dispose(atlas);
            spAttachmentLoader_dispose(loader);
            break;
        }

        ok = true;
    } while (false);

    if (ok)
    {
        auto newData = new SkeletonData(skeletonData, loader);
        _cacheTable.emplace(dataFile, newData);
        return newData;
    }
    return nullptr;
}

void SpineSkeletonDataCache::removeAllData(void)
{
    for (auto&& e : _cacheTable)
    {
        e.second->release();
    }
    _cacheTable.clear();
}

void SpineSkeletonDataCache::removeAllUnusedData(void)
{
    auto _First = _cacheTable.begin();
    auto _Last  = _cacheTable.end();
    for (; _First != _Last;)
    {
        if ((*_First).second->getReferenceCount() == 1)
        {
            (*_First).second->release();
            _cacheTable.erase(_First++);
            continue;
        }
        ++_First;
    }
}
#    endif

#endif  // defined(AX_ENABLE_EXT_SPINE)
