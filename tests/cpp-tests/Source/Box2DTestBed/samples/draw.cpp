#include "./draw.h"
#include "VisibleRect.h"
#include "axmol/physics/PhysicsHelper.h"
#include "imgui.h"

using namespace ax;

#define BUFFER_OFFSET( x ) ( (const void*)( x ) )

#define SHADER_TEXT( x ) "#version 330\n" #x

struct RGBA8
{
	uint8_t r, g, b, a;
};

static RGBA8 MakeRGBA8( b2HexColor c, float alpha )
{
	return { uint8_t( ( c >> 16 ) & 0xFF ), uint8_t( ( c >> 8 ) & 0xFF ), uint8_t( c & 0xFF ), uint8_t( 0xFF * alpha ) };
}

SampleCamera::SampleCamera()
{
	m_width = 1920;
	m_height = 1080;
	ResetView();
}

void SampleCamera::ResetView()
{
	m_center = { 0.0f, 20.0f };
	m_zoom = 1.0f;
}

b2Vec2 SampleCamera::ConvertScreenToWorld(b2Vec2 ps)
{
	float w = float( m_width );
	float h = float( m_height );
	float u = ps.x / w;
	float v = ( h - ps.y ) / h;

	float ratio = w / h;
	b2Vec2 extents = { m_zoom * ratio, m_zoom };

	b2Vec2 lower = b2Sub( m_center, extents );
	b2Vec2 upper = b2Add( m_center, extents );

	b2Vec2 pw = { ( 1.0f - u ) * lower.x + u * upper.x, ( 1.0f - v ) * lower.y + v * upper.y };
	return pw;
}

b2Vec2 SampleCamera::ConvertWorldToScreen(b2Vec2 pw)
{
	float w = float( m_width );
	float h = float( m_height );
	float ratio = w / h;

	b2Vec2 extents = { m_zoom * ratio, m_zoom };

	b2Vec2 lower = b2Sub( m_center, extents );
	b2Vec2 upper = b2Add( m_center, extents );

	float u = ( pw.x - lower.x ) / ( upper.x - lower.x );
	float v = ( pw.y - lower.y ) / ( upper.y - lower.y );

	b2Vec2 ps = { u * w, ( 1.0f - v ) * h };
	return ps;
}

// Convert from world coordinates to normalized device coordinates.
// http://www.songho.ca/opengl/gl_projectionmatrix.html
// This also includes the view transform
void SampleCamera::BuildProjectionMatrix(float* m, float zBias)
{
	float ratio = float( m_width ) / float( m_height );
	b2Vec2 extents = { m_zoom * ratio, m_zoom };

	b2Vec2 lower = b2Sub( m_center, extents );
	b2Vec2 upper = b2Add( m_center, extents );
	float w = upper.x - lower.x;
	float h = upper.y - lower.y;

	m[0] = 2.0f / w;
	m[1] = 0.0f;
	m[2] = 0.0f;
	m[3] = 0.0f;

	m[4] = 0.0f;
	m[5] = 2.0f / h;
	m[6] = 0.0f;
	m[7] = 0.0f;

	m[8] = 0.0f;
	m[9] = 0.0f;
	m[10] = -1.0f;
	m[11] = 0.0f;

	m[12] = -2.0f * m_center.x / w;
	m[13] = -2.0f * m_center.y / h;
	m[14] = zBias;
	m[15] = 1.0f;
}

b2AABB SampleCamera::GetViewBounds()
{
	b2AABB bounds;
	bounds.lowerBound = ConvertScreenToWorld( { 0.0f, (float)m_height } );
	bounds.upperBound = ConvertScreenToWorld( { (float)m_width, 0.0f } );
	return bounds;
}


#pragma region SampleDraw
void SampleDraw::DrawPolygon(const b2Vec2* vertices, int32_t vertexCount, b2HexColor color)
{
    m_debugDraw.DrawPolygonFcn(vertices, vertexCount, color, m_context);
}

void SampleDraw::DrawSolidPolygon(b2Transform transform,
                                  const b2Vec2* vertices,
                                  int32_t vertexCount,
                                  float radius,
                                  b2HexColor color)
{
    m_debugDraw.DrawSolidPolygonFcn(transform, vertices, vertexCount, radius, color, m_context);
}

