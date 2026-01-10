// axmol spec box2d draw implemention
#pragma once

#include "axmol/platform/RenderViewImpl.h"
#include "physics-nodes/PhysicsDebugNode.h"

using namespace ax;

struct ImFont;

// renamed: Camera -> SampleCamera avoid ambiguous symbol
struct SampleCamera
{
    SampleCamera();

    void ResetView();
    b2Vec2 ConvertScreenToWorld(b2Vec2 screenPoint);
    b2Vec2 ConvertWorldToScreen(b2Vec2 worldPoint);
    void BuildProjectionMatrix(float* m, float zBias);
    b2AABB GetViewBounds();

    b2Vec2 m_center;
    float m_zoom;
    int m_width;
    int m_height;
};

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
struct SampleDraw
{
    b2DebugDraw m_debugDraw{};
    ImFont* m_regularFont{nullptr};
    bool m_showUI{true};
    SampleDrawNode* m_context{nullptr};

    void DrawPolygon(const b2Vec2* vertices, int32_t vertexCount, b2HexColor color);
    void DrawSolidPolygon(b2Transform transform,
                          const b2Vec2* vertices,
                          int32_t vertexCount,
                          float radius,
                          b2HexColor color);

    void DrawCircle(b2Vec2 center, float radius, b2HexColor color);
    void DrawSolidCircle(b2Transform transform, b2Vec2 center, float radius, b2HexColor color);

    void DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color);

    void DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color);

    void DrawTransform(b2Transform transform);

    void DrawPoint(b2Vec2 p, float size, b2HexColor color);

    void DrawString(int x, int y, const char* string, ...);

    void DrawString(b2Vec2 p, const char* string, ...);

    void DrawAABB(b2AABB aabb, b2HexColor color);
};

class SampleDrawNode : public ax::extension::PhysicsDebugNode
{
public:
    SampleDrawNode(SampleDraw* draw) : ax::extension::PhysicsDebugNode(&draw->m_debugDraw) { draw->m_context = this; }
    ~SampleDrawNode();
    bool initWithWorld(b2WorldId worldId) override;

    void AddCircle(const CircleData& circle);
    void AddCircle(const SolidCircleData& circle);
    void AddCapsule(const CapsuleData& capsule);

    void submitDrawCommand(Renderer* renderer, CustomCommand& cmd, const Mat4& transform);

    void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;
    void clear() override;

    bool m_showUI{true};

    ax::Label* _textRender{nullptr};
    std::string _debugString;

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
