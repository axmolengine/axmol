/****************************************************************************
Copyright (c) 2013-2017 Chukong Technologies Inc.
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

#include "sceneext/ArmatureDefine.h"
#include "sceneext/AnimationData.h"
#include "sceneext/SceneExtMacros.h"

#ifndef PT_RATIO
#    define PT_RATIO 32
#endif

#if ENABLE_PHYSICS_BOX2D_DETECT
#    include "box2d/box2d.h"
#endif

namespace ax::ext
{

class Bone;

/**
 *  @lua NA
 */
class SCNEXT_API ColliderFilter
{
public:
    virtual ~ColliderFilter() {}
#if ENABLE_PHYSICS_BOX2D_DETECT
public:
    ColliderFilter(uint64_t categoryBits = 0x0001, uint64_t maskBits = 0xFFFF, int groupIndex = 0);
    void updateShape(b2ShapeId shape);

    virtual void setCategoryBits(uint64_t categoryBits) { _categoryBits = categoryBits; }
    virtual uint64_t getCategoryBits() const { return _categoryBits; }

    virtual void setMaskBits(uint64_t maskBits) { _maskBits = maskBits; }
    virtual uint64_t getMaskBits() const { return _maskBits; }

    virtual void setGroupIndex(int groupIndex) { _groupIndex = groupIndex; }
    virtual int getGroupIndex() const { return _groupIndex; }

protected:
    uint64_t _categoryBits;
    uint64_t _maskBits;
    int _groupIndex;
#endif
};

class SCNEXT_API ColliderBody : public ax::Object
{
public:
    ColliderBody(ContourData* contourData);
    ~ColliderBody();

    inline ContourData* getContourData() { return _contourData; }

#if ENABLE_PHYSICS_BOX2D_DETECT
    void setColliderFilter(ColliderFilter* filter);
    ColliderFilter* getColliderFilter();
    virtual void setShape(b2ShapeId shape) { _shape = shape; }
    virtual b2ShapeId getShape() const { return _shape; }
#elif ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
    virtual const std::vector<ax::Vec2>& getCalculatedVertexList() const { return _calculatedVertexList; }
#endif

private:
#if ENABLE_PHYSICS_BOX2D_DETECT
    b2ShapeId _shape{b2_nullShapeId};
    ColliderFilter* _filter;
#elif ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
    std::vector<ax::Vec2> _calculatedVertexList;
#endif

    ContourData* _contourData;

    friend class ColliderDetector;
};

/*
 *  @brief  ContourSprite used to draw the contour of the display
 *  @lua NA
 */
class SCNEXT_API ColliderDetector : public ax::Object
{
public:
    static ColliderDetector* create();
    static ColliderDetector* create(Bone* bone);

public:
    ColliderDetector();
    /**
     * @lua NA
     */
    ~ColliderDetector(void);

    virtual bool init();
    virtual bool init(Bone* bone);

    void addContourData(ContourData* contourData);
    void addContourDataList(ax::Vector<ContourData*>& contourDataList);

    void removeContourData(ContourData* contourData);
    void removeAll();

    void updateTransform(ax::Mat4& t);

    void setActive(bool active);
    bool getActive();

    const ax::Vector<ColliderBody*>& getColliderBodyList();

#if ENABLE_PHYSICS_BOX2D_DETECT
    virtual void setColliderFilter(ColliderFilter* filter);
    virtual ColliderFilter* getColliderFilter();
#endif

    virtual void setBone(Bone* bone) { _bone = bone; }
    virtual Bone* getBone() const { return _bone; }

#if ENABLE_PHYSICS_BOX2D_DETECT
    virtual void setBody(b2BodyId body);
    virtual b2BodyId getBody() const;
#endif
protected:
    ax::Vector<ColliderBody*> _colliderBodyList;

    Bone* _bone;

#if ENABLE_PHYSICS_BOX2D_DETECT
    b2BodyId _body{b2_nullBodyId};
    ColliderFilter* _filter;
#endif

protected:
    bool _active;
};

}  // namespace ax::ext
