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

#include "3d/MeshRenderer.h"
#include "3d/ObjLoader.h"
#include "3d/MeshSkin.h"
#include "3d/Bundle3D.h"
#include "3d/MeshMaterial.h"
#include "3d/AttachNode.h"
#include "3d/Mesh.h"

#include "base/Director.h"
#include "base/UTF8.h"
#include "base/Utils.h"
#include "2d/Light.h"
#include "2d/Camera.h"
#include "base/Macros.h"
#include "platform/PlatformMacros.h"
#include "platform/FileUtils.h"
#include "renderer/TextureCache.h"
#include "renderer/Renderer.h"
#include "renderer/Material.h"
#include "renderer/Technique.h"
#include "renderer/Pass.h"

NS_AX_BEGIN

static MeshMaterial* getMeshRendererMaterialForAttribs(MeshVertexData* meshVertexData, bool usesLight);

MeshRenderer* MeshRenderer::create()
{
    auto mesh = new MeshRenderer();
    if (mesh->init())
    {
        mesh->autorelease();
        return mesh;
    }
    AX_SAFE_DELETE(mesh);
    return nullptr;
}

MeshRenderer* MeshRenderer::create(std::string_view modelPath)
{
    return create(modelPath, hlookup::empty_sv);
}

MeshRenderer* MeshRenderer::create(std::string_view modelPath, std::string_view texturePath)
{
    AXASSERT(modelPath.length() >= 4, "Invalid filename.");

    auto meshRenderer = new MeshRenderer();
    if (meshRenderer->initWithFile(modelPath))
    {
        meshRenderer->setModelTexture(modelPath, texturePath);
        meshRenderer->_contentSize = meshRenderer->getBoundingBox().size;
        meshRenderer->autorelease();
        return meshRenderer;
    }
    AX_SAFE_DELETE(meshRenderer);
    return nullptr;
}

void MeshRenderer::createAsync(std::string_view modelPath,
                               const std::function<void(MeshRenderer*, void*)>& callback,
                               void* callbackparam)
{
    createAsync(modelPath, "", callback, callbackparam);
}

void MeshRenderer::createAsync(std::string_view modelPath,
                               std::string_view texturePath,
                               const std::function<void(MeshRenderer*, void*)>& callback,
                               void* callbackparam)
{
    MeshRenderer* meshRenderer = new MeshRenderer();
    if (meshRenderer->loadFromCache(modelPath))
    {
        meshRenderer->autorelease();
        meshRenderer->setModelTexture(modelPath, texturePath);
        callback(meshRenderer, callbackparam);
        return;
    }

    meshRenderer->_asyncLoadParam.afterLoadCallback = callback;
    meshRenderer->_asyncLoadParam.texPath           = texturePath;
    meshRenderer->_asyncLoadParam.modelPath         = modelPath;
    meshRenderer->_asyncLoadParam.modelFullPath     = FileUtils::getInstance()->fullPathForFilename(modelPath);
    meshRenderer->_asyncLoadParam.callbackParam     = callbackparam;
    meshRenderer->_asyncLoadParam.materialdatas     = new MaterialDatas();
    meshRenderer->_asyncLoadParam.meshdatas         = new MeshDatas();
    meshRenderer->_asyncLoadParam.nodeDatas         = new NodeDatas();

    auto director = Director::getInstance();
    director->getJobSystem()->enqueue(
        [director, meshRenderer] {
        auto& loadParam  = meshRenderer->_asyncLoadParam;
        loadParam.result = meshRenderer->loadFromFile(loadParam.modelFullPath, loadParam.nodeDatas, loadParam.meshdatas,
                                                      loadParam.materialdatas);
    },
        [meshRenderer] { meshRenderer->afterAsyncLoad(&meshRenderer->_asyncLoadParam); });
}

