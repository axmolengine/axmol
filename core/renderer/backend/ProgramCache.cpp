/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.

 https://axys1.github.io/

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

ProgramCache* ProgramCache::_sharedProgramCache = nullptr;

ProgramCache* ProgramCache::getInstance()
{
    if (!_sharedProgramCache)
    {
        _sharedProgramCache = new ProgramCache();
        if (!_sharedProgramCache->init())
        {
            AX_SAFE_DELETE(_sharedProgramCache);
        }
    }
    return _sharedProgramCache;
}

void ProgramCache::destroyInstance()
{
    AX_SAFE_RELEASE_NULL(_sharedProgramCache);
}

ProgramCache::~ProgramCache()
{
    for (auto&& program : _cachedPrograms)
    {
        AX_SAFE_RELEASE(program.second);
    }
    AXLOGINFO("deallocing ProgramCache: %p", this);
    ShaderCache::destroyInstance();
}

/*
 * shader vertex layout setup functions
 */
void VertexLayoutHelper::setupForDummy(Program*) {}
void VertexLayoutHelper::setupForTexture(Program* program) {
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

void VertexLayoutHelper::setupForSprite(Program* program)
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

void VertexLayoutHelper::setupForDrawNode(Program* program)
{
    auto vertexLayout = program->getVertexLayout();

    const auto&& attributeInfo = program->getActiveAttributes();
    auto iter                  = attributeInfo.find("a_position");
    if (iter != attributeInfo.end())
    {
        vertexLayout->setAttribute("a_position", iter->second.location, backend::VertexFormat::FLOAT2, 0, false);
    }

    iter = attributeInfo.find("a_texCoord");
    if (iter != attributeInfo.end())
    {
        vertexLayout->setAttribute("a_texCoord", iter->second.location, backend::VertexFormat::FLOAT2,
                                   offsetof(V2F_C4B_T2F, texCoords), false);
    }

    iter = attributeInfo.find("a_color");
    if (iter != attributeInfo.end())
    {
        vertexLayout->setAttribute("a_color", iter->second.location, backend::VertexFormat::UBYTE4,
                                   offsetof(V2F_C4B_T2F, colors), true);
    }
    vertexLayout->setStride(sizeof(V2F_C4B_T2F));
}

void VertexLayoutHelper::setupForDrawNode3D(Program* program)
{
    auto vertexLayout = program->getVertexLayout();

    const auto&& attributeInfo = program->getActiveAttributes();
    auto iter                  = attributeInfo.find("a_position");
    if (iter != attributeInfo.end())
    {
        vertexLayout->setAttribute("a_position", iter->second.location, backend::VertexFormat::FLOAT3, 0, false);
    }
    iter = attributeInfo.find("a_color");
    if (iter != attributeInfo.end())
    {
        vertexLayout->setAttribute("a_color", iter->second.location, backend::VertexFormat::UBYTE4, sizeof(Vec3), true);
    }
    vertexLayout->setStride(sizeof(V3F_C4B));
}

void VertexLayoutHelper::setupForSkyBox(Program* program)
{
    auto vertexLayout = program->getVertexLayout();
    auto attrNameLoc  = program->getAttributeLocation(shaderinfos::attribute::ATTRIBUTE_NAME_POSITION);
    vertexLayout->setAttribute(shaderinfos::attribute::ATTRIBUTE_NAME_POSITION, attrNameLoc,
                               backend::VertexFormat::FLOAT3, 0, false);
    vertexLayout->setStride(sizeof(Vec3));
}

void VertexLayoutHelper::setupForPU3D(Program* program)
{
    auto vertexLayout          = program->getVertexLayout();
    const auto&& attributeInfo = program->getActiveAttributes();
    auto iter                  = attributeInfo.find("a_position");
    if (iter != attributeInfo.end())
    {
        vertexLayout->setAttribute("a_position", iter->second.location, backend::VertexFormat::FLOAT3,
                                   offsetof(V3F_T2F_C4F, position), false);
    }
    iter = attributeInfo.find("a_texCoord");
    if (iter != attributeInfo.end())
    {
        vertexLayout->setAttribute("a_texCoord", iter->second.location, backend::VertexFormat::FLOAT2,
                                   offsetof(V3F_T2F_C4F, uv), false);
    }
    iter = attributeInfo.find("a_color");
    if (iter != attributeInfo.end())
    {
        vertexLayout->setAttribute("a_color", iter->second.location, backend::VertexFormat::FLOAT4,
                                   offsetof(V3F_T2F_C4F, color), false);
    }
    vertexLayout->setStride(sizeof(V3F_T2F_C4F));
}

void VertexLayoutHelper::setupForPos(Program* program)
{
    auto vertexLayout          = program->getVertexLayout();
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT2, 0, false);
    vertexLayout->setStride(sizeof(Vec2));
}

