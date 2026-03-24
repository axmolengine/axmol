/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#include "axmol/2d/physics/Collider2D.h"
#if defined(AX_ENABLE_PHYSICS_2D)

#    include <climits>
#    include <cmath>
#    include <unordered_map>

#    include "axmol/2d/physics/Rigidbody2D.h"
#    include "axmol/2d/physics/PhysicsWorld2D.h"
#    include "axmol/2d/physics/PhysicsUtility2D.h"

namespace ax
{
static b2Vec2 b2CentroidForPoly(const b2Vec2* verts, const int count)
{
    float sum = 0.0f;
    b2Vec2 vsum{0.0f, 0.0f};

    for (int i = 0; i < count; i++)
    {
        b2Vec2 v1   = verts[i];
        b2Vec2 v2   = verts[(i + 1) % count];
        float cross = b2Cross(v1, v2);  // v1.x * v2.y - v1.y * v2.x;

        sum += cross;
        vsum += (v1 + v2) * cross;
    }

    return vsum * (1.0f / (3.0f * sum));
}

Collider2D::Collider2D(const PhysicsMaterial2D& material)
    : _type(Type::UNKNOWN)
    , _sensor(false)
    , _preSolveEnabled(false)
    , _postSolveEnabled(false)
    , _scaleX(1.0f)
    , _scaleY(1.0f)
    , _newScaleX(1.0f)
    , _newScaleY(1.0f)
    , _material(material)
    , _tag(0)
    , _collisionFilter(DefaultCollisionFilter)
{}

Collider2D::~Collider2D()
{
    deatchFromBody();
}

bool Collider2D::attachToBody(Rigidbody2D* body)
{
    _attachedBody = body;
    applyMaterial();
    return true;
}

void Collider2D::deatchFromBody()
{
    if (!isAttached())
        return;
    for (auto&& shape : _shapeIds)
    {
        if (b2Shape_IsValid(shape))
            b2DestroyShape(shape, true);
    }
    _shapeIds.clear();
    _attachedBody = nullptr;
}

bool Collider2D::isAttached() const
{
    return _attachedBody != nullptr;
}

b2ShapeDef Collider2D::prepareShapeDef() const
{
    auto shapeDef                 = b2DefaultShapeDef();
    shapeDef.isSensor             = _sensor;
    shapeDef.enablePreSolveEvents = _preSolveEnabled;
    shapeDef.enableHitEvents      = _postSolveEnabled;
    return shapeDef;
}

void Collider2D::setPreSolveEnabled(bool bval)
{

    if (_preSolveEnabled == bval)
        return;
    _preSolveEnabled = bval;

    if (!isAttached())
        return;

    for (auto shapeId : _shapeIds)
        b2Shape_EnablePreSolveEvents(shapeId, bval);
}

void Collider2D::setPostSolveEnabled(bool bval)
{
    if (_postSolveEnabled == bval)
        return;
    _postSolveEnabled = bval;

    if (!isAttached())
        return;

    for (auto shapeId : _shapeIds)
        b2Shape_EnableHitEvents(shapeId, bval);
}

void Collider2D::setMaterial(const PhysicsMaterial2D& material)
{
    _material = material;

    applyMaterial();
    _attachedBody->syncMassData();
}

void Collider2D::applyMaterial()
{
    if (!isAttached())
        return;

    for (auto shape : _shapeIds)
    {
        b2Shape_SetDensity(shape, _material.density, false);
        b2Shape_SetRestitution(shape, _material.restitution);
        b2Shape_SetFriction(shape, _material.friction);
    }

    _attachedBody->setAutoMassDirty(true);
}

void Collider2D::setDensity(float density, bool apply)
{
    _material.density = std::clamp(density, physics2d::MinDensity, physics2d::MaxDensity);
    if (apply)
    {
        if (_shapeIds.empty())
            return;
        for (auto& shape : _shapeIds)
            b2Shape_SetDensity(shape, density, false);

        _attachedBody->setAutoMassDirty(true);
        _attachedBody->syncMassData();
    }
}

void Collider2D::setRestitution(float restitution, bool apply)
{
    _material.restitution = std::clamp(restitution, 0.0f, physics2d::LargeClamp);
    if (apply)
    {
        for (auto& shape : _shapeIds)
            b2Shape_SetRestitution(shape, restitution);
    }
}

void Collider2D::setFriction(float friction, bool apply)
{
    _material.friction = std::clamp(friction, 0.0f, physics2d::LargeClamp);

    if (apply && isAttached())
    {
        for (auto shape : _shapeIds)
            b2Shape_SetFriction(shape, friction);
    }
}

bool Collider2D::containsPoint(const Vec2& point) const
{
    auto b2pt = PhysicsUtility2D::tob2Vec2(point);
    for (auto&& shape : _shapeIds)
    {
        if (b2Shape_TestPoint(shape, b2pt))
            return true;
    }

    return false;
}

void Collider2D::setScale(float scaleX, float scaleY)
{
    if (std::abs(_scaleX - scaleX) > FLT_EPSILON || std::abs(_scaleY - scaleY) > FLT_EPSILON)
    {
        if (_type == Type::CIRCLE && scaleX != scaleY)
        {
            AXLOGD("CircleCollider2D WARNING: CANNOT support setScale with different x and y");
            return;
        }
        _newScaleX = scaleX;
        _newScaleY = scaleY;

        updateScale();
    }
}

void Collider2D::updateScale()
{
    _scaleX = _newScaleX;
    _scaleY = _newScaleY;
}

void Collider2D::updatePolyScale()
{
    auto factorX = _newScaleX / _scaleX;
    auto factorY = _newScaleY / _scaleY;

    auto shape   = _shapeIds[0];
    auto polygon = b2Shape_GetPolygon(shape);
    auto count   = polygon.count;
    auto& verts  = polygon.vertices;
    for (int i = 0; i < count; ++i)
    {
        verts[i].x *= factorX;
        verts[i].y *= factorY;
    }

    // convert hole to clockwise
    if (factorX * factorY < 0)
    {
        for (int i = 0; i < count / 2; ++i)
        {
            auto v               = polygon.vertices[i];
            verts[i]             = polygon.vertices[count - i - 1];
            verts[count - i - 1] = v;
        }
    }

    // FIXME: apply radius and offset
    auto transform = b2Transform_identity;
    auto hull      = b2ComputeHull(reinterpret_cast<const b2Vec2*>(polygon.vertices), count);
    auto polygon1  = b2MakePolygon(&hull, 0);

    b2Shape_SetPolygon(shape, &polygon1);

    Collider2D::updateScale();
}

void Collider2D::addShape(b2ShapeId shape)
{
    if (b2Shape_IsValid(shape))
    {
        b2Shape_SetUserData(shape, this);
        b2Shape_SetFilter(shape, _collisionFilter);
        _shapeIds.emplace_back(shape);
    }
}

void Collider2D::setSensor(bool sensor)
{
    if (sensor != _sensor)
    {
        _sensor = sensor;
    }
}

void Collider2D::recenterPoints(Vec2* points, int count, const Vec2& center)
{
    auto centroid = b2CentroidForPoly(reinterpret_cast<b2Vec2*>(points), count);
    for (int i = 0; i < count; i++)
    {
        points[i].x -= centroid.x;
        points[i].y -= centroid.y;
    }

    if (center != Vec2::ZERO)
    {
        for (int i = 0; i < count; ++i)
        {
            points[i] += center;
        }
    }
}

Vec2 Collider2D::getPolygonCenter(std::span<const Vec2> points)
{
    auto centroid = b2CentroidForPoly(reinterpret_cast<const b2Vec2*>(points.data()), static_cast<int>(points.size()));

    return PhysicsUtility2D::toVec2(centroid);
}

void Collider2D::setCategoryBits(uint64_t categoryBits)
{
    if (_collisionFilter.categoryBits == categoryBits)
        return;

    _collisionFilter.categoryBits = categoryBits;

    applyFilter();
}

void Collider2D::setMaskBits(uint64_t maskBits)
{
    if (_collisionFilter.maskBits == maskBits)
        return;

    _collisionFilter.maskBits = maskBits;

    applyFilter();
}

void Collider2D::setGroup(int groupIndex)
{
    if (_collisionFilter.groupIndex == groupIndex)
        return;

    _collisionFilter.groupIndex = groupIndex;

    applyFilter();
}

void Collider2D::applyFilter()
{
    if (isAttached())
    {
        for (auto&& shape : _shapeIds)
            b2Shape_SetFilter(shape, _collisionFilter);
    }
}

// CircleCollider2D
CircleCollider2D* CircleCollider2D::create(float radius,
                                           const PhysicsMaterial2D& material /* = MaterialDefault*/,
                                           const Vec2& offset /* = Vec2(0, 0)*/)
{
    CircleCollider2D* collider = new CircleCollider2D(material, radius, offset);
    collider->autorelease();
    return collider;
}

bool CircleCollider2D::attachToBody(Rigidbody2D* body)
{

    do
    {
        if (isAttached())
            return true;

        _type = Type::CIRCLE;

        auto shapeDef = prepareShapeDef();
        b2Circle circle{PhysicsUtility2D::tob2Vec2(_offset), _radius};
        auto shape = b2CreateCircleShape(body->internalHandle(), &shapeDef, &circle);

        AX_BREAK_IF(!b2Shape_IsValid(shape));

        addShape(shape);

        Collider2D::attachToBody(body);

        return true;
    } while (false);

    return false;
}

float CircleCollider2D::getRadius() const
{
    return b2Shape_GetCircle(_shapeIds[0]).radius;
}

Vec2 CircleCollider2D::getOffset()
{
    return PhysicsUtility2D::toVec2(b2Shape_GetCircle(_shapeIds[0]).center);
}

void CircleCollider2D::updateScale()
{
    auto factor = std::abs(_newScaleX / _scaleX);

    auto circle = b2Shape_GetCircle(_shapeIds[0]);
    circle.center *= factor;
    circle.radius *= factor;

    b2Shape_SetCircle(_shapeIds[0], &circle);

    Collider2D::updateScale();
}

// PolygonCollider2D
PolygonCollider2D* PolygonCollider2D::create(std::span<const Vec2> points,
                                             const PhysicsMaterial2D& material /* = MaterialDefault*/,
                                             const Vec2& offset /* = Vec2(0, 0)*/,
                                             float radius /* = 0.0f*/)
{
    PolygonCollider2D* collider = new PolygonCollider2D(material, points, offset, radius);
    collider->autorelease();
    return collider;
}

bool PolygonCollider2D::attachToBody(Rigidbody2D* body)
{
    do
    {
        if (isAttached())
            return true;

        _type = Type::POLYGON;

        auto shapeDef = prepareShapeDef();
        auto hull     = b2ComputeHull(reinterpret_cast<const b2Vec2*>(_points.data()), _points.size());
        auto polygon  = b2MakeOffsetPolygon(&hull, PhysicsUtility2D::tob2Vec2(_offset), b2MakeRot(_radius));
        auto shape    = b2CreatePolygonShape(body->internalHandle(), &shapeDef, &polygon);

        AX_BREAK_IF(!b2Shape_IsValid(shape));

        addShape(shape);

        Collider2D::attachToBody(body);

        return true;
    } while (false);

    return false;
}

Vec2 PolygonCollider2D::getPoint(int i) const
{
    auto polygon = b2Shape_GetPolygon(_shapeIds[0]);
    return i < polygon.count ? PhysicsUtility2D::toVec2(polygon.vertices[i]) : Vec2::ZERO;
}

void PolygonCollider2D::getPoints(Vec2* outPoints) const
{
    auto polygon = b2Shape_GetPolygon(_shapeIds[0]);
    memcpy(outPoints, polygon.vertices, sizeof(Vec2) * polygon.count);
}

int PolygonCollider2D::getPointsCount() const
{
    auto polygon = b2Shape_GetPolygon(_shapeIds[0]);
    return polygon.count;
}

Vec2 PolygonCollider2D::getCenter()
{
    auto polygon = b2Shape_GetPolygon(_shapeIds[0]);

    return PhysicsUtility2D::toVec2(polygon.centroid);
}

void PolygonCollider2D::updateScale()
{
    updatePolyScale();
}

// BoxCollider2D
BoxCollider2D* BoxCollider2D::create(const Vec2& size,
                                     const PhysicsMaterial2D& material /* = MaterialDefault*/,
                                     const Vec2& offset /* = Vec2(0, 0)*/,
                                     float radius /* = 0.0f*/)
{
    BoxCollider2D* collider = new BoxCollider2D(material, size, offset, radius);
    collider->autorelease();

    return collider;
}

bool BoxCollider2D::attachToBody(Rigidbody2D* body)
{
    do
    {
        if (isAttached())
            return true;

        _type = Type::BOX;

        b2ShapeDef shapeDef = prepareShapeDef();
        // !!!remark: should div 2 with b2MakeOffsetBox, otherwise, size will be double
        auto polygon =
            b2MakeOffsetBox(_size.x / 2, _size.y / 2, PhysicsUtility2D::tob2Vec2(_offset), b2MakeRot(_radius));
        auto shape = b2CreatePolygonShape(body->internalHandle(), &shapeDef, &polygon);
        AX_BREAK_IF(!b2Shape_IsValid(shape));

        addShape(shape);

        Collider2D::attachToBody(body);

        return true;
    } while (false);

    return false;
}

void BoxCollider2D::updateScale()
{
    updatePolyScale();
}

Vec2 BoxCollider2D::getSize() const
{
    auto polygon = b2Shape_GetPolygon(_shapeIds[0]);
    auto x       = b2Distance(polygon.vertices[1], polygon.vertices[2]);
    auto y       = b2Distance(polygon.vertices[0], polygon.vertices[1]);
    return PhysicsUtility2D::toVec2(b2Vec2{x, y});
}

// EdgeSegmentCollider2D
EdgeSegmentCollider2D* EdgeSegmentCollider2D::create(const Vec2& a,
                                                     const Vec2& b,
                                                     const PhysicsMaterial2D& material /* = MaterialDefault*/)
{
    EdgeSegmentCollider2D* collider = new EdgeSegmentCollider2D(material, a, b);
    collider->autorelease();
    return collider;
}

bool EdgeSegmentCollider2D::attachToBody(Rigidbody2D* body)
{
    do
    {
        if (isAttached())
            return true;

        _type = Type::EDGESEGMENT;

        auto shapeDef = prepareShapeDef();
        b2Segment segment{PhysicsUtility2D::tob2Vec2(_pointA), PhysicsUtility2D::tob2Vec2(_pointB)};
        auto shape = b2CreateSegmentShape(body->internalHandle(), &shapeDef, &segment);

        AX_BREAK_IF(!b2Shape_IsValid(shape));

        addShape(shape);

        Collider2D::attachToBody(body);

        return true;
    } while (false);

    return false;
}

Vec2 EdgeSegmentCollider2D::getPointA() const
{
    auto detail = b2Shape_GetSegment(_shapeIds[0]);
    return PhysicsUtility2D::toVec2(detail.point1);
}

Vec2 EdgeSegmentCollider2D::getPointB() const
{
    auto detail = b2Shape_GetSegment(_shapeIds[0]);
    return PhysicsUtility2D::toVec2(detail.point2);
}

Vec2 EdgeSegmentCollider2D::getCenter()
{
    auto detail = b2Shape_GetSegment(_shapeIds[0]);
    return PhysicsUtility2D::toVec2((detail.point1 + detail.point2) * 0.5f);
}

void EdgeSegmentCollider2D::updateScale()
{
    auto factorX = _newScaleX / _scaleX;
    auto factorY = _newScaleY / _scaleY;

    auto shape  = _shapeIds[0];
    auto detail = b2Shape_GetSegment(shape);
    detail.point1.x *= factorX;
    detail.point1.y *= factorY;
    detail.point2.x *= factorX;
    detail.point2.y *= factorY;
    b2Shape_SetSegment(shape, &detail);

    Collider2D::updateScale();
}

// EdgeBoxCollider2D
EdgeBoxCollider2D* EdgeBoxCollider2D::create(const Vec2& size,
                                             const PhysicsMaterial2D& material /* = MaterialDefault*/,
                                             const Vec2& offset /* = Vec2(0, 0)*/)
{
    EdgeBoxCollider2D* collider = new EdgeBoxCollider2D(material, size, offset);
    collider->autorelease();
    return collider;
}

bool EdgeBoxCollider2D::attachToBody(Rigidbody2D* body)
{
    do
    {
        if (isAttached())
            return true;

        _type = Type::EDGEBOX;

        b2Vec2 vec[4] = {};
        vec[0]        = PhysicsUtility2D::tob2Vec2(Vec2(-_size.width / 2 + _offset.x, -_size.height / 2 + _offset.y));
        vec[1]        = PhysicsUtility2D::tob2Vec2(Vec2(+_size.width / 2 + _offset.x, -_size.height / 2 + _offset.y));
        vec[2]        = PhysicsUtility2D::tob2Vec2(Vec2(+_size.width / 2 + _offset.x, +_size.height / 2 + _offset.y));
        vec[3]        = PhysicsUtility2D::tob2Vec2(Vec2(-_size.width / 2 + _offset.x, +_size.height / 2 + _offset.y));

        int i               = 0;
        b2ShapeDef shapeDef = prepareShapeDef();
        for (; i < 4; ++i)
        {
            b2Segment segment{vec[i], vec[(i + 1) % 4]};
            auto shape = b2CreateSegmentShape(body->internalHandle(), &shapeDef, &segment);
            AX_BREAK_IF(!b2Shape_IsValid(shape));

            addShape(shape);
        }
        AX_BREAK_IF(i < 4);

        Collider2D::attachToBody(body);

        return true;
    } while (false);

    return false;
}

// EdgeBoxCollider2D
EdgePolygonCollider2D* EdgePolygonCollider2D::create(std::span<const Vec2> points,
                                                     const PhysicsMaterial2D& material /* = MaterialDefault*/)
{
    EdgePolygonCollider2D* collider = new EdgePolygonCollider2D(material, points);
    collider->autorelease();
    return collider;
}

bool EdgePolygonCollider2D::attachToBody(Rigidbody2D* body)
{
    do
    {
        if (isAttached())
            return true;

        _type = Type::EDGEPOLYGON;

        auto shapeDef    = prepareShapeDef();
        int i            = 0;
        const auto count = static_cast<int>(_points.size());
        for (; i < count; ++i)
        {
            b2Segment segment{PhysicsUtility2D::tob2Vec2(_points[i]),
                              PhysicsUtility2D::tob2Vec2(_points[(i + 1) % count])};
            auto shape = b2CreateSegmentShape(body->internalHandle(), &shapeDef, &segment);
            AX_BREAK_IF(!b2Shape_IsValid(shape));

            addShape(shape);
        }

        AX_BREAK_IF(i < count);

        Collider2D::attachToBody(body);

        return true;
    } while (false);

    return false;
}

Vec2 EdgePolygonCollider2D::getCenter()
{
    // REMARK: check the logic whether identical to
    // collect points then invoke b2CentroidForPoly
    b2Vec2 centroid{0.0f, 0.0f};
    float signedArea = 0.0f;
    int count        = static_cast<int>(_shapeIds.size());

    for (int i = 0; i < count; ++i)
    {
        auto curSegment       = b2Shape_GetSegment(_shapeIds[i]);
        auto nextSegment      = b2Shape_GetSegment(_shapeIds[(i + 1) % count]);
        const b2Vec2& current = curSegment.point1;
        const b2Vec2& next    = nextSegment.point1;
        float crossProduct    = b2Cross(current, next);
        float triangleArea    = 0.5f * crossProduct;
        signedArea += triangleArea;
        centroid += triangleArea * (current + next) * (1 / 3.0f);
    }

    centroid *= (1 / signedArea);
    return PhysicsUtility2D::toVec2(centroid);
}

void EdgePolygonCollider2D::getPoints(ax::Vec2* outPoints) const
{
    int i = 0;
    for (auto&& shape : _shapeIds)
    {
        auto& outPoint = outPoints[i++];
        auto segment   = b2Shape_GetSegment(shape);
        auto& inPoint  = segment.point1;
        outPoint.x     = inPoint.x;
        outPoint.y     = inPoint.y;
    }
}

int EdgePolygonCollider2D::getPointsCount() const
{
    return static_cast<int>(_shapeIds.size());
}

// EdgeChainCollider2D
EdgeChainCollider2D* EdgeChainCollider2D::create(std::span<const Vec2> points,
                                                 const PhysicsMaterial2D& material /* = MaterialDefault*/)
{
    EdgeChainCollider2D* collider = new EdgeChainCollider2D(material, points);
    collider->autorelease();
    return collider;
}

void EdgePolygonCollider2D::updateScale()
{
    auto factorX = _newScaleX / _scaleX;
    auto factorY = _newScaleY / _scaleY;

    for (auto&& shape : _shapeIds)
    {
        auto segment = b2Shape_GetSegment(shape);
        auto& a      = segment.point1;
        a.x *= factorX;
        a.y *= factorY;
        auto& b = segment.point2;
        b.x *= factorX;
        b.y *= factorY;
        b2Shape_SetSegment(shape, &segment);
    }

    Collider2D::updateScale();
}

bool EdgeChainCollider2D::attachToBody(Rigidbody2D* body)
{
    do
    {
        if (isAttached())
            return true;

        _type = Type::EDGECHAIN;

        auto shapeDef = prepareShapeDef();
        int i         = 0;
        auto count    = static_cast<int>(_points.size());
        for (; i < count - 1; ++i)
        {
            b2Segment segment{PhysicsUtility2D::tob2Vec2(_points[i]), PhysicsUtility2D::tob2Vec2(_points[i + 1])};
            auto shape = b2CreateSegmentShape(body->internalHandle(), &shapeDef, &segment);
            AX_BREAK_IF(!b2Shape_IsValid(shape));

            addShape(shape);
        }

        AX_BREAK_IF(i < count - 1);

        Collider2D::attachToBody(body);

        return true;
    } while (false);

    return false;
}

Vec2 EdgeChainCollider2D::getCenter()
{
    // REMARK: check the logic whether identical to
    // collect points then invoke b2CentroidForPoly
    b2Vec2 centroid{0.0f, 0.0f};
    float signedArea = 0.0f;
    int count        = static_cast<int>(_shapeIds.size());

    for (int i = 0; i < count; ++i)
    {
        auto curSegment       = b2Shape_GetSegment(_shapeIds[i]);
        auto nextSegment      = b2Shape_GetSegment(_shapeIds[(i + 1) % count]);
        const b2Vec2& current = curSegment.point1;
        const b2Vec2& next    = i != (count - 1) ? nextSegment.point1 : nextSegment.point2;
        float crossProduct    = b2Cross(current, next);
        float triangleArea    = 0.5f * crossProduct;
        signedArea += triangleArea;
        centroid += triangleArea * (current + next) * (1 / 3.0f);
    }

    centroid *= (1 / signedArea);
    return PhysicsUtility2D::toVec2(centroid);
}

void EdgeChainCollider2D::getPoints(Vec2* outPoints) const
{
    int i = 0;
    for (auto&& shape : _shapeIds)
    {
        outPoints[i++] = PhysicsUtility2D::toVec2(b2Shape_GetSegment(shape).point1);
    }

    outPoints[i++] = PhysicsUtility2D::toVec2(b2Shape_GetSegment(_shapeIds.back()).point2);
}

int EdgeChainCollider2D::getPointsCount() const
{
    return static_cast<int>(_shapeIds.size() + 1);
}

void EdgeChainCollider2D::updateScale()
{
    auto factorX = _newScaleX / _scaleX;
    auto factorY = _newScaleY / _scaleY;

    for (auto&& shape : _shapeIds)
    {
        auto segment = b2Shape_GetSegment(shape);
        auto& a      = segment.point1;
        a.x *= factorX;
        a.y *= factorY;
        auto& b = segment.point2;
        b.x *= factorX;
        b.y *= factorY;
        b2Shape_SetSegment(shape, &segment);
    }

    Collider2D::updateScale();
}

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS_2D)