void MeshRenderer::afterAsyncLoad(void* param)
{
    MeshRenderer::AsyncLoadParam* asyncParam = (MeshRenderer::AsyncLoadParam*)param;
    autorelease();
    if (asyncParam)
    {
        if (asyncParam->result)
        {
            _meshes.clear();
            _meshVertexDatas.clear();
            AX_SAFE_RELEASE_NULL(_skeleton);
            removeAllAttachNode();

            // create in the main thread
            auto& meshdatas     = asyncParam->meshdatas;
            auto& materialdatas = asyncParam->materialdatas;
            auto& nodeDatas     = asyncParam->nodeDatas;
            if (initFrom(*nodeDatas, *meshdatas, *materialdatas))
            {
                auto meshdata = MeshRendererCache::getInstance()->getMeshRenderData(asyncParam->modelPath);
                if (meshdata == nullptr)
                {
                    // add to cache
                    auto data             = new MeshRendererCache::MeshRenderData();
                    data->materialdatas   = materialdatas;
                    data->nodedatas       = nodeDatas;
                    data->meshVertexDatas = _meshVertexDatas;
                    for (const auto mesh : _meshes)
                    {
                        data->programStates.pushBack(mesh->getProgramState());
                    }

                    MeshRendererCache::getInstance()->addMeshRenderData(asyncParam->modelPath, data);

                    AX_SAFE_DELETE(meshdatas);
                    materialdatas = nullptr;
                    nodeDatas     = nullptr;
                }
            }
            AX_SAFE_DELETE(meshdatas);
            AX_SAFE_DELETE(materialdatas);
            AX_SAFE_DELETE(nodeDatas);

            setModelTexture(asyncParam->modelPath, asyncParam->texPath);
        }
        else
        {
            AXLOG("file load failed: %s\n", asyncParam->modelPath.c_str());
        }
        asyncParam->afterLoadCallback(this, asyncParam->callbackParam);
    }
}

AABB MeshRenderer::getAABBRecursivelyImp(Node* node)
{
    AABB aabb;
    for (auto&& iter : node->getChildren())
    {
        aabb.merge(getAABBRecursivelyImp(iter));
    }

    MeshRenderer* meshRenderer = dynamic_cast<MeshRenderer*>(node);
    if (meshRenderer)
        aabb.merge(meshRenderer->getAABB());

    return aabb;
}

bool MeshRenderer::loadFromCache(std::string_view path)
{
    auto meshdata = MeshRendererCache::getInstance()->getMeshRenderData(path);
    if (meshdata)
    {
        for (auto&& it : meshdata->meshVertexDatas)
        {
            _meshVertexDatas.pushBack(it);
        }
        _skeleton = Skeleton3D::create(meshdata->nodedatas->skeleton);
        AX_SAFE_RETAIN(_skeleton);

        const bool singleMesh = (meshdata->nodedatas->nodes.size() == 1);
        for (const auto& it : meshdata->nodedatas->nodes)
        {
            if (it)
            {
                createNode(it, this, *(meshdata->materialdatas), singleMesh);
            }
        }

        for (const auto& it : meshdata->nodedatas->skeleton)
        {
            if (it)
            {
                createAttachMeshRendererNode(it, *(meshdata->materialdatas));
            }
        }

        for (ssize_t i = 0, size = _meshes.size(); i < size; ++i)
        {
            // cloning is needed in order to have one state per mesh
            auto glstate = meshdata->programStates.at(i);
            _meshes.at(i)->setProgramState(glstate->clone());
        }
        return true;
    }

    return false;
}

bool MeshRenderer::loadFromFile(std::string_view path,
                                NodeDatas* nodedatas,
                                MeshDatas* meshdatas,
                                MaterialDatas* materialdatas)
{
    std::string fullPath = FileUtils::getInstance()->fullPathForFilename(path);

    std::string ext = FileUtils::getInstance()->getFileExtension(path);
    if (ext == ".obj")
    {
        return Bundle3D::loadObj(*meshdatas, *materialdatas, *nodedatas, fullPath);
    }
    else if (ext == ".c3b" || ext == ".c3t")
    {
        // load from .c3b or .c3t
        auto bundle = Bundle3D::createBundle();
        if (!bundle->load(fullPath))
        {
            Bundle3D::destroyBundle(bundle);
            return false;
        }

        auto ret =
            bundle->loadMeshDatas(*meshdatas) && bundle->loadMaterials(*materialdatas) && bundle->loadNodes(*nodedatas);
        Bundle3D::destroyBundle(bundle);

        return ret;
    }
    return false;
}

