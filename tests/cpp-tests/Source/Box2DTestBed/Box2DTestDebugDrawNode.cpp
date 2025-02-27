#include "Box2DTestDebugDrawNode.h"
#include "VisibleRect.h"
#include "physics/PhysicsHelper.h"
#include "imgui.h"

using namespace ax;

Box2DTestDebugDrawNode* g_pDebugDrawNode;
GLFWwindow* g_mainWindow;
b2SampleCamera g_camera;

b2AABB b2SampleCamera::GetViewBounds()
{
    b2AABB bounds;
    bounds.lowerBound = ConvertScreenToWorld({0.0f, (float)m_height});
    bounds.upperBound = ConvertScreenToWorld({(float)m_width, 0.0f});
    return bounds;
}

b2Vec2 b2SampleCamera::ConvertScreenToWorld(b2Vec2 ps)
{
    float w = float(m_width);
    float h = float(m_height);
    float u = ps.x / w;
    float v = (h - ps.y) / h;

    float ratio    = w / h;
    b2Vec2 extents = {m_zoom * ratio, m_zoom};

    b2Vec2 lower = b2Sub(m_center, extents);
    b2Vec2 upper = b2Add(m_center, extents);

    b2Vec2 pw = {(1.0f - u) * lower.x + u * upper.x, (1.0f - v) * lower.y + v * upper.y};
    return pw;
}

b2Vec2 b2SampleCamera::ConvertWorldToScreen(b2Vec2 pw)
{
    float w     = float(m_width);
    float h     = float(m_height);
    float ratio = w / h;

    b2Vec2 extents = {m_zoom * ratio, m_zoom};

    b2Vec2 lower = b2Sub(m_center, extents);
    b2Vec2 upper = b2Add(m_center, extents);

    float u = (pw.x - lower.x) / (upper.x - lower.x);
    float v = (pw.y - lower.y) / (upper.y - lower.y);

    b2Vec2 ps = {u * w, (1.0f - v) * h};
    return ps;
}

static void b2DrawCircle(b2Vec2 center, float radius, b2HexColor color, Box2DTestDebugDrawNode* context)
{
    auto ratio  = context->getPTMRatio();
    auto offset = context->getWorldOffset();
    context->AddCircle(CircleData{b2Vec2{center.x * ratio + offset.x, center.y * ratio + offset.y}, radius * ratio,
                                  PhysicsHelper::toColor(color)});
}

static void b2DrawSolidCircle(b2Transform t, float radius, b2HexColor color, Box2DTestDebugDrawNode* context)
{
    // RGBA8 rgba = MakeRGBA8(color, 1.0f);
    // m_circles.push_back({{transform.p.x, transform.p.y, transform.q.c, transform.q.s}, radius, rgba});
    auto ratio  = context->getPTMRatio();
    auto offset = context->getWorldOffset();
    context->AddCircle({{t.p.x * ratio + offset.x, t.p.y * ratio + offset.y, t.q.c, t.q.s},
                        radius * ratio,
                        PhysicsHelper::toColor(color)});
}

static void b2DrawSolidCapsule(b2Vec2 pt1, b2Vec2 pt2, float radius, b2HexColor c, Box2DTestDebugDrawNode* context)
{
    auto ratio  = context->getPTMRatio();
    auto offset = context->getWorldOffset();
    Vec2 p1{pt1.x * ratio, pt1.y * ratio}, p2{pt2.x * ratio, pt2.y * ratio};

    Vec2 d       = (p2 - p1);
    float length = d.length();
    if (length < 0.001f)
    {
        printf("WARNING: sample app: capsule too short!\n");
        return;
    }

    b2Vec2 axis = {d.x / length, d.y / length};
    b2Transform transform;
    transform.p   = PhysicsHelper::tob2Vec2(0.5f * (p1 + p2));
    transform.q.c = axis.x;
    transform.q.s = axis.y;

    auto rgba = PhysicsHelper::toColor(c);

    context->AddCapsule({{transform.p.x + offset.x, transform.p.y + offset.y, transform.q.c, transform.q.s},
                         radius * ratio,
                         length,
                         rgba});
}

