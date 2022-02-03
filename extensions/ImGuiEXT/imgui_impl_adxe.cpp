/*
 * The imgui adxe backend
 * refer to
 * a. https://github.com/ocornut/imgui/blob/835a5344b01e79aca479e95fa9d36fb5fdef3d14/backends/imgui_impl_glfw.cpp
 * b. https://github.com/Xrysnow/cocos2d-x-imgui/blob/master/imgui_impl_cocos2dx.cpp
 */
#include "imgui_impl_adxe.h"
#include "cocos2d.h"
#include "renderer/backend/Backend.h"
#include <functional>

// clang-format off
// 
// GLFW
#include <GLFW/glfw3.h>
#ifdef _WIN32
#undef APIENTRY
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>   // for glfwGetWin32Window
#endif
#define GLFW_HAS_WINDOW_TOPMOST       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ GLFW_FLOATING
#define GLFW_HAS_WINDOW_HOVERED       (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ GLFW_HOVERED
#define GLFW_HAS_WINDOW_ALPHA         (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwSetWindowOpacity
#define GLFW_HAS_PER_MONITOR_DPI      (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwGetMonitorContentScale
#define GLFW_HAS_VULKAN               (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwCreateWindowSurface
#define GLFW_HAS_FOCUS_WINDOW         (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3200) // 3.2+ glfwFocusWindow
#define GLFW_HAS_FOCUS_ON_SHOW        (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ GLFW_FOCUS_ON_SHOW
#define GLFW_HAS_MONITOR_WORK_AREA    (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300) // 3.3+ glfwGetMonitorWorkarea
#define GLFW_HAS_OSX_WINDOW_POS_FIX   (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 + GLFW_VERSION_REVISION * 10 >= 3310) // 3.3.1+ Fixed: Resizing window repositions it on MacOS #1553
#ifdef GLFW_RESIZE_NESW_CURSOR        // Let's be nice to people who pulled GLFW between 2019-04-16 (3.4 define) and 2019-11-29 (cursors defines) // FIXME: Remove when GLFW 3.4 is released?
#define GLFW_HAS_NEW_CURSORS          (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3400) // 3.4+ GLFW_RESIZE_ALL_CURSOR, GLFW_RESIZE_NESW_CURSOR, GLFW_RESIZE_NWSE_CURSOR, GLFW_NOT_ALLOWED_CURSOR
#else
#define GLFW_HAS_NEW_CURSORS          (0)
#endif
#ifdef GLFW_MOUSE_PASSTHROUGH         // Let's be nice to people who pulled GLFW between 2019-04-16 (3.4 define) and 2020-07-17 (passthrough)
#define GLFW_HAS_MOUSE_PASSTHROUGH    (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3400) // 3.4+ GLFW_MOUSE_PASSTHROUGH
#else
#define GLFW_HAS_MOUSE_PASSTHROUGH    (0)
#endif
// clang-format on

using namespace cocos2d;
using namespace backend;

#define CC_PTR_CAST(v, pointer_type) reinterpret_cast<pointer_type>(v)

// TODO: mac metal
#if (defined(CC_USE_GL) || defined(CC_USE_GLES))
#    define CC_IMGUI_ENABLE_MULTI_VIEWPORT 1
#else
#    define CC_IMGUI_ENABLE_MULTI_VIEWPORT 0
#endif

// fps macro
#define CC_IMGUI_DEFAULT_DELTA (1 / 60.f)
#define CC_IMGUI_MIN_DELTA (1 / 1000.f)
#define CC_IMGUI_MAX_DELTA (1 / 30.f)

// ADXE data
constexpr IndexFormat IMGUI_INDEX_FORMAT = sizeof(ImDrawIdx) == 2 ? IndexFormat::U_SHORT : IndexFormat::U_INT;

struct ProgramInfoData
{
    Program* program = nullptr;
    // Uniforms location
    UniformLocation texture{};
    UniformLocation projection{};
    // Vertex attributes location
    int position = 0;
    int uv       = 0;
    int color    = 0;
    VertexLayout layout{};
};

struct SavedRenderStateData
{
    backend::CullMode cull{};
    Viewport vp{};
    ScissorRect scissorRect{};
    bool scissorTest{};
    bool depthTest{};
};

struct ImGui_ImplAdxe_Data
{
    GLFWwindow* Window;
    // GlfwClientApi ClientApi;
    double Time{};
    GLFWwindow* MouseWindow{};
    bool MouseJustPressed[ImGuiMouseButton_COUNT]    = {0};
    GLFWcursor* MouseCursors[ImGuiMouseCursor_COUNT] = {0};
    GLFWwindow* KeyOwnerWindows[512]                 = {0};
    bool InstalledCallbacks{};
    bool WantUpdateMonitors{};

    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
    GLFWwindowfocusfun PrevUserCallbackWindowFocus{};
    GLFWcursorenterfun PrevUserCallbackCursorEnter{};
    GLFWmousebuttonfun PrevUserCallbackMousebutton{};
    GLFWscrollfun PrevUserCallbackScroll{};
    GLFWkeyfun PrevUserCallbackKey{};
    GLFWcharfun PrevUserCallbackChar{};
    GLFWmonitorfun PrevUserCallbackMonitor{};

    std::chrono::steady_clock::time_point LastFrameTime{};

    ImGuiImplCocos2dxLoadFontFun LoadCustomFont = nullptr;
    void* LoadCustomFontUserData                = nullptr;

    ProgramInfoData ProgramInfo{};
    ProgramInfoData ProgramFontInfo{};
    bool FontDeviceObjectsDirty = false;
    Texture2D* FontTexture      = nullptr;
    Mat4 Projection;

    std::vector<std::shared_ptr<CallbackCommand>> CallbackCommands{};
    std::vector<std::shared_ptr<CustomCommand>> CustomCommands{};
    Vector<ProgramState*> ProgramStates{};