MeshRenderer::MeshRenderer()
    : _skeleton(nullptr)
    , _blend(BlendFunc::ALPHA_NON_PREMULTIPLIED)
    , _lightMask(-1)
    , _aabbDirty(true)
    , _shaderUsingLight(false)
    , _forceDepthWrite(false)
    , _wireframe(false)
    , _usingAutogeneratedGLProgram(true)
    , _transparentMaterialHint(false)
    , _meshTextureHint(0)
{}

MeshRenderer::~MeshRenderer()
{
    _meshes.clear();
    _meshVertexDatas.clear();
    AX_SAFE_RELEASE_NULL(_skeleton);
    removeAllAttachNode();
}

bool MeshRenderer::init()
{
    if (Node::init())
    {
        return true;
    }
    return false;
}

bool MeshRenderer::initWithFile(std::string_view path)
{
    _aabbDirty = true;
    _meshes.clear();
    _meshVertexDatas.clear();
    AX_SAFE_RELEASE_NULL(_skeleton);
    removeAllAttachNode();

    if (loadFromCache(path))
        return true;

    MeshDatas* meshdatas         = new MeshDatas();
    MaterialDatas* materialdatas = new MaterialDatas();
    NodeDatas* nodeDatas         = new NodeDatas();
    if (loadFromFile(path, nodeDatas, meshdatas, materialdatas))
    {
        if (initFrom(*nodeDatas, *meshdatas, *materialdatas))
        {
            // add to cache
            auto data             = new MeshRendererCache::MeshRenderData();
            data->materialdatas   = materialdatas;
            data->nodedatas       = nodeDatas;
            data->meshVertexDatas = _meshVertexDatas;
            for (const auto mesh : _meshes)
            {
                data->programStates.pushBack(mesh->getProgramState());
            }

            MeshRendererCache::getInstance()->addMeshRenderData(path, data);
            AX_SAFE_DELETE(meshdatas);
            _contentSize = getBoundingBox().size;
            return true;
        }
    }
    AX_SAFE_DELETE(meshdatas);
    AX_SAFE_DELETE(materialdatas);
    AX_SAFE_DELETE(nodeDatas);

    return false;
}

bool MeshRenderer::initFrom(const NodeDatas& nodeDatas, const MeshDatas& meshdatas, const MaterialDatas& materialdatas)
{
    for (const auto& it : meshdatas.meshDatas)
    {
        if (it)
        {
            //            Mesh* mesh = Mesh::create(*it);
            //            _meshes.pushBack(mesh);
            auto meshvertex = MeshVertexData::create(*it);
            _meshVertexDatas.pushBack(meshvertex);
        }
    }
    _skeleton = Skeleton3D::create(nodeDatas.skeleton);
    AX_SAFE_RETAIN(_skeleton);

    auto size = nodeDatas.nodes.size();
    for (const auto& it : nodeDatas.nodes)
    {
        if (it)
        {
            createNode(it, this, materialdatas, size == 1);
        }
    }
    for (const auto& it : nodeDatas.skeleton)
    {
        if (it)
        {
            createAttachMeshRendererNode(it, materialdatas);
        }
    }
    genMaterial();

    return true;
}