bool Box2DTestDebugDrawNode::initWithWorld(b2WorldId worldId)
{
    g_pDebugDrawNode = this;
    g_mainWindow     = static_cast<GLViewImpl*>(_director->getGLView())->getWindow();

    bool ret = ax::extension::PhysicsDebugNode::initWithWorld(worldId);

#define __b2_setfun(f) _debugDraw.f##Fcn = reinterpret_cast<decltype(_debugDraw.f##Fcn)>(b2##f);
    __b2_setfun(DrawCircle);
    __b2_setfun(DrawSolidCircle);
    __b2_setfun(DrawSolidCapsule);
#undef __b2_setfun

    // Demo messageString
    _textRender = Label::createWithTTF("TEST", "fonts/arial.ttf", 8.0f);
    _textRender->setAnchorPoint(Vec2(0, 1));
    _textRender->setPosition(VisibleRect::left().x, VisibleRect::top().y - 10);
    _textRender->setColor(Color3B::WHITE);
    this->addChild(_textRender, 99);

    /// circle shader
    {
        auto& cmd        = _customCommandCircle;
        auto& pipelinePS = cmd.getPipelineDescriptor().programState;
        AX_SAFE_RELEASE(pipelinePS);

        // vertex attributes
        auto program = backend::ProgramManager::getInstance()->loadProgram("custom/circle_vs", "custom/circle_fs");
        pipelinePS   = new backend::ProgramState(program);
        auto vfmt    = pipelinePS->getMutableVertexLayout();
        vfmt->setAttrib("a_localPosition", program->getAttributeLocation("a_localPosition"),
                        backend::VertexFormat::FLOAT2, 0, false);
        vfmt->setStride(sizeof(Vec2));
        cmd.createVertexBuffer(sizeof(Vec2), 6, CustomCommand::BufferUsage::STATIC);
        float a           = 1.1f;
        b2Vec2 vertices[] = {{-a, -a}, {a, -a}, {-a, a}, {a, -a}, {a, a}, {-a, a}};
        cmd.updateVertexBuffer(vertices, sizeof(vertices));
        cmd.setVertexDrawInfo(0, 6);

        // instanced attributes
        auto vfmtInstanced = pipelinePS->getMutableVertexLayout(true);
        vfmtInstanced->setAttrib("a_instanceTransform", program->getAttributeLocation("a_instancePosition"),
                                 backend::VertexFormat::FLOAT2, offsetof(CircleData, position), false);
        vfmtInstanced->setAttrib("a_instanceRadius", program->getAttributeLocation("a_instanceRadius"),
                                 backend::VertexFormat::FLOAT, offsetof(CircleData, radius), false);
        vfmtInstanced->setAttrib("a_instanceColor", program->getAttributeLocation("a_instanceColor"),
                                 backend::VertexFormat::FLOAT4, offsetof(CircleData, rgba), false);
        vfmtInstanced->setStride(sizeof(CircleData));

        cmd.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE);
        cmd.setDrawType(CustomCommand::DrawType::ARRAY_INSTANCED);
    }

    /// solid circle shader
    {
        auto& cmd        = _customCommandSolidCircle;
        auto& pipelinePS = cmd.getPipelineDescriptor().programState;
        AX_SAFE_RELEASE(pipelinePS);

        // vertex attributes
        auto program =
            backend::ProgramManager::getInstance()->loadProgram("custom/solid_circle_vs", "custom/solid_circle_fs");
        pipelinePS = new backend::ProgramState(program);
        auto vfmt  = pipelinePS->getMutableVertexLayout();
        vfmt->setAttrib("a_localPosition", program->getAttributeLocation("a_localPosition"),
                        backend::VertexFormat::FLOAT2, 0, false);
        vfmt->setStride(sizeof(Vec2));
        cmd.createVertexBuffer(sizeof(Vec2), 6, CustomCommand::BufferUsage::STATIC);
        float a           = 1.1f;
        b2Vec2 vertices[] = {{-a, -a}, {a, -a}, {-a, a}, {a, -a}, {a, a}, {-a, a}};
        cmd.updateVertexBuffer(vertices, sizeof(vertices));
        cmd.setVertexDrawInfo(0, 6);

        // instanced attributes
        auto vfmtInstanced = pipelinePS->getMutableVertexLayout(true);
        vfmtInstanced->setAttrib("a_instanceTransform", program->getAttributeLocation("a_instanceTransform"),
                                 backend::VertexFormat::FLOAT4, offsetof(SolidCircleData, transform), false);
        vfmtInstanced->setAttrib("a_instanceRadius", program->getAttributeLocation("a_instanceRadius"),
                                 backend::VertexFormat::FLOAT, offsetof(SolidCircleData, radius), false);
        vfmtInstanced->setAttrib("a_instanceColor", program->getAttributeLocation("a_instanceColor"),
                                 backend::VertexFormat::FLOAT4, offsetof(SolidCircleData, rgba), false);
        vfmtInstanced->setStride(sizeof(SolidCircleData));

        cmd.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE);
        cmd.setDrawType(CustomCommand::DrawType::ARRAY_INSTANCED);
    }

    /// solid capsule shader
    {
        auto& cmd        = _customCommandCapsule;
        auto& pipelinePS = cmd.getPipelineDescriptor().programState;
        AX_SAFE_RELEASE(pipelinePS);

        // vertex attributes
        auto program =
            backend::ProgramManager::getInstance()->loadProgram("custom/solid_capsule_vs", "custom/solid_capsule_fs");
        pipelinePS = new backend::ProgramState(program);
        auto vfmt  = pipelinePS->getMutableVertexLayout();
        vfmt->setAttrib("a_localPosition", program->getAttributeLocation("a_localPosition"),
                        backend::VertexFormat::FLOAT2, 0, false);
        vfmt->setStride(sizeof(Vec2));
        cmd.createVertexBuffer(sizeof(Vec2), 6, CustomCommand::BufferUsage::STATIC);
        float a           = 1.1f;
        b2Vec2 vertices[] = {{-a, -a}, {a, -a}, {-a, a}, {a, -a}, {a, a}, {-a, a}};
        cmd.updateVertexBuffer(vertices, sizeof(vertices));
        cmd.setVertexDrawInfo(0, 6);

        // instanced attributes
        auto vfmtInstanced = pipelinePS->getMutableVertexLayout(true);
        vfmtInstanced->setAttrib("a_instanceTransform", program->getAttributeLocation("a_instanceTransform"),
                                 backend::VertexFormat::FLOAT4, offsetof(CapsuleData, transform), false);
        vfmtInstanced->setAttrib("a_instanceRadius", program->getAttributeLocation("a_instanceRadius"),
                                 backend::VertexFormat::FLOAT, offsetof(CapsuleData, radius), false);
        vfmtInstanced->setAttrib("a_instanceLength", program->getAttributeLocation("a_instanceLength"),
                                 backend::VertexFormat::FLOAT, offsetof(CapsuleData, length), false);
        vfmtInstanced->setAttrib("a_instanceColor", program->getAttributeLocation("a_instanceColor"),
                                 backend::VertexFormat::FLOAT4, offsetof(CapsuleData, rgba), false);
        vfmtInstanced->setStride(sizeof(CapsuleData));

        cmd.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE);
        cmd.setDrawType(CustomCommand::DrawType::ARRAY_INSTANCED);
    }

    return ret;
}