void VertexLayoutHelper::setupForPosColor(Program* program)
{
    auto vertexLayout = program->getVertexLayout();
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_POSITION,
                               program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT3, 0, false);
    vertexLayout->setAttribute(backend::ATTRIBUTE_NAME_COLOR,
                               program->getAttributeLocation(backend::Attribute::COLOR),
                               backend::VertexFormat::FLOAT4, offsetof(V3F_C4F, colors), false);
    vertexLayout->setStride(sizeof(V3F_C4F));
    
}

void VertexLayoutHelper::setupForTerrain3D(Program* program) {
    auto vertexLayout = program->getVertexLayout();
    vertexLayout->setAttribute("a_position", program->getAttributeLocation(backend::Attribute::POSITION),
                               backend::VertexFormat::FLOAT3, 0, false);
    vertexLayout->setAttribute("a_texCoord", program->getAttributeLocation(backend::Attribute::TEXCOORD),
                               backend::VertexFormat::FLOAT2, offsetof(V3F_T2F_N3F, texcoord), false);
    vertexLayout->setAttribute("a_normal",program->getAttributeLocation(backend::Attribute::NORMAL), backend::VertexFormat::FLOAT3,
                                    offsetof(V3F_T2F_N3F, normal), false);
    vertexLayout->setStride(sizeof(V3F_T2F_N3F));
}

// ### end of vertex layout setup functions