MeshRenderer* MeshRenderer::createMeshRendererNode(NodeData* nodedata,
                                                   ModelData* modeldata,
                                                   const MaterialDatas& materialdatas)
{
    auto meshRenderer = new MeshRenderer();

    meshRenderer->setName(nodedata->id);
    auto mesh = Mesh::create(nodedata->id, getMeshIndexData(modeldata->subMeshId));

    if (_skeleton && !modeldata->bones.empty())
    {
        auto skin = MeshSkin::create(_skeleton, modeldata->bones, modeldata->invBindPose);
        mesh->setSkin(skin);
    }

    if (modeldata->materialId.empty() && !materialdatas.materials.empty())
    {
        const NTextureData* textureData = materialdatas.materials[0].getTextureData(NTextureData::Usage::Diffuse);
        setMeshTexture(mesh, textureData->filename);
    }
    else
    {
        const NMaterialData* materialData = materialdatas.getMaterialData(modeldata->materialId);
        if (materialData)
        {
            const NTextureData* textureData = materialData->getTextureData(NTextureData::Usage::Diffuse);
            if (textureData)
            {
                setMeshTexture(mesh, textureData->filename);
                auto tex = mesh->getTexture();
                if (tex)
                {
                    Texture2D::TexParams texParams;
                    texParams.minFilter    = backend::SamplerFilter::LINEAR;
                    texParams.magFilter    = backend::SamplerFilter::LINEAR;
                    texParams.sAddressMode = textureData->wrapS;
                    texParams.tAddressMode = textureData->wrapT;
                    tex->setTexParameters(texParams);
                    _transparentMaterialHint =
                        materialData->getTextureData(NTextureData::Usage::Transparency) != nullptr;
                }
            }
            textureData = materialData->getTextureData(NTextureData::Usage::Normal);
            if (textureData)
            {
                auto tex = setMeshTexture(mesh, textureData->filename, NTextureData::Usage::Normal);
                if (tex)
                {
                    Texture2D::TexParams texParams;
                    texParams.minFilter    = backend::SamplerFilter::LINEAR;
                    texParams.magFilter    = backend::SamplerFilter::LINEAR;
                    texParams.sAddressMode = textureData->wrapS;
                    texParams.tAddressMode = textureData->wrapT;
                    tex->setTexParameters(texParams);
                }
            }
        }
    }

    // set locale transform
    Vec3 pos;
    Quaternion qua;
    Vec3 scale;
    nodedata->transform.decompose(&scale, &qua, &pos);
    meshRenderer->setPosition3D(pos);
    meshRenderer->setRotationQuat(qua);
    meshRenderer->setScaleX(scale.x);
    meshRenderer->setScaleY(scale.y);
    meshRenderer->setScaleZ(scale.z);

    meshRenderer->addMesh(mesh);
    meshRenderer->autorelease();
    meshRenderer->genMaterial();

    return meshRenderer;
}
void MeshRenderer::createAttachMeshRendererNode(NodeData* nodedata, const MaterialDatas& materialdatas)
{
    for (const auto& it : nodedata->modelNodeDatas)
    {
        if (it && getAttachNode(nodedata->id))
        {
            auto mesh = createMeshRendererNode(nodedata, it, materialdatas);
            if (mesh)
            {
                getAttachNode(nodedata->id)->addChild(mesh);
            }
        }
    }
    for (const auto& it : nodedata->children)
    {
        createAttachMeshRendererNode(it, materialdatas);
    }
}

void MeshRenderer::setMaterial(Material* material)
{
    setMaterial(material, -1);
}

void MeshRenderer::setMaterial(Material* material, int meshIndex)
{
    AXASSERT(material, "Invalid Material");
    AXASSERT(meshIndex == -1 || (meshIndex >= 0 && meshIndex < _meshes.size()), "Invalid meshIndex.");

    if (meshIndex == -1)
    {
        for (ssize_t i = 0, size = _meshes.size(); i < size; ++i)
        {
            _meshes.at(i)->setMaterial(i == 0 ? material : material->clone());
        }
    }
    else
    {
        auto mesh = _meshes.at(meshIndex);
        mesh->setMaterial(material);
    }

    _usingAutogeneratedGLProgram = false;
}

Material* MeshRenderer::getMaterial(int meshIndex) const
{
    AXASSERT(meshIndex >= 0 && meshIndex < _meshes.size(), "Invalid meshIndex.");
    return _meshes.at(meshIndex)->getMaterial();
}

void MeshRenderer::genMaterial(bool useLight)
{
    _shaderUsingLight = useLight;

    std::unordered_map<const MeshVertexData*, MeshMaterial*> materials;
    for (auto&& meshVertexData : _meshVertexDatas)
    {
        auto material = getMeshRendererMaterialForAttribs(meshVertexData, useLight);
        AXASSERT(material, "material should cannot be null.");
        materials[meshVertexData] = material;
    }

    for (auto&& mesh : _meshes)
    {
        auto material = materials[mesh->getMeshIndexData()->getMeshVertexData()];
        material->setTransparent(_transparentMaterialHint);
        // keep original state block if exist
        auto oldmaterial = mesh->getMaterial();
        if (oldmaterial)
        {
            material->setStateBlock(oldmaterial->getStateBlock());
            material->setTransparent(oldmaterial->isTransparent());
        }

        if (material->getReferenceCount() == 1)
            mesh->setMaterial(material);
        else
            mesh->setMaterial(material->clone());
    }
}

