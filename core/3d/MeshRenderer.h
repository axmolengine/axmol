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

#ifndef __AX_MESH_RENDERER_H__
#define __AX_MESH_RENDERER_H__

#include <unordered_map>

#include "base/Vector.h"
#include "base/Types.h"
#include "base/Protocols.h"
#include "2d/Node.h"
#include "renderer/MeshCommand.h"
#include "3d/Skeleton3D.h"  // needs to be included for lua-bindings
#include "3d/AABB.h"
#include "3d/Bundle3DData.h"
#include "3d/MeshVertexIndexData.h"
#include "3d/MeshMaterial.h"

namespace ax
{

/**
 * @addtogroup _3d
 * @{
 */

class Mesh;
class Texture2D;
class MeshSkin;
class AttachNode;
struct NodeData;
/** @brief MeshRenderer: A mesh can be loaded from model files, .obj, .c3t, .c3b
 *and a mesh renderer renders a list of these loaded meshes with specified materials
 */
class AX_DLL MeshRenderer : public Node, public BlendProtocol
{
public:
    /**
     * Creates an empty MeshRenderer without a mesh or a texture.
     * Can be used to create procedural meshes on runtime.
     *
     * @return An autoreleased MeshRenderer object.
     */
    static MeshRenderer* create();

    /** creates a MeshRenderer with a specified path */
    static MeshRenderer* create(std::string_view modelPath);

    /** creates a MeshRenderer. A mesh can only have one texture, the default texture can be overridden with 'texturePath' */
    static MeshRenderer* create(std::string_view modelPath, std::string_view texturePath);

    /** create 3d mesh asynchronously
     * If the 3d model was previously loaded, it will create a new 3d mesh and the callback will be called once.
     * Otherwise it will load the model file in a new thread, and when the 3d mesh is loaded, the callback will be
     * called with the created MeshRenderer and a user-defined parameter. The callback will be called from the main thread,
     * so it is safe to create any object from the callback.
     * @param modelPath model to be loaded
     * @param callback callback when loading is finished
     * @param callbackparam user-defined parameter for the callback
     */
    static void createAsync(std::string_view modelPath,
                            const std::function<void(MeshRenderer*, void*)>& callback,
                            void* callbackparam);

    static void createAsync(std::string_view modelPath,
                            std::string_view texturePath,
                            const std::function<void(MeshRenderer*, void*)>& callback,
                            void* callbackparam);

    /** set diffuse texture, set the first mesh's texture if multiple textures exist */
    void setTexture(std::string_view texFile);
    void setTexture(Texture2D* texture);

    /** get Mesh by index */
    Mesh* getMeshByIndex(int index) const;

    /** get Mesh by Name, returns the first one if there are more than one mesh with the same name */
    Mesh* getMeshByName(std::string_view name) const;

    /**
     * get mesh array by name, returns all meshes with the given name
     *
     * @lua NA
     */
    std::vector<Mesh*> getMeshArrayByName(std::string_view name) const;

    /** get mesh at index 0 which is the default mesh */
    Mesh* getMesh() const;

    /** get mesh count */
    ssize_t getMeshCount() const { return _meshes.size(); }

    Skeleton3D* getSkeleton() const { return _skeleton; }

    /** return an AttachNode by bone name. Otherwise, return nullptr if it doesn't exist */
    AttachNode* getAttachNode(std::string_view boneName);

    /** remove an attached node */
    void removeAttachNode(std::string_view boneName);

    /** remove all attached nodes */
    void removeAllAttachNode();

    // overrides
    virtual void setBlendFunc(const BlendFunc& blendFunc) override;
    virtual const BlendFunc& getBlendFunc() const override;

    // overrides
    /** Sets ProgramState, attributes should be bound by the user */
    bool setProgramState(backend::ProgramState* programState, bool ownPS = false) override;

    /*
     * Get AABB
     * If the mesh has animations, it can't be calculated accurately,
     * because a bone can transform the vertices, and the untransformed vertices are used
     * to calculate the AABB.
     */
    const AABB& getAABB() const;

    /*
     * Get AABB Recursively
     * Because sometimes we may have an empty MeshRenderer Node as parent, If
     * the MeshRenderer doesn't contain any meshes, then we use getAABB()
     */
    AABB getAABBRecursively();

    /**
     * Executes an action, and returns the action that is executed. For the MeshRenderer special logic is needed to take
     * care of Fading.
     *
     * This node becomes the action's target. Refer to Action::getTarget()
     * @warning Actions don't retain their target.
     *
     * @return a pointer to Action
     */
    virtual Action* runAction(Action* action) override;

