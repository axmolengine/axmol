/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://adxeproject.github.io/

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

#include "3d/CCMeshRendererMaterial.h"
#include "3d/CCMesh.h"
#include "platform/CCFileUtils.h"
#include "renderer/CCTexture2D.h"
#include "base/CCDirector.h"
#include "base/CCEventType.h"
#include "base/CCConfiguration.h"
#include "renderer/backend/ProgramState.h"
#include "renderer/ccShaders.h"
#include "renderer/CCPass.h"

NS_CC_BEGIN

MeshRendererMaterialCache* MeshRendererMaterialCache::_cacheInstance = nullptr;

std::unordered_map<std::string, MeshRendererMaterial*> MeshRendererMaterial::_materials;
MeshRendererMaterial* MeshRendererMaterial::_unLitMaterial         = nullptr;
MeshRendererMaterial* MeshRendererMaterial::_unLitNoTexMaterial    = nullptr;
MeshRendererMaterial* MeshRendererMaterial::_vertexLitMaterial     = nullptr;
MeshRendererMaterial* MeshRendererMaterial::_diffuseMaterial       = nullptr;
MeshRendererMaterial* MeshRendererMaterial::_diffuseNoTexMaterial  = nullptr;
MeshRendererMaterial* MeshRendererMaterial::_bumpedDiffuseMaterial = nullptr;

MeshRendererMaterial* MeshRendererMaterial::_unLitMaterialSkin         = nullptr;
MeshRendererMaterial* MeshRendererMaterial::_vertexLitMaterialSkin     = nullptr;
MeshRendererMaterial* MeshRendererMaterial::_diffuseMaterialSkin       = nullptr;
MeshRendererMaterial* MeshRendererMaterial::_bumpedDiffuseMaterialSkin = nullptr;

backend::ProgramState* MeshRendererMaterial::_unLitMaterialProgState         = nullptr;
backend::ProgramState* MeshRendererMaterial::_unLitNoTexMaterialProgState    = nullptr;
backend::ProgramState* MeshRendererMaterial::_vertexLitMaterialProgState     = nullptr;
backend::ProgramState* MeshRendererMaterial::_diffuseMaterialProgState       = nullptr;
backend::ProgramState* MeshRendererMaterial::_diffuseNoTexMaterialProgState  = nullptr;
backend::ProgramState* MeshRendererMaterial::_bumpedDiffuseMaterialProgState = nullptr;

backend::ProgramState* MeshRendererMaterial::_unLitMaterialSkinProgState         = nullptr;
backend::ProgramState* MeshRendererMaterial::_vertexLitMaterialSkinProgState     = nullptr;
backend::ProgramState* MeshRendererMaterial::_diffuseMaterialSkinProgState       = nullptr;
backend::ProgramState* MeshRendererMaterial::_bumpedDiffuseMaterialSkinProgState = nullptr;