void Box2DTestDebugDrawNode::DrawPolygon(const b2Vec2* vertices, int32_t vertexCount, b2HexColor color)
{
    _debugDraw.DrawPolygonFcn(vertices, vertexCount, color, this);
}

void Box2DTestDebugDrawNode::DrawSolidPolygon(b2Transform transform,
                                              const b2Vec2* vertices,
                                              int32_t vertexCount,
                                              float radius,
                                              b2HexColor color)
{
    _debugDraw.DrawSolidPolygonFcn(transform, vertices, vertexCount, radius, color, this);
}

void Box2DTestDebugDrawNode::DrawCircle(b2Vec2 center, float radius, b2HexColor color)
{
    _debugDraw.DrawCircleFcn(center, radius, color, this);
}
void Box2DTestDebugDrawNode::DrawSolidCircle(b2Transform transform, b2Vec2 center, float radius, b2HexColor color)
{
    _debugDraw.DrawSolidCircleFcn(transform, radius, color, this);
}

void Box2DTestDebugDrawNode::DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color)
{
    _debugDraw.DrawSolidCapsuleFcn(p1, p2, radius, color, this);
}

void Box2DTestDebugDrawNode::DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color)
{
    _debugDraw.DrawSegmentFcn(p1, p2, color, this);
}

