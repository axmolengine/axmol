/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2022 Bytedance Inc.

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

#include "ProgramManager.h"
#include "Device.h"
#include "ShaderModule.h"
#include "renderer/ccShaders.h"
#include "base/ccMacros.h"
#include "base/CCConfiguration.h"

NS_AX_BACKEND_BEGIN

namespace
{
std::string getShaderMacrosForLight()
{
    char def[256];
    auto conf = Configuration::getInstance();

    snprintf(def, sizeof(def) - 1,
             "\n#define MAX_DIRECTIONAL_LIGHT_NUM %d \n"
             "\n#define MAX_POINT_LIGHT_NUM %d \n"
             "\n#define MAX_SPOT_LIGHT_NUM %d \n",
             conf->getMaxSupportDirLightInShader(), conf->getMaxSupportPointLightInShader(),
             conf->getMaxSupportSpotLightInShader());

    return std::string(def);
}
}  // namespace

ProgramManager* ProgramManager::_sharedProgramManager = nullptr;

Program* ProgramManager::newProgram(std::string_view vertShaderSource,
                                           std::string_view fragShaderSource,
                                           std::function<void(Program*)> fnSetupLayout)
{
    auto program = Device::getInstance()->newProgram(vertShaderSource, fragShaderSource);
    if (program)
        fnSetupLayout(program);
    return program;
}

ProgramManager* ProgramManager::getInstance()
{
    if (!_sharedProgramManager)
    {
        _sharedProgramManager = new ProgramManager();
        if (!_sharedProgramManager->init())
        {
            AX_SAFE_DELETE(_sharedProgramManager);
        }
    }
    return _sharedProgramManager;
}

void ProgramManager::destroyInstance()
{
    AX_SAFE_RELEASE_NULL(_sharedProgramManager);
}

ProgramManager::~ProgramManager()
{
    for (auto&& program : _cachedPrograms)
    {
        AX_SAFE_RELEASE(program.second);
    }
    AXLOGINFO("deallocing ProgramManager: %p", this);
    backend::ShaderCache::destroyInstance();
}

/*
 * shader vertex layout setup functions
 */
void VertexLayoutHelper::setupDummy(Program*) {}
void VertexLayoutHelper::setupTexture(Program* program)
{
    auto vertexLayout = program->getVertexLayout();

    /// a_position
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT2, 0, false);
    /// a_texCoord
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_TEXCOORD,
                               program->getAttributeLocation(backend::Attribute::TEXCOORD),
                               backend::VertexFormat::FLOAT2, 2 * sizeof(float), false);

    vertexLayout->setStride(4 * sizeof(float));
}

void VertexLayoutHelper::setupSprite(Program* program)
{
    auto vertexLayout = program->getVertexLayout();

    /// a_position
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT3, 0, false);
    /// a_texCoord
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_TEXCOORD,
                               program->getAttributeLocation(backend::Attribute::TEXCOORD),
                               backend::VertexFormat::FLOAT2, offsetof(V3F_C4B_T2F, texCoords), false);

    /// a_color
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_COLOR, program->getAttributeLocation(backend::Attribute::COLOR),
                               backend::VertexFormat::UBYTE4, offsetof(V3F_C4B_T2F, colors), true);
    vertexLayout->setStride(sizeof(V3F_C4B_T2F));
}

void VertexLayoutHelper::setupDrawNode(Program* program)
{
    auto vertexLayout = program->getVertexLayout();

    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT2, 0, false);

    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_TEXCOORD,
                               program->getAttributeLocation(backend::Attribute::TEXCOORD),
                               backend::VertexFormat::FLOAT2, offsetof(V2F_C4B_T2F, texCoords), false);

    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_COLOR, program->getAttributeLocation(backend::Attribute::COLOR),
                               backend::VertexFormat::UBYTE4, offsetof(V2F_C4B_T2F, colors), true);

    vertexLayout->setStride(sizeof(V2F_C4B_T2F));
}

void VertexLayoutHelper::setupDrawNode3D(Program* program)
{
    auto vertexLayout = program->getVertexLayout();

    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT3, 0, false);

    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_COLOR, program->getAttributeLocation(backend::Attribute::COLOR),
                               backend::VertexFormat::UBYTE4, sizeof(Vec3), true);

    vertexLayout->setStride(sizeof(V3F_C4B));
}

