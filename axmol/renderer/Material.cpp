/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

 Ideas taken from:
 - GamePlay3D: http://gameplay3d.org/
 - OGRE3D: http://www.ogre3d.org/
 - Qt3D: http://qt-project.org/
 ****************************************************************************/
#include "axmol/renderer/Material.h"
#include "axmol/renderer/Technique.h"
#include "axmol/renderer/Pass.h"
#include "axmol/renderer/TextureCache.h"
#include "axmol/renderer/Texture2D.h"
#include "axmol/rhi/DriverBase.h"
#include "axmol/base/Properties.h"
#include "axmol/base/Director.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/base/Logging.h"
#include "axmol/tlx/utility.hpp"
#include "yasio/tlx/string_view.hpp"

#include <sstream>

#if defined(_WIN32)
#    define strcasecmp _stricmp
#endif

namespace ax
{

// Helpers declaration
static inline bool isValidUniform(std::string_view name)
{
    return !(name == "defines"sv || name == "vertexShader"sv || name == "fragmentShader"sv);
}

Material* Material::createWithFilename(std::string_view filepath)
{
    AXLOGD("Loading material: {}", filepath);
    auto validfilename = FileUtils::getInstance()->fullPathForFilename(filepath);
    if (!validfilename.empty())
    {
        auto mat = new Material();
        if (mat->initWithFile(validfilename))
        {
            mat->autorelease();
            return mat;
        }
        delete mat;
    }

    return nullptr;
}

Material* Material::createWithProperties(Properties* materialProperties)
{
    auto mat = new Material();
    if (mat->initWithProperties(materialProperties))
    {
        mat->autorelease();
        return mat;
    }
    delete mat;
    return nullptr;
}

Material* Material::createWithProgramState(rhi::ProgramState* programState)
{
    AXASSERT(programState, "Invalid Program State");

    auto mat = new Material();
    if (mat->initWithProgramState(programState))
    {
        mat->autorelease();
        return mat;
    }
    delete mat;
    return nullptr;
}

bool Material::initWithProgramState(rhi::ProgramState* state)
{
    auto technique = Technique::createWithProgramState(this, state);
    if (technique)
    {
        _techniques.pushBack(technique);

        // weak pointer
        _currentTechnique = technique;

        return true;
    }
    return false;
}

bool Material::initWithFile(std::string_view validfilename)
{
    // Warning: properties is not a "Object" object, must be manually deleted
    Properties* properties = Properties::createNonRefCounted(validfilename);

    // get the first material
    parseProperties(!properties->getNamespace().empty() ? properties : properties->getNextNamespace());

    AX_SAFE_DELETE(properties);
    return true;
}

bool Material::initWithProperties(Properties* materialProperties)
{
    return parseProperties(materialProperties);
}

void Material::draw(MeshCommand* meshCommands,
                    float globalZOrder,
                    rhi::Buffer* vertexBuffer,
                    rhi::Buffer* indexBuffer,
                    CustomCommand::PrimitiveType primitive,
                    CustomCommand::IndexFormat indexFormat,
                    unsigned int indexCount,
                    const Mat4& modelView)
{
    int i = 0;
    for (const auto& pass : _currentTechnique->_passes)
    {
        pass->draw(&meshCommands[i], globalZOrder, vertexBuffer, indexBuffer, primitive, indexFormat, indexCount,
                   modelView);
        i++;
    }
}

void Material::setTarget(ax::Node* target)
{
    _target = target;
}

bool Material::parseProperties(Properties* materialProperties)
{
    setName(materialProperties->getId());
    auto space = materialProperties->getNextNamespace();
    while (space)
    {
        auto name = space->getNamespace();
        if (name == "technique"sv)
        {
            parseTechnique(space);
        }
        else if (name == "renderState"sv)
        {
            parseRenderState(&_renderState.getStateBlock(), space);
        }

        space = materialProperties->getNextNamespace();
    }
    return true;
}

bool Material::parseTechnique(Properties* techniqueProperties)
{
    auto technique = Technique::create(this);
    _techniques.pushBack(technique);

    // first one is the default one
    if (!_currentTechnique)
        _currentTechnique = technique;

    // name
    technique->setName(techniqueProperties->getId());

    // passes
    auto space = techniqueProperties->getNextNamespace();
    while (space)
    {
        std::string_view name = space->getNamespace();
        if (name == "pass"sv)
        {
            parsePass(technique, space);
        }
        else if (name == "renderState"sv)
        {
            parseRenderState(&technique->getStateBlock(), space);
        }

        space = techniqueProperties->getNextNamespace();
    }

    return true;
}

bool Material::parsePass(Technique* technique, Properties* passProperties)
{
    auto pass = Pass::create(technique);
    technique->addPass(pass);

    pass->setName(passProperties->getId());

    // Pass can have 3 different namespaces:
    //  - one or more "sampler"
    //  - one "renderState"
    //  - one "shader"

    auto space = passProperties->getNextNamespace();
    while (space)
    {
        std::string_view name = space->getNamespace();
        if (name == "shader"sv)
        {
            parseShader(pass, space);
        }
        else if (name == "renderState"sv)
        {
            parseRenderState(&pass->_renderState.getStateBlock(), space);
        }
        else
        {
            AXASSERT(false, "Invalid namespace");
            return false;
        }

        space = passProperties->getNextNamespace();
    }

    return true;
}

// axmol doesn't support Samplers yet.
bool Material::parseSampler(rhi::ProgramState* programState, Properties* samplerProperties)
{
    AXASSERT(!samplerProperties->getId().empty(), "Sampler must have an id. The id is the uniform name");

    // mipmap
    std::string_view mipmap = samplerProperties->getString("mipmap"sv, "false"sv);
    bool usemipmap          = tlx::ic::iequals(mipmap, "true"sv);
    // required
    auto filename = samplerProperties->getString("path"sv);

    auto texture = Director::getInstance()->getTextureCache()->addImage(filename, usemipmap);
    if (!texture)
    {
        AXLOGW("Invalid filepath");
        return false;
    }

    // optionals
    {
        Texture2D::TexParams texParams;

        // valid options: REPEAT, CLAMP
        auto wrapS = samplerProperties->getString("wrapS"sv, "CLAMP_TO_EDGE"sv);
        if (tlx::ic::iequals(wrapS, "REPEAT"sv))
            texParams.sAddressMode = rhi::SamplerAddressMode::REPEAT;
        else if (tlx::ic::iequals(wrapS, "CLAMP_TO_EDGE"sv))
            texParams.sAddressMode = rhi::SamplerAddressMode::CLAMP;
        else
            AXLOGW("Invalid wrapS: {}", wrapS);

        // valid options: REPEAT, CLAMP
        auto wrapT = samplerProperties->getString("wrapT"sv, "CLAMP_TO_EDGE"sv);
        if (tlx::ic::iequals(wrapT, "REPEAT"sv))
            texParams.tAddressMode = rhi::SamplerAddressMode::REPEAT;
        else if (tlx::ic::iequals(wrapT, "CLAMP_TO_EDGE"sv))
            texParams.tAddressMode = rhi::SamplerAddressMode::CLAMP;
        else
            AXLOGW("Invalid wrapT: {}", wrapT);

        // valid options: NEAREST, LINEAR, NEAREST_MIPMAP_NEAREST, LINEAR_MIPMAP_NEAREST, NEAREST_MIPMAP_LINEAR,
        // LINEAR_MIPMAP_LINEAR
        auto minFilter =
            samplerProperties->getString("minFilter"sv, usemipmap ? "LINEAR_MIPMAP_NEAREST"sv : "LINEAR"sv);
        if (tlx::ic::iequals(minFilter, "NEAREST"sv))
            texParams.minFilter = rhi::SamplerFilter::MIN_NEAREST;
        else if (tlx::ic::iequals(minFilter, "LINEAR"sv))
            texParams.minFilter = rhi::SamplerFilter::MIN_LINEAR;
        else if (tlx::ic::iequals(minFilter, "NEAREST_MIPMAP_NEAREST"sv))
        {
            texParams.minFilter = rhi::SamplerFilter::MIN_NEAREST;
            texParams.mipFilter = rhi::SamplerFilter::MIP_NEAREST;
        }
        else if (tlx::ic::iequals(minFilter, "LINEAR_MIPMAP_NEAREST"sv))
        {
            texParams.minFilter = rhi::SamplerFilter::MIN_LINEAR;
            texParams.mipFilter = rhi::SamplerFilter::MIP_NEAREST;
        }
        else if (tlx::ic::iequals(minFilter, "NEAREST_MIPMAP_LINEAR"sv))
        {
            texParams.minFilter = rhi::SamplerFilter::MIN_NEAREST;
            texParams.mipFilter = rhi::SamplerFilter::MIP_LINEAR;
        }
        else if (tlx::ic::iequals(minFilter, "LINEAR_MIPMAP_LINEAR"sv))
        {
            texParams.minFilter = rhi::SamplerFilter::LINEAR;
            texParams.mipFilter = rhi::SamplerFilter::MIP_LINEAR;
        }
        else
            AXLOGW("Invalid minFilter: {}", minFilter);

        // valid options: NEAREST, LINEAR
        auto magFilter = samplerProperties->getString("magFilter"sv, "LINEAR"sv);
        if (tlx::ic::iequals(magFilter, "NEAREST"sv))
            texParams.magFilter = rhi::SamplerFilter::NEAREST;
        else if (tlx::ic::iequals(magFilter, "LINEAR"sv))
            texParams.magFilter = rhi::SamplerFilter::LINEAR;
        else
            AXLOGW("Invalid magFilter: {}", magFilter);

        texture->setTexParameters(texParams);
    }

    auto textureName = samplerProperties->getId();
    auto location    = programState->getUniformLocation(textureName);

    if (!location)
    {
        AXLOGW("warning: failed to find texture uniform location {} when parsing material", textureName);
        return false;
    }

    if (_textureSlots.find(textureName) == _textureSlots.end())
    {
        _textureSlots[textureName] = _textureSlotIndex;
        programState->setTexture(location, _textureSlotIndex++, texture->getRHITexture());
    }
    else
    {
        programState->setTexture(location, _textureSlots[textureName], texture->getRHITexture());
    }

    return true;
}

bool Material::parseShader(Pass* pass, Properties* shaderProperties)
{
    // vertexShader
    std::string_view vertShader = shaderProperties->getString("vertexShader"sv);

    // fragmentShader
    std::string_view fragShader = shaderProperties->getString("fragmentShader"sv);

    // compileTimeDefines, since axmol-1.1 no longer support compile time defines
    // const char* compileTimeDefines = shaderProperties->getString("defines"sv);

    if (!vertShader.empty() && !fragShader.empty())
    {
        auto program      = ProgramManager::getInstance()->loadProgram(vertShader, fragShader);
        auto programState = new rhi::ProgramState(program);
        pass->setProgramState(programState);

        // Parse uniforms only if the ProgramState was created
        auto property = shaderProperties->getNextProperty();
        while (!property.empty())
        {
            if (isValidUniform(property))
            {
                parseUniform(programState, shaderProperties, property);
            }

            property = shaderProperties->getNextProperty();
        }

        auto space = shaderProperties->getNextNamespace();
        while (space)
        {
            auto name = space->getNamespace();
            if (name == "sampler"sv)
            {
                parseSampler(programState, space);
            }
            space = shaderProperties->getNextNamespace();
        }
        programState->release();
    }

    return true;
}

bool Material::parseUniform(rhi::ProgramState* programState, Properties* properties, std::string_view uniformName)
{
    bool ret = true;

    auto type = properties->getType(uniformName);

    rhi::UniformLocation location;
    location = programState->getUniformLocation(uniformName);

    switch (type)
    {
    case Properties::Type::NUMBER:
    {
        auto f = properties->getFloat(uniformName);
        programState->setUniform(location, &f, sizeof(f));
        break;
    }

    case Properties::Type::VECTOR2:
    {
        Vec2 v2;
        properties->getVec2(uniformName, &v2);
        programState->setUniform(location, &v2, sizeof(v2));
        break;
    }

    case Properties::Type::VECTOR3:
    {
        Vec3 v3;
        properties->getVec3(uniformName, &v3);
        programState->setUniform(location, &v3, sizeof(v3));
        break;
    }

    case Properties::Type::VECTOR4:
    {
        Vec4 v4;
        properties->getVec4(uniformName, &v4);
        programState->setUniform(location, &v4, sizeof(v4));
        break;
    }

    case Properties::Type::MATRIX:
    {
        Mat4 m4;
        properties->getMat4(uniformName, &m4);
        programState->setUniform(location, &m4.m, sizeof(m4.m));
        break;
    }

    case Properties::Type::STRING:
    default:
    {
        // Assume this is a parameter auto-binding.
        programState->setParameterAutoBinding(uniformName, properties->getString());
        break;
    }
    }
    return ret;
}

bool Material::parseRenderState(RenderState::StateBlock* state, Properties* properties)
{
    if (nullptr == state)
    {
        return false;
    }

    auto property = properties->getNextProperty();
    while (!property.empty())
    {
        // Parse uniforms only if the ProgramState was created
        // Render state only can have "strings" or numbers as values. No new namespaces
        state->setState(property, properties->getString(property));

        property = properties->getNextProperty();
    }

    return true;
}

void Material::setName(std::string_view name)
{
    _name = name;
}

std::string_view Material::getName() const
{
    return _name;
}

void Material::setTransparent(bool value)
{
    _isTransparent = value;
    getStateBlock().setBlend(_force2DQueue || _isTransparent);
}

void Material::setForce2DQueue(bool value)
{
    _force2DQueue = value;
    getStateBlock().setBlend(_force2DQueue || _isTransparent);
}

Material::Material() : _name(""), _currentTechnique(nullptr), _target(nullptr) {}

Material::~Material() {}

Material* Material::clone() const
{
    auto material = new Material();

    material->_renderState = _renderState;

    for (const auto& technique : _techniques)
    {
        auto t       = technique->clone();
        t->_material = material;
        material->_techniques.pushBack(t);
    }

    // current technique
    auto&& name                 = _currentTechnique->getName();
    material->_currentTechnique = material->getTechniqueByName(name);
    material->_textureSlots     = material->_textureSlots;
    material->_textureSlotIndex = material->_textureSlotIndex;
    material->autorelease();
    return material;
}

Technique* Material::getTechnique() const
{
    return _currentTechnique;
}

const Vector<Technique*>& Material::getTechniques() const
{
    return _techniques;
}

Technique* Material::getTechniqueByName(std::string_view name)
{
    for (const auto& technique : _techniques)
    {
        if (technique->getName().compare(name) == 0)
            return technique;
    }
    return nullptr;
}

Technique* Material::getTechniqueByIndex(ssize_t index)
{
    AX_ASSERT(index >= 0 && index < _techniques.size() && "Invalid size");

    return _techniques.at(index);
}

void Material::addTechnique(Technique* technique)
{
    _techniques.pushBack(technique);
}

void Material::setTechnique(std::string_view techniqueName)
{
    auto technique = getTechniqueByName(techniqueName);
    if (technique)
        _currentTechnique = technique;
}

ssize_t Material::getTechniqueCount() const
{
    return _techniques.size();
}

}  // namespace ax