void MeshRenderer::createNode(NodeData* nodedata, Node* root, const MaterialDatas& materialdatas, bool singleMesh)
{
    Node* node = nullptr;
    for (const auto& it : nodedata->modelNodeDatas)
    {
        if (it)
        {
            if (!it->bones.empty() || singleMesh)
            {
                if (singleMesh && root != nullptr)
                    root->setName(nodedata->id);
                auto mesh = Mesh::create(nodedata->id, getMeshIndexData(it->subMeshId));
                if (mesh)
                {
                    _meshes.pushBack(mesh);
                    if (_skeleton && it->bones.size())
                    {
                        auto skin = MeshSkin::create(_skeleton, it->bones, it->invBindPose);
                        mesh->setSkin(skin);
                    }
                    mesh->_visibleChanged = std::bind(&MeshRenderer::onAABBDirty, this);

                    if (it->materialId == "" && materialdatas.materials.size())
                    {
                        const NTextureData* textureData =
                            materialdatas.materials[0].getTextureData(NTextureData::Usage::Diffuse);
                        setMeshTexture(mesh, textureData->filename);
                    }
                    else
                    {
                        const NMaterialData* materialData = materialdatas.getMaterialData(it->materialId);
                        if (materialData)
                        {
                            const NTextureData* textureData =
                                materialData->getTextureData(NTextureData::Usage::Diffuse);
                            if (textureData)
                            {
                                setMeshTexture(mesh, textureData->filename);
                                auto tex = mesh->getTexture();
                                if (tex)
                                {
                                    Texture2D::TexParams texParams;
                                    texParams.minFilter    = backend::SamplerFilter::LINEAR;
                                    texParams.magFilter    = backend::SamplerFilter::LINEAR;
                                    texParams.sAddressMode = textureData->wrapS;
                                    texParams.tAddressMode = textureData->wrapT;
                                    tex->setTexParameters(texParams);
                                    _transparentMaterialHint =
                                        materialData->getTextureData(NTextureData::Usage::Transparency) != nullptr;
                                }
                            }
                            textureData = materialData->getTextureData(NTextureData::Usage::Normal);
                            if (textureData)
                            {
                                auto tex = setMeshTexture(mesh, textureData->filename, NTextureData::Usage::Normal);
                                if (tex)
                                {
                                    Texture2D::TexParams texParams;
                                    texParams.minFilter    = backend::SamplerFilter::LINEAR;
                                    texParams.magFilter    = backend::SamplerFilter::LINEAR;
                                    texParams.sAddressMode = textureData->wrapS;
                                    texParams.tAddressMode = textureData->wrapT;
                                    tex->setTexParameters(texParams);
                                }
                            }
                        }
                    }

                    Vec3 pos;
                    Quaternion qua;
                    Vec3 scale;
                    nodedata->transform.decompose(&scale, &qua, &pos);
                    setPosition3D(pos);
                    setRotationQuat(qua);
                    setScaleX(scale.x);
                    setScaleY(scale.y);
                    setScaleZ(scale.z);

                    node = this;
                }
            }
            else
            {
                auto mesh = createMeshRendererNode(nodedata, it, materialdatas);
                if (mesh)
                {
                    if (root)
                    {
                        root->addChild(mesh);
                    }
                }
                node = mesh;
            }
        }
    }
    if (nodedata->modelNodeDatas.size() == 0)
    {
        node = Node::create();
        if (node)
        {
            node->setName(nodedata->id);

            // set locale transform
            Vec3 pos;
            Quaternion qua;
            Vec3 scale;
            nodedata->transform.decompose(&scale, &qua, &pos);
            node->setPosition3D(pos);
            node->setRotationQuat(qua);
            node->setScaleX(scale.x);
            node->setScaleY(scale.y);
            node->setScaleZ(scale.z);

            if (root)
            {
                root->addChild(node);
            }
        }
    }

    auto size = nodedata->children.size();
    for (const auto& it : nodedata->children)
    {
        createNode(it, node, materialdatas, size == 1);
    }
}

MeshIndexData* MeshRenderer::getMeshIndexData(std::string_view indexId) const
{
    for (auto&& it : _meshVertexDatas)
    {
        auto index = it->getMeshIndexDataById(indexId);
        if (index)
            return index;
    }
    return nullptr;
}

void MeshRenderer::addMesh(Mesh* mesh)
{
    auto meshVertex = mesh->getMeshIndexData()->_vertexData;
    _meshVertexDatas.pushBack(meshVertex);
    _meshes.pushBack(mesh);
}