void VertexLayoutHelper::setupSkyBox(Program* program)
{
    auto vertexLayout = program->getVertexLayout();
    auto attrNameLoc  = program->getAttributeLocation(shaderinfos::attribute::ATTRIBUTE_NAME_POSITION);
    vertexLayout->setAttribute(shaderinfos::attribute::ATTRIBUTE_NAME_POSITION, attrNameLoc,
                               backend::VertexFormat::FLOAT3, 0, false);
    vertexLayout->setStride(sizeof(Vec3));
}

void VertexLayoutHelper::setupPU3D(Program* program)
{
    auto vertexLayout = program->getVertexLayout();

    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT3, offsetof(V3F_T2F_C4F, position), false);

    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_TEXCOORD,
                               program->getAttributeLocation(backend::Attribute::TEXCOORD),
                               backend::VertexFormat::FLOAT2, offsetof(V3F_T2F_C4F, uv), false);

    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_COLOR, program->getAttributeLocation(backend::Attribute::COLOR),
                               backend::VertexFormat::FLOAT4, offsetof(V3F_T2F_C4F, color), false);

    vertexLayout->setStride(sizeof(V3F_T2F_C4F));
}

void VertexLayoutHelper::setupPos(Program* program)
{
    auto vertexLayout = program->getVertexLayout();
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT2, 0, false);
    vertexLayout->setStride(sizeof(Vec2));
}

void VertexLayoutHelper::setupPosColor(Program* program)
{
    auto vertexLayout = program->getVertexLayout();
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT3, 0, false);
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_COLOR, program->getAttributeLocation(backend::Attribute::COLOR),
                               backend::VertexFormat::FLOAT4, offsetof(V3F_C4F, colors), false);
    vertexLayout->setStride(sizeof(V3F_C4F));
}

void VertexLayoutHelper::setupTerrain3D(Program* program)
{
    auto vertexLayout = program->getVertexLayout();
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT3, 0, false);
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_TEXCOORD,
                               program->getAttributeLocation(backend::Attribute::TEXCOORD),
                               backend::VertexFormat::FLOAT2, offsetof(V3F_T2F_N3F, texcoord), false);
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_NORMAL,
                               program->getAttributeLocation(backend::Attribute::NORMAL), backend::VertexFormat::FLOAT3,
                               offsetof(V3F_T2F_N3F, normal), false);
    vertexLayout->setStride(sizeof(V3F_T2F_N3F));
}

// ### end of vertex layout setup functions