    SavedRenderStateData SavedRenderState{};
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple
// windows) instead of multiple Dear ImGui contexts.
// FIXME: multi-context support is not well tested and probably dysfunctional in this backend.
// - Because glfwPollEvents() process all windows and some events may be called outside of it, you will need to register
// your own callbacks
//   (passing install_callbacks=false in ImGui_ImplGlfw_InitXXX functions), set the current dear imgui context and then
//   call our callbacks.
// - Otherwise we may need to store a GLFWWindow* -> ImGuiContext* map and handle this in the backend, adding a little
// bit of extra complexity to it.
// FIXME: some shared resources (mouse cursor shape, gamepad) are mishandled when using multi-context.
static ImGui_ImplAdxe_Data* ImGui_ImplAdxe_GetBackendData()
{
    return ImGui::GetCurrentContext() ? reinterpret_cast<ImGui_ImplAdxe_Data*>(ImGui::GetIO().BackendPlatformUserData)
                                      : nullptr;
}

// Forward Declarations
static void ImGui_ImplGlfw_InitPlatformInterface();
static void ImGui_ImplGlfw_ShutdownPlatformInterface();
static void ImGui_ImplGlfw_UpdateMonitors();

static bool ImGui_ImplAdxe_createShaderPrograms();

static void AddRendererCommand(const std::function<void()>& f)
{
    auto bd             = ImGui_ImplAdxe_GetBackendData();
    const auto renderer = Director::getInstance()->getRenderer();
    auto cmd            = std::make_shared<CallbackCommand>();
    cmd->init(0.f);
    cmd->func = f;
    renderer->addCommand(cmd.get());
    bd->CallbackCommands.push_back(cmd);
}

static void ImGui_ImplAdxe_SaveRenderState(cocos2d::Renderer* renderer)
{
    AddRendererCommand([renderer]() {
        auto bd                          = ImGui_ImplAdxe_GetBackendData();
        bd->SavedRenderState.cull        = renderer->getCullMode();
        bd->SavedRenderState.vp          = renderer->getViewport();
        bd->SavedRenderState.scissorTest = renderer->getScissorTest();
        bd->SavedRenderState.scissorRect = renderer->getScissorRect();
        bd->SavedRenderState.depthTest   = renderer->getDepthTest();
    });
}

static void ImGui_ImplAdxe_SetupRenderState(cocos2d::Renderer* renderer,
                                            ImDrawData* draw_data,
                                            int fb_width,
                                            int fb_height)
{
    AddRendererCommand([=]() {
        renderer->setCullMode(backend::CullMode::NONE);
        renderer->setDepthTest(false);
        renderer->setScissorTest(true);
        renderer->setViewPort(0, 0, fb_width, fb_height);
    });

    const auto L = draw_data->DisplayPos.x;
    const auto R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    const auto T = draw_data->DisplayPos.y;
    const auto B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

    auto bd = ImGui_ImplAdxe_GetBackendData();
    Mat4::createOrthographicOffCenter(L, R, B, T, -1.f, 1.f, &bd->Projection);
}

static void ImGui_ImplAdxe_RestoreRenderState(cocos2d::Renderer* renderer)
{
    AddRendererCommand([renderer]() {
        auto bd = ImGui_ImplAdxe_GetBackendData();
        renderer->setCullMode(bd->SavedRenderState.cull);
        auto& vp = bd->SavedRenderState.vp;
        renderer->setViewPort(vp.x, vp.y, vp.w, vp.h);
        renderer->setScissorTest(bd->SavedRenderState.scissorTest);
        auto& sc = bd->SavedRenderState.scissorRect;
        renderer->setScissorRect(sc.x, sc.y, sc.width, sc.height);
        renderer->setDepthTest(bd->SavedRenderState.depthTest);

        // apply raster state
        renderer->beginRenderPass();
        renderer->endRenderPass();
    });
}

void ImGui_ImplAdxe_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays
    // (screen coordinates != framebuffer coordinates)
    int fb_width  = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    const auto renderer = Director::getInstance()->getRenderer();

    ImGui_ImplAdxe_SaveRenderState(renderer);

    ImGui_ImplAdxe_SetupRenderState(renderer, draw_data, fb_width, fb_height);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off   = draw_data->DisplayPos;        // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale;  // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        size_t ibuffer_offset      = 0;

        // Upload vertex/index buffers
        const auto vsize = cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
        IM_ASSERT(vsize > 0);
        auto vbuffer = backend::Device::getInstance()->newBuffer(vsize, BufferType::VERTEX, BufferUsage::STATIC);
        vbuffer->autorelease();
        vbuffer->updateData(cmd_list->VtxBuffer.Data, vsize);
        const auto isize = cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
        IM_ASSERT(isize > 0);
        auto ibuffer = backend::Device::getInstance()->newBuffer(isize, BufferType::INDEX, BufferUsage::STATIC);
        ibuffer->autorelease();
        ibuffer->updateData(cmd_list->IdxBuffer.Data, isize);

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != nullptr)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user
                // to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    ImGui_ImplAdxe_SetupRenderState(renderer, draw_data, fb_width, fb_height);
                else
                {
                    AddRendererCommand([=]() { pcmd->UserCallback(cmd_list, pcmd); });
                }
            }
            else
            {
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clip_rect;
                clip_rect.x = (pcmd->ClipRect.x - clip_off.x) * clip_scale.x;
                clip_rect.y = (pcmd->ClipRect.y - clip_off.y) * clip_scale.y;
                clip_rect.z = (pcmd->ClipRect.z - clip_off.x) * clip_scale.x;
                clip_rect.w = (pcmd->ClipRect.w - clip_off.y) * clip_scale.y;

                if (clip_rect.x < fb_width && clip_rect.y < fb_height && clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                {
                    // Apply scissor/clipping rectangle
                    AddRendererCommand([=]() {
                        renderer->setScissorRect(clip_rect.x, fb_height - clip_rect.w, clip_rect.z - clip_rect.x,
                                                 clip_rect.w - clip_rect.y);
                    });

                    auto bd = ImGui_ImplAdxe_GetBackendData();

                    if (typeid(*((Ref*)pcmd->TextureId)) == typeid(Texture2D))
                    {
                        auto tex = CC_PTR_CAST(pcmd->TextureId, Texture2D*);
                        auto cmd = std::make_shared<CustomCommand>();
                        bd->CustomCommands.push_back(cmd);
                        cmd->init(0.f, BlendFunc::ALPHA_NON_PREMULTIPLIED);
                        const auto pinfo = tex == bd->FontTexture ? &bd->ProgramFontInfo : &bd->ProgramInfo;
                        // create new ProgramState
                        auto state = new ProgramState(pinfo->program);
                        state->autorelease();
                        bd->ProgramStates.pushBack(state);
                        auto& desc        = cmd->getPipelineDescriptor();
                        desc.programState = state;
                        // setup attributes for ImDrawVert
                        *desc.programState->getVertexLayout() = pinfo->layout;
                        desc.programState->setUniform(pinfo->projection, &bd->Projection, sizeof(Mat4));
                        desc.programState->setTexture(pinfo->texture, 0, tex->getBackendTexture());
                        // In order to composite our output buffer we need to preserve alpha
                        desc.blendDescriptor.sourceAlphaBlendFactor = BlendFactor::ONE;
                        // set vertex/index buffer
                        cmd->setIndexBuffer(ibuffer, IMGUI_INDEX_FORMAT);
                        cmd->setVertexBuffer(vbuffer);
                        cmd->setDrawType(CustomCommand::DrawType::ELEMENT);
                        cmd->setPrimitiveType(PrimitiveType::TRIANGLE);
                        cmd->setIndexDrawInfo(ibuffer_offset, pcmd->ElemCount);
                        renderer->addCommand(cmd.get());
                    }
                    else
                    {
                        auto node     = CC_PTR_CAST(pcmd->TextureId, Node*);
                        const auto tr = node->getNodeToParentTransform();
                        node->setVisible(true);
                        node->setNodeToParentTransform(tr);
                        const auto& proj =
                            Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
                        node->visit(Director::getInstance()->getRenderer(), proj.getInversed() * bd->Projection, 0);
                        node->setVisible(false);
                    }
                }
            }
            ibuffer_offset += pcmd->ElemCount;
        }
    }

    ImGui_ImplAdxe_RestoreRenderState(renderer);
}