void MeshRendererMaterial::createBuiltInMaterial()
{
    auto* program               = backend::Program::getBuiltinProgram(backend::ProgramType::SKINPOSITION_TEXTURE_3D);
    _unLitMaterialSkinProgState = new backend::ProgramState(program);
    _unLitMaterialSkin          = new MeshRendererMaterial();
    if (_unLitMaterialSkin && _unLitMaterialSkin->initWithProgramState(_unLitMaterialSkinProgState))
    {
        _unLitMaterialSkin->_type = MeshRendererMaterial::MaterialType::UNLIT;
    }

    program = backend::Program::getBuiltinProgram(backend::ProgramType::SKINPOSITION_NORMAL_TEXTURE_3D);
    _diffuseMaterialSkinProgState = new backend::ProgramState(program);
    _diffuseMaterialSkin          = new MeshRendererMaterial();
    if (_diffuseMaterialSkin && _diffuseMaterialSkin->initWithProgramState(_diffuseMaterialSkinProgState))
    {
        _diffuseMaterialSkin->_type = MeshRendererMaterial::MaterialType::DIFFUSE;
    }

    program                   = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_NORMAL_TEXTURE_3D);
    _diffuseMaterialProgState = new backend::ProgramState(program);
    _diffuseMaterial          = new MeshRendererMaterial();
    if (_diffuseMaterial && _diffuseMaterial->initWithProgramState(_diffuseMaterialProgState))
    {
        _diffuseMaterial->_type = MeshRendererMaterial::MaterialType::DIFFUSE;
    }

    program                 = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_TEXTURE_3D);
    _unLitMaterialProgState = new backend::ProgramState(program);
    _unLitMaterial          = new MeshRendererMaterial();
    if (_unLitMaterial && _unLitMaterial->initWithProgramState(_unLitMaterialProgState))
    {
        _unLitMaterial->_type = MeshRendererMaterial::MaterialType::UNLIT;
    }

    program                      = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_3D);
    _unLitNoTexMaterialProgState = new backend::ProgramState(program);
    _unLitNoTexMaterial          = new MeshRendererMaterial();
    if (_unLitNoTexMaterial && _unLitNoTexMaterial->initWithProgramState(_unLitNoTexMaterialProgState))
    {
        _unLitNoTexMaterial->_type = MeshRendererMaterial::MaterialType::UNLIT_NOTEX;
    }

    program                        = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_NORMAL_3D);
    _diffuseNoTexMaterialProgState = new backend::ProgramState(program);
    _diffuseNoTexMaterial          = new MeshRendererMaterial();
    if (_diffuseNoTexMaterial && _diffuseNoTexMaterial->initWithProgramState(_diffuseNoTexMaterialProgState))
    {
        _diffuseNoTexMaterial->_type = MeshRendererMaterial::MaterialType::DIFFUSE_NOTEX;
    }

    program = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_BUMPEDNORMAL_TEXTURE_3D);
    _bumpedDiffuseMaterialProgState = new backend::ProgramState(program);
    _bumpedDiffuseMaterial          = new MeshRendererMaterial();
    if (_bumpedDiffuseMaterial && _bumpedDiffuseMaterial->initWithProgramState(_bumpedDiffuseMaterialProgState))
    {
        _bumpedDiffuseMaterial->_type = MeshRendererMaterial::MaterialType::BUMPED_DIFFUSE;
    }

    program = backend::Program::getBuiltinProgram(backend::ProgramType::SKINPOSITION_BUMPEDNORMAL_TEXTURE_3D);
    _bumpedDiffuseMaterialSkinProgState = new backend::ProgramState(program);
    _bumpedDiffuseMaterialSkin          = new MeshRendererMaterial();
    if (_bumpedDiffuseMaterialSkin &&
        _bumpedDiffuseMaterialSkin->initWithProgramState(_bumpedDiffuseMaterialSkinProgState))
    {
        _bumpedDiffuseMaterialSkin->_type = MeshRendererMaterial::MaterialType::BUMPED_DIFFUSE;
    }
}

void MeshRendererMaterial::releaseBuiltInMaterial()
{
    CC_SAFE_RELEASE_NULL(_unLitMaterial);
    CC_SAFE_RELEASE_NULL(_unLitMaterialSkin);

    CC_SAFE_RELEASE_NULL(_unLitNoTexMaterial);
    CC_SAFE_RELEASE_NULL(_vertexLitMaterial);
    CC_SAFE_RELEASE_NULL(_diffuseMaterial);
    CC_SAFE_RELEASE_NULL(_diffuseNoTexMaterial);
    CC_SAFE_RELEASE_NULL(_bumpedDiffuseMaterial);

    CC_SAFE_RELEASE_NULL(_vertexLitMaterialSkin);
    CC_SAFE_RELEASE_NULL(_diffuseMaterialSkin);
    CC_SAFE_RELEASE_NULL(_bumpedDiffuseMaterialSkin);
    // release program states
    CC_SAFE_RELEASE_NULL(_unLitMaterialProgState);
    CC_SAFE_RELEASE_NULL(_unLitNoTexMaterialProgState);
    CC_SAFE_RELEASE_NULL(_vertexLitMaterialProgState);
    CC_SAFE_RELEASE_NULL(_diffuseMaterialProgState);
    CC_SAFE_RELEASE_NULL(_diffuseNoTexMaterialProgState);
    CC_SAFE_RELEASE_NULL(_bumpedDiffuseMaterialProgState);

    CC_SAFE_RELEASE_NULL(_unLitMaterialSkinProgState);
    CC_SAFE_RELEASE_NULL(_vertexLitMaterialSkinProgState);
    CC_SAFE_RELEASE_NULL(_diffuseMaterialSkinProgState);
    CC_SAFE_RELEASE_NULL(_bumpedDiffuseMaterialSkinProgState);
}

void MeshRendererMaterial::releaseCachedMaterial()
{
    for (auto& it : _materials)
    {
        if (it.second)
            it.second->release();
    }
    _materials.clear();
}

Material* MeshRendererMaterial::clone() const
{
    auto material = new MeshRendererMaterial();

    material->_renderState = _renderState;

    for (const auto& technique : _techniques)
    {
        auto t = technique->clone();
        t->setMaterial(material);
        for (ssize_t i = 0; i < t->getPassCount(); i++)
        {
            t->getPassByIndex(i)->setTechnique(t);
        }
        material->_techniques.pushBack(t);
    }

    // current technique
    auto name                   = _currentTechnique->getName();
    material->_currentTechnique = material->getTechniqueByName(name);
    material->_type             = _type;
    material->autorelease();

    return material;
}

