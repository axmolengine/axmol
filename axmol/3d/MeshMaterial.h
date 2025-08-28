/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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
 ****************************************************************************/
#pragma once

#include <string>
#include <unordered_map>
#include "axmol/base/Types.h"
#include "axmol/renderer/Material.h"
#include "axmol/3d/Bundle3DData.h"

namespace ax
{

/**
 * @addtogroup _3d
 * @{
 */

class Texture2D;

namespace rhi
{
class ProgramState;
}

/**
 * @brief MeshMaterial: a mesh material for MeshRenderers.
 */
class AX_DLL MeshMaterial : public Material
{
public:
    /**
     * Material types, there are mainly two types of materials. Built-in materials and Custom materials.
     */
    enum class MaterialType
    {
        // Built in materials
        UNLIT,           // unlit material
        UNLIT_INSTANCE,  // unlit instance material
        UNLIT_NOTEX,     // unlit material (without texture)
        VERTEX_LIT,      // vertex lit
        DIFFUSE,         // diffuse (pixel lighting)
        DIFFUSE_NOTEX,   // diffuse (without texture)
        BUMPED_DIFFUSE,  // bumped diffuse
        QUAD_TEXTURE,    // textured quad material
        QUAD_COLOR,      // colored quad material (without texture)

        // Custom material
        CUSTOM,  // Create from a material file
    };

    /**
     * Instanced Material types, there are mainly two types of materials. Built-in materials and Custom materials.
     */
    enum class InstanceMaterialType
    {
        NO_INSTANCING,   // disabled instancing
        UNLIT_INSTANCE,  // unlit instance material

        // Custom material
        CUSTOM,  // Create from a material file
    };

    /**
     * Get material type
     * @return Material type
     */
    MaterialType getMaterialType() const { return _type; }

    /**
     * Create built in material from material type
     * @param type Material type
     * @param skinned Has hardware skinning?
     * @return An autorelease material object
     */
    static MeshMaterial* createBuiltInMaterial(MaterialType type, bool skinned);

    /**
     * Create material with file name, it creates material from cache if it is previously loaded
     * @param path Path of material file
     * @return An autorelease material object
     */
    static MeshMaterial* createWithFilename(std::string_view path);

    /**
     * Create material with a ProgramState
     * @param programState GLProgramState instance
     * @return An autorelease material object
     */
    static MeshMaterial* createWithProgramState(rhi::ProgramState* programState);

    void setTexture(Texture2D* tex, NTextureData::Usage usage);

    /**
     * Create all built-in materials
     */
    static void createBuiltInMaterial();

    /**
     * Release all built-in materials
     */
    static void releaseBuiltInMaterial();

    /**
     * Release all cached materials
     */
    static void releaseCachedMaterial();

    /**
     * Clone this material.
     */
    Material* clone() const override;

protected:
    MaterialType _type;
    static std::unordered_map<std::string, MeshMaterial*> _materials;  // cached material
    static MeshMaterial* _unLitMaterial;
    static MeshMaterial* _unLitInstanceMaterial;
    static MeshMaterial* _unLitNoTexMaterial;
    static MeshMaterial* _vertexLitMaterial;
    static MeshMaterial* _diffuseMaterial;
    static MeshMaterial* _diffuseNoTexMaterial;
    static MeshMaterial* _bumpedDiffuseMaterial;

    static MeshMaterial* _unLitMaterialSkin;
    static MeshMaterial* _vertexLitMaterialSkin;
    static MeshMaterial* _diffuseMaterialSkin;
    static MeshMaterial* _bumpedDiffuseMaterialSkin;

    static MeshMaterial* _quadTextureMaterial;
    static MeshMaterial* _quadColorMaterial;

    static rhi::ProgramState* _unLitMaterialProgState;
    static rhi::ProgramState* _unLitInstanceMaterialProgState;
    static rhi::ProgramState* _unLitNoTexMaterialProgState;
    static rhi::ProgramState* _vertexLitMaterialProgState;
    static rhi::ProgramState* _diffuseMaterialProgState;
    static rhi::ProgramState* _diffuseNoTexMaterialProgState;
    static rhi::ProgramState* _bumpedDiffuseMaterialProgState;

    static rhi::ProgramState* _unLitMaterialSkinProgState;
    static rhi::ProgramState* _vertexLitMaterialSkinProgState;
    static rhi::ProgramState* _diffuseMaterialSkinProgState;
    static rhi::ProgramState* _bumpedDiffuseMaterialSkinProgState;

    static rhi::ProgramState* _quadTextureMaterialProgState;
    static rhi::ProgramState* _quadColorMaterialProgState;
};

/**
 * @brief MeshMaterialCache: the MeshRenderer material cache, it can only cache textures for now.
 * @lua NA
 */
class MeshMaterialCache
{
public:
    static MeshMaterialCache* getInstance();
    static void destroyInstance();

    /** add a material to cache */
    bool addMeshMaterial(std::string_view key, Texture2D* tex);

    /** get material from cache */
    Texture2D* getMeshMaterial(std::string_view key);

    /** remove all cached materials */
    void removeAllMeshMaterial();
    /** remove unused cached materials */
    void removeUnusedMeshMaterial();

    MeshMaterialCache();
    ~MeshMaterialCache();

protected:
    static MeshMaterialCache* _cacheInstance;    // cache instance
    hlookup::string_map<Texture2D*> _materials;  // cached materials
};

// end of 3d group
/// @}

}  // namespace ax
