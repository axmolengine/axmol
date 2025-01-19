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

#include "3d/Bundle3D.h"
#include "3d/ObjLoader.h"

#include "base/Macros.h"
#include "platform/FileUtils.h"
#include "base/Data.h"

#define BUNDLE_TYPE_SCENE             1
#define BUNDLE_TYPE_NODE              2
#define BUNDLE_TYPE_ANIMATIONS        3
#define BUNDLE_TYPE_ANIMATION         4
#define BUNDLE_TYPE_ANIMATION_CHANNEL 5
#define BUNDLE_TYPE_MODEL             10
#define BUNDLE_TYPE_MATERIAL          16
#define BUNDLE_TYPE_EFFECT            18
#define BUNDLE_TYPE_CAMERA            32
#define BUNDLE_TYPE_LIGHT             33
#define BUNDLE_TYPE_MESH              34
#define BUNDLE_TYPE_MESHPART          35
#define BUNDLE_TYPE_MESHSKIN          36

static const char* VERSION       = "version";
static const char* ID            = "id";
static const char* DEFAULTPART   = "body";
static const char* VERTEXSIZE    = "vertexsize";
static const char* VERTEX        = "vertex";
static const char* VERTICES      = "vertices";
static const char* INDEXNUM      = "indexnum";
static const char* INDICES       = "indices";
static const char* SUBMESH       = "submesh";
static const char* ATTRIBUTES    = "attributes";
static const char* ATTRIBUTESIZE = "size";
static const char* TYPE          = "type";
static const char* ATTRIBUTE     = "attribute";
static const char* SKIN          = "skin";
static const char* BINDSHAPE     = "bindshape";
static const char* MESH          = "mesh";
static const char* MESHES        = "meshes";
static const char* MESHPARTID    = "meshpartid";
static const char* MATERIALID    = "materialid";
static const char* NODE          = "node";
static const char* NODES         = "nodes";
static const char* CHILDREN      = "children";
static const char* PARTS         = "parts";
static const char* BONES         = "bones";
static const char* SKELETON      = "skeleton";
static const char* MATERIALS     = "materials";
static const char* ANIMATIONS    = "animations";
static const char* TRANSFORM     = "transform";
static const char* OLDTRANSFORM  = "tansform";
static const char* ANIMATION     = "animation";
static const char* MATERIAL      = "material";
static const char* BASE          = "base";
static const char* FILENAME      = "filename";
static const char* TEXTURES      = "textures";
static const char* LENGTH        = "length";
static const char* BONEID        = "boneId";
static const char* KEYFRAMES     = "keyframes";
static const char* TRANSLATION   = "translation";
static const char* ROTATION      = "rotation";
static const char* SCALE         = "scale";
static const char* KEYTIME       = "keytime";
static const char* AABBS         = "aabb";

