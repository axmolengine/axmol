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

#include "physics/PhysicsCollider.h"
#if defined(AX_ENABLE_PHYSICS)

#    include <climits>
#    include <cmath>
#    include <unordered_map>

#    include "physics/PhysicsBody.h"
#    include "physics/PhysicsWorld.h"
#    include "physics/PhysicsHelper.h"

namespace ax
{
extern const float PHYSICS_INFINITY;

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

PhysicsCollider::PhysicsCollider(PhysicsBody* body)
    : _body(body)
    , _type(Type::UNKNOWN)
    , _area(0.0f)
    , _sensor(false)
    , _scaleX(1.0f)
    , _scaleY(1.0f)
    , _newScaleX(1.0f)
    , _newScaleY(1.0f)
    , _tag(0)
    , _categoryBitmask(UINT_MAX)
    , _collisionBitmask(UINT_MAX)
    , _contactTestBitmask(0)
    , _group(0)
{}

PhysicsCollider::~PhysicsCollider()
{
    deatchFromBody();
}

void PhysicsCollider::deatchFromBody()
{
    for (auto&& shape : _b2Shapes)
    {
        if (b2Shape_IsValid(shape))
            b2DestroyShape(shape, true);
    }
    _b2Shapes.clear();
    _body = nullptr;
}

bool PhysicsCollider::isDeatched() const
{
    return _body != nullptr;
}

void PhysicsCollider::setMaterial(const PhysicsMaterial& material)
{
    setDensity(material.density, false);
    setRestitution(material.restitution, false);
    setFriction(material.friction, false);

    for (auto shape : _b2Shapes)
    {
        b2Shape_SetDensity(shape, material.density, false);
        b2Shape_SetRestitution(shape, material.restitution);
        b2Shape_SetFriction(shape, material.friction);
    }

    b2Body_ApplyMassFromShapes(_body->getB2Body());
}

void PhysicsCollider::setScale(float scaleX, float scaleY)
{
    if (std::abs(_scaleX - scaleX) > FLT_EPSILON || std::abs(_scaleY - scaleY) > FLT_EPSILON)
    {
        if (_type == Type::CIRCLE && scaleX != scaleY)
        {
            AXLOGD("PhysicsColliderCircle WARNING: CANNOT support setScale with different x and y");
            return;
        }
        _newScaleX = scaleX;
        _newScaleY = scaleY;

        updateScale();

        // re-calculate area
        _area = calculateArea();
    }
}

void PhysicsCollider::updateScale()
{
    _scaleX = _newScaleX;
    _scaleY = _newScaleY;
}

void PhysicsCollider::addShape(b2ShapeId shape)
{
    if (b2Shape_IsValid(shape))
    {
        b2Shape_SetUserData(shape, this);
        b2Shape_SetFilter(shape, b2Filter{(uint64_t)CollisionBits::All, (uint64_t)CollisionBits::All, _group});
        _b2Shapes.emplace_back(shape);
    }
}

PhysicsColliderCircle::~PhysicsColliderCircle() {}

PhysicsColliderBox::~PhysicsColliderBox() {}

PhysicsColliderPolygon::~PhysicsColliderPolygon() {}

PhysicsColliderEdgeBox::~PhysicsColliderEdgeBox() {}

PhysicsColliderEdgeChain::~PhysicsColliderEdgeChain() {}

PhysicsColliderEdgePolygon::~PhysicsColliderEdgePolygon() {}

PhysicsColliderEdgeSegment::~PhysicsColliderEdgeSegment() {}

void PhysicsCollider::setDensity(float density, bool apply)
{
    if (density < 0)
    {
        return;
    }

    _material.density = density;
    if (apply)
    {
        for (auto& shape : _b2Shapes)
            b2Shape_SetDensity(shape, density, false);

        b2Body_ApplyMassFromShapes(_body->getB2Body());
    }
}

void PhysicsCollider::setRestitution(float restitution, bool apply)
{
    _material.restitution = restitution;
    if (apply)
    {
        for (auto& shape : _b2Shapes)
            b2Shape_SetDensity(shape, restitution, false);

        b2Body_ApplyMassFromShapes(_body->getB2Body());
    }
}

void PhysicsCollider::setFriction(float friction, bool apply)
{
    _material.friction = friction;

    if (apply)
    {
        for (auto shape : _b2Shapes)
            b2Shape_SetFriction(shape, friction);
        b2Body_ApplyMassFromShapes(_body->getB2Body());
    }
}

void PhysicsCollider::setSensor(bool sensor)
{  // FIXME: NOT SUPPORT
    if (sensor != _sensor)
    {
        _sensor = sensor;
    }
}

void PhysicsCollider::recenterPoints(Vec2* points, int count, const Vec2& center)
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

Vec2 PhysicsCollider::getPolygonCenter(const Vec2* points, int count)
{
    auto centroid = b2CentroidForPoly(reinterpret_cast<const b2Vec2*>(points), count);

    return PhysicsHelper::toVec2(centroid);
}

// PhysicsColliderCircle
PhysicsColliderCircle* PhysicsColliderCircle::create(PhysicsBody* body,
                                                     float radius,
                                                     const PhysicsMaterial& material /* = MaterialDefault*/,
                                                     const Vec2& offset /* = Vec2(0, 0)*/)
{
    PhysicsColliderCircle* collider = new PhysicsColliderCircle(body);
    if (collider->init(radius, material, offset))
    {
        collider->autorelease();
        return collider;
    }

    AX_SAFE_DELETE(collider);
    return nullptr;
}

bool PhysicsColliderCircle::init(float radius,
                                 const PhysicsMaterial& material /* = MaterialDefault*/,
                                 const Vec2& offset /*= Vec2(0, 0)*/)
{
    do
    {
        _type = Type::CIRCLE;

        auto circleShapeDef = b2DefaultShapeDef();
        b2Circle circle{PhysicsHelper::tob2Vec2(offset), radius};
        auto shape = b2CreateCircleShape(_body->getB2Body(), &circleShapeDef, &circle);

        AX_BREAK_IF(!b2Shape_IsValid(shape));

        addShape(shape);

        _area = calculateArea();

        setMaterial(material);
        return true;
    } while (false);

    return false;
}

float PhysicsColliderCircle::calculateArea(float radius)
{
    return B2_PI * radius * radius;
}

float PhysicsColliderCircle::calculateArea()
{
    auto circle = b2Shape_GetCircle(_b2Shapes[0]);
    return calculateArea(circle.radius);
}

float PhysicsColliderCircle::getRadius() const
{
    return b2Shape_GetCircle(_b2Shapes[0]).radius;
}

Vec2 PhysicsColliderCircle::getOffset()
{
    return PhysicsHelper::toVec2(b2Shape_GetCircle(_b2Shapes[0]).center);
}

void PhysicsColliderCircle::updateScale()
{
    auto factor = std::abs(_newScaleX / _scaleX);

    auto circle = b2Shape_GetCircle(_b2Shapes[0]);
    circle.center *= factor;
    circle.radius *= factor;

    b2Shape_SetCircle(_b2Shapes[0], &circle);

    PhysicsCollider::updateScale();
}

// PhysicsColliderEdgeSegment
PhysicsColliderEdgeSegment* PhysicsColliderEdgeSegment::create(PhysicsBody* body,
                                                               const Vec2& a,
                                                               const Vec2& b,
                                                               const PhysicsMaterial& material /* = MaterialDefault*/,
                                                               float border /* = 1*/)
{
    PhysicsColliderEdgeSegment* collider = new PhysicsColliderEdgeSegment(body);
    if (collider->init(a, b, material, border))
    {
        collider->autorelease();
        return collider;
    }

    AX_SAFE_DELETE(collider);
    return nullptr;
}

bool PhysicsColliderEdgeSegment::init(const Vec2& a,
                                      const Vec2& b,
                                      const PhysicsMaterial& material /* = MaterialDefault*/,
                                      float border /* = 1*/)
{
    do
    {
        _type = Type::EDGESEGMENT;

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        b2Segment segment{PhysicsHelper::tob2Vec2(a), PhysicsHelper::tob2Vec2(b)};
        auto shape = b2CreateSegmentShape(_body->getB2Body(), &shapeDef, &segment);

        AX_BREAK_IF(!b2Shape_IsValid(shape));

        addShape(shape);

        setMaterial(material);

        return true;
    } while (false);

    return false;
}

Vec2 PhysicsColliderEdgeSegment::getPointA() const
{
    auto detail = b2Shape_GetSegment(_b2Shapes[0]);
    return PhysicsHelper::toVec2(detail.point1);
}

Vec2 PhysicsColliderEdgeSegment::getPointB() const
{
    auto detail = b2Shape_GetSegment(_b2Shapes[0]);
    return PhysicsHelper::toVec2(detail.point2);
}

Vec2 PhysicsColliderEdgeSegment::getCenter()
{
    auto detail = b2Shape_GetSegment(_b2Shapes[0]);
    return PhysicsHelper::toVec2((detail.point1 + detail.point2) * 0.5f);
}

void PhysicsColliderEdgeSegment::updateScale()
{
    auto factorX = _newScaleX / _scaleX;
    auto factorY = _newScaleY / _scaleY;

    auto shape  = _b2Shapes[0];
    auto detail = b2Shape_GetSegment(shape);
    detail.point1.x *= factorX;
    detail.point1.y *= factorY;
    detail.point2.x *= factorX;
    detail.point2.y *= factorY;
    b2Shape_SetSegment(shape, &detail);

    PhysicsCollider::updateScale();
}

// PhysicsColliderBox
PhysicsColliderBox* PhysicsColliderBox::create(PhysicsBody* body,
                                               const Vec2& size,
                                               const PhysicsMaterial& material /* = MaterialDefault*/,
                                               const Vec2& offset /* = Vec2(0, 0)*/,
                                               float radius /* = 0.0f*/)
{
    PhysicsColliderBox* collider = new PhysicsColliderBox(body);
    if (collider->init(size, material, offset, radius))
    {
        collider->autorelease();
        return collider;
    }

    AX_SAFE_DELETE(collider);
    return nullptr;
}

bool PhysicsColliderBox::init(const Vec2& size,
                              const PhysicsMaterial& material /* = MaterialDefault*/,
                              const Vec2& offset /*= Vec2(0, 0)*/,
                              float radius /* = 0.0f*/)
{
    do
    {
        _type = Type::BOX;

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        // !!!remark: should div 2 with b2MakeOffsetBox, otherwise, size will be double
        auto polygon = b2MakeOffsetBox(size.x / 2, size.y / 2, phl::tob2Vec2(offset), b2MakeRot(radius));
        auto shape   = b2CreatePolygonShape(_body->getB2Body(), &shapeDef, &polygon);
        AX_BREAK_IF(!b2Shape_IsValid(shape));

        addShape(shape);

        _area = calculateArea();

        setMaterial(material);

        return true;
    } while (false);

    return false;
}

Vec2 PhysicsColliderBox::getSize() const
{
    auto polygon = b2Shape_GetPolygon(_b2Shapes[0]);
    auto x       = b2Distance(polygon.vertices[1], polygon.vertices[2]);
    auto y       = b2Distance(polygon.vertices[0], polygon.vertices[1]);
    return phl::toVec2(b2Vec2{x, y});
}

// PhysicsColliderPolygon
PhysicsColliderPolygon* PhysicsColliderPolygon::create(PhysicsBody* body,
                                                       const Vec2* points,
                                                       int count,
                                                       const PhysicsMaterial& material /* = MaterialDefault*/,
                                                       const Vec2& offset /* = Vec2(0, 0)*/,
                                                       float radius /* = 0.0f*/)
{
    PhysicsColliderPolygon* collider = new PhysicsColliderPolygon(body);
    if (collider->init(points, count, material, offset, radius))
    {
        collider->autorelease();
        return collider;
    }

    AX_SAFE_DELETE(collider);
    return nullptr;
}

bool PhysicsColliderPolygon::init(const Vec2* points,
                                  int count,
                                  const PhysicsMaterial& material /* = MaterialDefault*/,
                                  const Vec2& offset /* = Vec2(0, 0)*/,
                                  float radius /* = 0.0f*/)
{
    do
    {
        _type = Type::POLYGON;

        b2ShapeDef shapeDef = b2DefaultShapeDef();
        auto hull           = b2ComputeHull(reinterpret_cast<const b2Vec2*>(points), count);
        auto polygon        = b2MakeOffsetPolygon(&hull,  PhysicsHelper::tob2Vec2(offset), b2MakeRot(radius));
        auto shape          = b2CreatePolygonShape(_body->getB2Body(), &shapeDef, &polygon);

        AX_BREAK_IF(!b2Shape_IsValid(shape));

        addShape(shape);

        _area = calculateArea();

        setMaterial(material);

        return true;
    } while (false);

    return false;
}

float PhysicsColliderPolygon::calculateArea(const Vec2* points, int count)
{
    float area = 0.0f;
    for (int i = 0; i < count; ++i)
    {
        const auto& current = points[i];
        const auto& next    = points[(i + 1) % count];
        area += current.x * next.y - next.x * current.y;
    }

    return 0.5f * fabs(area);
}

float PhysicsColliderPolygon::calculateArea()
{
    auto polygon = b2Shape_GetPolygon(_b2Shapes[0]);
    auto area    = calculateArea(reinterpret_cast<const Vec2*>(polygon.vertices), polygon.count);
    return area;
}

Vec2 PhysicsColliderPolygon::getPoint(int i) const
{
    auto polygon = b2Shape_GetPolygon(_b2Shapes[0]);
    return i < polygon.count ? PhysicsHelper::toVec2(polygon.vertices[i]) : Vec2::ZERO;
}

void PhysicsColliderPolygon::getPoints(Vec2* outPoints) const
{
    auto polygon = b2Shape_GetPolygon(_b2Shapes[0]);
    memcpy(outPoints, polygon.vertices, sizeof(Vec2) * polygon.count);
}

int PhysicsColliderPolygon::getPointsCount() const
{
    auto polygon = b2Shape_GetPolygon(_b2Shapes[0]);
    return polygon.count;
}

Vec2 PhysicsColliderPolygon::getCenter()
{
    auto polygon = b2Shape_GetPolygon(_b2Shapes[0]);

    return PhysicsHelper::toVec2(polygon.centroid);
}

void PhysicsColliderPolygon::updateScale()
{
    auto factorX = _newScaleX / _scaleX;
    auto factorY = _newScaleY / _scaleY;

    auto shape   = _b2Shapes[0];
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
    auto transform      = b2Transform_identity;
    auto hull           = b2ComputeHull(reinterpret_cast<const b2Vec2*>(polygon.vertices), count);
    auto polygon1        = b2MakePolygon(&hull, 0);

    b2Shape_SetPolygon(shape, &polygon1);

    PhysicsCollider::updateScale();
}

// PhysicsColliderEdgeBox
PhysicsColliderEdgeBox* PhysicsColliderEdgeBox::create(PhysicsBody* body,
                                                       const Vec2& size,
                                                       const PhysicsMaterial& material /* = MaterialDefault*/,
                                                       float border /* = 1*/,
                                                       const Vec2& offset /* = Vec2(0, 0)*/)
{
    PhysicsColliderEdgeBox* collider = new PhysicsColliderEdgeBox(body);
    if (collider->init(size, material, border, offset))
    {
        collider->autorelease();
        return collider;
    }

    AX_SAFE_DELETE(collider);
    return nullptr;
}

bool PhysicsColliderEdgeBox::init(const Vec2& size,
                                  const PhysicsMaterial& material /* = MaterialDefault*/,
                                  float border /* = 1*/,
                                  const Vec2& offset /*= Vec2(0, 0)*/)
{
    do
    {
        _type = Type::EDGEBOX;

        // NOTE: box2d doesn't support border

        b2Vec2 vec[4] = {};
        vec[0]        = phl::tob2Vec2(Vec2(-size.width / 2 + offset.x, -size.height / 2 + offset.y));
        vec[1]        = phl::tob2Vec2(Vec2(+size.width / 2 + offset.x, -size.height / 2 + offset.y));
        vec[2]        = phl::tob2Vec2(Vec2(+size.width / 2 + offset.x, +size.height / 2 + offset.y));
        vec[3]        = phl::tob2Vec2(Vec2(-size.width / 2 + offset.x, +size.height / 2 + offset.y));

        int i               = 0;
        b2ShapeDef shapeDef = b2DefaultShapeDef();
        for (; i < 4; ++i)
        {
            b2Segment segment{vec[i], vec[(i + 1) % 4]};
            auto shape = b2CreateSegmentShape(_body->getB2Body(), &shapeDef, &segment);
            AX_BREAK_IF(!b2Shape_IsValid(shape));

            addShape(shape);
        }
        AX_BREAK_IF(i < 4);

        setMaterial(material);

        return true;
    } while (false);

    return false;
}

// PhysicsColliderEdgeBox
PhysicsColliderEdgePolygon* PhysicsColliderEdgePolygon::create(PhysicsBody* body,
                                                               const Vec2* points,
                                                               int count,
                                                               const PhysicsMaterial& material /* = MaterialDefault*/,
                                                               float border /* = 1*/)
{
    PhysicsColliderEdgePolygon* collider = new PhysicsColliderEdgePolygon(body);
    if (collider->init(points, count, material, border))
    {
        collider->autorelease();
        return collider;
    }

    AX_SAFE_DELETE(collider);
    return nullptr;
}

bool PhysicsColliderEdgePolygon::init(const Vec2* points,
                                      int count,
                                      const PhysicsMaterial& material /* = MaterialDefault*/,
                                      float border /* = 1*/)
{
    do
    {
        _type = Type::EDGEPOLYGON;

        auto def = b2DefaultShapeDef();
        
        int i    = 0;
        for (; i < count; ++i)
        {
            b2Segment segment{PhysicsHelper::tob2Vec2(points[i]), PhysicsHelper::tob2Vec2(points[(i + 1) % count])};
            auto shape = b2CreateSegmentShape(_body->getB2Body(), &def, &segment);
            AX_BREAK_IF(!b2Shape_IsValid(shape));

            addShape(shape);
        }

        AX_BREAK_IF(i < count);

        setMaterial(material);

        return true;
    } while (false);

    return false;
}

Vec2 PhysicsColliderEdgePolygon::getCenter()
{
    // REMARK: check the logic whether identical to
    // collect points then invoke b2CentroidForPoly
    b2Vec2 centroid{0.0f, 0.0f};
    float signedArea = 0.0f;
    int count        = static_cast<int>(_b2Shapes.size());

    for (int i = 0; i < count; ++i)
    {
        auto curSegment       = b2Shape_GetSegment(_b2Shapes[i]);
        auto nextSegment      = b2Shape_GetSegment(_b2Shapes[(i + 1) % count]);
        const b2Vec2& current = curSegment.point1;
        const b2Vec2& next    = nextSegment.point1;
        float crossProduct    = b2Cross(current, next);
        float triangleArea    = 0.5f * crossProduct;
        signedArea += triangleArea;
        centroid += triangleArea * (current + next) * (1 / 3.0f);
    }

    centroid *= (1 / signedArea);
    return PhysicsHelper::toVec2(centroid);
}

void PhysicsColliderEdgePolygon::getPoints(ax::Vec2* outPoints) const
{
    int i = 0;
    for (auto&& shape : _b2Shapes)
    {
        auto& outPoint = outPoints[i++];
        auto segment   = b2Shape_GetSegment(shape);
        auto& inPoint  = segment.point1;
        outPoint.x     = inPoint.x;
        outPoint.y     = inPoint.y;
    }
}

int PhysicsColliderEdgePolygon::getPointsCount() const
{
    return static_cast<int>(_b2Shapes.size());
}

// PhysicsColliderEdgeChain
PhysicsColliderEdgeChain* PhysicsColliderEdgeChain::create(PhysicsBody* body,
                                                           const Vec2* points,
                                                           int count,
                                                           const PhysicsMaterial& material /* = MaterialDefault*/,
                                                           float border /* = 1*/)
{
    PhysicsColliderEdgeChain* collider = new PhysicsColliderEdgeChain(body);
    if (collider->init(points, count, material, border))
    {
        collider->autorelease();
        return collider;
    }

    AX_SAFE_DELETE(collider);
    return nullptr;
}

void PhysicsColliderEdgePolygon::updateScale()
{
    auto factorX = _newScaleX / _scaleX;
    auto factorY = _newScaleY / _scaleY;

    for (auto&& shape : _b2Shapes)
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

    PhysicsCollider::updateScale();
}

bool PhysicsColliderEdgeChain::init(const Vec2* points,
                                    int count,
                                    const PhysicsMaterial& material /* = MaterialDefault*/,
                                    float border /* = 1*/)
{
    do
    {
        _type = Type::EDGECHAIN;

        auto def = b2DefaultShapeDef();
        int i    = 0;
        for (; i < count - 1; ++i)
        {
            b2Segment segment{PhysicsHelper::tob2Vec2(points[i]), PhysicsHelper::tob2Vec2(points[i + 1])};
            auto shape = b2CreateSegmentShape(_body->getB2Body(), &def, &segment);
            AX_BREAK_IF(!b2Shape_IsValid(shape));

            addShape(shape);
        }

        AX_BREAK_IF(i < count - 1);

        setMaterial(material);

        return true;
    } while (false);

    return false;
}

Vec2 PhysicsColliderEdgeChain::getCenter()
{
    // REMARK: check the logic whether identical to
    // collect points then invoke b2CentroidForPoly
    b2Vec2 centroid{0.0f, 0.0f};
    float signedArea = 0.0f;
    int count        = static_cast<int>(_b2Shapes.size());

    for (int i = 0; i < count; ++i)
    {
        auto curSegment       = b2Shape_GetSegment(_b2Shapes[i]);
        auto nextSegment      = b2Shape_GetSegment(_b2Shapes[(i + 1) % count]);
        const b2Vec2& current = curSegment.point1;
        const b2Vec2& next    = i != (count - 1) ? nextSegment.point1 : nextSegment.point2;
        float crossProduct    = b2Cross(current, next);
        float triangleArea    = 0.5f * crossProduct;
        signedArea += triangleArea;
        centroid += triangleArea * (current + next) * (1 / 3.0f);
    }

    centroid *= (1 / signedArea);
    return PhysicsHelper::toVec2(centroid);
}

void PhysicsColliderEdgeChain::getPoints(Vec2* outPoints) const
{
    int i = 0;
    for (auto&& shape : _b2Shapes)
    {
        outPoints[i++] = PhysicsHelper::toVec2(b2Shape_GetSegment(shape).point1);
    }

    outPoints[i++] = PhysicsHelper::toVec2(b2Shape_GetSegment(_b2Shapes.back()).point2);
}

int PhysicsColliderEdgeChain::getPointsCount() const
{
    return static_cast<int>(_b2Shapes.size() + 1);
}

void PhysicsColliderEdgeChain::updateScale()
{
    auto factorX = _newScaleX / _scaleX;
    auto factorY = _newScaleY / _scaleY;

    for (auto&& shape : _b2Shapes)
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

    PhysicsCollider::updateScale();
}

void PhysicsCollider::setGroup(int group)
{
    if (group < 0)
    {
        const b2Filter filter{static_cast<uint64_t>(CollisionBits::All), static_cast<uint64_t>(CollisionBits::All),
                              group};
        for (auto&& shape : _b2Shapes)
        {

            b2Shape_SetFilter(shape, filter);
        }
    }

    _group = group;
}

bool PhysicsCollider::containsPoint(const Vec2& point) const
{
    auto b2pt = PhysicsHelper::tob2Vec2(point);
    for (auto&& shape : _b2Shapes)
    {
        if (b2Shape_TestPoint(shape, b2pt))
            return true;
    }

    return false;
}

}  // namespace ax

#endif  // defined(AX_ENABLE_PHYSICS)
