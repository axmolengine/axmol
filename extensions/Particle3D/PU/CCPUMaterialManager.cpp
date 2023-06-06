/****************************************************************************
 Copyright (C) 2013 Henry van Merode. All rights reserved.
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://axmolengine.github.io/

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

#include "CCPUMaterialManager.h"
#include "extensions/Particle3D/PU/CCPUScriptCompiler.h"
#include "extensions/Particle3D/PU/CCPUTranslateManager.h"
#include "platform/CCFileUtils.h"
#include "platform/CCPlatformMacros.h"
#include "renderer/backend/Types.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#    include <io.h>
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#    include "platform/android/CCFileUtils-android.h"
#    include <android/asset_manager.h>
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
#    include <ftw.h>
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#    include <sys/types.h>
#    include <sys/stat.h>
#    include <dirent.h>
#endif
NS_AX_BEGIN

PUMaterial::PUMaterial()
    : isEnabledLight(true)
    , ambientColor(Vec4::ONE)
    , diffuseColor(Vec4::ONE)
    , specularColor(Vec4::ZERO)
    , emissiveColor(Vec4::ZERO)
    , shininess(0.0f)
    , depthTest(true)
    , depthWrite(true)
    , wrapMode(backend::SamplerAddressMode::CLAMP_TO_EDGE)
{
    blendFunc.src = backend::BlendFactor::ONE;
    blendFunc.dst = backend::BlendFactor::ZERO;
}

PUMaterialCache::PUMaterialCache() {}

PUMaterialCache::~PUMaterialCache()
{
    for (auto&& iter : _materialMap)
    {
        iter->release();
    }
    _materialMap.clear();
}

PUMaterialCache* PUMaterialCache::Instance()
{
    static PUMaterialCache pmm;
    return &pmm;
}

PUMaterial* PUMaterialCache::getMaterial(std::string_view name)
{
    for (auto&& iter : _materialMap)
    {
        if (iter->name == name)
            return iter;
    }
    return nullptr;
}

bool PUMaterialCache::loadMaterials(std::string_view file)
{
    bool isFirstCompile = true;
    auto list           = PUScriptCompiler::Instance()->compile(file, isFirstCompile);
    if (list == nullptr || list->empty())
        return false;
    if (isFirstCompile)
    {
        PUTranslateManager::Instance()->translateMaterialSystem(this, list);
    }
    return true;
}

void PUMaterialCache::addMaterial(PUMaterial* material)
{
    for (auto&& iter : _materialMap)
    {
        if (iter->name == material->name)
        {
            AXLOG("warning: Material has existed (FilePath: %s,  MaterialName: %s)", material->fileName.c_str(),
                  material->name.c_str());
            return;
        }
    }

    material->retain();
    _materialMap.emplace_back(material);
}

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
int iterPath(const char* fpath, const struct stat* /*sb*/, int typeflag)
{
    if (typeflag == FTW_F)
    {
        if (FileUtils::getInstance()->getFileExtension(fpath) == ".material")
            PUMaterialCache::Instance()->loadMaterials(fpath);
    }
    return 0;
}
#endif

bool PUMaterialCache::loadMaterialsFromSearchPaths(std::string_view fileFolder)
{
    bool state = false;
#if defined(_WIN32)
    std::string seg("/");
    std::string fullPath{fileFolder};
    fullPath += seg;
    fullPath += std::string("*.material");
    WIN32_FIND_DATAA data;
    HANDLE handle =
        FindFirstFileExA(fullPath.c_str(), FindExInfoStandard, &data, FindExSearchNameMatch, NULL, 0);
    if (handle != INVALID_HANDLE_VALUE)
    {
        state = true;
        do
        {
            fullPath = fileFolder;
            fullPath += seg;
            fullPath += data.cFileName;
            loadMaterials(fullPath);
           
        } while (FindNextFileA(handle, &data));
        FindClose(handle);
    }
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID /* || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX*/)
    std::string::size_type pos    = fileFolder.find("assets/");
    std::string_view relativePath = fileFolder;
    if (pos != std::string::npos)
    {
        // "assets/" is at the beginning of the path and we don't want it
        relativePath = fileFolder.substr(pos + strlen("assets/"));
    }
    AAssetDir* dir       = AAssetManager_openDir(FileUtilsAndroid::getAssetManager(), relativePath.data());
    const char* fileName = nullptr;
    std::string_view seg("/", 1);
    std::string fullpath;
    while ((fileName = AAssetDir_getNextFileName(dir)) != nullptr)
    {
        if (FileUtils::getInstance()->getFileExtension(fileName) == ".material")
        {
            fullpath.assign(fileFolder).append(seg).append(fileName);
            loadMaterials(fullpath);
        }
    }
    AAssetDir_close(dir);

#elif (AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
    ftw(fileFolder.data(), iterPath, 500);
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX || AX_TARGET_PLATFORM == AX_PLATFORM_TIZEN)
    DIR* d;               // dir handle
    struct dirent* file;  // readdir
    struct stat statbuf;

    if (!(d = opendir(fileFolder.data())))
    {
        AXLOG("error opendir %s!!!\n", fileFolder.data());
        return false;
    }
    while ((file = readdir(d)) != NULL)
    {
        if (strncmp(file->d_name, ".", 1) == 0 || (stat(file->d_name, &statbuf) >= 0 && S_ISDIR(statbuf.st_mode)))
        {
            continue;
        }

        if (FileUtils::getInstance()->getFileExtension(file->d_name) == ".material")
        {
            std::string fullpath{fileFolder};
            fullpath.append("/"sv).append(file->d_name);
            AXLOG("%s", fullpath.c_str());
            loadMaterials(fullpath);
            state = true;
        }
    }
    closedir(d);
#else
    AXASSERT(0, "no implement for this platform");
#endif

    return state;
}

NS_AX_END