namespace ax
{
using namespace simdjson;
void getChildMap(std::map<int, std::vector<int>>& map,
                 SkinData* skinData,
                 simdjson::simdjson_result<ondemand::value>& val)
{
    if (!skinData)
        return;

    // get transform matrix
    Mat4 transform;
    auto parent_transform = val.get_object()[OLDTRANSFORM];
    int j                 = 0;
    for (auto vv : val)
    {
        transform.m[j++] = static_cast<double>(vv);
    }

    // set origin matrices
    std::string_view parent_name = val[ID];
    int parent_name_index        = skinData->getSkinBoneNameIndex(parent_name);
    if (parent_name_index < 0)
    {
        skinData->addNodeBoneNames(parent_name);
        skinData->nodeBoneOriginMatrices.emplace_back(transform);
        parent_name_index = skinData->getBoneNameIndex(parent_name);
    }
    else if (parent_name_index < static_cast<int>(skinData->skinBoneNames.size()))
    {
        skinData->skinBoneOriginMatrices[parent_name_index] = transform;
    }

    // set root bone index
    if (skinData->rootBoneIndex < 0)
        skinData->rootBoneIndex = parent_name_index;

    auto children = val[CHILDREN];
    if (children.is_null())
        return;
    for (auto child : children)
    {
        std::string_view child_name = child[ID];
        int child_name_index        = skinData->getSkinBoneNameIndex(child_name);
        if (child_name_index < 0)
        {
            skinData->addNodeBoneNames(child_name);
            child_name_index = skinData->getBoneNameIndex(child_name);
        }

        map[parent_name_index].emplace_back(child_name_index);

        getChildMap(map, skinData, child);
    }
}

Bundle3D* Bundle3D::createBundle()
{
    auto bundle = new Bundle3D();
    return bundle;
}

void Bundle3D::destroyBundle(Bundle3D* bundle)
{
    delete bundle;
}

void Bundle3D::clear()
{
    if (_isBinary)
    {
        _binaryBuffer.clear();
        AX_SAFE_DELETE_ARRAY(_references);
    }
    else
    {
        _jsonBuffer.clear();
    }
}

bool Bundle3D::load(std::string_view path)
{
    if (path.empty())
        return false;

    if (_path == path)
        return true;

    getModelRelativePath(path);

    bool ret        = false;
    std::string ext = FileUtils::getPathExtension(path);
    if (ext == ".c3t")
    {
        _isBinary = false;
        ret       = loadJson(path);
    }
    else if (ext == ".c3b")
    {
        _isBinary = true;
        ret       = loadBinary(path);
    }
    else
    {
        AXLOGW("warning: {} is invalid file format", path.data());
    }

    ret ? (_path = path) : (_path = "");

    return ret;
}

bool Bundle3D::loadObj(MeshDatas& meshdatas,
                       MaterialDatas& materialdatas,
                       NodeDatas& nodedatas,
                       std::string_view fullPath,
                       const char* mtl_basepath)
{
    meshdatas.resetData();
    materialdatas.resetData();
    nodedatas.resetData();

    std::string mtlPath = "";
    if (mtl_basepath)
        mtlPath = mtl_basepath;
    else
        mtlPath = fullPath.substr(0, fullPath.find_last_of("\\/") + 1);

    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    auto ret = tinyobj::LoadObj(shapes, materials, fullPath.data(), mtlPath.c_str());
    if (ret.empty())
    {
        // fill data
        // convert material
        int i = 0;
        char str[20];
        std::string dir = "";
        auto last       = fullPath.rfind('/');
        if (last != std::string::npos)
            dir = fullPath.substr(0, last + 1);
        for (auto&& material : materials)
        {
            NMaterialData materialdata;

            NTextureData tex;
            tex.filename = material.diffuse_texname.empty() ? material.diffuse_texname : dir + material.diffuse_texname;
            tex.type     = NTextureData::Usage::Diffuse;
            tex.wrapS    = backend::SamplerAddressMode::CLAMP_TO_EDGE;
            tex.wrapT    = backend::SamplerAddressMode::CLAMP_TO_EDGE;

            snprintf(str, sizeof(str), "%d", ++i);
            materialdata.textures.emplace_back(tex);
            materialdata.id = str;
            material.name   = str;
            materialdatas.materials.emplace_back(materialdata);
        }

        // convert mesh
        i = 0;
        for (auto&& shape : shapes)
        {
            auto mesh          = shape.mesh;
            MeshData* meshdata = new MeshData();
            MeshVertexAttrib attrib;
            attrib.type = parseGLDataType("GL_FLOAT", 3);

            if (mesh.positions.size())
            {
                attrib.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_POSITION;
                meshdata->attribs.emplace_back(attrib);
            }
            bool hasnormal = false, hastex = false;
            if (mesh.normals.size())
            {
                hasnormal           = true;
                attrib.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_NORMAL;
                meshdata->attribs.emplace_back(attrib);
            }
            if (mesh.texcoords.size())
            {
                hastex              = true;
                attrib.type         = parseGLDataType("GL_FLOAT", 2);
                attrib.vertexAttrib = shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD;
                meshdata->attribs.emplace_back(attrib);
            }

            auto vertexNum = mesh.positions.size() / 3;
            for (unsigned int k = 0; k < vertexNum; ++k)
            {
                meshdata->vertex.emplace_back(mesh.positions[k * 3]);
                meshdata->vertex.emplace_back(mesh.positions[k * 3 + 1]);
                meshdata->vertex.emplace_back(mesh.positions[k * 3 + 2]);

                if (hasnormal)
                {
                    meshdata->vertex.emplace_back(mesh.normals[k * 3]);
                    meshdata->vertex.emplace_back(mesh.normals[k * 3 + 1]);
                    meshdata->vertex.emplace_back(mesh.normals[k * 3 + 2]);
                }

                if (hastex)
                {
                    meshdata->vertex.emplace_back(mesh.texcoords[k * 2]);
                    meshdata->vertex.emplace_back(mesh.texcoords[k * 2 + 1]);
                }
            }

            // split into submesh according to material
            std::map<int, IndexArray> subMeshMap;
            for (size_t k = 0, size = mesh.material_ids.size(); k < size; ++k)
            {
                int id     = mesh.material_ids[k];
                size_t idx = k * 3;
                subMeshMap[id].emplace_back(mesh.indices[idx]);
                subMeshMap[id].emplace_back(mesh.indices[idx + 1]);
                subMeshMap[id].emplace_back(mesh.indices[idx + 2]);
            }

            auto node = new NodeData();
            node->id  = shape.name;
            for (auto&& submesh : subMeshMap)
            {
                auto& storedIndices = meshdata->subMeshIndices.emplace_back(std::move(submesh.second));
                meshdata->subMeshAABB.emplace_back(
                    calculateAABB(meshdata->vertex, meshdata->getPerVertexSize(), storedIndices));
                snprintf(str, sizeof(str), "%d", ++i);
                meshdata->subMeshIds.emplace_back(str);

                auto modelnode        = new ModelData();
                modelnode->materialId = submesh.first == -1 ? "" : materials[submesh.first].name;
                modelnode->subMeshId  = str;
                node->modelNodeDatas.emplace_back(modelnode);
            }
            nodedatas.nodes.emplace_back(node);
            meshdatas.meshDatas.emplace_back(meshdata);
        }

        return true;
    }
    AXLOGW("warning: load {} file error: {}", fullPath, ret);
    return false;
}

bool Bundle3D::loadSkinData(std::string_view /*id*/, SkinData* skindata)
{
    skindata->resetData();

    if (_isBinary)
    {
        return loadSkinDataBinary(skindata);
    }
    else
    {
        return loadSkinDataJson(skindata);
    }
}

bool Bundle3D::loadAnimationData(std::string_view id, Animation3DData* animationdata)
{
    animationdata->resetData();

    if (_isBinary)
    {
        return loadAnimationDataBinary(id, animationdata);
    }
    else
    {
        return loadAnimationDataJson(id, animationdata);
    }
}

// since 3.3, to support reskin
bool Bundle3D::loadMeshDatas(MeshDatas& meshdatas)
{
    meshdatas.resetData();
    if (_isBinary)
    {
        if (_version == "0.1" || _version == "0.2")
        {
            return loadMeshDatasBinary_0_1(meshdatas);
        }
        else
        {
            return loadMeshDatasBinary(meshdatas);
        }
    }
    else
    {
        if (_version == "1.2" || _version == "0.2")
        {
            return loadMeshDataJson_0_1(meshdatas);
        }
        else
        {
            return loadMeshDatasJson(meshdatas);
        }
    }
    return true;
}
bool Bundle3D::loadMeshDatasBinary(MeshDatas& meshdatas)
{
    if (!seekToFirstType(BUNDLE_TYPE_MESH))
        return false;
    MeshData* meshData = nullptr;
    try
    {
        unsigned int meshSize = _binaryReader.read<unsigned int>();
        for (unsigned int i = 0; i < meshSize; ++i)
        {
            unsigned int attribSize = _binaryReader.read<unsigned int>();
            
            // read mesh data
            if (attribSize < 1)
            {
                AXLOGW("warning: Failed to read meshdata: attribCount '{}'.", _path);
                goto FAILED;
            }
            meshData              = new MeshData();
            meshData->attribCount = attribSize;
            meshData->attribs.resize(meshData->attribCount);
            for (ssize_t j = 0; j < meshData->attribCount; ++j)
            {
                unsigned int vSize                = _binaryReader.read<unsigned int>();
                std::string_view type             = _binaryReader.read_v32();
                std::string_view attribute        = _binaryReader.read_v32();
                meshData->attribs[j].type         = parseGLDataType(type, vSize);
                meshData->attribs[j].vertexAttrib = parseGLProgramAttribute(attribute);
            }
            unsigned int vertexSizeInFloat = _binaryReader.read<unsigned int>();
            // Read vertex data
            if (vertexSizeInFloat == 0)
            {
                AXLOGW("warning: Failed to read meshdata: vertexSizeInFloat '{}'.", _path);
                goto FAILED;
            }

            meshData->vertex.resize(vertexSizeInFloat);

            _binaryReader.read_blob(&meshData->vertex[0], 4, vertexSizeInFloat);

            // Read index data
            unsigned int meshPartCount = _binaryReader.read<unsigned int>();
            for (unsigned int k = 0; k < meshPartCount; ++k)
            {
                IndexArray indexArray{};
                std::string_view meshPartid = _binaryReader.read_v32();
                meshData->subMeshIds.emplace_back(meshPartid);
                unsigned int nIndexCount = _binaryReader.read<unsigned int>();
                indexArray.resize(nIndexCount);
                _binaryReader.read_blob(indexArray.data(), 2, nIndexCount);
                auto& storedIndices = meshData->subMeshIndices.emplace_back(std::move(indexArray));
                meshData->numIndex  = (int)meshData->subMeshIndices.size();
                // meshData->subMeshAABB.emplace_back(calculateAABB(meshData->vertex, meshData->getPerVertexSize(),
                // indexArray));
                if (_version != "0.3" && _version != "0.4" && _version != "0.5")
                {
                    // read mesh aabb
                    Vec3 aabb[2];
                    _binaryReader.read_blob(aabb);
                    meshData->subMeshAABB.emplace_back(AABB(aabb[0], aabb[1]));
                }
                else
                {
                    meshData->subMeshAABB.emplace_back(
                        calculateAABB(meshData->vertex, meshData->getPerVertexSize(), storedIndices));
                }
            }
            meshdatas.meshDatas.emplace_back(meshData);
        }

        return true;
    }
    catch (const std::exception& ex)
    {
        AXLOGW("exception occurred: {}", ex.what());
    }
FAILED:
{
    AX_SAFE_DELETE(meshData);
    for (auto&& meshdata : meshdatas.meshDatas)
    {
        delete meshdata;
    }
    meshdatas.meshDatas.clear();
    return false;
}
}
bool Bundle3D::loadMeshDatasBinary_0_1(MeshDatas& meshdatas)
{
    if (!seekToFirstType(BUNDLE_TYPE_MESH))
        return false;

    meshdatas.resetData();

    MeshData* meshdata = new MeshData();

    try
    {
        // read mesh data
        unsigned int attribSize = _binaryReader.read<unsigned int>();
        if (attribSize < 1)
        {
            AXLOGW("warning: Failed to read meshdata: attribCount '{}'.", _path);
            AX_SAFE_DELETE(meshdata);
            return false;
        }
        enum
        {
            VERTEX_ATTRIB_POSITION,
            VERTEX_ATTRIB_COLOR,
            VERTEX_ATTRIB_TEX_COORD,
            VERTEX_ATTRIB_NORMAL,
            VERTEX_ATTRIB_BLEND_WEIGHT,
            VERTEX_ATTRIB_BLEND_INDEX,

            VERTEX_ATTRIB_MAX,

            // backward compatibility
            VERTEX_ATTRIB_TEX_COORDS = VERTEX_ATTRIB_TEX_COORD,
        };
        for (unsigned int i = 0; i < attribSize; ++i)
        {
            auto vUsage = _binaryReader.read<unsigned int>();
            auto vSize  = _binaryReader.read<unsigned int>();
            shaderinfos::VertexKey usage;

            MeshVertexAttrib meshVertexAttribute;
            meshVertexAttribute.type = parseGLDataType("GL_FLOAT", vSize);
            if (vUsage == VERTEX_ATTRIB_NORMAL)
            {
                usage = shaderinfos::VertexKey::VERTEX_ATTRIB_NORMAL;
            }
            else if (vUsage == VERTEX_ATTRIB_BLEND_WEIGHT)
            {
                usage = shaderinfos::VertexKey::VERTEX_ATTRIB_BLEND_WEIGHT;
            }
            else if (vUsage == VERTEX_ATTRIB_BLEND_INDEX)
            {
                usage = shaderinfos::VertexKey::VERTEX_ATTRIB_BLEND_INDEX;
            }
            else if (vUsage == VERTEX_ATTRIB_POSITION)
            {
                usage = shaderinfos::VertexKey::VERTEX_ATTRIB_POSITION;
            }
            else if (vUsage == VERTEX_ATTRIB_TEX_COORD)
            {
                usage = shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD;
            }
            else
            {
                AXASSERT(false, "invalidate usage value");
            }
            meshVertexAttribute.vertexAttrib = usage;

            meshdata->attribs.emplace_back(meshVertexAttribute);
        }

        // Read vertex data
        meshdata->vertexSizeInFloat = _binaryReader.read<int>();
        if (meshdata->vertexSizeInFloat == 0)
        {
            AXLOGW("warning: Failed to read meshdata: vertexSizeInFloat '{}'.", _path);
            AX_SAFE_DELETE(meshdata);
            return false;
        }

        meshdata->vertex.resize(meshdata->vertexSizeInFloat);
        _binaryReader.read_blob(&meshdata->vertex[0], 4, meshdata->vertexSizeInFloat);

        // Read index data
        unsigned int meshPartCount = 1;
        for (unsigned int i = 0; i < meshPartCount; ++i)
        {
            unsigned int nIndexCount = _binaryReader.read<unsigned int>();

            IndexArray indices{};
            indices.resize(nIndexCount);
            _binaryReader.read_blob(indices.data(), 2, nIndexCount);

            auto& storedIndices = meshdata->subMeshIndices.emplace_back(std::move(indices));
            meshdata->subMeshAABB.emplace_back(
                calculateAABB(meshdata->vertex, meshdata->getPerVertexSize(), storedIndices));
        }

        meshdatas.meshDatas.emplace_back(meshdata);

        return true;
    }
    catch (const std::exception& ex)
    {
        AXLOGW("exception occurred: {}", ex.what());
    }
    return false;
}

bool Bundle3D::loadMeshDatasBinary_0_2(MeshDatas& meshdatas)
{
    if (!seekToFirstType(BUNDLE_TYPE_MESH))
        return false;

    meshdatas.resetData();

    MeshData* meshdata = new MeshData();

    try
    {
        // read mesh data
        unsigned int attribSize = 0;
        if (_binaryReader.read_blob(attribSize) != 1 || attribSize < 1)
        {
            AXLOGW("warning: Failed to read meshdata: attribCount '{}'.", _path);
            AX_SAFE_DELETE(meshdata);
            return false;
        }
        enum
        {
            VERTEX_ATTRIB_POSITION,
            VERTEX_ATTRIB_COLOR,
            VERTEX_ATTRIB_TEX_COORD,
            VERTEX_ATTRIB_NORMAL,
            VERTEX_ATTRIB_BLEND_WEIGHT,
            VERTEX_ATTRIB_BLEND_INDEX,

            VERTEX_ATTRIB_MAX,

            // backward compatibility
            VERTEX_ATTRIB_TEX_COORDS = VERTEX_ATTRIB_TEX_COORD,
        };
        for (unsigned int i = 0; i < attribSize; ++i)
        {
            unsigned int vUsage, vSize;
            shaderinfos::VertexKey usage = shaderinfos::VertexKey::VERTEX_ATTRIB_ERROR;
            if (_binaryReader.read_blob(vUsage) != 1 || _binaryReader.read_blob(vSize) != 1)
            {
                AXLOGW("warning: Failed to read meshdata: usage or size '{}'.", _path);
                AX_SAFE_DELETE(meshdata);
                return false;
            }

            MeshVertexAttrib meshVertexAttribute;
            meshVertexAttribute.type = parseGLDataType("GL_FLOAT", vSize);
            if (vUsage == VERTEX_ATTRIB_NORMAL)
            {
                usage = shaderinfos::VertexKey::VERTEX_ATTRIB_NORMAL;
            }
            else if (vUsage == VERTEX_ATTRIB_BLEND_WEIGHT)
            {
                usage = shaderinfos::VertexKey::VERTEX_ATTRIB_BLEND_WEIGHT;
            }
            else if (vUsage == VERTEX_ATTRIB_BLEND_INDEX)
            {
                usage = shaderinfos::VertexKey::VERTEX_ATTRIB_BLEND_INDEX;
            }
            else if (vUsage == VERTEX_ATTRIB_POSITION)
            {
                usage = shaderinfos::VertexKey::VERTEX_ATTRIB_POSITION;
            }
            else if (vUsage == VERTEX_ATTRIB_TEX_COORD)
            {
                usage = shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD;
            }
            meshVertexAttribute.vertexAttrib = usage;

            meshdata->attribs.emplace_back(meshVertexAttribute);
        }

        // Read vertex data
        if (_binaryReader.read_blob(&meshdata->vertexSizeInFloat, 4, 1) != 1 || meshdata->vertexSizeInFloat == 0)
        {
            AXLOGW("warning: Failed to read meshdata: vertexSizeInFloat '{}'.", _path);
            AX_SAFE_DELETE(meshdata);
            return false;
        }

        meshdata->vertex.resize(meshdata->vertexSizeInFloat);
        if (_binaryReader.read_blob(&meshdata->vertex[0], 4, meshdata->vertexSizeInFloat) != meshdata->vertexSizeInFloat)
        {
            AXLOGW("warning: Failed to read meshdata: vertex element '{}'.", _path);
            AX_SAFE_DELETE(meshdata);
            return false;
        }

        // read submesh
        unsigned int submeshCount;
        if (_binaryReader.read_blob(submeshCount) != 1)
        {
            AXLOGW("warning: Failed to read meshdata: submeshCount '{}'.", _path);
            AX_SAFE_DELETE(meshdata);
            return false;
        }

        for (unsigned int i = 0; i < submeshCount; ++i)
        {
            unsigned int nIndexCount;
            if (_binaryReader.read_blob(nIndexCount) != 1)
            {
                AXLOGW("warning: Failed to read meshdata: nIndexCount '{}'.", _path);
                AX_SAFE_DELETE(meshdata);
                return false;
            }

            IndexArray indices{}; /* TODO: _version == 1.3 use U_INT?*/
            indices.resize(nIndexCount);
            if (_binaryReader.read_blob(indices.data(), 2, nIndexCount) != nIndexCount)
            {
                AXLOGW("warning: Failed to read meshdata: indices '{}'.", _path);
                AX_SAFE_DELETE(meshdata);
                return false;
            }

            auto& storedIndices = meshdata->subMeshIndices.emplace_back(std::move(indices));
            meshdata->subMeshAABB.emplace_back(
                calculateAABB(meshdata->vertex, meshdata->getPerVertexSize(), storedIndices));
        }

        meshdatas.meshDatas.emplace_back(meshdata);
        return true;
    }
    catch (const std::exception& ex)
    {
        AXLOGW("exception occurred: {}", ex.what());
    }

    return false;
}
bool Bundle3D::loadMeshDatasJson(MeshDatas& meshdatas)
{
    auto mesh_data_array = _jsonReader[MESHES];
    for (auto&& mesh_data : mesh_data_array)
    {
        MeshData* meshData = new MeshData();
        // mesh_vertex_attribute
        auto&& mesh_vertex_attribute = mesh_data[ATTRIBUTES].get_array();

        meshData->attribCount = mesh_vertex_attribute.count_elements();
        meshData->attribs.reserve(meshData->attribCount);
        for (auto&& mesh_vertex_attribute_val : mesh_vertex_attribute)
        {
            int size                   = static_cast<int>(mesh_vertex_attribute_val[ATTRIBUTESIZE]);
            std::string_view type      = mesh_vertex_attribute_val[TYPE];
            std::string_view attribute = mesh_vertex_attribute_val[ATTRIBUTE];

            meshData->attribs.emplace_back(parseGLDataType(type, size), parseGLProgramAttribute(attribute));
        }
        // mesh vertices
        ////////////////////////////////////////////////////////////////////////////////////////////////
        auto&& mesh_data_vertex_array = mesh_data[VERTICES].get_array();
        for (auto&& vertex_val : mesh_data_vertex_array)
        {
            meshData->vertex.emplace_back(static_cast<double>(vertex_val));
        }
        meshData->vertexSizeInFloat = meshData->vertex.size();
        // mesh part
        ////////////////////////////////////////////////////////////////////////////////////////////////
        auto&& mesh_part_array = mesh_data[PARTS].get_array();
        for (auto&& mesh_part : mesh_part_array)
        {
            IndexArray indexArray{};
            meshData->subMeshIds.emplace_back(static_cast<std::string_view>(mesh_part[ID]));
            // index_number
            auto&& indices_val_array = mesh_part[INDICES];
            for (auto&& indices_val : indices_val_array)
                indexArray.emplace_back(static_cast<unsigned short>(indices_val));

            auto& storedIndices = meshData->subMeshIndices.emplace_back(std::move(indexArray));
            meshData->numIndex  = (int)meshData->subMeshIndices.size();

            auto&& mesh_part_aabb = mesh_part[AABBS];
            if (!mesh_part_aabb.error())
            {
                if (mesh_part_aabb.count_elements() == 6)
                {
                    Vec3 aabb_vals[2];
                    int aabb_idx = 0;
                    for (auto aabb_val : mesh_part_aabb)
                    {
                        aabb_vals->comps[aabb_idx++] = static_cast<float>(aabb_val);
                        if (aabb_idx >= 6)
                            break;
                    }
                    meshData->subMeshAABB.emplace_back(AABB(aabb_vals[0], aabb_vals[1]));
                }
                else
                {
                    meshData->subMeshAABB.emplace_back(
                        calculateAABB(meshData->vertex, meshData->getPerVertexSize(), storedIndices));
                }
            }
            else
            {
                meshData->subMeshAABB.emplace_back(
                    calculateAABB(meshData->vertex, meshData->getPerVertexSize(), storedIndices));
            }
        }
        meshdatas.meshDatas.emplace_back(meshData);
    }
    return true;
}
bool Bundle3D::loadNodes(NodeDatas& nodedatas)
{
    if (_version == "0.1" || _version == "1.2" || _version == "0.2")
    {
        SkinData skinData;
        if (!loadSkinData("", &skinData))
        {
            auto node             = new NodeData();
            auto modelnode        = new ModelData();
            modelnode->materialId = "";
            modelnode->subMeshId  = "";
            node->modelNodeDatas.emplace_back(modelnode);
            nodedatas.nodes.emplace_back(node);
            return true;
        }

        auto nodeDatas = new NodeData*[skinData.skinBoneNames.size() + skinData.nodeBoneNames.size()];
        int index      = 0;
        size_t i;
        auto skinBoneSize = skinData.skinBoneNames.size();
        auto nodeBoneSize = skinData.nodeBoneNames.size();
        for (i = 0; i < skinBoneSize; ++i)
        {
            nodeDatas[index]            = new NodeData();
            nodeDatas[index]->id        = skinData.skinBoneNames[i];
            nodeDatas[index]->transform = skinData.skinBoneOriginMatrices[i];
            ++index;
        }
        for (i = 0; i < nodeBoneSize; ++i)
        {
            nodeDatas[index]            = new NodeData();
            nodeDatas[index]->id        = skinData.nodeBoneNames[i];
            nodeDatas[index]->transform = skinData.nodeBoneOriginMatrices[i];
            ++index;
        }
        for (const auto& it : skinData.boneChild)
        {
            const auto& children = it.second;
            auto parent          = nodeDatas[it.first];
            for (const auto& child : children)
            {
                parent->children.emplace_back(nodeDatas[child]);
            }
        }
        nodedatas.skeleton.emplace_back(nodeDatas[skinData.rootBoneIndex]);
        auto node              = new NodeData();
        auto modelnode         = new ModelData();
        modelnode->materialId  = "";
        modelnode->subMeshId   = "";
        modelnode->bones       = skinData.skinBoneNames;
        modelnode->invBindPose = skinData.inverseBindPoseMatrices;
        node->modelNodeDatas.emplace_back(modelnode);
        nodedatas.nodes.emplace_back(node);
        delete[] nodeDatas;
    }
    else
    {
        if (_isBinary)
        {
            loadNodesBinary(nodedatas);
        }
        else
        {
            loadNodesJson(nodedatas);
        }
    }
    return true;
}
bool Bundle3D::loadMaterials(MaterialDatas& materialdatas)
{
    materialdatas.resetData();
    if (_isBinary)
    {
        if (_version == "0.1")
        {
            return loadMaterialsBinary_0_1(materialdatas);
        }
        else if (_version == "0.2")
        {
            return loadMaterialsBinary_0_2(materialdatas);
        }
        else
        {
            return loadMaterialsBinary(materialdatas);
        }
    }
    else
    {
        if (_version == "1.2")
        {
            return loadMaterialDataJson_0_1(materialdatas);
        }
        else if (_version == "0.2")
        {
            return loadMaterialDataJson_0_2(materialdatas);
        }
        else
        {
            return loadMaterialsJson(materialdatas);
        }
    }
    return true;
}
bool Bundle3D::loadMaterialsBinary(MaterialDatas& materialdatas)
{
    if (!seekToFirstType(BUNDLE_TYPE_MATERIAL))
        return false;
    unsigned int materialnum = 1;
    try
    {
        _binaryReader.read_blob(materialnum);
        for (unsigned int i = 0; i < materialnum; ++i)
        {
            NMaterialData materialData;
            materialData.id = _binaryReader.read_v32();

            // skip: diffuse(3), ambient(3), emissive(3), opacity(1), specular(3), shininess(1)
            float data[14];
            _binaryReader.read_blob(data);

            unsigned int textureNum = 1;
            _binaryReader.read_blob(textureNum);
            for (unsigned int j = 0; j < textureNum; ++j)
            {
                NTextureData textureData;
                textureData.id = _binaryReader.read_v32();
                if (textureData.id.empty())
                {
                    AXLOGW("warning: Failed to read Materialdata: texturePath is empty '{}'.", textureData.id);
                    return false;
                }
                std::string_view texturePath = _binaryReader.read_v32();
                if (texturePath.empty())
                {
                    AXLOGW("warning: Failed to read Materialdata: texturePath is empty '{}'.", _path);
                    return false;
                }
                (textureData.filename = _modelPath) += texturePath;
                float uvdata[4];
                _binaryReader.read_blob(uvdata);
                textureData.type  = parseGLTextureType(_binaryReader.read_v32());
                textureData.wrapS = parseSamplerAddressMode(_binaryReader.read_v32());
                textureData.wrapT = parseSamplerAddressMode(_binaryReader.read_v32());
                materialData.textures.emplace_back(textureData);
            }
            materialdatas.materials.emplace_back(materialData);
        }
        return true;
    }
    catch (const std::exception& ex)
    {
        AXLOGW("exception occurred: {}", ex.what());
    }
    return false;
}
bool Bundle3D::loadMaterialsBinary_0_1(MaterialDatas& materialdatas)
{
    if (!seekToFirstType(BUNDLE_TYPE_MATERIAL))
        return false;

    NMaterialData materialData;

    std::string_view texturePath = _binaryReader.read_v32();
    if (texturePath.empty())
    {
        AXLOGW("warning: Failed to read Materialdata: texturePath is empty '{}'.", _path);
        return false;
    }

    NTextureData textureData;
    (textureData.filename = _modelPath) += texturePath;
    textureData.type = NTextureData::Usage::Diffuse;
    textureData.id   = "";
    materialData.textures.emplace_back(textureData);
    materialdatas.materials.emplace_back(materialData);
    return true;
}

bool Bundle3D::loadMaterialsBinary_0_2(MaterialDatas& materialdatas)
{
    if (!seekToFirstType(BUNDLE_TYPE_MATERIAL))
        return false;

    try
    {
        unsigned int materialnum = 1;
        _binaryReader.read_blob(materialnum);

        for (unsigned int i = 0; i < materialnum; ++i)
        {
            NMaterialData materialData;

            std::string_view texturePath = _binaryReader.read_v32();
            if (texturePath.empty())
            {
                AXLOGW("warning: Failed to read Materialdata: texturePath is empty '{}'.", _path);
                return true;
            }

            NTextureData textureData;
            (textureData.filename = _modelPath) += texturePath;
            textureData.type     = NTextureData::Usage::Diffuse;
            textureData.id       = "";
            materialData.textures.emplace_back(textureData);
            materialdatas.materials.emplace_back(materialData);
        }
        return true;
    }
    catch (const std::exception& ex)
    {
        AXLOGW("exception occurred: {}", ex.what());
    }
    return false;
}

bool loadMeshDataJson(MeshData* /*meshdata*/)
{
    return true;
}

bool loadMeshDataJson_0_1(MeshData* /*meshdata*/)
{
    return true;
}

bool loadMeshDataJson_0_2(MeshData* /*meshdata*/)
{
    return true;
}

bool Bundle3D::loadMaterialsJson(MaterialDatas& materialdatas)
{
    auto material_array = _jsonReader[MATERIALS].get_array();
    if (material_array.error())
        return false;
    for (auto&& material_val : material_array)
    {
        NMaterialData materialData;
        materialData.id      = static_cast<std::string_view>(material_val[ID]);
        auto&& texture_array = material_val[TEXTURES];
        if (!texture_array.error())
        {
            for (auto&& texture_val : texture_array.get_array())
            {
                NTextureData textureData;
                std::string_view filename = texture_val[FILENAME];
                if (!filename.empty())
                    (textureData.filename = _modelPath) += filename;
                textureData.type  = parseGLTextureType(texture_val["type"]);
                textureData.wrapS = parseSamplerAddressMode(texture_val["wrapModeU"]);
                textureData.wrapT = parseSamplerAddressMode(texture_val["wrapModeV"]);
                materialData.textures.emplace_back(textureData);
            }
        }
        materialdatas.materials.emplace_back(materialData);
    }
    return true;
}
bool Bundle3D::loadJson(std::string_view path)
{
    clear();

    FileUtils::getInstance()->getContents(path, &_jsonBuffer);

    _jsonReader = _jsonParser.iterate(_jsonBuffer);
    if (_jsonReader.is_null())
    {
        clear();
        AXLOGW("Parse json failed in Bundle3D::loadJson function");
        return false;
    }

    auto mash_data_array = _jsonReader[VERSION];
    if (mash_data_array.type() == ondemand::json_type::array)  // Compatible with the old version
        _version = "1.2";
    else
        _version = static_cast<std::string_view>(mash_data_array);

    return true;
}

bool Bundle3D::loadBinary(std::string_view path)
{
    clear();

    // get file data
    _binaryBuffer.clear();
    _binaryBuffer = FileUtils::getInstance()->getDataFromFile(path);
    if (_binaryBuffer.isNull())
    {
        clear();
        AXLOGW("warning: Failed to read file: {}", path);
        return false;
    }

    // Initialise bundle reader
    _binaryReader.reset((char*)_binaryBuffer.getBytes(), _binaryBuffer.getSize());

    try
    {
        // Read identifier info
        char identifier[] = {'C', '3', 'B', '\0'};
        char sig[4];
        if (_binaryReader.read_blob(sig) != 4 || memcmp(sig, identifier, 4) != 0)
        {
            clear();
            AXLOGW("warning: Invalid identifier: {}", path);
            return false;
        }

        // Read version
        unsigned char ver[2];
        if (_binaryReader.read_blob(ver) != 2)
        {
            AXLOGW("warning: Failed to read version:");
            return false;
        }

        char version[20] = {0};
        snprintf(version, sizeof(version), "%d.%d", ver[0], ver[1]);
        _version = version;

        // Read ref table size
        if (_binaryReader.read_blob(_referenceCount) != 1)
        {
            clear();
            AXLOGW("warning: Failed to read ref table size '{}'.", path);
            return false;
        }

        // Read all refs
        AX_SAFE_DELETE_ARRAY(_references);
        _references = new Reference[_referenceCount];
        for (unsigned int i = 0; i < _referenceCount; ++i)
        {
            if ((_references[i].id = _binaryReader.read_v32()).empty() ||
                _binaryReader.read_blob(_references[i].type) != 1 || _binaryReader.read_blob(_references[i].offset) != 1)
            {
                clear();
                AXLOGW("warning: Failed to read ref number {} for bundle '{}'.", i, path);
                AX_SAFE_DELETE_ARRAY(_references);
                return false;
            }
        }

        return true;
    }
    catch (const std::exception& ex)
    {
        AXLOGW("exception occurred: {}", ex.what());
    }
    return false;
}

bool Bundle3D::loadMeshDataJson_0_1(MeshDatas& meshdatas)
{
    auto&& mesh_data_array = _jsonReader[MESH];
    MeshData* meshdata     = new MeshData();
    auto&& mesh_data_val   = mesh_data_array.at(0);

    auto&& mesh_data_body_array = mesh_data_val[DEFAULTPART];

    auto&& mesh_data_body_array_0 = mesh_data_body_array.at(0);

    // mesh_vertex_attribute
    auto&& mesh_vertex_attribute = mesh_data_val[ATTRIBUTES].get_array();
    meshdata->attribCount        = mesh_vertex_attribute.count_elements();
    meshdata->attribs.reserve(meshdata->attribCount);
    for (auto&& mesh_vertex_attribute_val : mesh_vertex_attribute)
    {
        int size = mesh_vertex_attribute_val[ATTRIBUTESIZE];
        meshdata->attribs.emplace_back(parseGLDataType(mesh_vertex_attribute_val[TYPE], size),
                                       parseGLProgramAttribute(mesh_vertex_attribute_val[ATTRIBUTE]));
        /*meshdata->attribs[i].type         = parseGLDataType(mesh_vertex_attribute_val[TYPE], size);
        meshdata->attribs[i].vertexAttrib = parseGLProgramAttribute(mesh_vertex_attribute_val[ATTRIBUTE]);*/
    }
    // meshdata->attribCount = meshdata->attribs.size();

    // vertices
    meshdata->vertexSizeInFloat = mesh_data_body_array_0[VERTEXSIZE];
    meshdata->vertex.resize(meshdata->vertexSizeInFloat);

    auto mesh_data_body_vertices = mesh_data_body_array_0[VERTICES];
    int i                        = 0;
    for (auto&& vertex_val : mesh_data_body_vertices)
        meshdata->vertex[i++] = static_cast<double>(vertex_val);

    // index_number
    int indexnum = mesh_data_body_array_0[INDEXNUM];

    // indices
    IndexArray indices{};
    indices.resize(indexnum);

    auto&& indices_val_array = mesh_data_body_array_0[INDICES];
    i                        = 0;
    for (auto&& index_val : indices_val_array.get_array())
        indices.at<uint16_t>(i++) = static_cast<unsigned short>(indices_val_array);

    auto& storedIndices = meshdata->subMeshIndices.emplace_back(std::move(indices));
    meshdata->subMeshAABB.emplace_back(calculateAABB(meshdata->vertex, meshdata->getPerVertexSize(), storedIndices));
    meshdatas.meshDatas.emplace_back(meshdata);
    return true;
}

bool Bundle3D::loadMeshDataJson_0_2(MeshDatas& meshdatas)
{
    MeshData* meshdata  = new MeshData();
    auto&& mesh_array   = _jsonReader[MESH];
    auto&& mesh_array_0 = mesh_array.at(0);

    // mesh_vertex_attribute
    auto&& mesh_vertex_attribute = mesh_array_0[ATTRIBUTES];
    meshdata->attribCount        = mesh_vertex_attribute.count_elements();
    meshdata->attribs.resize(meshdata->attribCount);
    int i = 0;
    for (auto&& mesh_vertex_attribute_val : mesh_vertex_attribute.get_array())
    {
        auto size                         = static_cast<unsigned int>(mesh_vertex_attribute_val[ATTRIBUTESIZE]);
        meshdata->attribs[i].type         = parseGLDataType(mesh_vertex_attribute_val[TYPE], size);
        meshdata->attribs[i].vertexAttrib = parseGLProgramAttribute(mesh_vertex_attribute_val[ATTRIBUTE]);
    }

    // vertices
    auto&& mesh_data_vertex   = mesh_array_0[VERTEX];
    auto&& mesh_data_vertex_0 = mesh_data_vertex.at(0);

    meshdata->vertexSizeInFloat = static_cast<float>(mesh_data_vertex_0[VERTEXSIZE]);
    meshdata->vertex.resize(meshdata->vertexSizeInFloat);

    auto&& mesh_data_body_vertices = mesh_data_vertex_0[VERTICES];
    i                              = 0;
    for (auto&& vertex_val : mesh_data_body_vertices)
        meshdata->vertex[i++] = static_cast<float>(vertex_val);

    // submesh
    auto&& mesh_submesh_array = mesh_array_0[SUBMESH];
    for (auto&& mesh_submesh_val : mesh_submesh_array.get_array())
    {
        // std::string id = mesh_submesh_val[ID].GetString();

        // index_number
        auto indexnum = static_cast<unsigned int>(mesh_submesh_val[INDEXNUM]);

        // indices
        IndexArray indices{};
        indices.resize(indexnum);

        auto indices_val_array = mesh_submesh_val[INDICES];
        int j                  = 0;
        for (auto&& index_val : indices_val_array)
            indices.at<uint16_t>(j) = static_cast<unsigned short>(indices_val_array);

        auto& storedIndices = meshdata->subMeshIndices.emplace_back(std::move(indices));
        meshdata->subMeshAABB.emplace_back(
            calculateAABB(meshdata->vertex, meshdata->getPerVertexSize(), storedIndices));
    }
    meshdatas.meshDatas.emplace_back(meshdata);
    return true;
}

bool Bundle3D::loadSkinDataJson(SkinData* skindata)
{
    auto&& skin_data_array = _jsonReader[SKIN].get_array();
    if (skin_data_array.error())
        return false;

    // AXASSERT(skin_data_array.(), "skin data is not an array");
    auto&& skin_data_array_val_0 = skin_data_array.at(0);

    auto&& skin_data_bones = skin_data_array_val_0[BONES].get_array();
    if (skin_data_bones.error())
        return false;
    for (auto&& skin_data_bone : skin_data_bones)
    {
        std::string_view name = skin_data_bone[NODE];
        skindata->addSkinBoneNames(name);

        Mat4 mat_bind_pos;
        auto&& bind_pos_array = skin_data_bone[BINDSHAPE];
        int j                 = 0;
        for (auto&& pos : bind_pos_array.get_array())
        {
            mat_bind_pos.m[j++] = static_cast<float>(pos);
        }
        skindata->inverseBindPoseMatrices.emplace_back(mat_bind_pos);
    }

    // set root bone information
    auto&& skin_data_1 = skin_data_array.at(1);

    // parent and child relationship map
    skindata->skinBoneOriginMatrices.resize(skindata->skinBoneNames.size());
    getChildMap(skindata->boneChild, skindata, skin_data_1);
    return true;
}

bool Bundle3D::loadSkinDataBinary(SkinData* skindata)
{
    if (!seekToFirstType(BUNDLE_TYPE_MESHSKIN))
        return false;

    std::string_view boneName = _binaryReader.read_v32();

    try
    {
        // transform
        float bindShape[16];
        if (!_binaryReader.read_blob(bindShape))
        {
            AXLOGW("warning: Failed to read SkinData: bindShape matrix  '{}'.", _path);
            return false;
        }

        // bone count
        unsigned int boneNum;
        if (!_binaryReader.read_blob(boneNum))
        {
            AXLOGW("warning: Failed to read SkinData: boneNum  '{}'.", _path);
            return false;
        }

        // Fix bug: check if the bone number is 0.
        if (boneNum == 0)
            return false;

        // bone names and bind pos
        float bindpos[16];
        for (unsigned int i = 0; i < boneNum; ++i)
        {
            std::string_view skinBoneName = _binaryReader.read_v32();
            skindata->skinBoneNames.emplace_back(skinBoneName);
            if (!_binaryReader.read_blob(bindpos))
            {
                AXLOGW("warning: Failed to load SkinData: bindpos '{}'.", _path);
                return false;
            }
            skindata->inverseBindPoseMatrices.emplace_back(bindpos);
        }

        skindata->skinBoneOriginMatrices.resize(boneNum);

        boneName = _binaryReader.read_v32();

        // bind shape
        _binaryReader.read_blob(bindShape);
        int rootIndex = skindata->getSkinBoneNameIndex(boneName);
        if (rootIndex < 0)
        {
            skindata->addNodeBoneNames(boneName);
            rootIndex = skindata->getBoneNameIndex(boneName);
            skindata->nodeBoneOriginMatrices.emplace_back(bindShape);
        }
        else
        {
            skindata->skinBoneOriginMatrices[rootIndex] = bindShape;
        }

        // set root bone index
        skindata->rootBoneIndex = rootIndex;

        // read parent and child relationship map
        float transform[16];
        unsigned int linkNum;
        _binaryReader.read_blob(linkNum);
        for (unsigned int i = 0; i < linkNum; ++i)
        {
            std::string_view id = _binaryReader.read_v32();
            int index           = skindata->getSkinBoneNameIndex(id);

            std::string_view parentid = _binaryReader.read_v32();
            if (!_binaryReader.read_blob(transform))
            {
                AXLOGW("warning: Failed to load SkinData: transform '{}'.", _path);
                return false;
            }

            if (index < 0)
            {
                skindata->addNodeBoneNames(id);
                index = skindata->getBoneNameIndex(id);
                skindata->nodeBoneOriginMatrices.emplace_back(transform);
            }
            else
            {
                skindata->skinBoneOriginMatrices[index] = transform;
            }

            int parentIndex = skindata->getSkinBoneNameIndex(parentid);
            if (parentIndex < 0)
            {
                skindata->addNodeBoneNames(parentid);
                parentIndex = skindata->getBoneNameIndex(parentid);
            }

            skindata->boneChild[parentIndex].emplace_back(index);
        }

        return true;
    }
    catch (const std::exception& ex)
    {
        AXLOGW("exception occurred: {}", ex.what());
    }
    return false;
}

bool Bundle3D::loadMaterialDataJson_0_1(MaterialDatas& materialdatas)
{
    NMaterialData materialData;
    auto&& material_data_array = _jsonReader[MATERIAL].get_array();
    if (material_data_array.error())
        return false;

    auto&& material_data_array_0 = material_data_array.at(0);

    auto&& material_data_base_array = material_data_array_0[BASE].get_array();
    if (!material_data_base_array.error())
    {
        auto&& material_data_base_array_0 = material_data_base_array.at(0);
        NTextureData textureData;
        // set texture
        std::string_view filename = material_data_base_array_0[FILENAME];
        if (!filename.empty())
            (textureData.filename = _modelPath) += filename;
        textureData.type = NTextureData::Usage::Diffuse;
        materialData.textures.emplace_back(textureData);
        materialdatas.materials.emplace_back(materialData);
    }

    return true;
}

bool Bundle3D::loadMaterialDataJson_0_2(MaterialDatas& materialdatas)
{
    NMaterialData materialData;
    auto&& material_array = _jsonReader[MATERIAL];
    if (material_array.error())
        return false;

    for (auto&& material_val : material_array.get_array())
    {
        NTextureData textureData;

        // set texture
        std::string_view filename = material_val[TEXTURES];
        if (!filename.empty())
            (textureData.filename = _modelPath) += filename;
        textureData.type = NTextureData::Usage::Diffuse;
        materialData.textures.emplace_back(textureData);
    }
    materialdatas.materials.emplace_back(materialData);
    return true;
}

bool loadMaterialDataJson(MaterialData* /*materialdata*/)
{
    return true;
}

bool loadMaterialDataJson_0_1(MaterialData* /*materialdata*/)
{
    return true;
}

bool loadMaterialDataJson_0_2(MaterialData* /*materialdata*/)
{
    return true;
}

bool Bundle3D::loadAnimationDataJson(std::string_view id, Animation3DData* animationdata)
{
    std::string anim = "";
    if (_version == "1.2" || _version == "0.2")
        anim = ANIMATION;
    else
        anim = ANIMATIONS;

    int the_index               = -1;
    auto&& animation_data_array = _jsonReader[anim];
    if (animation_data_array.error())
        return false;

    if (!id.empty())
    {
        int i = 0;
        for (auto&& animation_data : animation_data_array.get_array())
        {
            std::string_view keyid = animation_data[ID];
            if (keyid == id)
            {
                the_index = static_cast<int>(i);
            }
            ++i;
        }
        if (the_index < 0)
            return false;
    }
    else
    {
        the_index = 0;
    }

    auto&& animation_data_array_val_0 = animation_data_array.at(the_index);

    animationdata->_totalTime = static_cast<float>(animation_data_array_val_0[LENGTH]);

    auto&& bones = animation_data_array_val_0[BONES];
    for (auto&& bone : bones.get_array())
    {
        std::string_view bone_name = bone[BONEID];
        auto&& bone_keyframes      = bone[KEYFRAMES];
        if (bone_keyframes.error())
            continue;
        size_t keyframe_size = bone_keyframes.count_elements();
        animationdata->_rotationKeys[bone_name].reserve(keyframe_size);
        animationdata->_scaleKeys[bone_name].reserve(keyframe_size);
        animationdata->_translationKeys[bone_name].reserve(keyframe_size);

        for (auto&& bone_keyframe : bone_keyframes)
        {
            float keytime                    = static_cast<float>(bone_keyframe[KEYTIME]);
            auto&& bone_keyframe_translation = bone_keyframe[TRANSLATION];
            if (!bone_keyframe_translation.error())
            {
                Vec3 val;
                int i = 0;
                for (auto&& axis_val : bone_keyframe_translation)
                {
                    val.comps[i++] = static_cast<float>(axis_val);
                    if (i >= 3)
                        break;
                }
                animationdata->_translationKeys[bone_name].emplace_back(Animation3DData::Vec3Key(keytime, Vec3{val}));
            }
            auto&& bone_keyframe_rotation = bone_keyframe[ROTATION];
            if (!bone_keyframe_rotation.error())
            {
                Quaternion val;
                int i = 0;
                for (auto&& axis_val : bone_keyframe_rotation)
                {
                    val.comps[i++] = static_cast<float>(axis_val);
                    if (i >= 4)
                        break;
                }
                animationdata->_rotationKeys[bone_name].emplace_back(Animation3DData::QuatKey(keytime, val));
            }
            auto&& bone_keyframe_scale = bone_keyframe[SCALE];
            if (!bone_keyframe_scale.error())
            {
                Vec3 val;
                int i = 0;
                for (auto&& axis_val : bone_keyframe_scale)
                {
                    val.comps[i++] = static_cast<float>(axis_val);
                    if (i >= 3)
                        break;
                }
                animationdata->_scaleKeys[bone_name].emplace_back(Animation3DData::Vec3Key(keytime, val));
            }
        }
    }

    return true;
}

bool Bundle3D::loadAnimationDataBinary(std::string_view id, Animation3DData* animationdata)
{

    if (_version == "0.1" || _version == "0.2" || _version == "0.3" || _version == "0.4")
    {
        if (!seekToFirstType(BUNDLE_TYPE_ANIMATIONS))
            return false;
    }
    else
    {
        // if id is not a null string, we need to add a suffix of "animation" for seeding.
        std::string id_{id};
        if (!id.empty())
            id_.append("animation");

        if (!seekToFirstType(BUNDLE_TYPE_ANIMATIONS, id_))
            return false;
    }

    try
    {
        unsigned int animNum = 1;
        if (_version == "0.3" || _version == "0.4")
        {
            if (!_binaryReader.read_blob(animNum))
            {
                AXLOGW("warning: Failed to read AnimationData: animNum '{}'.", _path);
                return false;
            }
        }

        bool has_found = false;
        for (unsigned int k = 0; k < animNum; ++k)
        {
            animationdata->resetData();
            std::string_view animId = _binaryReader.read_v32();

            if (!_binaryReader.read_blob(animationdata->_totalTime))
            {
                AXLOGW("warning: Failed to read AnimationData: totalTime '{}'.", _path);
                return false;
            }

            unsigned int nodeAnimationNum;
            if (!_binaryReader.read_blob(nodeAnimationNum))
            {
                AXLOGW("warning: Failed to read AnimationData: animNum '{}'.", _path);
                return false;
            }
            for (unsigned int i = 0; i < nodeAnimationNum; ++i)
            {
                std::string_view boneName = _binaryReader.read_v32();
                unsigned int keyframeNum;
                if (!_binaryReader.read_blob(keyframeNum))
                {
                    AXLOGW("warning: Failed to read AnimationData: keyframeNum '{}'.", _path);
                    return false;
                }

                animationdata->_rotationKeys[boneName].reserve(keyframeNum);
                animationdata->_scaleKeys[boneName].reserve(keyframeNum);
                animationdata->_translationKeys[boneName].reserve(keyframeNum);

                for (unsigned int j = 0; j < keyframeNum; ++j)
                {
                    float keytime;
                    if (!_binaryReader.read_blob(keytime))
                    {
                        AXLOGW("warning: Failed to read AnimationData: keytime '{}'.", _path);
                        return false;
                    }

                    // transform flag
                    unsigned char transformFlag(0);
                    if (_version != "0.1" && _version != "0.2" && _version != "0.3")
                    {
                        if (!_binaryReader.read_blob(transformFlag))
                        {
                            AXLOGW("warning: Failed to read AnimationData: transformFlag '{}'.", _path);
                            return false;
                        }
                    }

                    // rotation
                    bool hasRotate = true;
                    if (_version != "0.1" && _version != "0.2" && _version != "0.3")
                        hasRotate = transformFlag & 0x01;

                    if (hasRotate)
                    {
                        Quaternion rotate;
                        if (_binaryReader.read_blob(rotate.comps) != 4)
                        {
                            AXLOGW("warning: Failed to read AnimationData: rotate '{}'.", _path);
                            return false;
                        }
                        animationdata->_rotationKeys[boneName].emplace_back(Animation3DData::QuatKey(keytime, rotate));
                    }

                    // scale
                    bool hasScale = true;
                    if (_version != "0.1" && _version != "0.2" && _version != "0.3")
                        hasScale = (transformFlag >> 1) & 0x01;

                    if (hasScale)
                    {
                        Vec3 scale;
                        if (_binaryReader.read_blob(scale.comps) != 3)
                        {
                            AXLOGW("warning: Failed to read AnimationData: scale '{}'.", _path);
                            return false;
                        }
                        animationdata->_scaleKeys[boneName].emplace_back(Animation3DData::Vec3Key(keytime, scale));
                    }

                    // translation
                    bool hasTranslation = true;
                    if (_version != "0.1" && _version != "0.2" && _version != "0.3")
                        hasTranslation = (transformFlag >> 2) & 0x01;

                    if (hasTranslation)
                    {
                        Vec3 position;
                        if (_binaryReader.read_blob(position.comps) != 3)
                        {
                            AXLOGW("warning: Failed to read AnimationData: position '{}'.", _path);
                            return false;
                        }
                        animationdata->_translationKeys[boneName].emplace_back(
                            Animation3DData::Vec3Key(keytime, position));
                    }
                }
            }
            if (id == animId || id.empty())
            {
                has_found = true;
                break;
            }
        }
        if (!has_found)
        {
            animationdata->resetData();
            return false;
        }
        return true;
    }
    catch (const std::exception& ex)
    {
        AXLOGW("exception occurred: {}", ex.what());
    }
    return false;
}

bool Bundle3D::loadNodesJson(NodeDatas& nodedatas)
{
    auto&& nodes = _jsonReader[NODES];
    if (nodes.error())
        return false;

    // traverse the nodes again
    int node_count = nodes.count_elements();
    for (auto&& jnode : nodes.get_array())
    {

        NodeData* nodedata  = parseNodesRecursivelyJson(jnode, node_count == 1);

        bool isSkeleton = jnode[SKELETON];
        if (isSkeleton)
            nodedatas.skeleton.emplace_back(nodedata);
        else
            nodedatas.nodes.emplace_back(nodedata);
    }
    return true;
}
NodeData* Bundle3D::parseNodesRecursivelyJson(simdjson::simdjson_result<simdjson::ondemand::value> jvalue,
                                              bool singleSprite)
{
    NodeData* nodedata = new NodeData();
    // id
    nodedata->id = static_cast<std::string_view>(jvalue[ID]);

    // transform
    Mat4 transform;
    auto&& jtransform = jvalue[TRANSFORM];
    int j             = 0;
    for (auto&& val : jtransform.get_array())
    {
        transform.m[j++] = static_cast<float>(val);
    }

    nodedata->transform = transform;

    bool isSkin = false;

    // parts
    auto&& parts = jvalue[PARTS];
    if (!parts.error())
    {
        for (auto&& part : parts.get_array())
        {
            auto modelnodedata        = new ModelData();
            modelnodedata->subMeshId  = static_cast<std::string_view>(part[MESHPARTID]);
            modelnodedata->materialId = static_cast<std::string_view>(part[MATERIALID]);

            if (modelnodedata->subMeshId == "" || modelnodedata->materialId == "")
            {
                AXLOGW("warning: Node {} part is missing meshPartId or materialId", nodedata->id);
                AX_SAFE_DELETE(modelnodedata);
                AX_SAFE_DELETE(nodedata);
                return nullptr;
            }
            auto&& bones = part[BONES];
            if (!bones.error())
            {
                unsigned int bones_count = 0;
                for (auto&& bone : bones.get_array())
                {
                    // node
                    auto&& node_name = bone[NODE];
                    if (node_name.error())
                    {
                        AXLOGW("warning: Bone node ID missing");
                        AX_SAFE_DELETE(modelnodedata);
                        AX_SAFE_DELETE(nodedata);
                        return nullptr;
                    }

                    ++bones_count;
                    modelnodedata->bones.emplace_back(static_cast<std::string_view>(node_name));

                    Mat4 invbindpos;
                    auto&& jinvbindpos = bone[TRANSFORM];
                    int k              = 0;
                    for (auto&& val : jinvbindpos)
                    {
                        invbindpos.m[k] = static_cast<float>(val);
                    }

                    // invbindpos.inverse();
                    modelnodedata->invBindPose.emplace_back(invbindpos);
                }

                if (bones_count)
                    isSkin = true;
            }
            nodedata->modelNodeDatas.emplace_back(modelnodedata);
        }
    }

    // set transform
    if (_version == "0.1" || _version == "0.2" || _version == "0.3" || _version == "0.4" || _version == "0.5" ||
        _version == "0.6")
    {
        if (isSkin || singleSprite)
        {
            nodedata->transform = Mat4::IDENTITY;
        }
        else
        {
            nodedata->transform = transform;
        }
    }
    else
    {
        nodedata->transform = transform;
    }

    auto&& children = jvalue[CHILDREN].get_array();
    if (!children.error())
    {
        for (auto&& child : children)
        {
            NodeData* tempdata = parseNodesRecursivelyJson(child, singleSprite);
            nodedata->children.emplace_back(tempdata);
        }
    }
    return nodedata;
}

bool Bundle3D::loadNodesBinary(NodeDatas& nodedatas)
{
    if (!seekToFirstType(BUNDLE_TYPE_NODE))
        return false;

    unsigned int nodeSize = 0;
    if (_binaryReader.read_blob(nodeSize) != 1)
    {
        AXLOGW("warning: Failed to read nodes");
        return false;
    }

    // traverse the nodes again
    for (unsigned int i = 0; i < nodeSize; ++i)
    {
        bool skeleton      = false;
        NodeData* nodedata = parseNodesRecursivelyBinary(skeleton, nodeSize == 1);

        if (skeleton)
            nodedatas.skeleton.emplace_back(nodedata);
        else
            nodedatas.nodes.emplace_back(nodedata);
    }
    return true;
}
NodeData* Bundle3D::parseNodesRecursivelyBinary(bool& skeleton, bool singleSprite)
{
    try
    {
        // id
        std::string_view id = _binaryReader.read_v32();
        // is skeleton
        bool skeleton_;
        if (_binaryReader.read_blob(skeleton_) != 1)
        {
            AXLOGW("warning: Failed to read is skeleton");
            return nullptr;
        }
        if (skeleton_)
            skeleton = true;

        // transform
        Mat4 transform;
        if (!_binaryReader.read_blob(transform.m))
        {
            AXLOGW("warning: Failed to read transform matrix");
            return nullptr;
        }
        // parts
        unsigned int partsSize = 0;
        if (_binaryReader.read_blob(partsSize) != 1)
        {
            AXLOGW("warning: Failed to read meshdata: attribCount '{}'.", _path);
            return nullptr;
        }

        NodeData* nodedata = new NodeData();
        nodedata->id       = id;

        bool isSkin = false;

        if (partsSize > 0)
        {
            for (unsigned int i = 0; i < partsSize; ++i)
            {
                auto modelnodedata        = new ModelData();
                modelnodedata->subMeshId  = _binaryReader.read_v32();
                modelnodedata->materialId = _binaryReader.read_v32();

                if (modelnodedata->subMeshId.empty() || modelnodedata->materialId.empty())
                {
                    AXLOGW("Node {} part is missing meshPartId or materialId", nodedata->id);
                    AX_SAFE_DELETE(modelnodedata);
                    AX_SAFE_DELETE(nodedata);
                    return nullptr;
                }

                // read bone
                unsigned int bonesSize = 0;
                if (_binaryReader.read_blob(bonesSize) != 1)
                {
                    AXLOGW("warning: Failed to read meshdata: attribCount '{}'.", _path);
                    AX_SAFE_DELETE(modelnodedata);
                    AX_SAFE_DELETE(nodedata);
                    return nullptr;
                }

                if (bonesSize > 0)
                {
                    for (unsigned int j = 0; j < bonesSize; ++j)
                    {
                        std::string_view name = _binaryReader.read_v32();
                        modelnodedata->bones.emplace_back(name);

                        Mat4 invbindpos;
                        if (!_binaryReader.read_blob(invbindpos.m))
                        {
                            AX_SAFE_DELETE(modelnodedata);
                            AX_SAFE_DELETE(nodedata);
                            return nullptr;
                        }

                        modelnodedata->invBindPose.emplace_back(invbindpos);
                    }
                    isSkin = true;
                }
                unsigned int uvMapping = 0;
                if (_binaryReader.read_blob(uvMapping) != 1)
                {
                    AXLOGW("warning: Failed to read nodedata: uvMapping '{}'.", _path);
                    AX_SAFE_DELETE(modelnodedata);
                    AX_SAFE_DELETE(nodedata);
                    return nullptr;
                }
                for (unsigned int j = 0; j < uvMapping; ++j)
                {
                    unsigned int textureIndexSize = 0;
                    if (_binaryReader.read_blob(textureIndexSize) != 1)
                    {
                        AXLOGW("warning: Failed to read meshdata: attribCount '{}'.", _path);
                        AX_SAFE_DELETE(modelnodedata);
                        AX_SAFE_DELETE(nodedata);
                        return nullptr;
                    }
                    for (unsigned int k = 0; k < textureIndexSize; ++k)
                    {
                        unsigned int index = 0;
                        if (_binaryReader.read_blob(index) != 1)
                        {
                            AX_SAFE_DELETE(modelnodedata);
                            AX_SAFE_DELETE(nodedata);
                            return nullptr;
                        }
                    }
                }
                nodedata->modelNodeDatas.emplace_back(modelnodedata);
            }
        }

        // set transform
        if (_version == "0.1" || _version == "0.2" || _version == "0.3" || _version == "0.4" || _version == "0.5" ||
            _version == "0.6")
        {
            if (isSkin || singleSprite)
            {
                nodedata->transform = Mat4::IDENTITY;
            }
            else
            {
                nodedata->transform = transform;
            }
        }
        else
        {
            nodedata->transform = transform;
        }

        unsigned int childrenSize = 0;
        if (_binaryReader.read_blob(childrenSize) != 1)
        {
            AXLOGW("warning: Failed to read meshdata: attribCount '{}'.", _path);
            AX_SAFE_DELETE(nodedata);
            return nullptr;
        }
        if (childrenSize > 0)
        {
            for (unsigned int i = 0; i < childrenSize; ++i)
            {
                NodeData* tempdata = parseNodesRecursivelyBinary(skeleton, singleSprite);
                nodedata->children.emplace_back(tempdata);
            }
        }
        return nodedata;
    }
    catch (const std::exception& ex)
    {
        AXLOGW("exception occurred: {}", ex.what());
    }
    return nullptr;
}

backend::VertexFormat Bundle3D::parseGLDataType(std::string_view str, int size)
{
    backend::VertexFormat ret = backend::VertexFormat::INT;
    if (str == "GL_BYTE")
    {
        switch (size)
        {
        case 4:
            return backend::VertexFormat::UBYTE4;
        default:
            AXLOGE("parseVertexType GL_BYTE x {} error", size);
        }
    }
    else if (str == "GL_UNSIGNED_BYTE")
    {
        switch (size)
        {
        case 4:
            return backend::VertexFormat::UBYTE4;
        default:
            AXLOGE("parseVertexType GL_UNSIGNED_BYTE x {} error", size);
        }
    }
    else if (str == "GL_SHORT")
    {
        switch (size)
        {
        case 2:
            return backend::VertexFormat::USHORT2;
        case 4:
            return backend::VertexFormat::USHORT4;
        default:
            AXLOGE("parseVertexType GL_SHORT x {} error", size);
        }
    }
    else if (str == "GL_UNSIGNED_SHORT")
    {
        switch (size)
        {
        case 2:
            return backend::VertexFormat::USHORT2;
        case 4:
            return backend::VertexFormat::USHORT4;
        default:
            AXLOGE("parseVertexType GL_UNSIGNED_SHORT x {} error", size);
        }
    }
    else if (str == "GL_INT")
    {
        switch (size)
        {
        case 1:
            return backend::VertexFormat::INT;
        case 2:
            return backend::VertexFormat::INT2;
        case 3:
            return backend::VertexFormat::INT3;
        case 4:
            return backend::VertexFormat::INT4;
        default:
            AXLOGE("parseVertexType GL_INT x {} error", size);
        }
    }
    else if (str == "GL_UNSIGNED_INT")
    {
        switch (size)
        {
        case 1:
            return backend::VertexFormat::INT;
        case 2:
            return backend::VertexFormat::INT2;
        case 3:
            return backend::VertexFormat::INT3;
        case 4:
            return backend::VertexFormat::INT4;
        default:
            AXLOGE("parseVertexType GL_UNSIGNED_INT x {} error", size);
        }
    }
    else if (str == "GL_FLOAT")
    {
        switch (size)
        {
        case 1:
            return backend::VertexFormat::FLOAT;
        case 2:
            return backend::VertexFormat::FLOAT2;
        case 3:
            return backend::VertexFormat::FLOAT3;
        case 4:
            return backend::VertexFormat::FLOAT4;
        default:
            AXLOGE("parseVertexType GL_UNSIGNED_INT x {} error", size);
        }
    }
    AXASSERT(false, "parseVertexType failed!");
    return ret;
}

backend::SamplerAddressMode Bundle3D::parseSamplerAddressMode(std::string_view str)
{

    if (str == "REPEAT")
    {
        return backend::SamplerAddressMode::REPEAT;
    }
    else if (str == "CLAMP")
    {
        return backend::SamplerAddressMode::CLAMP_TO_EDGE;
    }
    else
    {
        AXASSERT(false, "Invalid GL type");
        return backend::SamplerAddressMode::REPEAT;
    }
}

NTextureData::Usage Bundle3D::parseGLTextureType(std::string_view str)
{
    if (str == "AMBIENT")
    {
        return NTextureData::Usage::Ambient;
    }
    else if (str == "BUMP")
    {
        return NTextureData::Usage::Bump;
    }
    else if (str == "DIFFUSE")
    {
        return NTextureData::Usage::Diffuse;
    }
    else if (str == "EMISSIVE")
    {
        return NTextureData::Usage::Emissive;
    }
    else if (str == "NONE")
    {
        return NTextureData::Usage::None;
    }
    else if (str == "NORMAL")
    {
        return NTextureData::Usage::Normal;
    }
    else if (str == "REFLECTION")
    {
        return NTextureData::Usage::Reflection;
    }
    else if (str == "SHININESS")
    {
        return NTextureData::Usage::Shininess;
    }
    else if (str == "SPECULAR")
    {
        return NTextureData::Usage::Specular;
    }
    else if (str == "TRANSPARENCY")
    {
        return NTextureData::Usage::Transparency;
    }
    else
    {
        AXASSERT(false, "Wrong Texture type");
        return NTextureData::Usage::Unknown;
    }
}
shaderinfos::VertexKey Bundle3D::parseGLProgramAttribute(std::string_view str)
{
    if (str == "VERTEX_ATTRIB_POSITION")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_POSITION;
    }
    else if (str == "VERTEX_ATTRIB_COLOR")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_COLOR;
    }
    else if (str == "VERTEX_ATTRIB_TEX_COORD")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD;
    }
    else if (str == "VERTEX_ATTRIB_TEX_COORD1")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD1;
    }
    else if (str == "VERTEX_ATTRIB_TEX_COORD2")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD2;
    }
    else if (str == "VERTEX_ATTRIB_TEX_COORD3")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD3;
    }
    // comment out them
    //    else if (str == "VERTEX_ATTRIB_TEX_COORD4")
    //    {
    //        return shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD4;
    //    }
    //    else if (str == "VERTEX_ATTRIB_TEX_COORD5")
    //    {
    //        return shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD5;
    //    }
    //    else if (str == "VERTEX_ATTRIB_TEX_COORD6")
    //    {
    //        return shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD6;
    //    }
    //    else if (str == "VERTEX_ATTRIB_TEX_COORD7")
    //    {
    //        return shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD7;
    //    }
    else if (str == "VERTEX_ATTRIB_NORMAL")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_NORMAL;
    }
    else if (str == "VERTEX_ATTRIB_BLEND_WEIGHT")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_BLEND_WEIGHT;
    }
    else if (str == "VERTEX_ATTRIB_BLEND_INDEX")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_BLEND_INDEX;
    }
    else if (str == "VERTEX_ATTRIB_TANGENT")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_TANGENT;
    }
    else if (str == "VERTEX_ATTRIB_BINORMAL")
    {
        return shaderinfos::VertexKey::VERTEX_ATTRIB_BINORMAL;
    }
    else
    {
        AXASSERT(false, "Wrong Attribute type");
        return shaderinfos::VertexKey::VERTEX_ATTRIB_ERROR;
    }
}