    /**
     * Force depth buffer writing, this is useful if you want to achieve effects like fading.
     */
    void setForceDepthWrite(bool value) { _forceDepthWrite = value; }
    bool isForceDepthWrite() const { return _forceDepthWrite; };

    /**
     * Returns a 2d bounding-box
     * Note: the bounding-box is taken from the mesh's AABB with Z-axis ignored.
     */
    virtual Rect getBoundingBox() const override;

    // set face culling side, CullFaceSide::BACK, CullFaceSide::FRONT and CullFaceSide::NONE.
    virtual void setCullFace(CullFaceSide side);
    // set face culling enabled.
    void setCullFaceEnabled(bool enable);

    /** light mask getter & setter, lighting only works when _lightmask & light's flag are set to true, the default
     value of _lightmask is 0xffff */
    void setLightMask(unsigned int mask) { _lightMask = mask; }
    unsigned int getLightMask() const { return _lightMask; }

    /** enables wireframe rendering mode for this mesh renderer only, this can be very useful for debugging and
     understanding generated meshes. */
    void setWireframe(bool value) { _wireframe = value; }
    bool isWireframe() const { return _wireframe; }

    /** render all meshes within this mesh renderer */
    virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;

    /** Adds a new material to this mesh renderer.
     The Material will be applied to all the meshes that belong to the mesh renderer.
     It will internally call `setMaterial(material,-1)`
     */
    void setMaterial(Material* material);

    /** Adds a new material to a particular mesh in this mesh renderer.
     * if meshIndex == -1, then it will be applied to all the meshes that belong to this mesh renderer.
     *
     * @param meshIndex Index of the mesh to apply the material to.
     */
    void setMaterial(Material* material, int meshIndex);

    /** Gets the material of a specific mesh in this mesh renderer.
     *
     * @param meshIndex Index of the mesh to get the material from. 0 is the default index.
     */
    Material* getMaterial(int meshIndex = 0) const;

    /** Get list of meshes used in this mesh renderer. */
    const Vector<Mesh*>& getMeshes() const { return _meshes; }

    MeshRenderer();
    virtual ~MeshRenderer();

    virtual bool init() override;

    bool initWithFile(std::string_view path);

    bool initFrom(const NodeDatas& nodedatas, const MeshDatas& meshdatas, const MaterialDatas& materialdatas);

    /** load a mesh renderer from cache, returns true if succeeded, false otherwise. */
    bool loadFromCache(std::string_view path);

    /** load a file and feed it's content into meshedatas, nodedatas and materialdatas, obj file and .mtl file
     should be in the same directory. */
    bool loadFromFile(std::string_view path, NodeDatas* nodedatas, MeshDatas* meshdatas, MaterialDatas* materialdatas);

    /**
     * Visits this MeshRenderer's children and draws them recursively.
     * Note: all children will be rendered in 3D space with depth, this behaviour can be changed using
     * setForce2DQueue()
     */
    virtual void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;

    /** generate default material. */
    void genMaterial(bool useLight = false);

    void createNode(NodeData* nodedata, Node* root, const MaterialDatas& materialdatas, bool singleMesh);
    void createAttachMeshRendererNode(NodeData* nodedata, const MaterialDatas& materialdatas);
    MeshRenderer* createMeshRendererNode(NodeData* nodedata, ModelData* modeldata, const MaterialDatas& materialdatas);

    /** get MeshIndexData by Id */
    MeshIndexData* getMeshIndexData(std::string_view indexId) const;

    void addMesh(Mesh* mesh);

    void onAABBDirty() { _aabbDirty = true; }

    void afterAsyncLoad(void* param);

    static AABB getAABBRecursivelyImp(Node* node);

    /** Enables instancing for this Mesh Renderer, keep in mind that
     a special vertex shader has to be used, make sure that your shader
     has a mat4 attribute set on the location of total vertex attributes +1

    @param instanceMat Prebuilt material made to work with instancing.
    @param count Count of Instances, 0 to enable auto expanstion, but you
    should call `shrinkToFitInstances` after adding children to maximize performance.*/
    void enableInstancing(MeshMaterial::InstanceMaterialType instanceMat, int count = 0);

