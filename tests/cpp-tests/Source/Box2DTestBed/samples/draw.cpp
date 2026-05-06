#include "./draw.h"
#include "VisibleRect.h"
#include "axmol/physics/2d/PhysicsUtility2D.h"
#include "imgui.h"
#include "sample.h"

using namespace ax;

#define BUFFER_OFFSET(x) ((const void*)(x))

#define SHADER_TEXT(x)   "#version 330\n" #x

struct RGBA8
{
    uint8_t r, g, b, a;
};

static RGBA8 MakeRGBA8(b2HexColor c, float alpha)
{
    return {uint8_t((c >> 16) & 0xFF), uint8_t((c >> 8) & 0xFF), uint8_t(c & 0xFF), uint8_t(0xFF * alpha)};
}

#pragma region SampleCamera
SampleCamera GetDefaultCamera(void)
{
    return SampleCamera{
        .center = {0.0f, 20.0f},
        .zoom   = 1.0f,
        .width  = 1920.0f,
        .height = 1080.0f,
    };
}

void ResetView(SampleCamera* camera)
{
    camera->center = b2Vec2{0.0f, 20.0f};
    camera->zoom   = 1.0f;
}

b2Vec2 ConvertScreenToWorld(SampleCamera* camera, b2Vec2 screenPoint)
{
    float w = camera->width;
    float h = camera->height;
    float u = screenPoint.x / w;
    float v = (h - screenPoint.y) / h;

    float ratio    = w / h;
    b2Vec2 extents = {camera->zoom * ratio, camera->zoom};

    b2Vec2 lower = b2Sub(camera->center, extents);
    b2Vec2 upper = b2Add(camera->center, extents);

    b2Vec2 pw = {(1.0f - u) * lower.x + u * upper.x, (1.0f - v) * lower.y + v * upper.y};
    return pw;
}

b2Vec2 ConvertWorldToScreen(SampleCamera* camera, b2Vec2 worldPoint)
{
    float w     = camera->width;
    float h     = camera->height;
    float ratio = w / h;

    b2Vec2 extents = {camera->zoom * ratio, camera->zoom};

    b2Vec2 lower = b2Sub(camera->center, extents);
    b2Vec2 upper = b2Add(camera->center, extents);

    float u = (worldPoint.x - lower.x) / (upper.x - lower.x);
    float v = (worldPoint.y - lower.y) / (upper.y - lower.y);

    b2Vec2 ps = {u * w, (1.0f - v) * h};
    return ps;
}

// Convert from world coordinates to normalized device coordinates.
// http://www.songho.ca/opengl/gl_projectionmatrix.html
// This also includes the view transform
static void BuildProjectionMatrix(SampleCamera* camera, float* m, float zBias)
{
    float ratio    = camera->width / camera->height;
    b2Vec2 extents = {camera->zoom * ratio, camera->zoom};

    b2Vec2 lower = b2Sub(camera->center, extents);
    b2Vec2 upper = b2Add(camera->center, extents);
    float w      = upper.x - lower.x;
    float h      = upper.y - lower.y;

    m[0] = 2.0f / w;
    m[1] = 0.0f;
    m[2] = 0.0f;
    m[3] = 0.0f;

    m[4] = 0.0f;
    m[5] = 2.0f / h;
    m[6] = 0.0f;
    m[7] = 0.0f;

    m[8]  = 0.0f;
    m[9]  = 0.0f;
    m[10] = -1.0f;
    m[11] = 0.0f;

    m[12] = -2.0f * camera->center.x / w;
    m[13] = -2.0f * camera->center.y / h;
    m[14] = zBias;
    m[15] = 1.0f;
}

b2AABB GetViewBounds(SampleCamera* camera)
{
    if (camera->height == 0.0f || camera->width == 0.0f)
    {
        b2AABB bounds = {.lowerBound = b2Vec2_zero, .upperBound = b2Vec2_zero};
        return bounds;
    }

    b2AABB bounds;
    bounds.lowerBound = ConvertScreenToWorld(camera, b2Vec2{0.0f, camera->height});
    bounds.upperBound = ConvertScreenToWorld(camera, b2Vec2{camera->width, 0.0f});
    return bounds;
}

#pragma endregion

#pragma region SampleDraw
void DrawPoint(SampleDraw* draw, b2Vec2 p, float size, b2HexColor color)
{
    draw->impl->getB2DebugDraw().DrawPointFcn(p, size, color, draw->impl);
}

void DrawLine(SampleDraw* draw, b2Vec2 p1, b2Vec2 p2, b2HexColor color)
{
    draw->impl->getB2DebugDraw().DrawLineFcn(p1, p2, color, draw->impl);
}

void DrawCircle(SampleDraw* draw, b2Vec2 center, float radius, b2HexColor color)
{
    draw->impl->getB2DebugDraw().DrawCircleFcn(center, radius, color, draw->impl);
}