void ImGui_ImplAdxe_RenderPlatform()
{
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* prev_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(prev_current_context);
        AddRendererCommand([=]() { glfwMakeContextCurrent(prev_current_context); });
    }
}

static const char* ImGui_ImplAdxe_GetClipboardText(void* user_data)
{
    return glfwGetClipboardString(CC_PTR_CAST(user_data, GLFWwindow*));
}

static void ImGui_ImplAdxe_SetClipboardText(void* user_data, const char* text)
{
    glfwSetClipboardString(CC_PTR_CAST(user_data, GLFWwindow*), text);
}

void ImGui_ImplAdxe_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    auto bd = ImGui_ImplAdxe_GetBackendData();
    if (bd->PrevUserCallbackMousebutton != nullptr && window == bd->Window)
        bd->PrevUserCallbackMousebutton(window, button, action, mods);

    if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(bd->MouseJustPressed))
        bd->MouseJustPressed[button] = true;
}

void ImGui_ImplAdxe_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    auto bd = ImGui_ImplAdxe_GetBackendData();
    if (bd->PrevUserCallbackScroll != nullptr && window == bd->Window)
        bd->PrevUserCallbackScroll(window, xoffset, yoffset);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

void ImGui_ImplAdxe_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    auto bd = ImGui_ImplAdxe_GetBackendData();
    if (bd->PrevUserCallbackKey != nullptr && window == bd->Window)
        bd->PrevUserCallbackKey(window, key, scancode, action, mods);

    if (key < 0)
        return;
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    // Modifiers are not reliable across systems
    io.KeyCtrl  = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt   = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    io.KeySuper = false;
#else
    io.KeySuper                                 = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
}

void ImGui_ImplAdxe_WindowFocusCallback(GLFWwindow* window, int focused)
{
    ImGui_ImplAdxe_Data* bd = ImGui_ImplAdxe_GetBackendData();
    if (bd->PrevUserCallbackWindowFocus != NULL && window == bd->Window)
        bd->PrevUserCallbackWindowFocus(window, focused);

    ImGuiIO& io = ImGui::GetIO();
    io.AddFocusEvent(focused != 0);
}

void ImGui_ImplAdxe_CursorEnterCallback(GLFWwindow* window, int entered)
{
    ImGui_ImplAdxe_Data* bd = ImGui_ImplAdxe_GetBackendData();
    if (bd->PrevUserCallbackCursorEnter != NULL && window == bd->Window)
        bd->PrevUserCallbackCursorEnter(window, entered);

    if (entered)
        bd->MouseWindow = window;
    if (!entered && bd->MouseWindow == window)
        bd->MouseWindow = NULL;
}

void ImGui_ImplAdxe_CharCallback(GLFWwindow* window, unsigned int c)
{
    auto bd = ImGui_ImplAdxe_GetBackendData();
    if (bd->PrevUserCallbackChar != nullptr && window == bd->Window)
        bd->PrevUserCallbackChar(window, c);

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(c);
}

void ImGui_ImplGlfw_MonitorCallback(GLFWmonitor*, int)
{
    auto bd                = ImGui_ImplAdxe_GetBackendData();
    bd->WantUpdateMonitors = true;
}

void ImGui_ImplAdxe_SetCustomFontLoader(ImGuiImplCocos2dxLoadFontFun fun, void* userdata)
{
    auto bd                    = ImGui_ImplAdxe_GetBackendData();
    bd->LoadCustomFont         = fun;
    bd->LoadCustomFontUserData = userdata;
}

void* ImGui_ImplAdxe_GetFontsTexture()
{
    auto bd = ImGui_ImplAdxe_GetBackendData();
    return bd->FontTexture;
}

void ImGui_ImplAdxe_SetDeviceObjectsDirty()
{
    auto bd                    = ImGui_ImplAdxe_GetBackendData();
    bd->FontDeviceObjectsDirty = true;
}

bool ImGui_ImplAdxe_CreateDeviceObjects()
{
    auto bd = ImGui_ImplAdxe_GetBackendData();
    if (bd->LoadCustomFont)
        bd->LoadCustomFont(bd->LoadCustomFontUserData);

    ImGui_ImplAdxe_createShaderPrograms();
    ImGui_ImplAdxe_CreateFontsTexture();

    bd->FontDeviceObjectsDirty = false;
    return true;
}

void ImGui_ImplAdxe_DestroyDeviceObjects()
{
    auto bd = ImGui_ImplAdxe_GetBackendData();
    CC_SAFE_RELEASE_NULL(bd->ProgramInfo.program);
    CC_SAFE_RELEASE_NULL(bd->ProgramFontInfo.program);
    ImGui_ImplAdxe_DestroyFontsTexture();
}

static bool ImGui_ImplAdxe_createShaderPrograms()
{
    auto vertex_shader =
        "uniform mat4 u_MVPMatrix;\n"
        "attribute vec2 a_position;\n"
        "attribute vec2 a_texCoord;\n"
        "attribute vec4 a_color;\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_fragmentColor;\n"
        "void main()\n"
        "{\n"
        "    v_texCoord = a_texCoord;\n"
        "    v_fragmentColor = a_color;\n"
        "    gl_Position = u_MVPMatrix * vec4(a_position.xy, 0.0, 1.0);\n"
        "}\n";
    auto fragment_shader =
        "#ifdef GL_ES\n"
        "    precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D u_texture;\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_fragmentColor;\n"
        "void main()\n"
        "{\n"
        "    gl_FragColor = v_fragmentColor * texture2D(u_texture, v_texCoord);\n"
        "}\n";
    auto fragment_shader_font =
        "#ifdef GL_ES\n"
        "    precision mediump float;\n"
        "#endif\n"
        "uniform sampler2D u_texture;\n"
        "varying vec2 v_texCoord;\n"
        "varying vec4 v_fragmentColor;\n"
        "void main()\n"
        "{\n"
        "    float a = texture2D(u_texture, v_texCoord).a;\n"
        "    gl_FragColor = vec4(v_fragmentColor.rgb, v_fragmentColor.a * a);\n"
        "}\n";

    auto bd = ImGui_ImplAdxe_GetBackendData();

    CC_SAFE_RELEASE(bd->ProgramInfo.program);
    CC_SAFE_RELEASE(bd->ProgramFontInfo.program);
    bd->ProgramInfo.program     = backend::Device::getInstance()->newProgram(vertex_shader, fragment_shader);
    bd->ProgramFontInfo.program = backend::Device::getInstance()->newProgram(vertex_shader, fragment_shader_font);
    IM_ASSERT(bd->ProgramInfo.program);
    IM_ASSERT(bd->ProgramFontInfo.program);
    if (!bd->ProgramInfo.program || !bd->ProgramFontInfo.program)
        return false;

    for (auto& p : {&bd->ProgramInfo, &bd->ProgramFontInfo})
    {
        p->texture    = p->program->getUniformLocation(TEXTURE);
        p->projection = p->program->getUniformLocation(MVP_MATRIX);
        p->position   = p->program->getAttributeLocation(POSITION);
        p->uv         = p->program->getAttributeLocation(TEXCOORD);
        p->color      = p->program->getAttributeLocation(COLOR);
        IM_ASSERT(bool(p->texture));
        IM_ASSERT(bool(p->projection));
        IM_ASSERT(p->position >= 0);
        IM_ASSERT(p->uv >= 0);
        IM_ASSERT(p->color >= 0);
        auto& layout = p->layout;
        layout.setAttribute("a_position", p->position, VertexFormat::FLOAT2, 0, false);
        layout.setAttribute("a_texCoord", p->uv, VertexFormat::FLOAT2, offsetof(ImDrawVert, uv), false);
        layout.setAttribute("a_color", p->color, VertexFormat::UBYTE4, offsetof(ImDrawVert, col), true);
        layout.setLayout(sizeof(ImDrawVert));
    }

    return true;
}

