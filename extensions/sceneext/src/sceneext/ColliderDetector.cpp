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

#include "sceneext/ColliderDetector.h"
#include "sceneext/Bone.h"
#include "sceneext/TransformHelp.h"

using namespace ax;

namespace ax::ext
{

#if ENABLE_PHYSICS_BOX2D_DETECT
ColliderFilter::ColliderFilter(uint64_t categoryBits, uint64_t maskBits, int groupIndex)
    : _categoryBits(categoryBits), _maskBits(maskBits), _groupIndex(groupIndex)
{}

void ColliderFilter::updateShape(b2ShapeId shape)
{
    if (b2Shape_IsValid(shape))
    {
        b2Filter filter{.categoryBits = _categoryBits, .maskBits = _maskBits, .groupIndex = _groupIndex};
        b2Shape_SetFilter(shape, filter);
    }
}
#endif

#if ENABLE_PHYSICS_BOX2D_DETECT
ColliderBody::ColliderBody(ContourData* contourData) : _contourData(contourData)
{
    AX_SAFE_RETAIN(_contourData);
    _filter = new ColliderFilter();
}
#elif ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
ColliderBody::ColliderBody(ContourData* contourData) : _contourData(contourData)
{
    AX_SAFE_RETAIN(_contourData);
}
#endif

ColliderBody::~ColliderBody()
{
    AX_SAFE_RELEASE(_contourData);

#if ENABLE_PHYSICS_BOX2D_DETECT
    AX_SAFE_DELETE(_filter);
#endif
}

#if ENABLE_PHYSICS_BOX2D_DETECT
void ColliderBody::setColliderFilter(ColliderFilter* filter)
{
    *_filter = *filter;
}
ColliderFilter* ColliderBody::getColliderFilter()
{
    return _filter;
}
#endif

ColliderDetector* ColliderDetector::create()
{
    ColliderDetector* pColliderDetector = new ColliderDetector();
    if (pColliderDetector->init())
    {
        pColliderDetector->autorelease();
        return pColliderDetector;
    }
    AX_SAFE_DELETE(pColliderDetector);
    return nullptr;
}

ColliderDetector* ColliderDetector::create(Bone* bone)
{
    ColliderDetector* pColliderDetector = new ColliderDetector();
    if (pColliderDetector->init(bone))
    {
        pColliderDetector->autorelease();
        return pColliderDetector;
    }
    AX_SAFE_DELETE(pColliderDetector);
    return nullptr;
}

ColliderDetector::ColliderDetector() : _bone(nullptr), _active(false)
{
#if ENABLE_PHYSICS_BOX2D_DETECT
    _filter = nullptr;
#endif
}

ColliderDetector::~ColliderDetector()
{
    _colliderBodyList.clear();

#if ENABLE_PHYSICS_BOX2D_DETECT
    AX_SAFE_DELETE(_filter);
#endif
}

bool ColliderDetector::init()
{
    _colliderBodyList.clear();

#if ENABLE_PHYSICS_BOX2D_DETECT
    _filter = new ColliderFilter();
#endif

    return true;
}

bool ColliderDetector::init(Bone* bone)
{
    init();
    setBone(bone);

    return true;
}

void ColliderDetector::addContourData(ContourData* contourData)
{
    ColliderBody* colliderBody = new ColliderBody(contourData);
    _colliderBodyList.pushBack(colliderBody);
    colliderBody->release();

#if !ENABLE_PHYSICS_BOX2D_DETECT && ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
    std::vector<Vec2>& calculatedVertexList = colliderBody->_calculatedVertexList;

    unsigned long num = contourData->vertexList.size();
    for (unsigned long i = 0; i < num; i++)
    {
        calculatedVertexList.emplace_back(Vec2());
    }
#endif
}

void ColliderDetector::addContourDataList(ax::Vector<ContourData*>& contourDataList)
{
    for (const auto& contourData : contourDataList)
    {
        this->addContourData(contourData);
    }
}

void ColliderDetector::removeContourData(ContourData* contourData)
{
    std::vector<ColliderBody*> eraseList;

    for (const auto& body : _colliderBodyList)
    {
        if (body && body->getContourData() == contourData)
        {
            eraseList.emplace_back(body);
        }
    }

    for (const auto& body : eraseList)
    {
        this->_colliderBodyList.eraseObject(body);
    }
}

void ColliderDetector::removeAll()
{
    _colliderBodyList.clear();
}

void ColliderDetector::setActive(bool active)
{
    if (_active == active)
    {
        return;
    }

    _active = active;

#if ENABLE_PHYSICS_BOX2D_DETECT
    if (b2Body_IsValid(_body))
    {
        if (active)
        {
            setBody(_body);
        }
        else
        {
            for (auto&& object : _colliderBodyList)
            {
                ColliderBody* colliderBody = (ColliderBody*)object;
                auto shape                 = colliderBody->getShape();
                if (b2Shape_IsValid(shape))
                {
                    b2DestroyShape(shape, true);
                    colliderBody->setShape(b2_nullShapeId);
                }
            }
        }
    }
#endif
}

bool ColliderDetector::getActive()
{
    return _active;
}

const ax::Vector<ColliderBody*>& ColliderDetector::getColliderBodyList()
{
    return _colliderBodyList;
}

#if ENABLE_PHYSICS_BOX2D_DETECT
void ColliderDetector::setColliderFilter(ColliderFilter* filter)
{
    *_filter = *filter;

    for (auto&& object : _colliderBodyList)
    {
        ColliderBody* colliderBody = (ColliderBody*)object;
        colliderBody->setColliderFilter(filter);

#    if ENABLE_PHYSICS_BOX2D_DETECT
        if (b2Shape_IsValid(colliderBody->getShape()))
        {
            colliderBody->getColliderFilter()->updateShape(colliderBody->getShape());
        }
#    endif
    }
}
ColliderFilter* ColliderDetector::getColliderFilter()
{
    return _filter;
}
#endif

Vec2 helpPoint;

void ColliderDetector::updateTransform(Mat4& t)
{
    if (!_active)
    {
        return;
    }

    for (auto&& object : _colliderBodyList)
    {
        ColliderBody* colliderBody = (ColliderBody*)object;
        ContourData* contourData   = colliderBody->getContourData();

#if ENABLE_PHYSICS_BOX2D_DETECT
        b2ShapeId shape;
        b2Polygon shapePolygon;
        unsigned int mods = 0;
        if (b2Body_IsValid(_body))
        {
            shape = colliderBody->getShape();
            if (b2Shape_IsValid(shape))
            {
                shapePolygon = b2Shape_GetPolygon(shape);
            }
        }
#endif

        unsigned long num         = contourData->vertexList.size();
        std::vector<ax::Vec2>& vs = contourData->vertexList;

#if !ENABLE_PHYSICS_BOX2D_DETECT && ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
        std::vector<ax::Vec2>& cvs = colliderBody->_calculatedVertexList;
#endif

        for (unsigned long i = 0; i < num; i++)
        {
            helpPoint.setPoint(vs.at(i).x, vs.at(i).y);
            helpPoint = PointApplyTransform(helpPoint, t);

#if ENABLE_PHYSICS_BOX2D_DETECT
            if (b2Shape_IsValid(shape) && i < shapePolygon.count)
            {
                b2Vec2& bv = shapePolygon.vertices[i];
                bv.x       = helpPoint.x / PT_RATIO;
                bv.y       = helpPoint.y / PT_RATIO;
                ++mods;
            }
#elif ENABLE_PHYSICS_SAVE_CALCULATED_VERTEX
            cvs.at(i).x = helpPoint.x;
            cvs.at(i).y = helpPoint.y;
#endif
        }

#if ENABLE_PHYSICS_BOX2D_DETECT
        if (mods)
            b2Shape_SetPolygon(shape, &shapePolygon);
#endif
    }
}

#if ENABLE_PHYSICS_BOX2D_DETECT

void ColliderDetector::setBody(b2BodyId pBody)
{
    _body = pBody;

    for (auto&& object : _colliderBodyList)
    {
        ColliderBody* colliderBody = (ColliderBody*)object;

        ContourData* contourData = colliderBody->getContourData();

        b2Vec2* b2bv = new b2Vec2[contourData->vertexList.size()];

        int i = 0;
        for (auto&& v : contourData->vertexList)
        {
            b2bv[i] = b2Vec2{v.x / PT_RATIO, v.y / PT_RATIO};
            i++;
        }

        auto hull = b2ComputeHull(b2bv, static_cast<int>(contourData->vertexList.size()));
        if (!b2ValidateHull(&hull))
        {
            AXLOGW("ColliderDetector::setBody: invalid hull for contour data, ignore this contour data");
            continue;
        }

        auto polygon                = b2MakeOffsetPolygon(&hull, b2Vec2_zero, b2Rot_identity);
        auto shapeDef               = b2DefaultShapeDef();
        shapeDef.isSensor           = true;
        shapeDef.enableSensorEvents = true;
        shapeDef.userData           = _bone;

        AX_SAFE_DELETE(b2bv);
        auto oldShape = colliderBody->getShape();
        if (b2Shape_IsValid(oldShape))
            b2DestroyShape(oldShape, true);

        auto shape = b2CreatePolygonShape(_body, &shapeDef, &polygon);
        if (b2Shape_IsValid(shape))
        {
            colliderBody->setShape(shape);
            colliderBody->getColliderFilter()->updateShape(shape);
        }
    }
}

b2BodyId ColliderDetector::getBody() const
{
    return _body;
}
#endif

}  // namespace ax::ext