void SampleDraw::DrawCircle(b2Vec2 center, float radius, b2HexColor color)
{
    m_debugDraw.DrawCircleFcn(center, radius, color, m_context);
}
void SampleDraw::DrawSolidCircle(b2Transform transform, b2Vec2 center, float radius, b2HexColor color)
{
    m_debugDraw.DrawSolidCircleFcn(transform, radius, color, m_context);
}

void SampleDraw::DrawSolidCapsule(b2Vec2 p1, b2Vec2 p2, float radius, b2HexColor color)
{
    m_debugDraw.DrawSolidCapsuleFcn(p1, p2, radius, color, m_context);
}

void SampleDraw::DrawSegment(b2Vec2 p1, b2Vec2 p2, b2HexColor color)
{
    m_debugDraw.DrawSegmentFcn(p1, p2, color, m_context);
}

void SampleDraw::DrawTransform(b2Transform transform)
{
    m_debugDraw.DrawTransformFcn(transform, m_context);
}

void SampleDraw::DrawPoint(b2Vec2 p, float size, b2HexColor color)
{
    m_debugDraw.DrawPointFcn(p, size, color, m_context);
}

void SampleDraw::DrawAABB(b2AABB aabb, b2HexColor color)
{
    m_context->drawRect(Vec2{aabb.lowerBound.x, aabb.lowerBound.y}, Vec2{aabb.upperBound.x, aabb.upperBound.y},
                   PhysicsHelper::toColor(color));
}

void SampleDraw::DrawString(int x, int y, const char* pszFormat, ...)
{
    // va_list args;
    // va_start(args, pszFormat);
    // auto ret = text_utils::vformat(pszFormat, args);
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
    ImGui::PushFont(m_regularFont, ImGui::GetStyle().FontSizeBase);
    ImGui::SetCursorPos(ImVec2(float(x), float(y)));
    ImGui::TextColoredV(ImColor(230, 153, 153, 255), pszFormat, arg);
    ImGui::PopFont();
    ImGui::End();
    va_end(arg);
}

void SampleDraw::DrawString(b2Vec2 p, const char* pszFormat, ...)
{
    /*va_list args;
    va_start(args, pszFormat);
    auto ret = text_utils::vformat(pszFormat, args);
    va_end(args);

    _debugString.append(ret);
    _debugString.push_back('\n');
    _textRender->setString(_debugString);*/

    extern SampleCamera& getBox2dTestBedCamera();

    b2Vec2 ps = getBox2dTestBedCamera().ConvertWorldToScreen(p);

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

#pragma endregion


#pragma region SampleDrawNode
static void b2DrawCircle(b2Vec2 center, float radius, b2HexColor color, SampleDrawNode* context)
{
    auto ratio  = context->getPTMRatio();
    auto offset = context->getWorldOffset();
    context->AddCircle(CircleData{PhysicsHelper::toColor(color),
                                  b2Vec2{center.x * ratio + offset.x, center.y * ratio + offset.y}, radius * ratio});
}

static void b2DrawSolidCircle(b2Transform t, float radius, b2HexColor color, SampleDrawNode* context)
{
    // RGBA8 rgba = MakeRGBA8(color, 1.0f);
    // m_circles.push_back({{transform.p.x, transform.p.y, transform.q.c, transform.q.s}, radius, rgba});
    auto ratio  = context->getPTMRatio();
    auto offset = context->getWorldOffset();
    context->AddCircle({{t.p.x * ratio + offset.x, t.p.y * ratio + offset.y, t.q.c, t.q.s},
                        PhysicsHelper::toColor(color),
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
    transform.p   = PhysicsHelper::tob2Vec2(0.5f * (p1 + p2));
    transform.q.c = axis.x;
    transform.q.s = axis.y;

    auto rgba = PhysicsHelper::toColor(c);

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

    // Demo messageString
    _textRender = Label::createWithTTF("TEST", "fonts/arial.ttf", 8.0f);
    _textRender->setAnchorPoint(Vec2(0, 1));
    _textRender->setPosition(VisibleRect::left().x, VisibleRect::top().y - 10);
    _textRender->setColor(Color32::WHITE);
    this->addChild(_textRender, 99);

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

    _debugString.clear();
}

#pragma endregion