bool ImGui_ImplAdxe_CreateFontsTexture()
{
    auto bd = ImGui_ImplAdxe_GetBackendData();
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small)
    // because it is more likely to be compatible with user's existing shaders.
    // If your ImTextureId represent a higher-level concept than just a GL texture id,
    // consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    CC_SAFE_RELEASE(bd->FontTexture);
    bd->FontTexture = new Texture2D();

    bd->FontTexture->setAntiAliasTexParameters();
    bd->FontTexture->initWithData(pixels, width * height, backend::PixelFormat::A8, width, height,
                                  Size{static_cast<float>(width), static_cast<float>(height)});
    io.Fonts->TexID = (ImTextureID)bd->FontTexture;
    return true;
}

void ImGui_ImplAdxe_DestroyFontsTexture()
{
    auto bd = ImGui_ImplAdxe_GetBackendData();
    if (bd->FontTexture)
    {
        ImGui::GetIO().Fonts->TexID = nullptr;
        CC_SAFE_RELEASE_NULL(bd->FontTexture);
    }
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports
// simultaneously. If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you
// completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

static void ImGui_ImplOpenGL2_RenderWindow(ImGuiViewport* viewport, void*)
{
    if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
    {
        const auto renderer = Director::getInstance()->getRenderer();
        renderer->clear(ClearFlag::COLOR, {0, 0, 0, 1}, 1, 0, 0);
    }
    ImGui_ImplAdxe_RenderDrawData(viewport->DrawData);
}

bool ImGui_ImplAdxe_Init(bool install_callbacks /*TODO: need check whether callbacks installed at shutdown*/)
{
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();

    // Setup backend capabilities flags
    ImGui_ImplAdxe_Data* bd    = new ImGui_ImplAdxe_Data();
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName     = "imgui_impl_adxe";
    io.BackendRendererName     = "imgui_impl_adxe";
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking

    // Enable Multi-Viewport / Platform Windows
    // TODO: mac osx have problem when create new window, the game scene will go to black
#if CC_IMGUI_ENABLE_MULTI_VIEWPORT
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;
    const auto window = static_cast<GLViewImpl*>(Director::getInstance()->getOpenGLView())->getWindow();
    // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    // We can create multi-viewports on the Platform side (optional)
    io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;
#if GLFW_HAS_GLFW_HOVERED && CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    // We can set io.MouseHoveredViewport correctly (optional, not easy)
    io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport;
#endif
    bd->Window             = window;
    bd->Time               = 0.0;
    bd->WantUpdateMonitors = true;

    // backend
    io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
    ImGuiPlatformIO& platform_io      = ImGui::GetPlatformIO();
    platform_io.Renderer_RenderWindow = ImGui_ImplOpenGL2_RenderWindow;

    // disable auto load and save
    io.IniFilename = nullptr;

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab]        = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow]  = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow]    = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow]  = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp]     = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown]   = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home]       = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End]        = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert]     = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete]     = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace]  = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space]      = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter]      = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape]     = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A]          = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C]          = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V]          = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X]          = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y]          = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z]          = GLFW_KEY_Z;

    io.SetClipboardTextFn = ImGui_ImplAdxe_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplAdxe_GetClipboardText;
    io.ClipboardUserData  = bd->Window;

    // Create mouse cursors
    // (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
    // GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
    // Missing cursors will return NULL and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
    GLFWerrorfun prev_error_callback             = glfwSetErrorCallback(nullptr);
    bd->MouseCursors[ImGuiMouseCursor_Arrow]     = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    bd->MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
    bd->MouseCursors[ImGuiMouseCursor_ResizeNS]  = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
    bd->MouseCursors[ImGuiMouseCursor_ResizeEW]  = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
    bd->MouseCursors[ImGuiMouseCursor_Hand]      = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
    bd->MouseCursors[ImGuiMouseCursor_ResizeAll]  = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
    bd->MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
    bd->MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
    bd->MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
    bd->MouseCursors[ImGuiMouseCursor_ResizeAll]  = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    bd->MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    bd->MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    bd->MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
    glfwSetErrorCallback(prev_error_callback);

    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
    bd->PrevUserCallbackWindowFocus = nullptr;
    bd->PrevUserCallbackMousebutton = nullptr;
    bd->PrevUserCallbackScroll      = nullptr;
    bd->PrevUserCallbackKey         = nullptr;
    bd->PrevUserCallbackChar        = nullptr;
    bd->PrevUserCallbackMonitor     = nullptr;
    if (install_callbacks)
    {
        bd->InstalledCallbacks          = true;
        bd->PrevUserCallbackWindowFocus = glfwSetWindowFocusCallback(window, ImGui_ImplAdxe_WindowFocusCallback);
        bd->PrevUserCallbackCursorEnter = glfwSetCursorEnterCallback(window, ImGui_ImplAdxe_CursorEnterCallback);
        bd->PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, ImGui_ImplAdxe_MouseButtonCallback);
        bd->PrevUserCallbackScroll      = glfwSetScrollCallback(window, ImGui_ImplAdxe_ScrollCallback);
        bd->PrevUserCallbackKey         = glfwSetKeyCallback(window, ImGui_ImplAdxe_KeyCallback);
        bd->PrevUserCallbackChar        = glfwSetCharCallback(window, ImGui_ImplAdxe_CharCallback);
        bd->PrevUserCallbackMonitor     = glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);
    }

    // Note: monitor callback are broken GLFW 3.2 and earlier (see github.com/glfw/glfw/issues/784)
    ImGui_ImplGlfw_UpdateMonitors();
    // Since: required GLFW 3.3, comment follow line
    // glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);

    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    ImGuiViewport* main_viewport  = ImGui::GetMainViewport();
    main_viewport->PlatformHandle = (void*)bd->Window;

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    main_viewport->PlatformHandleRaw = glfwGetWin32Window(bd->Window);
#endif

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        ImGui_ImplGlfw_InitPlatformInterface();
    return true;
}