void DrawSolidCircle(SampleDraw* draw, b2Transform transform, float radius, b2HexColor color)
{
    draw->impl->getB2DebugDraw().DrawSolidCircleFcn(transform, radius, color, draw->impl);
}

void DrawSolidCapsule(SampleDraw* draw, b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color)
{
    draw->impl->getB2DebugDraw().DrawSolidCapsuleFcn(p1, p2, radius, color, draw->impl);
}

void DrawPolygon(SampleDraw* draw, const b2Vec2* vertices, int vertexCount, b2HexColor color)
{
    draw->impl->getB2DebugDraw().DrawPolygonFcn(vertices, vertexCount, color, draw->impl);
}

void DrawSolidPolygon(SampleDraw* draw,
                      b2Transform transform,
                      const b2Vec2* vertices,
                      int vertexCount,
                      float radius,
                      b2HexColor color)
{
    draw->impl->getB2DebugDraw().DrawSolidPolygonFcn(transform, vertices, vertexCount, radius, color, draw->impl);
}

void DrawTransform(SampleDraw* draw, b2Transform transform, float scale)
{
    draw->impl->getB2DebugDraw().DrawTransformFcn(transform, draw->impl);
}

void DrawBounds(SampleDraw* draw, b2AABB aabb, b2HexColor color)
{
    auto drawImpl   = draw->impl;
    auto& debugDraw = drawImpl->getB2DebugDraw();
    b2Vec2 p1       = aabb.lowerBound;
    b2Vec2 p2       = {aabb.upperBound.x, aabb.lowerBound.y};
    b2Vec2 p3       = aabb.upperBound;
    b2Vec2 p4       = {aabb.lowerBound.x, aabb.upperBound.y};

    debugDraw.DrawLineFcn(p1, p2, color, drawImpl);
    debugDraw.DrawLineFcn(p2, p3, color, drawImpl);
    debugDraw.DrawLineFcn(p3, p4, color, drawImpl);
    debugDraw.DrawLineFcn(p4, p1, color, drawImpl);
}

void DrawScreenString(SampleDraw* draw, float x, float y, b2HexColor color, const char* string, ...)
{
    auto rgbaColor = b2util::cast(color);

    va_list arg;
    va_start(arg, string);
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoScrollbar);
    ImGui::PushFont(draw->context->regularFont, ImGui::GetStyle().FontSizeBase);
    ImGui::SetCursorPos(ImVec2(float(x), float(y)));
    ImGui::TextColoredV(ImColor(rgbaColor.r, rgbaColor.g, rgbaColor.b, rgbaColor.a), string, arg);
    ImGui::PopFont();
    ImGui::End();
    va_end(arg);
}

void DrawWorldString(SampleDraw* draw, SampleCamera* camera, b2Vec2 p, b2HexColor color, const char* string, ...)
{
    b2Vec2 ps = ConvertWorldToScreen(camera, p);

    va_list arg;
    va_start(arg, string);
    ImGui::Begin("Overlay", nullptr,
                 ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
                     ImGuiWindowFlags_NoScrollbar);
    ImGui::SetCursorPos(ImVec2(ps.x, ps.y));
    auto rgbaColor = b2util::cast(color);
    ImGui::TextColoredV(ImColor(rgbaColor.r, rgbaColor.g, rgbaColor.b, rgbaColor.a), string, arg);
    ImGui::End();
    va_end(arg);
}

// No need in axmol
// void FlushDraw(SampleDraw* draw, Camera* camera)
//{
//    // order matters
//    FlushSolidCircles(&draw->circles, camera);
//    FlushCapsules(&draw->capsules, camera);
//    FlushPolygons(&draw->polygons, camera);
//    FlushCircles(&draw->hollowCircles, camera);
//    FlushLines(&draw->lines, camera);
//    FlushPoints(&draw->points, camera);
//    FlushText(&draw->font, camera);
//    CheckOpenGL();
//}

void DrawBackground(SampleDraw* draw, Camera* camera)
{
    // RenderBackground(&draw->background, camera);
}

#pragma endregion

#pragma region SampleDrawNode
static void b2DrawCircle(b2Vec2 center, float radius, b2HexColor color, SampleDrawNode* context)
{
    auto ratio  = context->getPTMRatio();
    auto offset = context->getWorldOffset();
    context->AddCircle(CircleData{b2util::cast(color),
                                  b2Vec2{center.x * ratio + offset.x, center.y * ratio + offset.y}, radius * ratio});
}