    /* Enables instancing for this Mesh Renderer, keep in mind that
    / a special vertex shader has to be used, make sure that your shader
    / has a mat4 attribute set on the location of total vertex attributes +1

    @param instanceMat Custom material that supports instancing.
    @param count Count of Instances, 0 to enable auto expanstion, but you
    should call `shrinkToFitInstances` after adding children to maximize performance.*/
    void enableInstancing(MeshMaterial* instanceMat, int count = 0);

    // Disables instancing for this Mesh Renderer.
    void disableInstancing();

    /** Set this to true and instancing objects within this mesh renderer
    will be recalculated each frame, use it when you plan to move objects,
    Otherwise, transforms will be built once for better performance.
    * to update transforms on demand use `rebuildInstances()` */
    void setDynamicInstancing(bool dynamic);

    /** Adds a child to use it's transformations for instancing.
    * The child is in the space of this Node, keep in mind that
    the node isn't added to the scene graph, it is instead retained
    and it's parent is set to this node, updates and actions will not run.
    * the reason for this is performance.
    *
    * @param child, The child to use for instancing.
    * @param active, Child will run updates & actions but performance issues may arise.
    */
    void addInstanceChild(Node* child, bool active = false);

    /** shrinks the instance transform buffer after many steps of expansion to increase performance. */
    void shrinkToFitInstances();

    /** rebuilds the instance transform buffer next frame. */
    void rebuildInstances();

protected:
    /** set specific mesh texture, for private use (create mesh stage) only */
    Texture2D* setMeshTexture(Mesh* mesh,
                              std::string_view texPath,
                              NTextureData::Usage usage = NTextureData::Usage::Diffuse);

    /** set model texture from model path when model file not contains texture and texPath is empty
    * only for create mesh renderer
    */
    void setModelTexture(std::string_view modelPath, std::string_view texPath);

    Skeleton3D* _skeleton;

    Vector<MeshVertexData*> _meshVertexDatas;

    hlookup::string_map<AttachNode*> _attachments;

    BlendFunc _blend;

    Vector<Mesh*> _meshes;

    mutable AABB _aabb;                  // cache current aabb
    mutable Mat4 _nodeToWorldTransform;  // cache current matrix
    unsigned int _lightMask;
    mutable bool _aabbDirty;
    bool _shaderUsingLight;  // Is the current shader using lighting?
    bool _forceDepthWrite;   // Always write to depth buffer
    bool _wireframe;         // render in wireframe mode
    bool _usingAutogeneratedGLProgram;
    bool _transparentMaterialHint; // Generate transparent materials when building from files
    unsigned short _meshTextureHint; // Whether model file has texture config

    struct AsyncLoadParam
    {
        std::function<void(MeshRenderer*, void*)> afterLoadCallback;  // callback after loading is finished
        void* callbackParam;
        bool result;  // mesh renderer loading result
        std::string modelPath;
        std::string modelFullPath;
        std::string texPath;
        MeshDatas* meshdatas;
        MaterialDatas* materialdatas;
        NodeDatas* nodeDatas;
    };
    AsyncLoadParam _asyncLoadParam;
};

///////////////////////////////////////////////////////
/**
 * @brief MeshRendererCache: the cache data of MeshRenderer, used to speed up the creation process of MeshRenderer
 */
class AX_DLL MeshRendererCache
{
public:
    struct MeshRenderData
    {
        Vector<MeshVertexData*> meshVertexDatas;
        Vector<backend::ProgramState*> programStates;
        NodeDatas* nodedatas;
        MaterialDatas* materialdatas;
        ~MeshRenderData()
        {
            if (nodedatas)
                delete nodedatas;
            if (materialdatas)
                delete materialdatas;
            meshVertexDatas.clear();
            programStates.clear();
        }
    };

    static MeshRendererCache* getInstance();
    static void destroyInstance();

    /**
     * get a MeshData object by key
     *
     * @lua NA
     */
    MeshRenderData* getMeshRenderData(std::string_view key) const;

    /**
     * add a MeshData object into the MeshRenderer with a specified key
     *
     * @lua NA
     */
    bool addMeshRenderData(std::string_view key, MeshRenderData* meshdata);

    /** remove a MeshData from the MeshRenderer with a specified key */
    void removeMeshRenderData(std::string_view key);

    /** remove all the MeshData objects from the MeshRenderer */
    void removeAllMeshRenderData();

    MeshRendererCache();
    ~MeshRendererCache();

protected:
    static MeshRendererCache* _cacheInstance;
    hlookup::string_map<MeshRenderData*> _meshDatas;  // cached mesh data
};

// end of 3d group
/// @}

}
#endif  // __AX_MESH_RENDERER_H__