void ImGui_ImplAdxe_Shutdown()
{
    auto bd = ImGui_ImplAdxe_GetBackendData();

    if (bd->InstalledCallbacks)
    {
        // Restore mouse and char callback
        glfwSetWindowFocusCallback(bd->Window, bd->PrevUserCallbackWindowFocus);
        glfwSetCursorEnterCallback(bd->Window, bd->PrevUserCallbackCursorEnter);
        glfwSetMouseButtonCallback(bd->Window, bd->PrevUserCallbackMousebutton);
        glfwSetScrollCallback(bd->Window, bd->PrevUserCallbackScroll);
        glfwSetKeyCallback(bd->Window, bd->PrevUserCallbackKey);
        glfwSetCharCallback(bd->Window, bd->PrevUserCallbackChar);
        glfwSetMonitorCallback(bd->PrevUserCallbackMonitor);
    }

    // Destroy cursors
    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
    {
        glfwDestroyCursor(bd->MouseCursors[cursor_n]);
    }

    ImGui_ImplAdxe_DestroyDeviceObjects();
    ImGui::DestroyContext();

    delete bd;
}

static void ImGui_ImplAdxe_UpdateMousePosAndButtons()
{
    auto bd = ImGui_ImplAdxe_GetBackendData();

    ImGuiIO& io                  = ImGui::GetIO();
    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();

    const ImVec2 mouse_pos_prev = io.MousePos;
    io.MousePos                 = ImVec2(-FLT_MAX, -FLT_MAX);
    io.MouseHoveredViewport     = 0;

    // Update mouse buttons
    // (if a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events
    // that are shorter than 1 frame)
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {

        io.MouseDown[i]         = bd->MouseJustPressed[i] || glfwGetMouseButton(bd->Window, i) != 0;
        bd->MouseJustPressed[i] = false;
    }

    for (int n = 0; n < platform_io.Viewports.Size; n++)
    {
        ImGuiViewport* viewport = platform_io.Viewports[n];
        GLFWwindow* window      = CC_PTR_CAST(viewport->PlatformHandle, GLFWwindow*);

#ifdef __EMSCRIPTEN__
        const bool focused = true;
#else
        const bool focused = glfwGetWindowAttrib(window, GLFW_FOCUSED) != 0;
#endif
        GLFWwindow* mouse_window = (bd->MouseWindow == window || focused) ? window : NULL;

        // Update mouse buttons
        if (focused)
            for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
                io.MouseDown[i] |= glfwGetMouseButton(window, i) != 0;

        // Set OS mouse position from Dear ImGui if requested (rarely used, only when
        // ImGuiConfigFlags_NavEnableSetMousePos is enabled by user) (When multi-viewports are enabled, all Dear ImGui
        // positions are same as OS positions)
        if (io.WantSetMousePos && focused)
            glfwSetCursorPos(window, (double)(mouse_pos_prev.x - viewport->Pos.x),
                             (double)(mouse_pos_prev.y - viewport->Pos.y));

        // Set Dear ImGui mouse position from OS position
        if (mouse_window != NULL)
        {
            double mouse_x, mouse_y;
            glfwGetCursorPos(mouse_window, &mouse_x, &mouse_y);
            if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
            {
                // Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse
                // is on the upper-left of the primary monitor)
                int window_x, window_y;
                glfwGetWindowPos(window, &window_x, &window_y);
                io.MousePos = ImVec2((float)mouse_x + window_x, (float)mouse_y + window_y);
            }
            else
            {
                // Single viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the
                // mouse is on the upper-left corner of the app window)
                io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
            }
        }

        // (Optional) When using multiple viewports: set io.MouseHoveredViewport to the viewport the OS mouse cursor is
        // hovering. Important: this information is not easy to provide and many high-level windowing library won't be
        // able to provide it correctly, because
        // - This is _ignoring_ viewports with the ImGuiViewportFlags_NoInputs flag (pass-through windows).
        // - This is _regardless_ of whether another viewport is focused or being dragged from.
        // If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the backend, imgui will ignore this field and
        // infer the information by relying on the rectangles and last focused time of every viewports it knows about.
        // It will be unaware of other windows that may be sitting between or over your windows. [GLFW] FIXME: This is
        // currently only correct on Win32. See what we do below with the WM_NCHITTEST, missing an equivalent for other
        // systems. See https://github.com/glfw/glfw/issues/1236 if you want to help in making this a GLFW feature.
#if GLFW_HAS_MOUSE_PASSTHROUGH || (GLFW_HAS_WINDOW_HOVERED && defined(_WIN32))
        const bool window_no_input = (viewport->Flags & ImGuiViewportFlags_NoInputs) != 0;
#    if GLFW_HAS_MOUSE_PASSTHROUGH
        glfwSetWindowAttrib(window, GLFW_MOUSE_PASSTHROUGH, window_no_input);
#    endif
        if (glfwGetWindowAttrib(window, GLFW_HOVERED) && !window_no_input)
            io.MouseHoveredViewport = viewport->ID;
#endif
    }
}

static void ImGui_ImplAdxe_UpdateMouseCursor()
{
    auto bd     = ImGui_ImplAdxe_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) ||
        glfwGetInputMode(bd->Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    ImGuiPlatformIO& platform_io  = ImGui::GetPlatformIO();
    for (int n = 0; n < platform_io.Viewports.Size; n++)
    {
        GLFWwindow* window = CC_PTR_CAST(platform_io.Viewports[n]->PlatformHandle, GLFWwindow*);
        if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
        {
            // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        }
        else
        {
            // Show OS mouse cursor
            // FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works
            // here.
            glfwSetCursor(window, bd->MouseCursors[imgui_cursor] ? bd->MouseCursors[imgui_cursor]
                                                                 : bd->MouseCursors[ImGuiMouseCursor_Arrow]);
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
    }
}

static void ImGui_ImplAdxe_UpdateGamepads()
{
    ImGuiIO& io = ImGui::GetIO();
    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
        return;

    // Update gamepad inputs
    int axes_count = 0, buttons_count = 0;

#define MAP_BUTTON(NAV_NO, BUTTON_NO)                                      \
    {                                                                      \
        if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) \
            io.NavInputs[NAV_NO] = 1.0f;                                   \
    }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1)                    \
    {                                                          \
        float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0; \
        v       = (v - V0) / (V1 - V0);                        \
        if (v > 1.0f)                                          \
            v = 1.0f;                                          \
        if (io.NavInputs[NAV_NO] < v)                          \
            io.NavInputs[NAV_NO] = v;                          \
    }
    const float* axes            = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
    MAP_BUTTON(ImGuiNavInput_Activate, 0);    // Cross / A
    MAP_BUTTON(ImGuiNavInput_Cancel, 1);      // Circle / B
    MAP_BUTTON(ImGuiNavInput_Menu, 2);        // Square / X
    MAP_BUTTON(ImGuiNavInput_Input, 3);       // Triangle / Y
    MAP_BUTTON(ImGuiNavInput_DpadLeft, 13);   // D-Pad Left
    MAP_BUTTON(ImGuiNavInput_DpadRight, 11);  // D-Pad Right
    MAP_BUTTON(ImGuiNavInput_DpadUp, 10);     // D-Pad Up
    MAP_BUTTON(ImGuiNavInput_DpadDown, 12);   // D-Pad Down
    MAP_BUTTON(ImGuiNavInput_FocusPrev, 4);   // L1 / LB
    MAP_BUTTON(ImGuiNavInput_FocusNext, 5);   // R1 / RB
    MAP_BUTTON(ImGuiNavInput_TweakSlow, 4);   // L1 / LB
    MAP_BUTTON(ImGuiNavInput_TweakFast, 5);   // R1 / RB
    MAP_ANALOG(ImGuiNavInput_LStickLeft, 0, -0.3f, -0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickRight, 0, +0.3f, +0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickUp, 1, +0.3f, +0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickDown, 1, -0.3f, -0.9f);
#undef MAP_BUTTON
#undef MAP_ANALOG

    if (axes_count > 0 && buttons_count > 0)
        io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    else
        io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
}