Texture2D* MeshRenderer::setMeshTexture(Mesh* mesh, std::string_view texPath, NTextureData::Usage usage)
{
    auto tex = _director->getTextureCache()->addImage(texPath);
    mesh->setTexture(texPath, usage);
    if (tex)
        ++_meshTextureHint;
    return tex;
}

void MeshRenderer::setModelTexture(std::string_view modelPath, std::string_view texturePath)
{
    if (!texturePath.empty())
        setTexture(texturePath);
    else if (!_meshTextureHint)
    {
        auto pos = modelPath.find_last_of('.');
        if (pos != std::string_view::npos)
        {
            std::string modelTexPath{modelPath};
            modelTexPath.resize(pos);
            modelTexPath.append(".png"sv);
            setTexture(modelTexPath);
        }
    }
}

void MeshRenderer::enableInstancing(MeshMaterial::InstanceMaterialType instanceMat, int count)
{
    switch (instanceMat)
    {
    case MeshMaterial::InstanceMaterialType::UNLIT_INSTANCE:
    {
        auto mat = MeshMaterial::createBuiltInMaterial(MeshMaterial::MaterialType::UNLIT_INSTANCE, false);
        enableInstancing(mat, count);
    }
    }
}

void MeshRenderer::enableInstancing(MeshMaterial* instanceMat, int count)
{
    for (auto&& mesh : _meshes)
    {
        mesh->enableInstancing(true, MAX(1, count));
        mesh->setMaterial(instanceMat);
    }
}

void MeshRenderer::disableInstancing()
{
    for (auto&& mesh : _meshes)
        mesh->enableInstancing(false, 0);
}

void MeshRenderer::setDynamicInstancing(bool dynamic)
{
    for (auto&& mesh : _meshes)
        mesh->setDynamicInstancing(dynamic);
}

void MeshRenderer::addInstanceChild(Node* child, bool active)
{
    for (auto&& mesh : _meshes)
    {
        mesh->addInstanceChild(child);
        if (active)
            addChild(child);
        else
            child->setParent(this);
    }
}

void MeshRenderer::shrinkToFitInstances()
{
    for (auto&& mesh : _meshes)
        mesh->shrinkToFitInstances();
}

void MeshRenderer::rebuildInstances()
{
    for (auto&& mesh : _meshes)
        mesh->rebuildInstances();
}

void MeshRenderer::setTexture(std::string_view texFile)
{
    auto tex = _director->getTextureCache()->addImage(texFile);
    setTexture(tex);
}

void MeshRenderer::setTexture(Texture2D* texture)
{
    for (auto&& mesh : _meshes)
    {
        mesh->setTexture(texture);
    }
}
AttachNode* MeshRenderer::getAttachNode(std::string_view boneName)
{
    auto it = _attachments.find(boneName);
    if (it != _attachments.end())
        return it->second;

    if (_skeleton)
    {
        auto bone = _skeleton->getBoneByName(boneName);
        if (bone)
        {
            auto attachNode = AttachNode::create(bone);
            addChild(attachNode);
            _attachments.emplace(boneName, attachNode);  // _attachments[boneName] = attachNode;
            return attachNode;
        }
    }

    return nullptr;
}

void MeshRenderer::removeAttachNode(std::string_view boneName)
{
    auto it = _attachments.find(boneName);
    if (it != _attachments.end())
    {
        removeChild(it->second);
        _attachments.erase(it);
    }
}

void MeshRenderer::removeAllAttachNode()
{
    for (auto&& it : _attachments)
    {
        removeChild(it.second);
    }
    _attachments.clear();
}

