#pragma once

#include "platform/GLViewImpl.h"
#include "physics-nodes/PhysicsDebugNode.h"

using namespace ax;

struct CircleData
{
    b2Vec2 position;
    float radius;
    ax::Color4F rgba;
};

struct SolidCircleData
{
    b2Transform transform;
    float radius;
    ax::Color4F rgba;
};

struct CapsuleData
{
    b2Transform transform;
    float radius;
    float length;
    ax::Color4F rgba;
};

class Box2DTestDebugDrawNode : public ax::extension::PhysicsDebugNode
{
public:
    Box2DTestDebugDrawNode() : m_debugDraw(_debugDraw) {}
    ~Box2DTestDebugDrawNode()
    {

    }
    bool initWithWorld(b2WorldId worldId) override;

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

    void AddCircle(const CircleData& circle);
    void AddCircle(const SolidCircleData& circle);
    void AddCapsule(const CapsuleData& capsule);
    
    void submitDrawCommand(Renderer* renderer, CustomCommand& cmd, const Mat4& transform);

    void draw(Renderer* renderer, const Mat4& transform, uint32_t flags);
    void clear() override;

    b2DebugDraw& m_debugDraw;
    bool m_showUI{true};

    ax::Label* _textRender{nullptr};
    std::string _debugString;

    CustomCommand _customCommandCircle;
    CustomCommand _customCommandSolidCircle;
    CustomCommand _customCommandCapsule;
    axstd::pod_vector<CircleData> _circles;
    axstd::pod_vector<SolidCircleData> _solidCircles;
    axstd::pod_vector<CapsuleData> _capsules;
    bool _circlesDirty{true};
    bool _solidCirclesDirty{true};
    bool _capsulesDirty{true};
};

extern Box2DTestDebugDrawNode* g_pDebugDrawNode;
#define g_draw (*g_pDebugDrawNode)

extern GLFWwindow* g_mainWindow;

struct b2SampleCamera
{
    b2Vec2 ConvertScreenToWorld(b2Vec2 ps);
    b2AABB GetViewBounds();
    b2Vec2 m_center{0.0f, 0.0f};
    float m_zoom{1.0f};
    float m_width{480};
    float m_height{320};
};

extern b2SampleCamera g_camera;