static void b2DrawSolidCircle(b2Transform t, float radius, b2HexColor color, SampleDrawNode* context)
{
    // RGBA8 rgba = MakeRGBA8(color, 1.0f);
    // m_circles.push_back({{transform.p.x, transform.p.y, transform.q.c, transform.q.s}, radius, rgba});
    auto ratio  = context->getPTMRatio();
    auto offset = context->getWorldOffset();
    context->AddCircle({{t.p.x * ratio + offset.x, t.p.y * ratio + offset.y, t.q.c, t.q.s}, b2util::cast(color),
                        radius * ratio});
}

static void b2DrawSolidCapsule(b2Vec2 pt1, b2Vec2 pt2, float radius, b2HexColor c, SampleDrawNode* context)
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
    transform.p   = b2util::cast(0.5f * (p1 + p2));
    transform.q.c = axis.x;
    transform.q.s = axis.y;

    auto rgba = b2util::cast(c);

    context->AddCapsule({{transform.p.x + offset.x, transform.p.y + offset.y, transform.q.c, transform.q.s},
                         rgba,
                         radius * ratio,
                         length});
}

SampleDrawNode::~SampleDrawNode()
{
    _customCommandCircle.releasePSVL();
    _customCommandSolidCircle.releasePSVL();
    _customCommandCapsule.releasePSVL();
}

bool SampleDrawNode::initWithWorld(b2WorldId worldId)
{
    bool ret = ax::extension::PhysicsDebugNode::initWithWorld(worldId);

#define __b2_setfun(f) _debugDraw->f##Fcn = reinterpret_cast<decltype(_debugDraw->f##Fcn)>(b2##f);
    __b2_setfun(DrawCircle);
    __b2_setfun(DrawSolidCircle);
    __b2_setfun(DrawSolidCapsule);
#undef __b2_setfun

    /// circle shader
    {
        auto& cmd = _customCommandCircle;

        // vertex attributes
        auto program    = axpm->loadProgram("custom/circle_vs", "custom/circle_fs");
        auto pipelinePS = new rhi::ProgramState(program);
        auto vfmt       = axvlm->allocateVertexLayoutDesc();
        vfmt.startLayout(3);
        vfmt.addAttrib("a_localPosition", program->getVertexInputDesc("a_localPosition"), rhi::VertexFormat::FLOAT2, 0,
                       false);
        cmd.createVertexBuffer(sizeof(Vec2), 6, CustomCommand::BufferUsage::STATIC);
        float a           = 1.1f;
        b2Vec2 vertices[] = {{-a, -a}, {a, -a}, {-a, a}, {a, -a}, {a, a}, {-a, a}};
        cmd.updateVertexBuffer(vertices, sizeof(vertices));
        cmd.setVertexDrawInfo(0, 6);

        // instanced attributes
        vfmt.addAttrib("a_instanceColor", program->getVertexInputDesc("a_instanceColor"), rhi::VertexFormat::FLOAT4,
                       offsetof(CircleData, rgba), false, 1);
        vfmt.addAttrib("a_instancePosAndRadius", program->getVertexInputDesc("a_instancePosAndRadius"),
                       rhi::VertexFormat::FLOAT4, offsetof(CircleData, position), false, 1);
        vfmt.endLayout();

        auto pipelineVL = axvlm->getVertexLayout(std::move(vfmt));

        cmd.setOwnPSVL(pipelinePS, pipelineVL, RenderCommand::ADOPT_FLAG_PS);

        cmd.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE);
        cmd.setDrawType(CustomCommand::DrawType::ARRAY_INSTANCED);
    }

    /// solid circle shader
    {
        auto& cmd = _customCommandSolidCircle;

        // vertex attributes
        auto program    = axpm->loadProgram("custom/solid_circle_vs", "custom/solid_circle_fs");
        auto pipelinePS = new rhi::ProgramState(program);
        auto vfmt       = axvlm->allocateVertexLayoutDesc();
        vfmt.startLayout(4);
        vfmt.addAttrib("a_localPosition", program->getVertexInputDesc("a_localPosition"), rhi::VertexFormat::FLOAT2, 0,
                       false);

        cmd.createVertexBuffer(sizeof(Vec2), 6, CustomCommand::BufferUsage::STATIC);
        float a           = 1.1f;
        b2Vec2 vertices[] = {{-a, -a}, {a, -a}, {-a, a}, {a, -a}, {a, a}, {-a, a}};
        cmd.updateVertexBuffer(vertices, sizeof(vertices));
        cmd.setVertexDrawInfo(0, 6);

        // instanced attributes
        vfmt.addAttrib("a_instanceTransform", program->getVertexInputDesc("a_instanceTransform"),
                       rhi::VertexFormat::FLOAT4, offsetof(SolidCircleData, transform), false, 1);
        vfmt.addAttrib("a_instanceColor", program->getVertexInputDesc("a_instanceColor"), rhi::VertexFormat::FLOAT4,
                       offsetof(SolidCircleData, rgba), false, 1);
        vfmt.addAttrib("a_instanceRadius", program->getVertexInputDesc("a_instanceRadius"), rhi::VertexFormat::FLOAT4,
                       offsetof(SolidCircleData, radius), false, 1);
        vfmt.endLayout();

        auto pipelineVL = axvlm->getVertexLayout(std::move(vfmt));
        cmd.setOwnPSVL(pipelinePS, pipelineVL, RenderCommand::ADOPT_FLAG_PS);

        cmd.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE);
        cmd.setDrawType(CustomCommand::DrawType::ARRAY_INSTANCED);
    }

    /// solid capsule shader
    {
        auto& cmd = _customCommandCapsule;

        // vertex attributes
        auto program    = axpm->loadProgram("custom/solid_capsule_vs", "custom/solid_capsule_fs");
        auto pipelinePS = new rhi::ProgramState(program);
        auto vfmt       = axvlm->allocateVertexLayoutDesc();
        vfmt.startLayout(4);
        vfmt.addAttrib("a_localPosition", program->getVertexInputDesc("a_localPosition"), rhi::VertexFormat::FLOAT2, 0,
                       false);

        cmd.createVertexBuffer(sizeof(Vec2), 6, CustomCommand::BufferUsage::STATIC);
        float a           = 1.1f;
        b2Vec2 vertices[] = {{-a, -a}, {a, -a}, {-a, a}, {a, -a}, {a, a}, {-a, a}};
        cmd.updateVertexBuffer(vertices, sizeof(vertices));
        cmd.setVertexDrawInfo(0, 6);

        // instanced attributes
        vfmt.addAttrib("a_instanceTransform", program->getVertexInputDesc("a_instanceTransform"),
                       rhi::VertexFormat::FLOAT4, offsetof(CapsuleData, transform), false, 1);
        vfmt.addAttrib("a_instanceColor", program->getVertexInputDesc("a_instanceColor"), rhi::VertexFormat::FLOAT4,
                       offsetof(CapsuleData, rgba), false, 1);
        vfmt.addAttrib("a_instanceRadiusAndLength", program->getVertexInputDesc("a_instanceRadiusAndLength"),
                       rhi::VertexFormat::FLOAT4, offsetof(CapsuleData, radius), false, 1);
        vfmt.endLayout();

        auto pipelineVL = axvlm->getVertexLayout(std::move(vfmt));

        cmd.setOwnPSVL(pipelinePS, pipelineVL, RenderCommand::ADOPT_FLAG_PS);
        cmd.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE);
        cmd.setDrawType(CustomCommand::DrawType::ARRAY_INSTANCED);
    }

    return ret;
}

