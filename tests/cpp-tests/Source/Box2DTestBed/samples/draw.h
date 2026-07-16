// axmol spec box2d draw implemention
#pragma once

#include "axmol/platform/RenderView.h"
#include "physics-nodes/PhysicsDebugNode.h"

using namespace ax;

struct ImFont;

// renamed: Camera -> SampleCamera avoid ambiguous symbol
struct SampleCamera
{
	b2Vec2 center;
	float zoom;
	float width;
	float height;
};

SampleCamera GetDefaultCamera(void);
void ResetView(SampleCamera* camera);
b2Vec2 ConvertScreenToWorld(SampleCamera* camera, b2Vec2 screenPoint);
b2Vec2 ConvertWorldToScreen(SampleCamera* camera, b2Vec2 worldPoint);
b2AABB GetViewBounds(SampleCamera* camera);

struct CircleData
{
    ax::Color rgba;
    b2Vec2 position;
    float radius;
    float padding;
};

struct SolidCircleData
{
    b2Transform transform;
    ax::Color rgba;
    float radius;
    float padding[3];
};

struct CapsuleData
{
    b2Transform transform;
    ax::Color rgba;
    float radius;
    float length;
    float padding[2];
};

class SampleDrawNode;
struct SampleContext;
struct SampleDraw
{
    SampleContext* context{nullptr};  // weak ref
    SampleDrawNode* impl{nullptr}; // weak ref
};

void DrawPoint(SampleDraw* draw, b2Vec2 p, float size, b2HexColor color);
void DrawLine(SampleDraw* draw, b2Vec2 p1, b2Vec2 p2, b2HexColor color);
void DrawCircle(SampleDraw* draw, b2Vec2 center, float radius, b2HexColor color);
void DrawSolidCircle(SampleDraw* draw, b2Transform transform, float radius, b2HexColor color);
void DrawSolidCapsule(SampleDraw* draw, b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color);
void DrawPolygon(SampleDraw* SampleDraw, const b2Vec2* vertices, int vertexCount, b2HexColor color);
void DrawSolidPolygon(SampleDraw* draw,
                      b2Transform transform,
                      const b2Vec2* vertices,
                      int vertexCount,
                      float radius,
                      b2HexColor color);
void DrawTransform(SampleDraw* draw, b2Transform transform, float scale);
void DrawBounds(SampleDraw* draw, b2AABB aabb, b2HexColor color);
void DrawScreenString(SampleDraw* draw, float x, float y, b2HexColor color, const char* string, ...);
void DrawWorldString(SampleDraw* draw, SampleCamera* camera, b2Vec2 p, b2HexColor color, const char* string, ...);

//void FlushDraw(SampleDraw* draw, SampleCamera* camera);

//void DrawBackground(SampleDraw* draw, SampleCamera* camera);

class SampleDrawNode : public ax::extension::PhysicsDebugNode
{
public:
    SampleDrawNode(b2DebugDraw* debugDraw) : ax::extension::PhysicsDebugNode(debugDraw)
    {
    }
    ~SampleDrawNode();
    bool initWithWorld(b2WorldId worldId) override;

    void AddCircle(const CircleData& circle);
    void AddCircle(const SolidCircleData& circle);
    void AddCapsule(const CapsuleData& capsule);

    void submitDrawCommand(Renderer* renderer, CustomCommand& cmd, const Mat4& transform);

    void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;
    void clear() override;

    CustomCommand _customCommandCircle;
    CustomCommand _customCommandSolidCircle;
    CustomCommand _customCommandCapsule;
    tlx::pod_vector<CircleData> _circles;
    tlx::pod_vector<SolidCircleData> _solidCircles;
    tlx::pod_vector<CapsuleData> _capsules;
    bool _circlesDirty{true};
    bool _solidCirclesDirty{true};
    bool _capsulesDirty{true};
};