void Box2DTestDebugDrawNode::DrawTransform(b2Transform transform)
{
    _debugDraw.DrawTransformFcn(transform, this);
}

void Box2DTestDebugDrawNode::DrawPoint(b2Vec2 p, float size, b2HexColor color)
{
    _debugDraw.DrawPointFcn(p, size, color, this);
}

void Box2DTestDebugDrawNode::DrawString(int x, int y, const char* pszFormat, ...)
{
    // va_list args;
    // va_start(args, pszFormat);
    // auto ret = StringUtils::vformat(pszFormat, args);
    // va_end(args);
    // 
    // _debugString.append(ret);
    // _debugString.push_back('\n');
    // _textRender->setString(_debugString);

    va_list arg;
    va_start(arg, pszFormat);
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoScrollbar);
    ImGui::PushFont(g_draw.m_regularFont);
    ImGui::SetCursorPos(ImVec2(float(x), float(y)));
    ImGui::TextColoredV(ImColor(230, 153, 153, 255), pszFormat, arg);
    ImGui::PopFont();
    ImGui::End();
    va_end(arg);
}

void Box2DTestDebugDrawNode::DrawString(b2Vec2 p, const char* pszFormat, ...)
{
    /*va_list args;
    va_start(args, pszFormat);
    auto ret = StringUtils::vformat(pszFormat, args);
    va_end(args);

    _debugString.append(ret);
    _debugString.push_back('\n');
    _textRender->setString(_debugString);*/

    b2Vec2 ps = g_camera.ConvertWorldToScreen(p);

    va_list arg;
    va_start(arg, pszFormat);
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoScrollbar);
    ImGui::SetCursorPos(ImVec2(ps.x, ps.y));
    ImGui::TextColoredV(ImColor(230, 230, 230, 255), pszFormat, arg);
    ImGui::End();
    va_end(arg);
}

void Box2DTestDebugDrawNode::AddCapsule(const CapsuleData& capsule)
{
    _capsules.push_back(capsule);
    _capsulesDirty = true;
}

void Box2DTestDebugDrawNode::AddCircle(const CircleData& circle)
{
    _circles.push_back(circle);
    _circlesDirty = true;
}

void Box2DTestDebugDrawNode::AddCircle(const SolidCircleData& circle)
{
    _solidCircles.push_back(circle);
    _solidCirclesDirty = true;
}