void SampleDrawNode::AddCapsule(const CapsuleData& capsule)
{
    _capsules.push_back(capsule);
    _capsulesDirty = true;
}

void SampleDrawNode::AddCircle(const CircleData& circle)
{
    _circles.push_back(circle);
    _circlesDirty = true;
}

void SampleDrawNode::AddCircle(const SolidCircleData& circle)
{
    _solidCircles.push_back(circle);
    _solidCirclesDirty = true;
}

void SampleDrawNode::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
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

void SampleDrawNode::submitDrawCommand(Renderer* renderer, CustomCommand& cmd, const Mat4& transform)
{
    rhi::BlendDesc& blendDescriptor             = cmd.blendDesc();
    blendDescriptor.blendEnabled                = true;
    blendDescriptor.sourceRGBBlendFactor        = rhi::BlendFactor::SRC_ALPHA;
    blendDescriptor.destinationRGBBlendFactor   = rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
    blendDescriptor.sourceAlphaBlendFactor      = rhi::BlendFactor::SRC_ALPHA;
    blendDescriptor.destinationAlphaBlendFactor = rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;

    auto pipelinePS     = cmd.unsafePS();
    const auto& matrixP = _director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
    Mat4 matrixMVP      = matrixP * transform;
    auto mvpLocation    = pipelinePS->getUniformLocation("u_MVPMatrix");
    pipelinePS->setUniform(mvpLocation, matrixMVP.m, sizeof(matrixMVP.m));

    auto viewHeight      = _director->getRenderView()->getDesignResolutionSize().height;
    constexpr float zoom = 25.0 * 2.35f;
    float pixelScale     = viewHeight / zoom;
    auto uniformLocation = pipelinePS->getUniformLocation("u_pixelScale");
    pipelinePS->setUniform(uniformLocation, &pixelScale, sizeof(pixelScale));

    cmd.init(_globalZOrder);
    renderer->addCommand(&cmd);
}

void SampleDrawNode::clear()
{
    ax::extension::PhysicsDebugNode::clear();

    _circles.clear();
    _circlesDirty = true;

    _solidCircles.clear();
    _solidCirclesDirty = true;

    _capsules.clear();
    _capsulesDirty = true;
}

#pragma endregion
