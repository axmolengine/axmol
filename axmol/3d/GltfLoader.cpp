#include "axmol/3d/GltfLoader.h"

#include "axmol/platform/FileUtils.h"
#include "rapidjson/document.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace ax::GltfLoader
{
namespace
{
struct BufferView
{
    const uint8_t* data = nullptr;
    size_t size = 0;
};

struct Accessor
{
    const uint8_t* data = nullptr;
    size_t stride = 0;
    size_t count = 0;
    int componentType = 0;
    int components = 0;
    bool normalized = false;
};

struct SkinInfo
{
    std::vector<int> joints;
    std::vector<Mat4> inverseBindPoses;
};

uint32_t readUint32(const uint8_t* data)
{
    return static_cast<uint32_t>(data[0]) | (static_cast<uint32_t>(data[1]) << 8) |
           (static_cast<uint32_t>(data[2]) << 16) | (static_cast<uint32_t>(data[3]) << 24);
}

int componentCount(std::string_view type)
{
    if (type == "SCALAR") return 1;
    if (type == "VEC2") return 2;
    if (type == "VEC3") return 3;
    if (type == "VEC4") return 4;
    if (type == "MAT4") return 16;
    return 0;
}

size_t componentSize(int type)
{
    switch (type)
    {
    case 5120:
    case 5121: return 1;
    case 5122:
    case 5123: return 2;
    case 5125:
    case 5126: return 4;
    default: return 0;
    }
}

float readComponent(const uint8_t* value, int type, bool normalized)
{
    int8_t signedByte = 0;
    int16_t signedShort = 0;
    uint16_t unsignedShort = 0;
    uint32_t unsignedInt = 0;
    float floatValue = 0.0f;
    switch (type)
    {
    case 5120:
        std::memcpy(&signedByte, value, sizeof(signedByte));
        return normalized ? std::max(-1.0f, static_cast<float>(signedByte) / 127.0f) : static_cast<float>(signedByte);
    case 5121: return normalized ? static_cast<float>(*value) / 255.0f : static_cast<float>(*value);
    case 5122:
        std::memcpy(&signedShort, value, sizeof(signedShort));
        return normalized ? std::max(-1.0f, static_cast<float>(signedShort) / 32767.0f) : static_cast<float>(signedShort);
    case 5123:
        std::memcpy(&unsignedShort, value, sizeof(unsignedShort));
        return normalized ? static_cast<float>(unsignedShort) / 65535.0f : static_cast<float>(unsignedShort);
    case 5125:
        std::memcpy(&unsignedInt, value, sizeof(unsignedInt));
        return static_cast<float>(unsignedInt);
    case 5126:
        std::memcpy(&floatValue, value, sizeof(floatValue));
        return floatValue;
    default: return 0.0f;
    }
}

uint32_t readIndex(const uint8_t* value, int type)
{
    if (type == 5121)
        return *value;
    if (type == 5123)
    {
        uint16_t result = 0;
        std::memcpy(&result, value, sizeof(result));
        return result;
    }
    uint32_t result = 0;
    std::memcpy(&result, value, sizeof(result));
    return result;
}

bool decodeBase64(std::string_view source, std::vector<uint8_t>& output)
{
    static constexpr char alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int value = 0;
    int bits = -8;
    for (const char character : source)
    {
        if (character == '=') break;
        const char* found = std::strchr(alphabet, character);
        if (!found) continue;
        value = (value << 6) + static_cast<int>(found - alphabet);
        bits += 6;
        if (bits >= 0)
        {
            output.push_back(static_cast<uint8_t>((value >> bits) & 0xff));
            bits -= 8;
        }
    }
    return !output.empty();
}

bool decodeDataUri(std::string_view uri, std::vector<uint8_t>& output)
{
    if (!uri.starts_with("data:")) return false;
    const auto comma = uri.find(',');
    if (comma == std::string_view::npos) return false;
    const auto metadata = uri.substr(5, comma - 5);
    if (metadata.find(";base64") == std::string_view::npos) return false;
    return decodeBase64(uri.substr(comma + 1), output);
}

bool getAccessor(const rapidjson::Value& root, const std::vector<BufferView>& buffers, int index, Accessor& result)
{
    if (!root.HasMember("accessors") || !root["accessors"].IsArray() || index < 0 ||
        index >= static_cast<int>(root["accessors"].Size()))
        return false;
    const auto& accessor = root["accessors"][index];
    if (!accessor.IsObject() || !accessor.HasMember("bufferView") || !accessor["bufferView"].IsInt()) return false;
    const int viewIndex = accessor["bufferView"].GetInt();
    if (!root.HasMember("bufferViews") || !root["bufferViews"].IsArray() || viewIndex < 0 ||
        viewIndex >= static_cast<int>(root["bufferViews"].Size()))
        return false;
    const auto& view = root["bufferViews"][viewIndex];
    if (!view.IsObject() || !view.HasMember("buffer") || !view["buffer"].IsInt()) return false;
    const size_t viewOffset = view.HasMember("byteOffset") && view["byteOffset"].IsUint64()
                                  ? view["byteOffset"].GetUint64()
                                  : 0;
    if (!view.HasMember("byteLength") || !view["byteLength"].IsUint64()) return false;
    const size_t viewSize = view["byteLength"].GetUint64();
    const int bufferIndex = view["buffer"].GetInt();
    if (bufferIndex < 0 || bufferIndex >= static_cast<int>(buffers.size()) || viewOffset > buffers[bufferIndex].size ||
        viewSize > buffers[bufferIndex].size - viewOffset)
        return false;
    if (!accessor.HasMember("count") || !accessor["count"].IsUint64() || !accessor.HasMember("componentType") ||
        !accessor["componentType"].IsInt() || !accessor.HasMember("type") || !accessor["type"].IsString())
        return false;
    const size_t accessorOffset = accessor.HasMember("byteOffset") && accessor["byteOffset"].IsUint64()
                                      ? accessor["byteOffset"].GetUint64()
                                      : 0;
    result.count = accessor["count"].GetUint64();
    result.componentType = accessor["componentType"].GetInt();
    result.components = componentCount(accessor["type"].GetString());
    result.normalized = accessor.HasMember("normalized") && accessor["normalized"].IsBool() &&
                        accessor["normalized"].GetBool();
    const size_t elementSize = componentSize(result.componentType) * result.components;
    result.stride = view.HasMember("byteStride") ? view["byteStride"].GetUint64() : elementSize;
    if (result.components <= 0 || elementSize == 0 || result.stride < elementSize || accessorOffset > viewSize)
        return false;
    if (result.count > 0 && (result.count - 1) > (viewSize - accessorOffset - elementSize) / result.stride)
        return false;
    result.data = buffers[bufferIndex].data + viewOffset + accessorOffset;
    return true;
}

bool getBufferView(const rapidjson::Value& root, const std::vector<BufferView>& buffers, int index,
                   const uint8_t*& data, size_t& size)
{
    if (!root.HasMember("bufferViews") || !root["bufferViews"].IsArray() || index < 0 ||
        index >= static_cast<int>(root["bufferViews"].Size()))
        return false;
    const auto& view = root["bufferViews"][index];
    if (!view.IsObject() || !view.HasMember("buffer") || !view["buffer"].IsInt() ||
        !view.HasMember("byteLength") || !view["byteLength"].IsUint64())
        return false;
    const int bufferIndex = view["buffer"].GetInt();
    const size_t viewOffset = view.HasMember("byteOffset") && view["byteOffset"].IsUint64()
                                  ? view["byteOffset"].GetUint64()
                                  : 0;
    size = view["byteLength"].GetUint64();
    if (bufferIndex < 0 || bufferIndex >= static_cast<int>(buffers.size()) || viewOffset > buffers[bufferIndex].size ||
        size > buffers[bufferIndex].size - viewOffset)
        return false;
    data = buffers[bufferIndex].data + viewOffset;
    return true;
}

bool readMatrixAccessor(const Accessor& accessor, size_t index, Mat4& matrix)
{
    if (accessor.componentType != 5126 || accessor.components != 16 || index >= accessor.count) return false;
    const auto* values = accessor.data + index * accessor.stride;
    for (int component = 0; component < 16; ++component)
        matrix.m[component] = readComponent(values + component * sizeof(float), 5126, false);
    return true;
}

bool readBuffers(const rapidjson::Document& document, std::string_view path, const uint8_t* glbBin, size_t glbBinSize,
                 std::vector<std::vector<uint8_t>>& owned, std::vector<BufferView>& buffers)
{
    if (!document.HasMember("buffers") || !document["buffers"].IsArray()) return false;
    const auto directory = FileUtils::getPathDirName(path);
    size_t bufferIndex = 0;
    for (const auto& buffer : document["buffers"].GetArray())
    {
        if (!buffer.IsObject() || !buffer.HasMember("byteLength") || !buffer["byteLength"].IsUint64()) return false;
        const size_t declaredSize = buffer["byteLength"].GetUint64();
        std::vector<uint8_t> data;
        if (buffer.HasMember("uri") && buffer["uri"].IsString())
        {
            const std::string_view uri = buffer["uri"].GetString();
            if (uri.starts_with("data:"))
            {
                if (!decodeDataUri(uri, data)) return false;
            }
            else
            {
                const auto fileData = FileUtils::getInstance()->getDataFromFile(directory + "/" + std::string(uri));
                if (!fileData.getBytes() || fileData.getSize() == 0) return false;
                data.assign(fileData.getBytes(), fileData.getBytes() + fileData.getSize());
            }
        }
        else if (glbBin && bufferIndex == 0)
        {
            if (declaredSize > glbBinSize) return false;
            data.assign(glbBin, glbBin + declaredSize);
        }
        else return false;
        if (data.size() < declaredSize) return false;
        data.resize(declaredSize);
        owned.emplace_back(std::move(data));
        buffers.push_back({owned.back().data(), owned.back().size()});
        ++bufferIndex;
    }
    return true;
}

bool readDocument(std::string_view path, rapidjson::Document& document, std::vector<uint8_t>& bin)
{
    const auto data = FileUtils::getInstance()->getDataFromFile(path);
    if (!data.getBytes() || data.getSize() == 0) return false;
    if (data.getSize() >= 12 && std::memcmp(data.getBytes(), "glTF", 4) == 0)
    {
        const auto* bytes = data.getBytes();
        const uint32_t version = readUint32(bytes + 4);
        const uint32_t length = readUint32(bytes + 8);
        if (version != 2 || length > data.getSize() || length < 12) return false;
        size_t offset = 12;
        std::string json;
        while (offset + 8 <= data.getSize() && offset < length)
        {
            const uint32_t chunkLength = readUint32(bytes + offset);
            const uint32_t chunkType = readUint32(bytes + offset + 4);
            offset += 8;
            if (offset + chunkLength > length) return false;
            if (chunkType == 0x4e4f534a) json.assign(reinterpret_cast<const char*>(bytes + offset), chunkLength);
            if (chunkType == 0x004e4942) bin.assign(bytes + offset, bytes + offset + chunkLength);
            offset += chunkLength;
        }
        if (json.empty()) return false;
        document.Parse(json.data(), json.size());
    }
    else document.Parse(reinterpret_cast<const char*>(data.getBytes()), data.getSize());
    return !document.HasParseError() && document.IsObject();
}

Mat4 readNodeTransform(const rapidjson::Value& node)
{
    if (node.HasMember("matrix") && node["matrix"].IsArray() && node["matrix"].Size() == 16)
    {
        float matrix[16];
        for (rapidjson::SizeType index = 0; index < 16; ++index)
        {
            if (!node["matrix"][index].IsNumber())
            {
                Mat4 identity;
                identity.setIdentity();
                return identity;
            }
            matrix[index] = node["matrix"][index].GetFloat();
        }
        return Mat4(matrix);
    }

    Vec3 translation;
    Vec3 scale(1.0f, 1.0f, 1.0f);
    Quat rotation;
    if (node.HasMember("translation") && node["translation"].IsArray() && node["translation"].Size() == 3 &&
        node["translation"][0].IsNumber() && node["translation"][1].IsNumber() &&
        node["translation"][2].IsNumber())
        translation.set(node["translation"][0].GetFloat(), node["translation"][1].GetFloat(),
                        node["translation"][2].GetFloat());
    if (node.HasMember("scale") && node["scale"].IsArray() && node["scale"].Size() == 3 &&
        node["scale"][0].IsNumber() && node["scale"][1].IsNumber() && node["scale"][2].IsNumber())
        scale.set(node["scale"][0].GetFloat(), node["scale"][1].GetFloat(), node["scale"][2].GetFloat());
    if (node.HasMember("rotation") && node["rotation"].IsArray() && node["rotation"].Size() == 4 &&
        node["rotation"][0].IsNumber() && node["rotation"][1].IsNumber() && node["rotation"][2].IsNumber() &&
        node["rotation"][3].IsNumber())
        rotation.set(node["rotation"][0].GetFloat(), node["rotation"][1].GetFloat(), node["rotation"][2].GetFloat(),
                     node["rotation"][3].GetFloat());

    Mat4 translationMatrix;
    Mat4 rotationMatrix;
    Mat4 scaleMatrix;
    Mat4::createTranslation(translation, &translationMatrix);
    Mat4::createRotation(rotation, &rotationMatrix);
    Mat4::createScale(scale, &scaleMatrix);
    return translationMatrix * rotationMatrix * scaleMatrix;
}
}

bool load(MeshDatas& meshes, MaterialDatas& materials, NodeDatas& nodes, std::string_view path)
{
    rapidjson::Document document;
    std::vector<uint8_t> glbBin;
    if (!readDocument(path, document, glbBin)) return false;
    std::vector<std::vector<uint8_t>> owned;
    std::vector<BufferView> buffers;
    if (!readBuffers(document, path, glbBin.empty() ? nullptr : glbBin.data(), glbBin.size(), owned, buffers)) return false;

    std::vector<SkinInfo> skins;
    if (document.HasMember("skins") && document["skins"].IsArray())
    {
        skins.reserve(document["skins"].Size());
        for (const auto& sourceSkin : document["skins"].GetArray())
        {
            if (!sourceSkin.IsObject() || !sourceSkin.HasMember("joints") || !sourceSkin["joints"].IsArray())
                return false;
            SkinInfo skin;
            for (const auto& joint : sourceSkin["joints"].GetArray())
            {
                if (!joint.IsUint()) return false;
                skin.joints.emplace_back(static_cast<int>(joint.GetUint()));
            }
            if (sourceSkin.HasMember("inverseBindMatrices"))
            {
                if (!sourceSkin["inverseBindMatrices"].IsInt()) return false;
                Accessor inverseBindMatrices;
                if (!getAccessor(document, buffers, sourceSkin["inverseBindMatrices"].GetInt(), inverseBindMatrices) ||
                    inverseBindMatrices.count != skin.joints.size())
                    return false;
                skin.inverseBindPoses.resize(skin.joints.size());
                for (size_t index = 0; index < skin.joints.size(); ++index)
                    if (!readMatrixAccessor(inverseBindMatrices, index, skin.inverseBindPoses[index])) return false;
            }
            else
            {
                skin.inverseBindPoses.resize(skin.joints.size());
                for (auto& matrix : skin.inverseBindPoses)
                    matrix.setIdentity();
            }
            skins.emplace_back(std::move(skin));
        }
    }

    if (document.HasMember("materials") && document["materials"].IsArray())
    {
        for (const auto& source : document["materials"].GetArray())
        {
            NMaterialData material;
            material.id = std::to_string(materials.materials.size());
            if (source.IsObject() && source.HasMember("pbrMetallicRoughness") &&
                source["pbrMetallicRoughness"].IsObject() &&
                source["pbrMetallicRoughness"].HasMember("baseColorTexture") &&
                source["pbrMetallicRoughness"]["baseColorTexture"].IsObject() &&
                source["pbrMetallicRoughness"]["baseColorTexture"].HasMember("index") &&
                source["pbrMetallicRoughness"]["baseColorTexture"]["index"].IsInt())
            {
                const int textureIndex = source["pbrMetallicRoughness"]["baseColorTexture"]["index"].GetInt();
                if (document.HasMember("textures") && document["textures"].IsArray() && textureIndex >= 0 &&
                    textureIndex < static_cast<int>(document["textures"].Size()))
                {
                    const auto& textureSource = document["textures"][textureIndex];
                    const int imageIndex = textureSource.IsObject() && textureSource.HasMember("source") &&
                                                   textureSource["source"].IsInt()
                                               ? textureSource["source"].GetInt()
                                               : -1;
                    if (document.HasMember("images") && document["images"].IsArray() && imageIndex >= 0 &&
                        imageIndex < static_cast<int>(document["images"].Size()))
                    {
                        const auto& image = document["images"][imageIndex];
                        NTextureData texture;
                        texture.type = NTextureData::Usage::Diffuse;
                        texture.wrapS = rhi::SamplerAddressMode::REPEAT;
                        texture.wrapT = rhi::SamplerAddressMode::REPEAT;
                        texture.filename = std::string(path) + "#image" + std::to_string(imageIndex);
                        if (image.IsObject() && image.HasMember("uri") && image["uri"].IsString())
                        {
                            const std::string_view uri = image["uri"].GetString();
                            if (uri.starts_with("data:"))
                            {
                                std::vector<uint8_t> imageBytes;
                                if (!decodeDataUri(uri, imageBytes))
                                    return false;
                                texture.imageData.copy(imageBytes.data(), imageBytes.size());
                            }
                            else
                            {
                                texture.filename = FileUtils::getPathDirName(path) + "/" + std::string(uri);
                            }
                        }
                        else if (image.IsObject() && image.HasMember("bufferView") && image["bufferView"].IsInt())
                        {
                            const uint8_t* imageBytes = nullptr;
                            size_t imageSize = 0;
                            if (!getBufferView(document, buffers, image["bufferView"].GetInt(), imageBytes, imageSize))
                                return false;
                            texture.imageData.copy(imageBytes, imageSize);
                        }
                        material.textures.emplace_back(std::move(texture));
                    }
                }
            }
            materials.materials.emplace_back(std::move(material));
        }
    }

    if (!document.HasMember("meshes") || !document["meshes"].IsArray()) return false;
    size_t meshIndex = 0;
    std::vector<std::vector<std::string>> primitiveIds;
    std::vector<std::vector<std::string>> primitiveMaterials;
    for (const auto& sourceMesh : document["meshes"].GetArray())
    {
        primitiveIds.emplace_back();
        primitiveMaterials.emplace_back();
        if (!sourceMesh.IsObject()) return false;
        if (!sourceMesh.HasMember("primitives") || !sourceMesh["primitives"].IsArray()) continue;
        for (const auto& primitive : sourceMesh["primitives"].GetArray())
        {
            if (!primitive.IsObject()) return false;
            if (primitive.HasMember("mode") && (!primitive["mode"].IsInt() || primitive["mode"].GetInt() != 4))
                continue;
            if (!primitive.HasMember("attributes") || !primitive["attributes"].IsObject() ||
                !primitive["attributes"].HasMember("POSITION") || !primitive["attributes"]["POSITION"].IsInt())
                continue;
            Accessor position;
            if (!getAccessor(document, buffers, primitive["attributes"]["POSITION"].GetInt(), position)) return false;
            if (position.components != 3) return false;
            Accessor normal, texcoord, joints, weights, indices;
            const bool hasNormal = primitive["attributes"].HasMember("NORMAL") &&
                                   primitive["attributes"]["NORMAL"].IsInt() &&
                                   getAccessor(document, buffers, primitive["attributes"]["NORMAL"].GetInt(), normal) &&
                                   normal.components == 3 && normal.count == position.count;
            const bool hasTexcoord = primitive["attributes"].HasMember("TEXCOORD_0") &&
                                     primitive["attributes"]["TEXCOORD_0"].IsInt() &&
                                     getAccessor(document, buffers, primitive["attributes"]["TEXCOORD_0"].GetInt(), texcoord) &&
                                     texcoord.components == 2 && texcoord.count == position.count;
            const bool hasJoints = primitive["attributes"].HasMember("JOINTS_0") &&
                                   primitive["attributes"]["JOINTS_0"].IsInt() &&
                                   getAccessor(document, buffers, primitive["attributes"]["JOINTS_0"].GetInt(), joints) &&
                                   joints.components == 4 && joints.count == position.count &&
                                   (joints.componentType == 5121 || joints.componentType == 5123);
            const bool hasWeights = primitive["attributes"].HasMember("WEIGHTS_0") &&
                                    primitive["attributes"]["WEIGHTS_0"].IsInt() &&
                                    getAccessor(document, buffers, primitive["attributes"]["WEIGHTS_0"].GetInt(), weights) &&
                                    weights.components == 4 && weights.count == position.count;
            const bool hasSkinAttributes = hasJoints && hasWeights;
            bool hasIndices = false;
            if (primitive.HasMember("indices"))
            {
                if (!primitive["indices"].IsInt() || !getAccessor(document, buffers, primitive["indices"].GetInt(), indices) ||
                    indices.components != 1 ||
                    (indices.componentType != 5121 && indices.componentType != 5123 && indices.componentType != 5125))
                    return false;
                hasIndices = true;
            }
            auto* mesh = new MeshData();
            auto addAttrib = [&](MeshVertexAttribute attribute, rhi::VertexElementType type) {
                MeshVertexAttrib value;
                value.vertexAttrib = attribute;
                value.type = type;
                mesh->attribs.emplace_back(value);
            };
            addAttrib(MeshVertexAttribute::POSITION, rhi::VertexElementType::FLOAT3);
            if (hasNormal) addAttrib(MeshVertexAttribute::NORMAL, rhi::VertexElementType::FLOAT3);
            if (hasTexcoord) addAttrib(MeshVertexAttribute::TEXCOORD0, rhi::VertexElementType::FLOAT2);
            if (hasSkinAttributes)
            {
                addAttrib(MeshVertexAttribute::BLENDINDICES, rhi::VertexElementType::FLOAT4);
                addAttrib(MeshVertexAttribute::BLENDWEIGHT, rhi::VertexElementType::FLOAT4);
            }
            for (size_t vertex = 0; vertex < position.count; ++vertex)
            {
                const auto* p = position.data + vertex * position.stride;
                for (int component = 0; component < 3; ++component) mesh->vertex.emplace_back(readComponent(p + component * componentSize(position.componentType), position.componentType, position.normalized));
                if (hasNormal) for (int component = 0; component < 3; ++component) mesh->vertex.emplace_back(readComponent(normal.data + vertex * normal.stride + component * componentSize(normal.componentType), normal.componentType, normal.normalized));
                if (hasTexcoord) for (int component = 0; component < 2; ++component) mesh->vertex.emplace_back(readComponent(texcoord.data + vertex * texcoord.stride + component * componentSize(texcoord.componentType), texcoord.componentType, texcoord.normalized));
                if (hasSkinAttributes)
                {
                    for (int component = 0; component < 4; ++component)
                        mesh->vertex.emplace_back(readComponent(joints.data + vertex * joints.stride + component * componentSize(joints.componentType),
                                                                joints.componentType, false));
                    for (int component = 0; component < 4; ++component)
                        mesh->vertex.emplace_back(readComponent(weights.data + vertex * weights.stride + component * componentSize(weights.componentType),
                                                                weights.componentType, weights.normalized));
                }
            }
            mesh->vertexSizeInFloat = 3 + (hasNormal ? 3 : 0) + (hasTexcoord ? 2 : 0) + (hasSkinAttributes ? 8 : 0);
            const bool use32BitIndices = (hasIndices && indices.componentType == 5125) ||
                                         (!hasIndices && position.count > UINT16_MAX);
            IndexArray indexArray(use32BitIndices ? rhi::IndexFormat::U_INT : rhi::IndexFormat::U_SHORT);
            if (hasIndices)
            {
                for (size_t index = 0; index < indices.count; ++index)
                {
                    const auto value = readIndex(indices.data + index * indices.stride, indices.componentType);
                    if (indexArray.format() == rhi::IndexFormat::U_INT) indexArray.emplace_back<uint32_t>(value);
                    else indexArray.emplace_back<uint16_t>(static_cast<uint16_t>(value));
                }
            }
            else
            {
                for (uint32_t index = 0; index < position.count; ++index)
                {
                    if (indexArray.format() == rhi::IndexFormat::U_INT)
                        indexArray.emplace_back<uint32_t>(index);
                    else
                        indexArray.emplace_back<uint16_t>(static_cast<uint16_t>(index));
                }
            }
            mesh->subMeshIndices.emplace_back(std::move(indexArray));
            mesh->subMeshIds.emplace_back(std::to_string(meshIndex));
            meshes.meshDatas.emplace_back(mesh);
            primitiveIds.back().emplace_back(std::to_string(meshIndex));
            primitiveMaterials.back().emplace_back(primitive.HasMember("material") && primitive["material"].IsInt()
                                                       ? std::to_string(primitive["material"].GetInt())
                                                       : std::string{});
            ++meshIndex;
        }
    }

    if (document.HasMember("nodes") && document["nodes"].IsArray())
    {
        const auto& sourceNodes = document["nodes"];
        for (const auto& skin : skins)
            for (const auto joint : skin.joints)
                if (joint < 0 || joint >= static_cast<int>(sourceNodes.Size()) || !sourceNodes[joint].IsObject())
                    return false;
        auto nodeName = [&](int nodeIndex) {
            const auto& node = sourceNodes[static_cast<rapidjson::SizeType>(nodeIndex)];
            return node.HasMember("name") && node["name"].IsString() ? std::string(node["name"].GetString())
                                                                       : std::to_string(nodeIndex);
        };
        for (const auto& skin : skins)
        {
            std::vector<bool> isJoint(sourceNodes.Size());
            std::vector<bool> hasJointParent(sourceNodes.Size());
            for (const auto joint : skin.joints)
                if (joint >= 0 && joint < static_cast<int>(sourceNodes.Size())) isJoint[joint] = true;
            for (const auto joint : skin.joints)
            {
                if (!isJoint[joint]) continue;
                const auto& sourceNode = sourceNodes[joint];
                if (!sourceNode.IsObject() || !sourceNode.HasMember("children") || !sourceNode["children"].IsArray())
                    continue;
                for (const auto& child : sourceNode["children"].GetArray())
                    if (child.IsUint() && child.GetUint() < isJoint.size() && isJoint[child.GetUint()])
                        hasJointParent[child.GetUint()] = true;
            }
            auto makeBone = [&](auto&& self, int nodeIndex) -> NodeData* {
                const auto& sourceNode = sourceNodes[nodeIndex];
                auto* bone = new NodeData();
                bone->id = nodeName(nodeIndex);
                bone->transform = readNodeTransform(sourceNode);
                if (sourceNode.HasMember("children") && sourceNode["children"].IsArray())
                    for (const auto& child : sourceNode["children"].GetArray())
                        if (child.IsUint() && child.GetUint() < isJoint.size() && isJoint[child.GetUint()])
                            bone->children.emplace_back(self(self, static_cast<int>(child.GetUint())));
                return bone;
            };
            for (const auto joint : skin.joints)
                if (joint >= 0 && joint < static_cast<int>(sourceNodes.Size()) && !hasJointParent[joint])
                    nodes.skeleton.emplace_back(makeBone(makeBone, joint));
        }
        std::vector<bool> hasParent(sourceNodes.Size());
        for (const auto& sourceNode : sourceNodes.GetArray())
        {
            if (!sourceNode.IsObject()) return false;
            if (!sourceNode.HasMember("children") || !sourceNode["children"].IsArray()) continue;
            for (const auto& child : sourceNode["children"].GetArray())
                if (child.IsUint() && child.GetUint() < hasParent.size()) hasParent[child.GetUint()] = true;
        }

        auto addNode = [&](auto&& self, int nodeIndex, const Mat4& parentTransform) -> void {
            if (nodeIndex < 0 || nodeIndex >= static_cast<int>(sourceNodes.Size())) return;
            const auto& sourceNode = sourceNodes[nodeIndex];
            if (!sourceNode.IsObject()) return;
            const Mat4 worldTransform = parentTransform * readNodeTransform(sourceNode);
            if (sourceNode.HasMember("mesh") && sourceNode["mesh"].IsUint() &&
                sourceNode["mesh"].GetUint() < primitiveIds.size())
            {
                const auto meshId = sourceNode["mesh"].GetUint();
                for (size_t primitive = 0; primitive < primitiveIds[meshId].size(); ++primitive)
                {
                    auto* node = new NodeData();
                    node->id = sourceNode.HasMember("name") && sourceNode["name"].IsString()
                                   ? sourceNode["name"].GetString()
                                   : std::to_string(nodeIndex);
                    node->transform = worldTransform;
                    auto* model = new ModelData();
                    model->subMeshId = primitiveIds[meshId][primitive];
                    model->materialId = primitiveMaterials[meshId][primitive];
                    if (sourceNode.HasMember("skin") && sourceNode["skin"].IsUint() &&
                        sourceNode["skin"].GetUint() < skins.size())
                    {
                        const auto& skin = skins[sourceNode["skin"].GetUint()];
                        for (const auto joint : skin.joints)
                            model->bones.emplace_back(nodeName(joint));
                        model->invBindPose = skin.inverseBindPoses;
                    }
                    node->modelNodeDatas.emplace_back(model);
                    nodes.nodes.emplace_back(node);
                }
            }
            if (sourceNode.HasMember("children") && sourceNode["children"].IsArray())
                for (const auto& child : sourceNode["children"].GetArray())
                    if (child.IsUint()) self(self, static_cast<int>(child.GetUint()), worldTransform);
        };

        Mat4 identity;
        bool addedSceneNodes = false;
        if (document.HasMember("scenes") && document["scenes"].IsArray() && !document["scenes"].Empty())
        {
            size_t sceneIndex = document.HasMember("scene") && document["scene"].IsUint()
                                    ? document["scene"].GetUint()
                                    : 0;
            if (sceneIndex < document["scenes"].Size() && document["scenes"][sceneIndex].HasMember("nodes") &&
                document["scenes"][sceneIndex]["nodes"].IsArray())
            {
                for (const auto& root : document["scenes"][sceneIndex]["nodes"].GetArray())
                    if (root.IsUint()) self(addNode, static_cast<int>(root.GetUint()), identity);
                addedSceneNodes = true;
            }
        }
        if (!addedSceneNodes)
            for (rapidjson::SizeType index = 0; index < sourceNodes.Size(); ++index)
                if (!hasParent[index]) addNode(addNode, static_cast<int>(index), identity);
    }

    if (nodes.nodes.empty())
    {
        for (size_t index = 0; index < meshIndex; ++index)
        {
            auto* node = new NodeData();
            node->id = std::to_string(index);
            auto* model = new ModelData();
            model->subMeshId = std::to_string(index);
            node->modelNodeDatas.emplace_back(model);
            nodes.nodes.emplace_back(node);
        }
    }
    return !meshes.meshDatas.empty();
}

bool loadAnimationData(Animation3DData& animation, std::string_view path, std::string_view animationName)
{
    animation.resetData();
    rapidjson::Document document;
    std::vector<uint8_t> glbBin;
    if (!readDocument(path, document, glbBin)) return false;

    std::vector<std::vector<uint8_t>> owned;
    std::vector<BufferView> buffers;
    if (!readBuffers(document, path, glbBin.empty() ? nullptr : glbBin.data(), glbBin.size(), owned, buffers))
        return false;
    if (!document.HasMember("animations") || !document["animations"].IsArray()) return false;

    const rapidjson::Value* sourceAnimation = nullptr;
    for (rapidjson::SizeType index = 0; index < document["animations"].Size(); ++index)
    {
        const auto& candidate = document["animations"][index];
        if (animationName.empty() && index == 0)
            sourceAnimation = &candidate;
        else if (candidate.IsObject() && candidate.HasMember("name") && candidate["name"].IsString() &&
                 candidate["name"].GetString() == animationName)
            sourceAnimation = &candidate;
        if (sourceAnimation) break;
    }
    if (!sourceAnimation || !sourceAnimation->IsObject() || !sourceAnimation->HasMember("samplers") ||
        !sourceAnimation->HasMember("channels") || !(*sourceAnimation)["samplers"].IsArray() ||
        !(*sourceAnimation)["channels"].IsArray())
        return false;

    std::vector<std::string> nodeNames;
    if (document.HasMember("nodes") && document["nodes"].IsArray())
    {
        nodeNames.reserve(document["nodes"].Size());
        for (rapidjson::SizeType index = 0; index < document["nodes"].Size(); ++index)
        {
            const auto& node = document["nodes"][index];
            nodeNames.emplace_back(node.IsObject() && node.HasMember("name") && node["name"].IsString()
                                       ? node["name"].GetString()
                                       : std::to_string(index));
        }
    }

    for (const auto& channel : (*sourceAnimation)["channels"].GetArray())
    {
        if (!channel.IsObject() || !channel.HasMember("sampler") || !channel["sampler"].IsUint() ||
            !channel.HasMember("target") || !channel["target"].IsObject())
            return false;
        const auto& target = channel["target"];
        if (!target.HasMember("node") || !target["node"].IsUint() || !target.HasMember("path") ||
            !target["path"].IsString() || target["node"].GetUint() >= nodeNames.size())
            return false;
        const auto samplerIndex = channel["sampler"].GetUint();
        const auto& samplers = (*sourceAnimation)["samplers"];
        if (samplerIndex >= samplers.Size() || !samplers[samplerIndex].IsObject()) return false;
        const auto& sampler = samplers[samplerIndex];
        if (!sampler.HasMember("input") || !sampler.HasMember("output") || !sampler["input"].IsInt() ||
            !sampler["output"].IsInt())
            return false;
        if (sampler.HasMember("interpolation") && !sampler["interpolation"].IsString()) return false;
        const std::string_view interpolation = sampler.HasMember("interpolation")
                                                   ? sampler["interpolation"].GetString()
                                                   : "LINEAR";
        if (interpolation == "CUBICSPLINE" || interpolation == "STEP") return false;
        if (interpolation != "LINEAR") return false;
        Accessor input;
        Accessor output;
        const std::string_view pathName = target["path"].GetString();
        if (pathName != "translation" && pathName != "rotation" && pathName != "scale") return false;
        if (!getAccessor(document, buffers, sampler["input"].GetInt(), input) ||
            !getAccessor(document, buffers, sampler["output"].GetInt(), output) || input.componentType != 5126 ||
            output.componentType != 5126 || input.components != 1 ||
            ((pathName == "rotation" && output.components != 4) ||
             (pathName != "rotation" && output.components != 3)) ||
            input.count != output.count)
            return false;

        const std::string nodeName = nodeNames[target["node"].GetUint()];
        for (size_t key = 0; key < input.count; ++key)
        {
            const float time = readComponent(input.data + key * input.stride, input.componentType, false);
            animation._totalTime = std::max(animation._totalTime, time);
            const auto* values = output.data + key * output.stride;
            if (pathName == "translation" && output.components == 3)
            {
                animation._translationKeys[nodeName].emplace_back(
                    time, Vec3(readComponent(values, 5126, false), readComponent(values + 4, 5126, false),
                               readComponent(values + 8, 5126, false)));
            }
            else if (pathName == "scale" && output.components == 3)
            {
                animation._scaleKeys[nodeName].emplace_back(
                    time, Vec3(readComponent(values, 5126, false), readComponent(values + 4, 5126, false),
                               readComponent(values + 8, 5126, false)));
            }
            else if (pathName == "rotation" && output.components == 4)
            {
                Quat rotation(readComponent(values, 5126, false), readComponent(values + 4, 5126, false),
                              readComponent(values + 8, 5126, false), readComponent(values + 12, 5126, false));
                rotation.normalize();
                animation._rotationKeys[nodeName].emplace_back(time, rotation);
            }
        }
    }
    return animation._totalTime > 0.0f || !animation._translationKeys.empty() || !animation._rotationKeys.empty() ||
           !animation._scaleKeys.empty();
}
}