bool ProgramCache::init()
{
    registerProgramFactory(ProgramType::POSITION_TEXTURE_COLOR, positionTextureColor_vert, positionTextureColor_frag,
                           VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::DUAL_SAMPLER, positionTextureColor_vert, dualSampler_frag,
                           VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::LABEL_DISTANCE_NORMAL, positionTextureColor_vert, label_distanceNormal_frag,
                           VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::LABEL_NORMAL, positionTextureColor_vert, label_normal_frag,
                           VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::LABLE_OUTLINE, positionTextureColor_vert, labelOutline_frag,
                           VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::LABLE_DISTANCEFIELD_GLOW, positionTextureColor_vert,
                           labelDistanceFieldGlow_frag, VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::POSITION_COLOR_LENGTH_TEXTURE, positionColorLengthTexture_vert,
                           positionColorLengthTexture_frag, VertexLayoutHelper::setupForDrawNode);
    registerProgramFactory(ProgramType::POSITION_COLOR_TEXTURE_AS_POINTSIZE, positionColorTextureAsPointsize_vert,
                           positionColor_frag, VertexLayoutHelper::setupForDrawNode);
    registerProgramFactory(ProgramType::POSITION_COLOR, positionColor_vert, positionColor_frag,
                           VertexLayoutHelper::setupForPosColor);
    registerProgramFactory(ProgramType::LAYER_RADIA_GRADIENT, position_vert, layer_radialGradient_frag,
                           VertexLayoutHelper::setupForPos);
    registerProgramFactory(ProgramType::POSITION_TEXTURE, positionTexture_vert, positionTexture_frag,
                           VertexLayoutHelper::setupForTexture);
    registerProgramFactory(ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST, positionTextureColor_vert,
                           positionTextureColorAlphaTest_frag, VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::POSITION_UCOLOR, positionUColor_vert, positionColor_frag,
                           VertexLayoutHelper::setupForPos);
    registerProgramFactory(ProgramType::DUAL_SAMPLER_GRAY, positionTextureColor_vert, dualSampler_gray_frag,
                           VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::GRAY_SCALE, positionTextureColor_vert, grayScale_frag,
                           VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::LINE_COLOR_3D, lineColor3D_vert, lineColor3D_frag,
                           VertexLayoutHelper::setupForDrawNode3D);
    registerProgramFactory(ProgramType::CAMERA_CLEAR, cameraClear_vert, cameraClear_frag,
                           VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::SKYBOX_3D, CC3D_skybox_vert, CC3D_skybox_frag,
                           VertexLayoutHelper::setupForSkyBox);
    registerProgramFactory(ProgramType::SKINPOSITION_TEXTURE_3D, CC3D_skinPositionTexture_vert, CC3D_colorTexture_frag,
                           VertexLayoutHelper::setupForDummy);
    auto lightDef = getShaderMacrosForLight();
    registerProgramFactory(ProgramType::SKINPOSITION_NORMAL_TEXTURE_3D, lightDef + CC3D_skinPositionNormalTexture_vert,
                           lightDef + CC3D_colorNormalTexture_frag, VertexLayoutHelper::setupForDummy);
    registerProgramFactory(ProgramType::POSITION_NORMAL_TEXTURE_3D, lightDef + CC3D_positionNormalTexture_vert,
                           lightDef + CC3D_colorNormalTexture_frag, VertexLayoutHelper::setupForDummy);
    registerProgramFactory(ProgramType::POSITION_TEXTURE_3D, CC3D_positionTexture_vert, CC3D_colorTexture_frag,
                           VertexLayoutHelper::setupForDummy);
    registerProgramFactory(ProgramType::POSITION_3D, CC3D_positionTexture_vert, CC3D_color_frag,
                           VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::POSITION_NORMAL_3D, lightDef + CC3D_positionNormalTexture_vert,
                           lightDef + CC3D_colorNormal_frag, VertexLayoutHelper::setupForDummy);
    const char* normalMapDef = "\n#define USE_NORMAL_MAPPING 1 \n";
    registerProgramFactory(ProgramType::POSITION_BUMPEDNORMAL_TEXTURE_3D,
                           lightDef + normalMapDef + CC3D_positionNormalTexture_vert,
                           lightDef + normalMapDef + CC3D_colorNormalTexture_frag, VertexLayoutHelper::setupForDummy);
    registerProgramFactory(ProgramType::SKINPOSITION_BUMPEDNORMAL_TEXTURE_3D,
                           lightDef + normalMapDef + CC3D_skinPositionNormalTexture_vert,
                           lightDef + normalMapDef + CC3D_colorNormalTexture_frag, VertexLayoutHelper::setupForDummy);
    registerProgramFactory(ProgramType::TERRAIN_3D, CC3D_terrain_vert, CC3D_terrain_frag,
                           VertexLayoutHelper::setupForTerrain3D);
    registerProgramFactory(ProgramType::PARTICLE_TEXTURE_3D, CC3D_particle_vert, CC3D_particleTexture_frag,
                           VertexLayoutHelper::setupForPU3D);
    registerProgramFactory(ProgramType::PARTICLE_COLOR_3D, CC3D_particle_vert, CC3D_particleColor_frag,
                           VertexLayoutHelper::setupForPU3D);
    registerProgramFactory(ProgramType::QUAD_COLOR_2D, CC2D_quadColor_vert, CC2D_quadColor_frag,
                           VertexLayoutHelper::setupForDummy);
    registerProgramFactory(ProgramType::QUAD_TEXTURE_2D, CC2D_quadTexture_vert, CC2D_quadTexture_frag,
                           VertexLayoutHelper::setupForDummy);
    registerProgramFactory(ProgramType::HSV, positionTextureColor_vert, hsv_frag, VertexLayoutHelper::setupForSprite);
    registerProgramFactory(ProgramType::HSV_DUAL_SAMPLER, positionTextureColor_vert, dualSampler_hsv_frag,
                           VertexLayoutHelper::setupForSprite);

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

void ProgramCache::registerCustomProgramFactory(uint32_t type,
                                                std::string vertShaderSource,
                                                std::string fragShaderSource,
                                                std::function<void(Program*)> cbSetupLayout)
{
    auto internalType = ProgramType::CUSTOM_PROGRAM | type;
    registerProgramFactory(internalType, std::move(vertShaderSource), std::move(fragShaderSource),
                           std::move(cbSetupLayout));
}

void ProgramCache::registerProgramFactory(uint32_t internalType,
                                          std::string&& vertShaderSource,
                                          std::string&& fragShaderSource,
                                          std::function<void(Program*)> cbSetupLayout)
{
    auto constructProgram = [vsrc = std::move(vertShaderSource), fsrc = std::move(fragShaderSource),
                             setupLayout = std::move(cbSetupLayout)]() {
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
            //            AXLOG("axys: TextureCache: removing unused program");
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
    for (auto&& program : _cachedPrograms)
    {
        program.second->release();
    }
    _cachedPrograms.clear();
}

NS_AX_BACKEND_END