MeshRendererMaterial* MeshRendererMaterial::createBuiltInMaterial(MaterialType type, bool skinned)
{
    /////
    if (_diffuseMaterial == nullptr)
        createBuiltInMaterial();

    MeshRendererMaterial* material = nullptr;
    switch (type)
    {
    case MeshRendererMaterial::MaterialType::UNLIT:
        material = skinned ? _unLitMaterialSkin : _unLitMaterial;
        break;

    case MeshRendererMaterial::MaterialType::UNLIT_NOTEX:
        material = _unLitNoTexMaterial;
        break;

    case MeshRendererMaterial::MaterialType::VERTEX_LIT:
        CCASSERT(0, "not implemented.");
        break;

    case MeshRendererMaterial::MaterialType::DIFFUSE:
        material = skinned ? _diffuseMaterialSkin : _diffuseMaterial;
        break;

    case MeshRendererMaterial::MaterialType::DIFFUSE_NOTEX:
        material = _diffuseNoTexMaterial;
        break;

    case MeshRendererMaterial::MaterialType::BUMPED_DIFFUSE:
        material = skinned ? _bumpedDiffuseMaterialSkin : _bumpedDiffuseMaterial;
        break;

    default:
        break;
    }
    if (material)
        return (MeshRendererMaterial*)material->clone();

    return nullptr;
}

MeshRendererMaterial* MeshRendererMaterial::createWithFilename(std::string_view path)
{
    auto validfilename = FileUtils::getInstance()->fullPathForFilename(path);
    if (!validfilename.empty())
    {
        auto it = _materials.find(validfilename);
        if (it != _materials.end())
            return (MeshRendererMaterial*)it->second->clone();

        auto material = new MeshRendererMaterial();
        if (material->initWithFile(path))
        {
            material->_type           = MeshRendererMaterial::MaterialType::CUSTOM;
            _materials[validfilename] = material;

            return (MeshRendererMaterial*)material->clone();
        }
        CC_SAFE_DELETE(material);
    }
    return nullptr;
}

MeshRendererMaterial* MeshRendererMaterial::createWithProgramState(backend::ProgramState* programState)
{
    CCASSERT(programState, "Invalid program state.");

    auto mat = new MeshRendererMaterial();
    if (mat->initWithProgramState(programState))
    {
        mat->_type = MeshRendererMaterial::MaterialType::CUSTOM;
        mat->autorelease();
        return mat;
    }
    CC_SAFE_DELETE(mat);
    return nullptr;
}

void MeshRendererMaterial::setTexture(Texture2D* tex, NTextureData::Usage usage)
{
    const auto& passes = getTechnique()->getPasses();
    for (auto& pass : passes)
    {
        pass->setUniformTexture(0, tex->getBackendTexture());
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////

MeshRendererMaterialCache::MeshRendererMaterialCache() {}

MeshRendererMaterialCache::~MeshRendererMaterialCache()
{
    removeAllMeshRendererMaterial();
}

MeshRendererMaterialCache* MeshRendererMaterialCache::getInstance()
{
    if (!_cacheInstance)
    {
        _cacheInstance = new MeshRendererMaterialCache();
    }

    return _cacheInstance;
}

void MeshRendererMaterialCache::destroyInstance()
{
    if (_cacheInstance)
    {
        CC_SAFE_DELETE(_cacheInstance);
    }
}

bool MeshRendererMaterialCache::addMeshRendererMaterial(std::string_view key, Texture2D* texture)
{
    auto itr = _materials.find(key);
    if (itr == _materials.end())
    {
        CC_SAFE_RETAIN(texture);
        _materials.emplace(key, texture);
        return true;
    }
    return false;
}

Texture2D* MeshRendererMaterialCache::getMeshRendererMaterial(std::string_view key)
{
    auto itr = _materials.find(key);
    if (itr != _materials.end())
    {
        return itr->second;
    }
    return nullptr;
}

void MeshRendererMaterialCache::removeAllMeshRendererMaterial()
{
    for (auto& itr : _materials)
    {
        CC_SAFE_RELEASE_NULL(itr.second);
    }
    _materials.clear();
}
void MeshRendererMaterialCache::removeUnusedMeshRendererMaterial()
{
    for (auto it = _materials.cbegin(), itCend = _materials.cend(); it != itCend; /* nothing */)
    {
        auto value = it->second;
        if (value->getReferenceCount() == 1)
        {
            CCLOG("cocos2d: MeshRendererMaterialCache: removing unused mesh renderer materials.");

            value->release();
            it = _materials.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

NS_CC_END