static void ImGui_ImplGlfw_UpdateMonitors()
{
    auto bd = ImGui_ImplAdxe_GetBackendData();

    ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
    int monitors_count           = 0;
    GLFWmonitor** glfw_monitors  = glfwGetMonitors(&monitors_count);
    platform_io.Monitors.resize(0);
    for (int n = 0; n < monitors_count; n++)
    {
        ImGuiPlatformMonitor monitor;
        int x, y;
        glfwGetMonitorPos(glfw_monitors[n], &x, &y);
        const GLFWvidmode* vid_mode = glfwGetVideoMode(glfw_monitors[n]);
#if GLFW_HAS_MONITOR_WORK_AREA
        monitor.MainPos  = ImVec2((float)x, (float)y);
        monitor.MainSize = ImVec2((float)vid_mode->width, (float)vid_mode->height);
        int w, h;
        glfwGetMonitorWorkarea(glfw_monitors[n], &x, &y, &w, &h);
        monitor.WorkPos = ImVec2((float)x, (float)y);
        ;
        monitor.WorkSize = ImVec2((float)w, (float)h);
#else
        monitor.MainPos = monitor.WorkPos = ImVec2((float)x, (float)y);
        monitor.MainSize = monitor.WorkSize = ImVec2((float)vid_mode->width, (float)vid_mode->height);
#endif
#if GLFW_HAS_PER_MONITOR_DPI
        // Warning: the validity of monitor DPI information on Windows depends on the application DPI awareness
        // settings, which generally needs to be set in the manifest or at runtime.
        float x_scale, y_scale;
        glfwGetMonitorContentScale(glfw_monitors[n], &x_scale, &y_scale);
        monitor.DpiScale = x_scale;
#endif
        platform_io.Monitors.push_back(monitor);
    }
    bd->WantUpdateMonitors = false;
}

void ImGui_ImplAdxe_NewFrame()
{
    auto bd = ImGui_ImplAdxe_GetBackendData();

    // Calculate deltaTime by self to avoid error when pause cocos2d::Director
    auto now = std::chrono::steady_clock::now();
    auto deltaTime =
        std::chrono::duration_cast<std::chrono::microseconds>(now - bd->LastFrameTime).count() / 1000000.0f;
    deltaTime         = cocos2d::clampf(deltaTime, CC_IMGUI_MIN_DELTA, CC_IMGUI_MAX_DELTA);
    bd->LastFrameTime = now;

    bd->CallbackCommands.clear();
    bd->CustomCommands.clear();
    bd->ProgramStates.clear();

    if (!bd->FontTexture)
        ImGui_ImplAdxe_CreateDeviceObjects();
    else if (bd->FontDeviceObjectsDirty)
    {  // recreate device objects, fonts also should be device objects
        ImGui_ImplAdxe_DestroyDeviceObjects();
        ImGui_ImplAdxe_CreateDeviceObjects();
    }

    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! Missing call to renderer _NewFrame() function?");

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int buffer_w, buffer_h;
    glfwGetWindowSize(bd->Window, &w, &h);
    glfwGetFramebufferSize(bd->Window, &buffer_w, &buffer_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2((float)buffer_w / w, (float)buffer_h / h);

    if (bd->WantUpdateMonitors)
        ImGui_ImplGlfw_UpdateMonitors();

    // Setup time step
    io.DeltaTime = deltaTime;

    ImGui_ImplAdxe_UpdateMousePosAndButtons();
    ImGui_ImplAdxe_UpdateMouseCursor();

    // Update game controllers (if enabled and available)
    ImGui_ImplAdxe_UpdateGamepads();

    ImGui::NewFrame();
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports
// simultaneously. If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you
// completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

// Helper structure we store in the void* RenderUserData field of each ImGuiViewport to easily retrieve our backend
// data.
struct ImGui_ImplGlfw_ViewportData
{
    GLFWwindow* Window;
    bool WindowOwned;
    int IgnoreWindowPosEventFrame;
    int IgnoreWindowSizeEventFrame;

    ImGui_ImplGlfw_ViewportData()
    {
        Window                     = NULL;
        WindowOwned                = false;
        IgnoreWindowSizeEventFrame = IgnoreWindowPosEventFrame = -1;
    }
    ~ImGui_ImplGlfw_ViewportData() { IM_ASSERT(Window == NULL); }
};

#define IMGUI_GLFW_VD(vp) reinterpret_cast<ImGui_ImplGlfw_ViewportData*>(vp->PlatformUserData)

static void ImGui_ImplGlfw_WindowCloseCallback(GLFWwindow* window)
{
    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
        viewport->PlatformRequestClose = true;
}

// GLFW may dispatch window pos/size events after calling glfwSetWindowPos()/glfwSetWindowSize().
// However: depending on the platform the callback may be invoked at different time:
// - on Windows it appears to be called within the glfwSetWindowPos()/glfwSetWindowSize() call
// - on Linux it is queued and invoked during glfwPollEvents()
// Because the event doesn't always fire on glfwSetWindowXXX() we use a frame counter tag to only
// ignore recent glfwSetWindowXXX() calls.
static void ImGui_ImplGlfw_WindowPosCallback(GLFWwindow* window, int, int)
{
    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
    {
        if (ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport))
        {
            bool ignore_event = (ImGui::GetFrameCount() <= vd->IgnoreWindowPosEventFrame + 1);
            // data->IgnoreWindowPosEventFrame = -1;
            if (ignore_event)
                return;
        }
        viewport->PlatformRequestMove = true;
    }
}

static void ImGui_ImplGlfw_WindowSizeCallback(GLFWwindow* window, int, int)
{
    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
    {
        if (ImGui_ImplGlfw_ViewportData* data = IMGUI_GLFW_VD(viewport))
        {
            bool ignore_event = (ImGui::GetFrameCount() <= data->IgnoreWindowSizeEventFrame + 1);
            // data->IgnoreWindowSizeEventFrame = -1;
            if (ignore_event)
                return;
        }
        viewport->PlatformRequestResize = true;
    }
}

static void ImGui_ImplGlfw_CreateWindow(ImGuiViewport* viewport)
{
    ImGui_ImplAdxe_Data* bd         = ImGui_ImplAdxe_GetBackendData();
    ImGui_ImplGlfw_ViewportData* vd = IM_NEW(ImGui_ImplGlfw_ViewportData)();
    viewport->PlatformUserData      = vd;

    const bool multi_viewport_enabled = !!(ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable);

    // GLFW 3.2 unfortunately always set focus on glfwCreateWindow() if GLFW_VISIBLE is set, regardless of GLFW_FOCUSED
    // With GLFW 3.3, the hint GLFW_FOCUS_ON_SHOW fixes this problem
    glfwWindowHint(GLFW_VISIBLE, false);
    glfwWindowHint(GLFW_FOCUSED, false);
#if GLFW_HAS_FOCUS_ON_SHOW
    glfwWindowHint(GLFW_FOCUS_ON_SHOW, false);
#endif
    glfwWindowHint(GLFW_DECORATED, (viewport->Flags & ImGuiViewportFlags_NoDecoration) ? false : true);
#if GLFW_HAS_WINDOW_TOPMOST
    glfwWindowHint(GLFW_FLOATING, (viewport->Flags & ImGuiViewportFlags_TopMost) ? true : false);
#endif

    GLFWwindow* share_window = multi_viewport_enabled ? bd->Window : nullptr;

    vd->Window = glfwCreateWindow((int)viewport->Size.x, (int)viewport->Size.y, "No Title Yet", nullptr, share_window);
    vd->WindowOwned          = true;
    viewport->PlatformHandle = (void*)vd->Window;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
    viewport->PlatformHandleRaw = glfwGetWin32Window(vd->Window);
#endif
    glfwSetWindowPos(vd->Window, (int)viewport->Pos.x, (int)viewport->Pos.y);

    // Install GLFW callbacks for secondary viewports
    glfwSetWindowFocusCallback(vd->Window, ImGui_ImplAdxe_WindowFocusCallback);
    glfwSetCursorEnterCallback(vd->Window, ImGui_ImplAdxe_CursorEnterCallback);
    glfwSetMouseButtonCallback(vd->Window, ImGui_ImplAdxe_MouseButtonCallback);
    glfwSetScrollCallback(vd->Window, ImGui_ImplAdxe_ScrollCallback);
    glfwSetKeyCallback(vd->Window, ImGui_ImplAdxe_KeyCallback);
    glfwSetCharCallback(vd->Window, ImGui_ImplAdxe_CharCallback);
    glfwSetWindowCloseCallback(vd->Window, ImGui_ImplGlfw_WindowCloseCallback);
    glfwSetWindowPosCallback(vd->Window, ImGui_ImplGlfw_WindowPosCallback);
    glfwSetWindowSizeCallback(vd->Window, ImGui_ImplGlfw_WindowSizeCallback);
    if (multi_viewport_enabled)
    {
        const auto window = vd->Window;
        glfwMakeContextCurrent(window);
        glfwSwapInterval(0);
        AddRendererCommand([=]() { glfwMakeContextCurrent(window); });
    }
}

static void ImGui_ImplGlfw_DestroyWindow(ImGuiViewport* viewport)
{
    auto bd = ImGui_ImplAdxe_GetBackendData();
    if (ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport))
    {
        if (vd->WindowOwned)
        {
#if !GLFW_HAS_MOUSE_PASSTHROUGH && GLFW_HAS_WINDOW_HOVERED && defined(_WIN32)
            HWND hwnd = (HWND)viewport->PlatformHandleRaw;
            ::RemovePropA(hwnd, "IMGUI_VIEWPORT");
#endif

            // Release any keys that were pressed in the window being destroyed and are still held down,
            // because we will not receive any release events after window is destroyed.
            for (int i = 0; i < IM_ARRAYSIZE(bd->KeyOwnerWindows); i++)
                if (bd->KeyOwnerWindows[i] == vd->Window)
                    ImGui_ImplAdxe_KeyCallback(
                        vd->Window, i, 0, GLFW_RELEASE,
                        0);  // Later params are only used for main viewport, on which this function is never called.

            glfwDestroyWindow(vd->Window);
        }
        vd->Window = NULL;
        IM_DELETE(vd);
    }
    viewport->PlatformUserData = viewport->PlatformHandle = NULL;
}