void MeshRenderer::visit(ax::Renderer* renderer, const ax::Mat4& parentTransform, uint32_t parentFlags)
{
    // quick return if not visible. children won't be drawn.
    if (!_visible)
    {
        return;
    }

    uint32_t flags = processParentFlags(parentTransform, parentFlags);
    flags |= FLAGS_RENDER_AS_3D;

    //
    _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

    bool visibleByCamera = isVisitableByVisitingCamera();

    int i = 0;

    if (!_children.empty())
    {
        sortAllChildren();
        // draw children zOrder < 0
        for (auto size = _children.size(); i < size; i++)
        {
            auto node = _children.at(i);

            if (node && node->getLocalZOrder() < 0)
                node->visit(renderer, _modelViewTransform, flags);
            else
                break;
        }
        // self draw
        if (visibleByCamera)
            this->draw(renderer, _modelViewTransform, flags);

        for (auto it = _children.cbegin() + i, itCend = _children.cend(); it != itCend; ++it)
            (*it)->visit(renderer, _modelViewTransform, flags);
    }
    else if (visibleByCamera)
    {
        this->draw(renderer, _modelViewTransform, flags);
    }

    _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void MeshRenderer::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
#if AX_USE_CULLING
    // TODO new-renderer: interface isVisibleInFrustum removal
    //  camera clipping
//    if(_children.empty() && Camera::getVisitingCamera() &&
//    !Camera::getVisitingCamera()->isVisibleInFrustum(&getAABB()))
//        return;
#endif

    if (_skeleton)
        _skeleton->updateBoneMatrix();

    Color4F color(getDisplayedColor());
    color.a = getDisplayedOpacity() / 255.0f;

    // check light and determine the shader used
    const auto& scene = Director::getInstance()->getRunningScene();

    // Don't override GLProgramState if using manually set Material
    if (_usingAutogeneratedGLProgram && scene)
    {
        const auto lights = scene->getLights();
        bool usingLight   = false;
        for (const auto light : lights)
        {
            usingLight = light->isEnabled() && ((static_cast<unsigned int>(light->getLightFlag()) & _lightMask) > 0);
            if (usingLight)
                break;
        }
        if (usingLight != _shaderUsingLight)
        {
            genMaterial(usingLight);
        }
    }

    for (auto&& mesh : _meshes)
    {
        mesh->draw(renderer, _globalZOrder, transform, flags, _lightMask, Vec4(color.r, color.g, color.b, color.a),
                   _forceDepthWrite, _wireframe);
    }
}

bool MeshRenderer::setProgramState(backend::ProgramState* programState, bool ownPS /* = false*/)
{
    if (Node::setProgramState(programState, ownPS))
    {
        for (auto&& state : _meshes)
        {
            state->setProgramState(programState);
        }
        return true;
    }
    return false;
}

void MeshRenderer::setBlendFunc(const BlendFunc& blendFunc)
{
    if (_blend.src != blendFunc.src || _blend.dst != blendFunc.dst)
    {
        _blend = blendFunc;
        for (auto&& mesh : _meshes)
        {
            mesh->setBlendFunc(blendFunc);
        }
    }
}

const BlendFunc& MeshRenderer::getBlendFunc() const
{
    return _blend;
}

AABB MeshRenderer::getAABBRecursively()
{
    return getAABBRecursivelyImp(this);
}

const AABB& MeshRenderer::getAABB() const
{
    Mat4 nodeToWorldTransform(getNodeToWorldTransform());

    // If nodeToWorldTransform matrix isn't changed, we don't need to transform aabb.
    if (memcmp(_nodeToWorldTransform.m, nodeToWorldTransform.m, sizeof(Mat4)) == 0 && !_aabbDirty)
    {
        return _aabb;
    }
    else
    {
        _aabb.reset();
        if (_meshes.size())
        {
            Mat4 transform(nodeToWorldTransform);
            for (const auto& it : _meshes)
            {
                if (it->isVisible())
                    _aabb.merge(it->getAABB());
            }

            _aabb.transform(transform);
            _nodeToWorldTransform = nodeToWorldTransform;
            _aabbDirty            = false;
        }
    }

    return _aabb;
}

Action* MeshRenderer::runAction(Action* action)
{
    setForceDepthWrite(true);
    return Node::runAction(action);
}

Rect MeshRenderer::getBoundingBox() const
{
    AABB aabb = getAABB();
    Rect ret(aabb._min.x, aabb._min.y, (aabb._max.x - aabb._min.x), (aabb._max.y - aabb._min.y));
    return ret;
}

void MeshRenderer::setCullFace(CullFaceSide side)
{
    for (auto&& it : _meshes)
    {
        it->getMaterial()->getStateBlock().setCullFaceSide(side);
    }
}

void MeshRenderer::setCullFaceEnabled(bool enable)
{
    for (auto&& it : _meshes)
    {
        it->getMaterial()->getStateBlock().setCullFace(enable);
    }
}

