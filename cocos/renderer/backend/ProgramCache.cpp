/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

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
 
#include "ProgramCache.h"
#include "Device.h"
#include "ShaderModule.h"
#include "renderer/ccShaders.h"
#include "base/ccMacros.h"
#include "base/CCConfiguration.h"

CC_BACKEND_BEGIN

namespace
{
    std::string getShaderMacrosForLight()
    {
        char def[256];
        auto conf = Configuration::getInstance();

        snprintf(def, sizeof(def) - 1, "\n#define MAX_DIRECTIONAL_LIGHT_NUM %d \n"
            "\n#define MAX_POINT_LIGHT_NUM %d \n"
            "\n#define MAX_SPOT_LIGHT_NUM %d \n",
            conf->getMaxSupportDirLightInShader(),
            conf->getMaxSupportPointLightInShader(),
            conf->getMaxSupportSpotLightInShader());

        return std::string(def);
    }
}

ProgramCache* ProgramCache::_sharedProgramCache = nullptr;

ProgramCache* ProgramCache::getInstance()
{
    if(!_sharedProgramCache)
    {
        _sharedProgramCache = new (std::nothrow) ProgramCache();
        if(!_sharedProgramCache->init())
        {
            CC_SAFE_RELEASE(_sharedProgramCache);
        }
    }
    return _sharedProgramCache;
}

void ProgramCache::destroyInstance()
{
    CC_SAFE_RELEASE_NULL(_sharedProgramCache);
}

ProgramCache::~ProgramCache()
{
    for(auto& program : _cachedPrograms)
    {
        CC_SAFE_RELEASE(program.second);
    }
    CCLOGINFO("deallocing ProgramCache: %p", this);
    ShaderCache::destroyInstance();
}

bool ProgramCache::init()
{
    registerProgramFactory(ProgramType::POSITION_TEXTURE_COLOR, positionTextureColor_vert, positionTextureColor_frag);
    registerProgramFactory(ProgramType::ETC1, positionTextureColor_vert, etc1_frag);
    registerProgramFactory(ProgramType::LABEL_DISTANCE_NORMAL, positionTextureColor_vert, label_distanceNormal_frag);
    registerProgramFactory(ProgramType::LABEL_NORMAL, positionTextureColor_vert, label_normal_frag);
    registerProgramFactory(ProgramType::LABLE_OUTLINE, positionTextureColor_vert, labelOutline_frag);
    registerProgramFactory(ProgramType::LABLE_DISTANCEFIELD_GLOW, positionTextureColor_vert, labelDistanceFieldGlow_frag);
    registerProgramFactory(ProgramType::POSITION_COLOR_LENGTH_TEXTURE, positionColorLengthTexture_vert, positionColorLengthTexture_frag);
    registerProgramFactory(ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE, positionColorTextureAsPointsize_vert, positionColor_frag);
    registerProgramFactory(ProgramType::POSITION_COLOR, positionColor_vert, positionColor_frag);
    registerProgramFactory(ProgramType::LAYER_RADIA_GRADIENT, position_vert, layer_radialGradient_frag);
    registerProgramFactory(ProgramType::POSITION_TEXTURE, positionTexture_vert, positionTexture_frag);
    registerProgramFactory(ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST, positionTextureColor_vert, positionTextureColorAlphaTest_frag);
    registerProgramFactory(ProgramType::POSITION_UCOLOR, positionUColor_vert, positionUColor_frag);
    registerProgramFactory(ProgramType::ETC1_GRAY, positionTextureColor_vert, etc1Gray_frag);
    registerProgramFactory(ProgramType::GRAY_SCALE, positionTextureColor_vert, grayScale_frag);
    registerProgramFactory(ProgramType::LINE_COLOR_3D, lineColor3D_vert, lineColor3D_frag);
    registerProgramFactory(ProgramType::CAMERA_CLEAR, cameraClear_vert, cameraClear_frag);
    registerProgramFactory(ProgramType::SKYBOX_3D, CC3D_skybox_vert, CC3D_skybox_frag);
    registerProgramFactory(ProgramType::SKINPOSITION_TEXTURE_3D, CC3D_skinPositionTexture_vert, CC3D_colorTexture_frag);
    auto lightDef = getShaderMacrosForLight();
    registerProgramFactory(ProgramType::SKINPOSITION_NORMAL_TEXTURE_3D, lightDef + CC3D_skinPositionNormalTexture_vert, lightDef + CC3D_colorNormalTexture_frag);
    registerProgramFactory(ProgramType::POSITION_NORMAL_TEXTURE_3D, lightDef + CC3D_positionNormalTexture_vert, lightDef + CC3D_colorNormalTexture_frag);
    registerProgramFactory(ProgramType::POSITION_TEXTURE_3D, CC3D_positionTexture_vert, CC3D_colorTexture_frag);
    registerProgramFactory(ProgramType::POSITION_3D, CC3D_positionTexture_vert, CC3D_color_frag);
    registerProgramFactory(ProgramType::POSITION_NORMAL_3D, lightDef + CC3D_positionNormalTexture_vert, lightDef + CC3D_colorNormal_frag);
    const char* normalMapDef = "\n#define USE_NORMAL_MAPPING 1 \n";
    registerProgramFactory(ProgramType::POSITION_BUMPEDNORMAL_TEXTURE_3D, lightDef + normalMapDef + CC3D_positionNormalTexture_vert, lightDef + normalMapDef + CC3D_colorNormalTexture_frag);
    registerProgramFactory(ProgramType::SKINPOSITION_BUMPEDNORMAL_TEXTURE_3D, lightDef + normalMapDef + CC3D_skinPositionNormalTexture_vert, lightDef + normalMapDef + CC3D_colorNormalTexture_frag);
    registerProgramFactory(ProgramType::TERRAIN_3D, CC3D_terrain_vert, CC3D_terrain_frag);
    registerProgramFactory(ProgramType::PARTICLE_TEXTURE_3D, CC3D_particle_vert, CC3D_particleTexture_frag);
    registerProgramFactory(ProgramType::PARTICLE_COLOR_3D, CC3D_particle_vert, CC3D_particleColor_frag);
    registerProgramFactory(ProgramType::HSV, positionTextureColor_vert, hsv_frag);
    registerProgramFactory(ProgramType::HSV_ETC1, positionTextureColor_vert, hsv_etc1_frag);

    /* FIXME: Naming style
    ** ETC1: POSITION_TEXTURE_COLOR_ETC1
    ** GRAY_SCALE maybe: POSITION_TEXTURE_COLOR_GRAY
    ** ETC1_GRAY maybe: POSITION_TEXTURE_COLOR_GRAY_ETC1
    */
    ProgramStateRegistry::getInstance()->registerProgram(ProgramType::POSITION_TEXTURE_COLOR, TextureFormatEXT::ETC1_ALPHA,
        getBuiltinProgram(ProgramType::ETC1));
    ProgramStateRegistry::getInstance()->registerProgram(ProgramType::GRAY_SCALE, TextureFormatEXT::ETC1_ALPHA,
        getBuiltinProgram(ProgramType::ETC1_GRAY));

    ProgramStateRegistry::getInstance()->registerProgram(ProgramType::HSV, TextureFormatEXT::ETC1_ALPHA,
        getBuiltinProgram(ProgramType::HSV_ETC1));
    return true;
}

