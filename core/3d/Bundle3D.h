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

#ifndef __CCBUNDLE3D_H__
#define __CCBUNDLE3D_H__

#include "base/Data.h"
#include "3d/Bundle3DData.h"
#include "3d/BundleReader.h"
#include "rapidjson/rapidjson.h"
#include "rapidjson/document.h"


namespace ax
{

/**
 * @addtogroup _3d
 * @{
 */

class Animation3D;

/**
 * @brief Defines a bundle file that contains a collection of assets. Mesh, Material, MeshSkin, Animation
 * There are two types of bundle files, c3t and c3b.
 * c3t text file
 * c3b binary file
 * @js NA
 * @lua NA
 */
class AX_DLL Bundle3D
{
public:
    /**
     * create a new bundle, destroy it when finish using it
     */
    static Bundle3D* createBundle();

    static void destroyBundle(Bundle3D* bundle);

    virtual void clear();

    /**
     * get define data type
     * @param str The type in string
     */
    static backend::VertexFormat parseGLDataType(std::string_view str, int size);

    /**
     * get define data type
     * @param str The type in string
     */
    static backend::SamplerAddressMode parseSamplerAddressMode(std::string_view str);

    /**
     * load a file. You must load a file first, then call loadMeshData, loadSkinData, and so on
     * @param path File to be loaded
     * @return result of load
     */
    virtual bool load(std::string_view path);

    /**
     * load skin data from bundle
     * @param id The ID of the skin, load the first Skin in the bundle if it is empty
     */
    virtual bool loadSkinData(std::string_view id, SkinData* skindata);

    /**
     * load material data from bundle
     * @param id The ID of the animation, load the first animation in the bundle if it is empty
     */
    virtual bool loadAnimationData(std::string_view id, Animation3DData* animationdata);

    // since 3.3, to support reskin
    virtual bool loadMeshDatas(MeshDatas& meshdatas);
    // since 3.3, to support reskin
    virtual bool loadNodes(NodeDatas& nodedatas);
    // since 3.3, to support reskin
    virtual bool loadMaterials(MaterialDatas& materialdatas);

    /**
     * load triangle list
     * @param path the file path to load
     */
    static std::vector<Vec3> getTrianglesList(std::string_view path);

    // load .obj file
    static bool loadObj(MeshDatas& meshdatas,
                        MaterialDatas& materialdatas,
                        NodeDatas& nodedatas,
                        std::string_view fullPath,
                        const char* mtl_basepath = nullptr);

    // calculate aabb
    static AABB calculateAABB(const std::vector<float>& vertex,
                              int stride, const IndexArray& indices);

    Bundle3D();
    virtual ~Bundle3D();
protected:
    bool loadJson(std::string_view path);
    bool loadBinary(std::string_view path);
    bool loadMeshDatasJson(MeshDatas& meshdatas);
    bool loadMeshDataJson_0_1(MeshDatas& meshdatas);
    bool loadMeshDataJson_0_2(MeshDatas& meshdatas);
    bool loadMeshDatasBinary(MeshDatas& meshdatas);
    bool loadMeshDatasBinary_0_1(MeshDatas& meshdatas);
    bool loadMeshDatasBinary_0_2(MeshDatas& meshdatas);
    bool loadMaterialsJson(MaterialDatas& materialdatas);
    bool loadMaterialDataJson_0_1(MaterialDatas& materialdatas);
    bool loadMaterialDataJson_0_2(MaterialDatas& materialdatas);
    bool loadMaterialsBinary(MaterialDatas& materialdatas);
    bool loadMaterialsBinary_0_1(MaterialDatas& materialdatas);
    bool loadMaterialsBinary_0_2(MaterialDatas& materialdatas);
    bool loadMeshDataJson(MeshData* meshdata);
    bool loadMeshDataJson_0_1(MeshData* meshdata);
    bool loadMeshDataJson_0_2(MeshData* meshdata);
    bool loadSkinDataJson(SkinData* skindata);
    bool loadSkinDataBinary(SkinData* skindata);
    bool loadMaterialDataJson(MaterialData* materialdata);
    bool loadMaterialDataJson_0_1(MaterialData* materialdata);
    bool loadMaterialDataJson_0_2(MaterialData* materialdata);
    bool loadAnimationDataJson(std::string_view id, Animation3DData* animationdata);
    bool loadAnimationDataBinary(std::string_view id, Animation3DData* animationdata);

    /**
     * load nodes of json
     */
    bool loadNodesJson(NodeDatas& nodedatas);
    NodeData* parseNodesRecursivelyJson(const rapidjson::Value& jvalue, bool singleSprite);

    /**
     * load nodes of binary
     */
    bool loadNodesBinary(NodeDatas& nodedatas);
    NodeData* parseNodesRecursivelyBinary(bool& skeleton, bool singleSprite);

    /**
     * get define data type
     * @param str The type in string
     */
    NTextureData::Usage parseGLTextureType(std::string_view str);

    /**
     * get vertex attribute type
     * @param str The type in string
     */
    shaderinfos::VertexKey parseGLProgramAttribute(std::string_view str);

    /*
     * get model path
     * @param str Full path of model file
     */
    void getModelRelativePath(std::string_view path);

    /*
     * set the read position in buffer to the target type
     * @param The data type
     * @param The data id
     */
    Reference* seekToFirstType(unsigned int type, std::string_view id = "");

protected:
    std::string _modelPath;
    std::string _path;
    std::string _version;  // the c3b or c3t version

    // for json reading
    std::string _jsonBuffer;
    rapidjson::Document _jsonReader;

    // for binary reading
    Data _binaryBuffer;
    BundleReader _binaryReader;
    unsigned int _referenceCount;
    Reference* _references;
    bool _isBinary;
};

// end of 3d group
/// @}

}

#endif  // __CCBUNDLE3D_H__