// We have submitted https://github.com/glfw/glfw/pull/1568 to allow GLFW to support "transparent inputs".
// In the meanwhile we implement custom per-platform workarounds here (FIXME-VIEWPORT: Implement same work-around for
// Linux/OSX!)
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && GLFW_HAS_GLFW_HOVERED
static WNDPROC g_GlfwWndProc = NULL;
static LRESULT CALLBACK WndProcNoInputs(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCHITTEST)
    {
        // Let mouse pass-through the window. This will allow the back-end to set io.MouseHoveredViewport properly
        // (which is OPTIONAL). The ImGuiViewportFlags_NoInputs flag is set while dragging a viewport, as want to detect
        // the window behind the one we are dragging. If you cannot easily access those viewport flags from your
        // windowing/event code: you may manually synchronize its state e.g. in your main loop after calling
        // UpdatePlatformWindows(). Iterate all viewports/platform windows and pass the flag to your windowing system.
        ImGuiViewport* viewport = CC_PTR_CAST(::GetPropA(hWnd, "IMGUI_VIEWPORT"), ImGuiViewport*);
        if (viewport->Flags & ImGuiViewportFlags_NoInputs)
            return HTTRANSPARENT;
    }
    return ::CallWindowProc(g_GlfwWndProc, hWnd, msg, wParam, lParam);
}
#endif

static void ImGui_ImplGlfw_ShowWindow(ImGuiViewport* viewport)
{
    ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);

#if defined(_WIN32)
    // GLFW hack: Hide icon from task bar
    HWND hwnd = (HWND)viewport->PlatformHandleRaw;
    if (viewport->Flags & ImGuiViewportFlags_NoTaskBarIcon)
    {
        LONG ex_style = ::GetWindowLong(hwnd, GWL_EXSTYLE);
        ex_style &= ~WS_EX_APPWINDOW;
        ex_style |= WS_EX_TOOLWINDOW;
        ::SetWindowLong(hwnd, GWL_EXSTYLE, ex_style);
    }

    // GLFW hack: install hook for WM_NCHITTEST message handler
#    if !GLFW_HAS_MOUSE_PASSTHROUGH && GLFW_HAS_WINDOW_HOVERED && defined(_WIN32)
    ::SetPropA(hwnd, "IMGUI_VIEWPORT", viewport);
    if (g_GlfwWndProc == NULL)
        g_GlfwWndProc = (WNDPROC)::GetWindowLongPtr(hwnd, GWLP_WNDPROC);
    ::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProcNoInputs);
#    endif

#    if !GLFW_HAS_FOCUS_ON_SHOW
    // GLFW hack: GLFW 3.2 has a bug where glfwShowWindow() also activates/focus the window.
    // The fix was pushed to GLFW repository on 2018/01/09 and should be included in GLFW 3.3 via a GLFW_FOCUS_ON_SHOW
    // window attribute. See https://github.com/glfw/glfw/issues/1189
    // FIXME-VIEWPORT: Implement same work-around for Linux/OSX in the meanwhile.
    if (viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
    {
        ::ShowWindow(hwnd, SW_SHOWNA);
        return;
    }
#    endif
#endif

    glfwShowWindow(vd->Window);
}

