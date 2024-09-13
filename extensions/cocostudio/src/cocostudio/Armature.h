/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.

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

#ifndef __CCARMATURE_H__
#define __CCARMATURE_H__

#include "ArmatureDefine.h"
#include "Bone.h"
#include "BatchNode.h"
#include "ArmatureAnimation.h"
#include "SpriteFrameCacheHelper.h"
#include "ArmatureDataManager.h"
#include "CocosStudioExport.h"
#include "math/Math.h"

class b2Body;
struct cpBody;

namespace cocostudio
{
class CCS_DLL Armature : public ax::Node, public ax::BlendProtocol
{

public:
    /**
     * Allocates and initializes an armature.
     * @return An initialized armature which is marked as "autorelease".
     */
    static Armature* create();

    /**
     * Allocates an armature, and use the ArmatureData named name in ArmatureDataManager to initializes the armature.
     *
     * @param  name Armature will use the name to find the ArmatureData to initializes it.
     * @return A initialized armature which is marked as "autorelease".
     */
    static Armature* create(std::string_view name);

    static Armature* create(std::string_view name, Bone* parentBone);

public:
    /**
     *  @js ctor
     */
    Armature();
    /**
     * @js NA
     * @lua NA
     */
    virtual ~Armature();

    /**
     * Init the empty armature
     */
    virtual bool init() override;

    /**
     * Init an armature with specified name
     * @param name Armature name
     */
    virtual bool init(std::string_view name);

    virtual bool init(std::string_view name, Bone* parentBone);
    /**
     * Add a Bone to this Armature,
     *
     * @param bone  The Bone you want to add to Armature
     * @param parentName   The parent Bone's name you want to add to . If it's  nullptr, then set Armature to its parent
     */
    virtual void addBone(Bone* bone, std::string_view parentName);
    /**
     * Get a bone with the specified name
     *
     * @param name The bone's name you want to get
     */
    virtual Bone* getBone(std::string_view name) const;
    /**
     * Change a bone's parent with the specified parent name.
     *
     * @param bone The bone you want to change parent
     * @param parentName The new parent's name.
     */
    virtual void changeBoneParent(Bone* bone, std::string_view parentName);
    /**
     * Remove a bone with the specified name. If recursion it will also remove child Bone recursionly.
     *
     * @param bone The bone you want to remove
     * @param recursion Determine whether remove the bone's child  recursion.
     */
    virtual void removeBone(Bone* bone, bool recursion);

    /**
     * Get Armature's bone dictionary
     * @return Armature's bone dictionary
     */
    const ax::StringMap<Bone*>& getBoneDic() const;

    /**
     * This boundingBox will calculate all bones' boundingBox every time
     */
    virtual ax::Rect getBoundingBox() const override;

    Bone* getBoneAtPoint(float x, float y) const;

    // overrides
    /**
     * @js NA
     * @lua NA
     */
    virtual void visit(ax::Renderer* renderer,
                       const ax::Mat4& parentTransform,
                       uint32_t parentFlags) override;
    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;
    virtual void update(float dt) override;

    virtual void onEnter() override;
    virtual void onExit() override;

    virtual const ax::Mat4& getNodeToParentTransform() const override;
    /**
     *  @js NA
     *  @lua NA
     */
    inline void setBlendFunc(const ax::BlendFunc& blendFunc) override { _blendFunc = blendFunc; }
    /**
     *  @js NA
     *  @lua NA
     */
    inline const ax::BlendFunc& getBlendFunc() const override { return _blendFunc; }

    /**
     * Set contentsize and Calculate anchor point.
     */
    virtual void updateOffsetPoint();
    virtual void setAnchorPoint(const ax::Vec2& point) override;
    virtual const ax::Vec2& getAnchorPointInPoints() const override;
    virtual const ax::Vec2& getOffsetPoints() const;

    virtual void setAnimation(ArmatureAnimation* animation);
    virtual ArmatureAnimation* getAnimation() const;

    virtual bool getArmatureTransformDirty() const;

#if ENABLE_PHYSICS_BOX2D_DETECT || ENABLE_PHYSICS_CHIPMUNK_DETECT
    virtual void setColliderFilter(ColliderFilter* filter);
#elif ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
    AX_DEPRECATED(2.1) virtual void drawContour();
#endif

    virtual void setArmatureData(ArmatureData* armatureData) { _armatureData = armatureData; }
    virtual ArmatureData* getArmatureData() const { return _armatureData; }

    virtual void setParentBone(Bone* parentBone);
    virtual Bone* getParentBone() const;

    virtual void setVersion(float version) { _version = version; }
    virtual float getVersion() const { return _version; }

    virtual void setBatchNode(BatchNode* batchNode) { _batchNode = batchNode; }
    virtual BatchNode* getBatchNode() const { return _batchNode; }

#if ENABLE_PHYSICS_BOX2D_DETECT
    virtual b2Fixture* getShapeList();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void setBody(b2Body* body);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual b2Body* getBody() const;
#elif ENABLE_PHYSICS_CHIPMUNK_DETECT
    /**
     *  @js NA
     *  @lua NA
     */
    virtual cpShape* getShapeList();
    /**
     *  @js NA
     *  @lua NA
     */
    virtual void setBody(cpBody* body);
    /**
     *  @js NA
     *  @lua NA
     */
    virtual cpBody* getBody() const;
#endif

protected:
    /*
     * Used to create Bone internal
     * @js NA
     * @lua NA
     */
    Bone* createBone(std::string_view boneName);

protected:
    ArmatureData* _armatureData;

    BatchNode* _batchNode;

    Bone* _parentBone;
    float _version;

    mutable bool _armatureTransformDirty;

    ax::StringMap<Bone*>
        _boneDic;  //! The dictionary of the bones, include all bones in the armature, no matter it is the direct bone
                   //! or the indirect bone. It is different from m_pChindren.

    ax::Vector<Bone*> _topBoneList;

    ax::BlendFunc _blendFunc;  //! It's required for CCTextureProtocol inheritance

    ax::Vec2 _offsetPoint;
    ax::Vec2 _realAnchorPointInPoints;

    ArmatureAnimation* _animation;

#if ENABLE_PHYSICS_BOX2D_DETECT
    b2Body* _body;
#elif ENABLE_PHYSICS_CHIPMUNK_DETECT
    cpBody* _body;
#endif
};

}  // namespace cocostudio

#endif /*__CCARMATURE_H__*/