backend::Program* ProgramCache::getCustomProgram(uint32_t type) const
{
    return getBuiltinProgram(type | ProgramType::CUSTOM_PROGRAM);
}

backend::Program* ProgramCache::getBuiltinProgram(uint32_t type) const
{
    auto iter = _cachedPrograms.find(type);
    if (iter != _cachedPrograms.end())
        return iter->second;

    return addProgram(type);
}

Program* ProgramCache::addProgram(uint32_t internalType) const
{
    Program* program = nullptr;
    if (internalType < ProgramType::BUILTIN_COUNT)
    {
        auto& func = _builtinFactories[internalType];
        if (func) program = func();
    }
    else {
        auto iter = _customFactories.find(internalType);
        if (iter != _customFactories.end()) {
            auto& func = iter->second;
            if (func) program = func();
        }
    }

    if (program) {
        program->setProgramType(internalType);
        _cachedPrograms.emplace(internalType, program);
    }

    return program;
}

void ProgramCache::registerCustomProgramFactory(uint32_t type, std::string vertShaderSource, std::string fragShaderSource)
{
    auto internalType = ProgramType::CUSTOM_PROGRAM | type;
    registerProgramFactory(internalType, std::move(vertShaderSource), std::move(fragShaderSource));
}

void ProgramCache::registerProgramFactory(uint32_t internalType, std::string&& vertShaderSource, std::string&& fragShaderSource)
{
    auto constructProgram = [vsrc = std::move(vertShaderSource), fsrc = std::move(fragShaderSource)]() {
        return backend::Device::getInstance()->newProgram(vsrc, fsrc);
    };

    if (internalType < ProgramType::BUILTIN_COUNT) {
        _builtinFactories[internalType] = constructProgram;
    }
    else {
        auto it = _customFactories.find(internalType);
        if (it == _customFactories.end())
            _customFactories.emplace(internalType, constructProgram);
        else
            it->second = constructProgram;
    }
}

void ProgramCache::removeProgram(backend::Program* program)
{
    if (!program)
    {
        return;
    }
    
    for (auto it = _cachedPrograms.cbegin(); it != _cachedPrograms.cend();)
    {
        if (it->second == program)
        {
            it->second->release();
            it = _cachedPrograms.erase(it);
            break;
        }
        else
            ++it;
    }
}

void ProgramCache::removeUnusedProgram()
{
    for (auto iter = _cachedPrograms.cbegin(); iter != _cachedPrograms.cend();)
    {
        auto program = iter->second;
        if (program->getReferenceCount() == 1)
        {
//            CCLOG("cocos2d: TextureCache: removing unused program");
            program->release();
            iter = _cachedPrograms.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void ProgramCache::removeAllPrograms()
{
    ProgramStateRegistry::getInstance()->clearPrograms();
    for (auto& program : _cachedPrograms)
    {
        program.second->release();
    }
    _cachedPrograms.clear();
}

CC_BACKEND_END