Mesh* MeshRenderer::getMeshByIndex(int index) const
{
    AXASSERT(index < _meshes.size(), "Invalid index.");
    return _meshes.at(index);
}

/**get Mesh by Name */
Mesh* MeshRenderer::getMeshByName(std::string_view name) const
{
    for (const auto& it : _meshes)
    {
        if (it->getName() == name)
            return it;
    }
    return nullptr;
}

std::vector<Mesh*> MeshRenderer::getMeshArrayByName(std::string_view name) const
{
    std::vector<Mesh*> meshes;
    for (const auto& it : _meshes)
    {
        if (it->getName() == name)
            meshes.emplace_back(it);
    }
    return meshes;
}

Mesh* MeshRenderer::getMesh() const
{
    if (_meshes.empty())
    {
        return nullptr;
    }
    return _meshes.at(0);
}

///////////////////////////////////////////////////////////////////////////////////
MeshRendererCache* MeshRendererCache::_cacheInstance = nullptr;
MeshRendererCache* MeshRendererCache::getInstance()
{
    if (_cacheInstance == nullptr)
        _cacheInstance = new MeshRendererCache();
    return _cacheInstance;
}
void MeshRendererCache::destroyInstance()
{
    if (_cacheInstance)
    {
        delete _cacheInstance;
        _cacheInstance = nullptr;
    }
}

MeshRendererCache::MeshRenderData* MeshRendererCache::getMeshRenderData(std::string_view key) const
{
    auto it = _meshDatas.find(key);
    if (it != _meshDatas.end())
        return it->second;
    return nullptr;
}

bool MeshRendererCache::addMeshRenderData(std::string_view key, MeshRendererCache::MeshRenderData* meshdata)
{
    auto it = _meshDatas.find(key);
    if (it == _meshDatas.end())
    {
        _meshDatas.emplace(key, meshdata);
        return true;
    }
    return false;
}

void MeshRendererCache::removeMeshRenderData(std::string_view key)
{
    auto it = _meshDatas.find(key);
    if (it != _meshDatas.end())
    {
        delete it->second;
        _meshDatas.erase(it);
    }
}

void MeshRendererCache::removeAllMeshRenderData()
{
    for (auto&& it : _meshDatas)
    {
        delete it.second;
    }
    _meshDatas.clear();
}

MeshRendererCache::MeshRendererCache() {}
MeshRendererCache::~MeshRendererCache()
{
    removeAllMeshRenderData();
}

static MeshMaterial* getMeshRendererMaterialForAttribs(MeshVertexData* meshVertexData, bool usesLight)
{
    bool textured = meshVertexData->hasVertexAttrib(shaderinfos::VertexKey::VERTEX_ATTRIB_TEX_COORD);
    bool hasSkin  = meshVertexData->hasVertexAttrib(shaderinfos::VertexKey::VERTEX_ATTRIB_BLEND_INDEX) &&
                   meshVertexData->hasVertexAttrib(shaderinfos::VertexKey::VERTEX_ATTRIB_BLEND_WEIGHT);
    bool hasNormal       = meshVertexData->hasVertexAttrib(shaderinfos::VertexKey::VERTEX_ATTRIB_NORMAL);
    bool hasTangentSpace = meshVertexData->hasVertexAttrib(shaderinfos::VertexKey::VERTEX_ATTRIB_TANGENT) &&
                           meshVertexData->hasVertexAttrib(shaderinfos::VertexKey::VERTEX_ATTRIB_BINORMAL);
    MeshMaterial::MaterialType type;
    if (textured)
    {
        if (hasTangentSpace)
        {
            type =
                hasNormal && usesLight ? MeshMaterial::MaterialType::BUMPED_DIFFUSE : MeshMaterial::MaterialType::UNLIT;
        }
        else
        {
            type = hasNormal && usesLight ? MeshMaterial::MaterialType::DIFFUSE : MeshMaterial::MaterialType::UNLIT;
        }
    }
    else
    {
        type = hasNormal && usesLight ? MeshMaterial::MaterialType::DIFFUSE_NOTEX
                                      : MeshMaterial::MaterialType::UNLIT_NOTEX;
    }

    return MeshMaterial::createBuiltInMaterial(type, hasSkin);
}

NS_AX_END