static ImVec2 ImGui_ImplGlfw_GetWindowPos(ImGuiViewport* viewport)
{
    ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
    int x = 0, y = 0;
    glfwGetWindowPos(vd->Window, &x, &y);
    return ImVec2((float)x, (float)y);
}

static void ImGui_ImplGlfw_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
    ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
    glfwSetWindowPos(vd->Window, (int)pos.x, (int)pos.y);
}

static ImVec2 ImGui_ImplGlfw_GetWindowSize(ImGuiViewport* viewport)
{
    ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
    int w = 0, h = 0;
    glfwGetWindowSize(vd->Window, &w, &h);
    return ImVec2((float)w, (float)h);
}

static void ImGui_ImplGlfw_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
    ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC && !GLFW_HAS_OSX_WINDOW_POS_FIX
    // Native OS windows are positioned from the bottom-left corner on macOS, whereas on other platforms they are
    // positioned from the upper-left corner. GLFW makes an effort to convert macOS style coordinates, however it
    // doesn't handle it when changing size. We are manually moving the window in order for changes of size to be based
    // on the upper-left corner.
    int x, y, width, height;
    glfwGetWindowPos(vd->Window, &x, &y);
    glfwGetWindowSize(vd->Window, &width, &height);
    glfwSetWindowPos(vd->Window, x, y - height + size.y);
#endif
    vd->IgnoreWindowSizeEventFrame = ImGui::GetFrameCount();
    glfwSetWindowSize(vd->Window, (int)size.x, (int)size.y);
}

static void ImGui_ImplGlfw_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
    ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
    glfwSetWindowTitle(vd->Window, title);
}

static void ImGui_ImplGlfw_SetWindowFocus(ImGuiViewport* viewport)
{
#if GLFW_HAS_FOCUS_WINDOW
    ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
    glfwFocusWindow(vd->Window);
#else
    // FIXME: What are the effect of not having this function? At the moment imgui doesn't actually call SetWindowFocus
    // - we set that up ahead, will answer that question later.
    (void)viewport;
#endif
}

static bool ImGui_ImplGlfw_GetWindowFocus(ImGuiViewport* viewport)
{
    ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
    return glfwGetWindowAttrib(vd->Window, GLFW_FOCUSED) != 0;
}

static bool ImGui_ImplGlfw_GetWindowMinimized(ImGuiViewport* viewport)
{
    ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
    return glfwGetWindowAttrib(vd->Window, GLFW_ICONIFIED) != 0;
}

#if GLFW_HAS_WINDOW_ALPHA
static void ImGui_ImplGlfw_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
{
    ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
    glfwSetWindowOpacity(vd->Window, alpha);
}
#endif

static void ImGui_ImplGlfw_RenderWindow(ImGuiViewport* viewport, void*)
{
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
        const auto window               = vd->Window;
        AddRendererCommand([=]() { glfwMakeContextCurrent(window); });
    }
}

static void ImGui_ImplGlfw_SwapBuffers(ImGuiViewport* viewport, void*)
{
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui_ImplGlfw_ViewportData* vd = IMGUI_GLFW_VD(viewport);
        const auto window               = vd->Window;
        glfwMakeContextCurrent(window);
        AddRendererCommand([=]() {
            glfwMakeContextCurrent(window);
            glfwSwapBuffers(window);
        });
    }
}

// clang-format off
// 
//--------------------------------------------------------------------------------------------------------
// IME (Input Method Editor) basic support for e.g. Asian language users
//--------------------------------------------------------------------------------------------------------

// We provide a Win32 implementation because this is such a common issue for IME users
#if defined(_WIN32) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS) && !defined(IMGUI_DISABLE_WIN32_DEFAULT_IME_FUNCTIONS)
#define HAS_WIN32_IME   1
#include <imm.h>
#ifdef _MSC_VER
#pragma comment(lib, "imm32")
#endif
static void ImGui_ImplWin32_SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos)
{
    COMPOSITIONFORM cf = { CFS_FORCE_POSITION, { (LONG)(pos.x - viewport->Pos.x), (LONG)(pos.y - viewport->Pos.y) }, { 0, 0, 0, 0 } };
    if (HWND hwnd = (HWND)viewport->PlatformHandleRaw)
        if (HIMC himc = ::ImmGetContext(hwnd))
        {
            ::ImmSetCompositionWindow(himc, &cf);
            ::ImmReleaseContext(hwnd, himc);
        }
}
#else
#define HAS_WIN32_IME   0
#endif

// clang-format on

static void ImGui_ImplGlfw_InitPlatformInterface()
{
    // Register platform interface (will be coupled with a renderer interface)
    ImGui_ImplAdxe_Data* bd                 = ImGui_ImplAdxe_GetBackendData();
    ImGuiPlatformIO& platform_io            = ImGui::GetPlatformIO();
    platform_io.Platform_CreateWindow       = ImGui_ImplGlfw_CreateWindow;
    platform_io.Platform_DestroyWindow      = ImGui_ImplGlfw_DestroyWindow;
    platform_io.Platform_ShowWindow         = ImGui_ImplGlfw_ShowWindow;
    platform_io.Platform_SetWindowPos       = ImGui_ImplGlfw_SetWindowPos;
    platform_io.Platform_GetWindowPos       = ImGui_ImplGlfw_GetWindowPos;
    platform_io.Platform_SetWindowSize      = ImGui_ImplGlfw_SetWindowSize;
    platform_io.Platform_GetWindowSize      = ImGui_ImplGlfw_GetWindowSize;
    platform_io.Platform_SetWindowFocus     = ImGui_ImplGlfw_SetWindowFocus;
    platform_io.Platform_GetWindowFocus     = ImGui_ImplGlfw_GetWindowFocus;
    platform_io.Platform_GetWindowMinimized = ImGui_ImplGlfw_GetWindowMinimized;
    platform_io.Platform_SetWindowTitle     = ImGui_ImplGlfw_SetWindowTitle;
    platform_io.Platform_RenderWindow       = ImGui_ImplGlfw_RenderWindow;
    platform_io.Platform_SwapBuffers        = ImGui_ImplGlfw_SwapBuffers;
#if GLFW_HAS_WINDOW_ALPHA
    platform_io.Platform_SetWindowAlpha = ImGui_ImplGlfw_SetWindowAlpha;
#endif

#if HAS_WIN32_IME
    platform_io.Platform_SetImeInputPos = ImGui_ImplWin32_SetImeInputPos;
#endif
    // Register main window handle (which is owned by the main application, not by us)
    // This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use same logic for
    // main and secondary viewports.
    ImGuiViewport* main_viewport    = ImGui::GetMainViewport();
    ImGui_ImplGlfw_ViewportData* vd = IM_NEW(ImGui_ImplGlfw_ViewportData)();
    vd->Window                      = bd->Window;
    vd->WindowOwned                 = false;
    main_viewport->PlatformUserData = vd;
    main_viewport->PlatformHandle   = (void*)bd->Window;
}

static void ImGui_ImplGlfw_ShutdownPlatformInterface() {}