bool ProgramManager::init()
{
    registerProgramFactory(ProgramType::POSITION_TEXTURE_COLOR, positionTextureColor_vert, positionTextureColor_frag,
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::DUAL_SAMPLER, positionTextureColor_vert, dualSampler_frag,
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::LABEL_DISTANCE_NORMAL, positionTextureColor_vert, label_distanceNormal_frag,
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::LABEL_NORMAL, positionTextureColor_vert, label_normal_frag,
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::LABLE_OUTLINE, positionTextureColor_vert, labelOutline_frag,
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::LABLE_DISTANCEFIELD_GLOW, positionTextureColor_vert,
                           labelDistanceFieldGlow_frag, VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::POSITION_COLOR_LENGTH_TEXTURE, positionColorLengthTexture_vert,
                           positionColorLengthTexture_frag, VertexLayoutHelper::setupDrawNode);
    registerProgramFactory(ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE, positionColorTextureAsPointsize_vert,
                           positionColor_frag, VertexLayoutHelper::setupDrawNode);
    registerProgramFactory(ProgramType::POSITION_COLOR, positionColor_vert, positionColor_frag,
                           VertexLayoutHelper::setupPosColor);
    registerProgramFactory(ProgramType::LAYER_RADIA_GRADIENT, position_vert, layer_radialGradient_frag,
                           VertexLayoutHelper::setupPos);
    registerProgramFactory(ProgramType::POSITION_TEXTURE, positionTexture_vert, positionTexture_frag,
                           VertexLayoutHelper::setupTexture);
    registerProgramFactory(ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST, positionTextureColor_vert,
                           positionTextureColorAlphaTest_frag, VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::POSITION_UCOLOR, positionUColor_vert, positionColor_frag,
                           VertexLayoutHelper::setupPos);
    registerProgramFactory(ProgramType::DUAL_SAMPLER_GRAY, positionTextureColor_vert, dualSampler_gray_frag,
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::GRAY_SCALE, positionTextureColor_vert, grayScale_frag,
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::LINE_COLOR_3D, lineColor3D_vert, lineColor3D_frag,
                           VertexLayoutHelper::setupDrawNode3D);
    registerProgramFactory(ProgramType::CAMERA_CLEAR, cameraClear_vert, cameraClear_frag,
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::SKYBOX_3D, CC3D_skybox_vert, CC3D_skybox_frag, VertexLayoutHelper::setupSkyBox);
    registerProgramFactory(ProgramType::SKINPOSITION_TEXTURE_3D, CC3D_skinPositionTexture_vert, CC3D_colorTexture_frag,
                           VertexLayoutHelper::setupDummy);
    auto lightDef = getShaderMacrosForLight();
    registerProgramFactory(ProgramType::SKINPOSITION_NORMAL_TEXTURE_3D, lightDef + CC3D_skinPositionNormalTexture_vert,
                           lightDef + CC3D_colorNormalTexture_frag, VertexLayoutHelper::setupDummy);
    registerProgramFactory(ProgramType::POSITION_NORMAL_TEXTURE_3D, lightDef + CC3D_positionNormalTexture_vert,
                           lightDef + CC3D_colorNormalTexture_frag, VertexLayoutHelper::setupDummy);
    registerProgramFactory(ProgramType::POSITION_TEXTURE_3D, CC3D_positionTexture_vert, CC3D_colorTexture_frag,
                           VertexLayoutHelper::setupDummy);
    registerProgramFactory(ProgramType::POSITION_3D, CC3D_positionTexture_vert, CC3D_color_frag,
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::POSITION_NORMAL_3D, lightDef + CC3D_positionNormalTexture_vert,
                           lightDef + CC3D_colorNormal_frag, VertexLayoutHelper::setupDummy);
    const char* normalMapDef = "\n#define USE_NORMAL_MAPPING 1 \n";
    registerProgramFactory(ProgramType::POSITION_BUMPEDNORMAL_TEXTURE_3D,
                           lightDef + normalMapDef + CC3D_positionNormalTexture_vert,
                           lightDef + normalMapDef + CC3D_colorNormalTexture_frag, VertexLayoutHelper::setupDummy);
    registerProgramFactory(ProgramType::SKINPOSITION_BUMPEDNORMAL_TEXTURE_3D,
                           lightDef + normalMapDef + CC3D_skinPositionNormalTexture_vert,
                           lightDef + normalMapDef + CC3D_colorNormalTexture_frag, VertexLayoutHelper::setupDummy);
    registerProgramFactory(ProgramType::TERRAIN_3D, CC3D_terrain_vert, CC3D_terrain_frag,
                           VertexLayoutHelper::setupTerrain3D);
    registerProgramFactory(ProgramType::PARTICLE_TEXTURE_3D, CC3D_particle_vert, CC3D_particleTexture_frag,
                           VertexLayoutHelper::setupPU3D);
    registerProgramFactory(ProgramType::PARTICLE_COLOR_3D, CC3D_particle_vert, CC3D_particleColor_frag,
                           VertexLayoutHelper::setupPU3D);
    registerProgramFactory(ProgramType::QUAD_COLOR_2D, CC2D_quadColor_vert, CC2D_quadColor_frag,
                           VertexLayoutHelper::setupDummy);
    registerProgramFactory(ProgramType::QUAD_TEXTURE_2D, CC2D_quadTexture_vert, CC2D_quadTexture_frag,
                           VertexLayoutHelper::setupDummy);
    registerProgramFactory(ProgramType::HSV, positionTextureColor_vert, hsv_frag, VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::HSV_DUAL_SAMPLER, positionTextureColor_vert, dualSampler_hsv_frag,
                           VertexLayoutHelper::setupSprite);

    registerProgramFactory(ProgramType::VIDEO_TEXTURE_YUY2, positionTextureColor_vert,
                           std::string{videoTextureYUY2_frag},
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::VIDEO_TEXTURE_NV12, positionTextureColor_vert,
                           std::string{videoTextureNV12_frag},
                           VertexLayoutHelper::setupSprite);
    registerProgramFactory(ProgramType::VIDEO_TEXTURE_BGR32, positionTextureColor_vert,
                           std::string{videoTextureBGRA_frag},
                           VertexLayoutHelper::setupSprite);

    // The builtin dual sampler shader registry
    ProgramStateRegistry::getInstance()->registerProgram(ProgramType::POSITION_TEXTURE_COLOR,
                                                         TextureSamplerFlag::DUAL_SAMPLER,
                                                         getBuiltinProgram(ProgramType::DUAL_SAMPLER));

    ProgramStateRegistry::getInstance()->registerProgram(ProgramType::GRAY_SCALE, TextureSamplerFlag::DUAL_SAMPLER,
                                                         getBuiltinProgram(ProgramType::DUAL_SAMPLER_GRAY));

    ProgramStateRegistry::getInstance()->registerProgram(ProgramType::HSV, TextureSamplerFlag::DUAL_SAMPLER,
                                                         getBuiltinProgram(ProgramType::HSV_DUAL_SAMPLER));
    return true;
}