void Box2DTestDebugDrawNode::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    PhysicsDebugNode::draw(renderer, transform, flags);

    /// circle

    // update buffer
    if (_circlesDirty)
    {
        _circlesDirty = false;
        if (_circles.empty())
            _customCommandCircle.setInstanceBuffer(nullptr, 0);
        else
        {
            if (_customCommandCircle.getInstanceCapacity() < static_cast<int>(_circles.size()))
                _customCommandCircle.createInstanceBuffer(sizeof(CircleData), _circles.size(),
                                                          CustomCommand::BufferUsage::DYNAMIC);
            _customCommandCircle.updateInstanceBuffer(_circles.data(), _circles.size() * sizeof(CircleData));
            _customCommandCircle.setInstanceDrawInfo(static_cast<int>(_circles.size()));
        }
    }

    // submit draw command
    if (_customCommandCircle.getInstanceCount() > 0)
        submitDrawCommand(renderer, _customCommandCircle, transform);

    /// solid circle

    // update buffer
    if (_solidCirclesDirty)
    {
        _solidCirclesDirty = false;
        if (_solidCircles.empty())
            _customCommandSolidCircle.setInstanceBuffer(nullptr, 0);
        else
        {
            if (_customCommandSolidCircle.getInstanceCapacity() < static_cast<int>(_solidCircles.size()))
                _customCommandSolidCircle.createInstanceBuffer(sizeof(SolidCircleData), _solidCircles.size(),
                                                               CustomCommand::BufferUsage::DYNAMIC);
            _customCommandSolidCircle.updateInstanceBuffer(_solidCircles.data(),
                                                           _solidCircles.size() * sizeof(SolidCircleData));
            _customCommandSolidCircle.setInstanceDrawInfo(static_cast<int>(_solidCircles.size()));
        }
    }

    // submit draw command
    if (_customCommandSolidCircle.getInstanceCount() > 0)
        submitDrawCommand(renderer, _customCommandSolidCircle, transform);

    /// capsule

    // update buffer
    if (_capsulesDirty)
    {
        _capsulesDirty = false;
        if (_capsules.empty())
            _customCommandCapsule.setInstanceBuffer(nullptr, 0);
        else
        {
            if (_customCommandCapsule.getInstanceCapacity() < static_cast<int>(_capsules.size()))
                _customCommandCapsule.createInstanceBuffer(sizeof(CapsuleData), _capsules.size(),
                                                           CustomCommand::BufferUsage::DYNAMIC);
            _customCommandCapsule.updateInstanceBuffer(_capsules.data(), _capsules.size() * sizeof(CapsuleData));
            _customCommandCapsule.setInstanceDrawInfo(static_cast<int>(_capsules.size()));
        }
    }

    // submit draw command
    if (_customCommandCapsule.getInstanceCount() > 0)
        submitDrawCommand(renderer, _customCommandCapsule, transform);
}

void Box2DTestDebugDrawNode::submitDrawCommand(Renderer* renderer, CustomCommand& cmd, const Mat4& transform)
{
    backend::BlendDescriptor& blendDescriptor   = cmd.getPipelineDescriptor().blendDescriptor;
    blendDescriptor.blendEnabled                = true;
    blendDescriptor.sourceRGBBlendFactor        = backend::BlendFactor::SRC_ALPHA;
    blendDescriptor.destinationRGBBlendFactor   = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
    blendDescriptor.sourceAlphaBlendFactor      = backend::BlendFactor::SRC_ALPHA;
    blendDescriptor.destinationAlphaBlendFactor = backend::BlendFactor::ONE_MINUS_SRC_ALPHA;

    auto& pipelineDescriptor = cmd.getPipelineDescriptor();
    const auto& matrixP      = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    Mat4 matrixMVP           = matrixP * transform;
    auto mvpLocation         = pipelineDescriptor.programState->getUniformLocation("u_MVPMatrix");
    pipelineDescriptor.programState->setUniform(mvpLocation, matrixMVP.m, sizeof(matrixMVP.m));

    auto viewHeight      = _director->getGLView()->getDesignResolutionSize().height;
    constexpr float zoom = 25.0 * 2.35f;
    float pixelScale     = viewHeight / zoom;
    auto uniformLocation = pipelineDescriptor.programState->getUniformLocation("u_pixelScale");
    pipelineDescriptor.programState->setUniform(uniformLocation, &pixelScale, sizeof(pixelScale));

    cmd.init(_globalZOrder);
    renderer->addCommand(&cmd);
}

void Box2DTestDebugDrawNode::clear()
{
    ax::extension::PhysicsDebugNode::clear();

    _circles.clear();
    _circlesDirty = true;

    _solidCircles.clear();
    _solidCirclesDirty = true;

    _capsules.clear();
    _capsulesDirty = true;

    _debugString.clear();
}

void Box2DTestDebugDrawNode::DrawAABB(b2AABB aabb, b2HexColor color)
{
    this->drawRect(Vec2{aabb.lowerBound.x, aabb.lowerBound.y}, Vec2{aabb.upperBound.x, aabb.upperBound.y},
                   PhysicsHelper::toColor(color));
}
