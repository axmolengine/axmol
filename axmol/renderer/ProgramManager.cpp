/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/renderer/ProgramManager.h"
#include "axmol/rhi/DriverBase.h"
#include "axmol/rhi/ShaderModule.h"
#include "axmol/renderer/VertexLayoutManager.h"
#include "axmol/renderer/Shaders.h"
#include "axmol/base/Macros.h"
#include "axmol/base/Environment.h"

#include "xxhash/xxhash.h"

namespace ax
{

ProgramManager* ProgramManager::_instance = nullptr;

ProgramManager* ProgramManager::getInstance()
{
    if (!_instance)
    {
        _instance = new ProgramManager();
        _instance->init();
    }
    return _instance;
}

void ProgramManager::destroyInstance()
{
    AX_SAFE_DELETE(_instance);
}

int ProgramManager::chooseSpriteProgramType(rhi::PixelFormat pixelFormat)
{
    switch (pixelFormat)
    {
    case PixelFormat::R8:
        return rhi::ProgramType::POSITION_TEXTURE_GRAY;
    case PixelFormat::RG8:
        return rhi::ProgramType::POSITION_TEXTURE_GRAY_ALPHA;
    case PixelFormat::RGBA8:
        return rhi::ProgramType::POSITION_TEXTURE_COLOR;
    default:
        AXLOGW("Warning: chooseSpriteProgramType() unhandled pixel format {}", (int)pixelFormat);
        return rhi::ProgramType::POSITION_TEXTURE_COLOR;
    }
}

ProgramManager::ProgramManager()
{
    _programIdGen = XXH64_createState();
}

ProgramManager::~ProgramManager()
{
    XXH64_freeState(_programIdGen);

    for (auto&& program : _cachedPrograms)
    {
        AX_SAFE_RELEASE(program.second);
    }
    AXLOGD("deallocing ProgramManager: {}", fmt::ptr(this));
    rhi::ShaderCache::destroyInstance();
}

// ### end of vertex layout setup functions
static std::string joinPath(std::string_view path, std::string_view childPath)
{
    std::string ret;
    ret.reserve(path.length() + childPath.length());
    ret += path;
    ret += childPath;
    return ret;
}

void ProgramManager::init()
{
    auto fileUtils = FileUtils::getInstance();
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX
    // load compiled shader from exeDir/axslc
    auto axslcDir = joinPath(fileUtils->getAppRoot(), "axslc"sv);
    fileUtils->addSearchPath(axslcDir);
#else  // APPLE, ANDROID, WINRT
    fileUtils->addSearchPath("axslc"sv);
#endif

    registerProgram(ProgramType::POSITION_TEXTURE_COLOR, positionTextureColor_vert, positionTextureColor_frag,
                    VertexLayoutKind::Sprite);
    registerProgram(ProgramType::DUAL_SAMPLER, positionTextureColor_vert, dualSampler_frag, VertexLayoutKind::Sprite);
    registerProgram(ProgramType::LABEL_DISTANCE_NORMAL, positionTextureColor_vert, label_distanceNormal_frag,
                    VertexLayoutKind::Sprite);
    registerProgram(ProgramType::LABEL_NORMAL, positionTextureColor_vert, label_normal_frag, VertexLayoutKind::Sprite);
    registerProgram(ProgramType::LABLE_OUTLINE, positionTextureColor_vert, label_outline_frag,
                    VertexLayoutKind::Sprite);
    registerProgram(ProgramType::LABEL_DISTANCE_OUTLINE, positionTextureColor_vert, label_distanceOutline_frag,
                    VertexLayoutKind::Sprite);
    registerProgram(ProgramType::LABLE_DISTANCE_GLOW, positionTextureColor_vert, label_distanceGlow_frag,
                    VertexLayoutKind::Sprite);
    registerProgram(ProgramType::POSITION_COLOR_LENGTH_TEXTURE, positionColorLengthTexture_vert,
                    positionColorLengthTexture_frag, VertexLayoutKind::DrawNode);
    registerProgram(ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE, positionColorTextureAsPointsize_vert,
                    positionColor_frag, VertexLayoutKind::DrawNode);
    registerProgram(ProgramType::POSITION_COLOR, positionColor_vert, positionColor_frag, VertexLayoutKind::posColor);
    registerProgram(ProgramType::LAYER_RADIA_GRADIENT, position_vert, layer_radialGradient_frag, VertexLayoutKind::Pos);
    registerProgram(ProgramType::POSITION_TEXTURE, positionTexture_vert, positionTexture_frag,
                    VertexLayoutKind::Texture);
    registerProgram(ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST, positionTextureColor_vert,
                    positionTextureColorAlphaTest_frag, VertexLayoutKind::Sprite);
    registerProgram(ProgramType::POSITION_TEXTURE_GRAY_ALPHA, positionTextureColor_vert, positionTextureGrayAlpha_frag,
                    VertexLayoutKind::Sprite);
    registerProgram(ProgramType::POSITION_TEXTURE_GRAY, positionTextureColor_vert, positionTextureGray_frag,
                    VertexLayoutKind::Sprite);
    registerProgram(ProgramType::POSITION_UCOLOR, positionUColor_vert, positionColor_frag, VertexLayoutKind::Pos);
    registerProgram(ProgramType::DUAL_SAMPLER_GRAY, positionTextureColor_vert, dualSampler_gray_frag,
                    VertexLayoutKind::Sprite);
    registerProgram(ProgramType::GRAY_SCALE, positionTextureColor_vert, grayScale_frag, VertexLayoutKind::Sprite);
    registerProgram(ProgramType::LINE_COLOR_3D, lineColor_vert, lineColor_frag, VertexLayoutKind::DrawNode3D);
    registerProgram(ProgramType::CAMERA_CLEAR, cameraClear_vert, cameraClear_frag, VertexLayoutKind::Sprite);
    registerProgram(ProgramType::SKYBOX_3D, skybox_vert, skybox_frag, VertexLayoutKind::SkyBox);
    registerProgram(ProgramType::SKINPOSITION_TEXTURE_3D, skinPositionTexture_vert, colorTexture_frag,
                    VertexLayoutKind::Invalid);
    registerProgram(ProgramType::SKINPOSITION_NORMAL_TEXTURE_3D, skinPositionNormalTexture_vert,
                    colorNormalTexture_frag, VertexLayoutKind::Invalid);
    registerProgram(ProgramType::POSITION_NORMAL_TEXTURE_3D, positionNormalTexture_vert, colorNormalTexture_frag,
                    VertexLayoutKind::Invalid);
    registerProgram(ProgramType::UNLIT, unlit_vert, colorTexture_frag, VertexLayoutKind::Invalid);
    registerProgram(ProgramType::UNLIT_INSTANCE, unlit_instance_vert, colorTexture_frag, VertexLayoutKind::Instanced);
    registerProgram(ProgramType::POSITION_3D, position_vert, color_frag, VertexLayoutKind::Invalid);
    registerProgram(ProgramType::POSITION_NORMAL_3D, positionNormalTexture_vert, colorNormal_frag,
                    VertexLayoutKind::Invalid);
    registerProgram(ProgramType::POSITION_BUMPEDNORMAL_TEXTURE_3D, positionNormalTexture_vert_1,
                    colorNormalTexture_frag_1, VertexLayoutKind::Invalid);
    registerProgram(ProgramType::SKINPOSITION_BUMPEDNORMAL_TEXTURE_3D, skinPositionNormalTexture_vert_1,
                    colorNormalTexture_frag_1, VertexLayoutKind::Invalid);
    registerProgram(ProgramType::TERRAIN_3D, terrain_vert, terrain_frag, VertexLayoutKind::Terrain3D);
    registerProgram(ProgramType::PARTICLE_TEXTURE_3D, particle_vert, particleTexture_frag,
                    VertexLayoutKind::PosUvColor);
    registerProgram(ProgramType::PARTICLE_COLOR_3D, particle_vert, particleColor_frag, VertexLayoutKind::PosUvColor);
    registerProgram(ProgramType::QUAD_COLOR_2D, quadColor_vert, quadColor_frag, VertexLayoutKind::Invalid);
    registerProgram(ProgramType::QUAD_TEXTURE_2D, quadTexture_vert, quadTexture_frag, VertexLayoutKind::Invalid);
    registerProgram(ProgramType::HSV, positionTextureColor_vert, hsv_frag, VertexLayoutKind::Sprite);
    registerProgram(ProgramType::HSV_DUAL_SAMPLER, positionTextureColor_vert, dualSampler_hsv_frag,
                    VertexLayoutKind::Sprite);

    registerProgram(ProgramType::VIDEO_TEXTURE_YUY2, positionTextureColor_vert, videoTextureYUY2_frag,
                    VertexLayoutKind::Sprite);
    registerProgram(ProgramType::VIDEO_TEXTURE_NV12, positionTextureColor_vert, videoTextureNV12_frag,
                    VertexLayoutKind::Sprite);
    registerProgram(ProgramType::VIDEO_TEXTURE_I420, positionTextureColor_vert, videoTextureI420_frag,
                    VertexLayoutKind::Sprite);

    registerProgram(ProgramType::POS_UV_COLOR_2D, posUVColor2D_vert, positionTextureColor_frag,
                    VertexLayoutKind::Sprite2D);

    // The builtin dual sampler shader registry
    ProgramStateRegistry::getInstance()->registerProgram(ProgramType::POSITION_TEXTURE_COLOR,
                                                         TextureSamplerFlag::DUAL_SAMPLER, ProgramType::DUAL_SAMPLER);

    ProgramStateRegistry::getInstance()->registerProgram(ProgramType::GRAY_SCALE, TextureSamplerFlag::DUAL_SAMPLER,
                                                         ProgramType::DUAL_SAMPLER_GRAY);

    ProgramStateRegistry::getInstance()->registerProgram(ProgramType::HSV, TextureSamplerFlag::DUAL_SAMPLER,
                                                         ProgramType::HSV_DUAL_SAMPLER);
}

Program* ProgramManager::getBuiltinProgram(uint32_t type)
{
    assert(type < ProgramType::BUILTIN_COUNT);

    auto& info = _builtinRegistry[static_cast<int>(type)];
    return loadProgram(info.vsName, info.fsName, type, static_cast<uint64_t>(type), info.vlk);
}

Program* ProgramManager::loadProgram(uint64_t progId)
{
    if (progId < ProgramType::BUILTIN_COUNT)
        return getBuiltinProgram(static_cast<uint32_t>(progId));

    auto it = _customRegistry.find(progId);
    if (it != _customRegistry.end())
        return loadProgram(it->second.vsName, it->second.fsName, ProgramType::CUSTOM_PROGRAM, progId, it->second.vlk);
    return nullptr;
}

Program* ProgramManager::loadProgram(std::string_view vsName, std::string_view fsName, VertexLayoutKind vlk)
{
    return loadProgram(vsName, fsName, ProgramType::CUSTOM_PROGRAM, computeProgramId(vsName, fsName), vlk);
}

Program* ProgramManager::loadProgram(std::string_view vsName,
                                     std::string_view fsName,
                                     uint32_t progType,
                                     uint64_t progId,
                                     VertexLayoutKind vlk)
{
    assert(!vsName.empty() && !fsName.empty());

    auto it = _cachedPrograms.find(progId);
    if (it != _cachedPrograms.end())
        return it->second;

    AXLOGD("Loading shader: {} {}, {} ...", progId, vsName.data(), fsName.data());

    auto fileUtils  = FileUtils::getInstance();
    auto vertFile   = fileUtils->fullPathForFilename(vsName);
    auto fragFile   = fileUtils->fullPathForFilename(fsName);
    auto vertSource = fileUtils->getStringFromFile(vertFile);
    auto fragSource = fileUtils->getStringFromFile(fragFile);
    auto program    = axdrv->createProgram(vertSource, fragSource);

    if (program)
    {
        program->setProgramIds(progType, progId);
        if ((unsigned int)vlk < (unsigned int)VertexLayoutKind::Count)
        {
            auto layout = axvlm->acquireBuiltinVertexLayout(vlk, program);
            program->setVertexLayout(layout);
        }
        _cachedPrograms.emplace(progId, program);
    }
    return program;
}

uint64_t ProgramManager::registerCustomProgram(std::string_view vsName,
                                               std::string_view fsName,
                                               VertexLayoutKind vlk,
                                               bool force)
{
    return registerProgram(ProgramType::CUSTOM_PROGRAM, vsName, fsName, vlk, force);
}

uint64_t ProgramManager::registerProgram(uint32_t progType,
                                         std::string_view vsName,
                                         std::string_view fsName,
                                         VertexLayoutKind vlk,
                                         bool force)
{
    uint64_t progId = 0;
    if (progType < ProgramType::BUILTIN_COUNT)
    {
        _builtinRegistry[static_cast<int>(progType)] = {vsName, fsName, vlk};
        progId                                       = progType;
    }
    else
    {
        progId  = computeProgramId(vsName, fsName);
        auto it = _customRegistry.find(progId);
        if (it == _customRegistry.end())
            _customRegistry.emplace(progId, BuiltinRegInfo{vsName, fsName, vlk});
        else if (force)
            it->second = BuiltinRegInfo{vsName, fsName, vlk};
        else
            progId = 0;
    }
    return progId;
}

uint64_t ProgramManager::computeProgramId(std::string_view vsName, std::string_view fsName)
{
    XXH64_reset(_programIdGen, 0);
    XXH64_update(_programIdGen, vsName.data(), vsName.length());
    XXH64_update(_programIdGen, fsName.data(), fsName.length());
    return XXH64_digest(_programIdGen);
}

void ProgramManager::unloadProgram(Program* program)
{
    if (!program)
    {
        return;
    }

    auto it = _cachedPrograms.find(program->_programId);
    if (it != _cachedPrograms.end())
    {
        it->second->release();
        _cachedPrograms.erase(it);
    }
}

void ProgramManager::unloadUnusedPrograms()
{
    for (auto iter = _cachedPrograms.cbegin(); iter != _cachedPrograms.cend();)
    {
        auto program = iter->second;
        if (program->getReferenceCount() == 1)
        {
            program->release();
            iter = _cachedPrograms.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void ProgramManager::unloadAllPrograms()
{
    ProgramStateRegistry::getInstance()->clearPrograms();
    for (auto&& program : _cachedPrograms)
    {
        program.second->release();
    }
    _cachedPrograms.clear();
}

}  // namespace ax