Program* ProgramManager::getCustomProgram(uint32_t type) const
{
    return getBuiltinProgram(type | ProgramType::CUSTOM_PROGRAM);
}

Program* ProgramManager::getBuiltinProgram(uint32_t type) const
{
    auto iter = _cachedPrograms.find(type);
    if (iter != _cachedPrograms.end())
        return iter->second;

    return addProgram(type);
}

Program* ProgramManager::addProgram(uint32_t internalType) const
{
    Program* program = nullptr;
    if (internalType < ProgramType::BUILTIN_COUNT)
    {
        auto& func = _builtinFactories[internalType];
        if (func)
            program = func();
    }
    else
    {
        auto iter = _customFactories.find(internalType);
        if (iter != _customFactories.end())
        {
            auto& func = iter->second;
            if (func)
                program = func();
        }
    }

    if (program)
    {
        program->setProgramType(internalType);
        _cachedPrograms.emplace(internalType, program);
    }

    return program;
}

void ProgramManager::registerCustomProgramFactory(uint32_t type,
                                                std::string vertShaderSource,
                                                std::string fragShaderSource,
                                                std::function<void(Program*)> fnSetupLayout)
{
    auto internalType = ProgramType::CUSTOM_PROGRAM | type;
    registerProgramFactory(internalType, std::move(vertShaderSource), std::move(fragShaderSource),
                           std::move(fnSetupLayout));
}

void ProgramManager::registerProgramFactory(uint32_t internalType,
                                          std::string&& vertShaderSource,
                                          std::string&& fragShaderSource,
                                          std::function<void(Program*)> fnSetupLayout)
{
    auto constructProgram = [vsrc = std::move(vertShaderSource), fsrc = std::move(fragShaderSource),
                             setupLayout = std::move(fnSetupLayout)]() {
        auto program = backend::Device::getInstance()->newProgram(vsrc, fsrc);
        setupLayout(program);
        return program;
    };

    if (internalType < ProgramType::BUILTIN_COUNT)
    {
        _builtinFactories[internalType] = constructProgram;
    }
    else
    {
        auto it = _customFactories.find(internalType);
        if (it == _customFactories.end())
            _customFactories.emplace(internalType, constructProgram);
        else
            it->second = constructProgram;
    }
}

void ProgramManager::removeProgram(Program* program)
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

void ProgramManager::removeUnusedProgram()
{
    for (auto iter = _cachedPrograms.cbegin(); iter != _cachedPrograms.cend();)
    {
        auto program = iter->second;
        if (program->getReferenceCount() == 1)
        {
            //            AXLOG("axmol: TextureCache: removing unused program");
            program->release();
            iter = _cachedPrograms.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

void ProgramManager::removeAllPrograms()
{
    ProgramStateRegistry::getInstance()->clearPrograms();
    for (auto&& program : _cachedPrograms)
    {
        program.second->release();
    }
    _cachedPrograms.clear();
}

NS_AX_BACKEND_END