void Bundle3D::getModelRelativePath(std::string_view path)
{
    ssize_t index = path.find_last_of('/');
    std::string fullModelPath;
    _modelPath = path.substr(0, index + 1);
}

Reference* Bundle3D::seekToFirstType(unsigned int type, std::string_view id)
{
    // for each Reference
    for (unsigned int i = 0; i < _referenceCount; ++i)
    {
        Reference* ref = &_references[i];
        if (ref->type == type)
        {
            // if id is not a null string, we also need to check the Reference's id.
            if (id != "" && id != ref->id)
            {
                continue;
            }

            // Found a match
            if (_binaryReader.seek(ref->offset, SEEK_SET) == false)
            {
                AXLOGW("warning: Failed to seek to object '{}' in bundle '{}'.", ref->id, _path);
                return nullptr;
            }
            return ref;
        }
    }
    return nullptr;
}

std::vector<Vec3> Bundle3D::getTrianglesList(std::string_view path)
{
    std::vector<Vec3> trianglesList;

    if (path.length() <= 4)
        return trianglesList;

    auto bundle     = Bundle3D::createBundle();
    std::string ext = FileUtils::getPathExtension(path);
    MeshDatas meshs;
    if (ext == ".obj")
    {
        MaterialDatas materials;
        NodeDatas nodes;
        if (!Bundle3D::loadObj(meshs, materials, nodes, path))
        {
            Bundle3D::destroyBundle(bundle);
            return trianglesList;
        }
    }
    else
    {
        if (!bundle->load(path))
        {
            Bundle3D::destroyBundle(bundle);
            return trianglesList;
        }

        bundle->loadMeshDatas(meshs);
    }

    Bundle3D::destroyBundle(bundle);
    for (auto&& iter : meshs.meshDatas)
    {
        int preVertexSize = iter->getPerVertexSize() / sizeof(float);
        for (const auto& indices : iter->subMeshIndices)
        {
            indices.for_each([&](unsigned int ind) {
                trianglesList.emplace_back(Vec3(iter->vertex[ind * preVertexSize],
                                                iter->vertex[ind * preVertexSize + 1],
                                                iter->vertex[ind * preVertexSize + 2]));
            });
        }
    }

    return trianglesList;
}

Bundle3D::Bundle3D()
    : _modelPath(""), _path(""), _version(""), _referenceCount(0), _references(nullptr), _isBinary(false)
{}
Bundle3D::~Bundle3D()
{
    clear();
}

ax::AABB Bundle3D::calculateAABB(const std::vector<float>& vertex, int stride, const IndexArray& indices)
{
    AABB aabb;
    stride /= 4;

    indices.for_each([&](uint32_t i) {
        Vec3 point(vertex[i * stride], vertex[i * stride + 1], vertex[i * stride + 2]);
        aabb.updateMinMax(&point, 1);
    });

    return aabb;
}

}  // namespace ax
