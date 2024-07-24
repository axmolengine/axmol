#include "imgui_impl_ax_android.h"
#include "cocos2d.h"
#include "renderer/backend/Backend.h"
#include <functional>
#include <android/native_window.h>
#include <android/input.h>
#include <android/keycodes.h>
#include <android/log.h>
#include <imgui_internal.h>
#include "base/IMEDelegate.h"

USING_NS_AX;
using namespace backend;

// Clang warnings with -Weverything
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wold-style-cast"     // warning: use of old-style cast
#pragma clang diagnostic ignored "-Wsign-conversion"    // warning: implicit conversion changes signedness
#if __has_warning("-Wzero-as-null-pointer-constant")
#pragma clang diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
#endif

// Text handling
class KeyboardInputDelegate : public IMEDelegate
{
protected:
    bool canAttachWithIME() override
    {
        return true;
    }

    bool canDetachWithIME() override
    {
        return true;
    }

    void controlKey(EventKeyboard::KeyCode keyCode) override
    {
        // Not handled at the moment
    }

    void insertText(const char* text, size_t len) override
    {
        ImGuiIO& io = ImGui::GetIO();
        for (int i = 0; i < len && text[i] != 0; ++i)
        {
            io.AddInputCharacter(text[i]);
        }
    }
};


// Android data
static char                                     g_LogTag[] = "ImGuiAndroid";

// axmol spec data
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
// end of axmol spec


struct ImGui_ImplAndroid_Data
{
    GLView*          Window;
    double                  Time;
    bool                    InstalledCallbacks;

    // ImGui_ImplAndroid_Data()   { memset(this, 0, sizeof(*this)); }

    // axmol spec data
    std::chrono::steady_clock::time_point LastFrameTime{};

    ImGuiImplCocos2dxLoadFontFun LoadCustomFont = nullptr;
    void* LoadCustomFontUserData                = nullptr;

    ProgramInfoData ProgramInfo{};
    ProgramInfoData ProgramFontInfo{};
    bool FontDeviceObjectsDirty = false;
    Texture2D* FontTexture      = nullptr;
    Mat4 Projection;
    ImVec2                  LastValidMousePos;
    EventListener* TouchListener = nullptr;

    // std::vector<std::shared_ptr<CallbackCommand>> CallbackCommands{};
    std::vector<std::shared_ptr<CustomCommand>> CustomCommands{};
    Vector<ProgramState*> ProgramStates{};

    SavedRenderStateData SavedRenderState{};
    Vec2 ViewResolution = Vec2(1920, 1080);

    KeyboardInputDelegate KeyboardInputDelegate;
};

// Backend data stored in io.BackendPlatformUserData to allow support for multiple Dear ImGui contexts
static ImGui_ImplAndroid_Data* ImGui_ImplAndroid_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplAndroid_Data*)ImGui::GetIO().BackendPlatformUserData : NULL;
}

// Forward Declarations
static void ImGui_ImplAndroid_ShutdownPlatformInterface();

static ax::Vec2 convertToUICoordinates(const Vec2& pos)
{
    auto* bd = ImGui_ImplAndroid_GetBackendData();
    ImGuiIO& io = ImGui::GetIO();
    auto origin = bd->Window->getViewPortRect().origin;
    auto uiX = (pos.x * bd->Window->getScaleX() + origin.x) / io.DisplayFramebufferScale.x;
    auto uiY = (pos.y * bd->Window->getScaleY() + origin.y) / io.DisplayFramebufferScale.y;

    return Vec2(uiX, uiY);
}

// Functions
static bool ImGui_ImplAndroid_Init(GLView* window, bool install_callbacks)
{
    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.BackendPlatformUserData == NULL && "Already initialized a platform backend!");

    // Setup backend capabilities flags
    ImGui_ImplAndroid_Data* bd = IM_NEW(ImGui_ImplAndroid_Data)();
    io.BackendPlatformUserData = (void*)bd;
    io.BackendPlatformName = "imgui_impl_android";
    //io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    //io.BackendFlags |= ImGuiBackendFlags_PlatformHasViewports;    // We can create multi-viewports on the Platform side (optional)

    bd->Window = window;
    bd->Time = 0.0;

    io.ClipboardUserData = bd->Window;

    // Our mouse update function expect PlatformHandle to be filled for the main viewport
    ImGuiViewport* main_viewport = ImGui::GetMainViewport();
    main_viewport->PlatformHandle = (void*)bd->Window;

    io.AddFocusEvent(true);

    auto* touchListener = EventListenerTouchOneByOne::create();
    touchListener->setSwallowTouches(true);
    touchListener->retain();
    bd->TouchListener = touchListener;

    touchListener->onTouchBegan = [](Touch* touch, Event* event) -> bool {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
        auto location = convertToUICoordinates(touch->getLocationInView());
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
        io.AddMouseButtonEvent(0, true);

        ImGui::UpdateHoveredWindowAndCaptureFlags();
        if (ImGui::GetIO().WantCaptureMouse)
        {
            event->stopPropagation();
        }

        // We can't check if we're actually hovering over a ImGui element, since the
        // AddMousePosEvent is not instant, it's queued. So, just return true here
        // to indicate that we're handling this event.
        return true;
    };

    touchListener->onTouchMoved = [](Touch* touch, Event* /*event*/) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
        auto location = convertToUICoordinates(touch->getLocationInView());
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
    };

    touchListener->onTouchEnded = [](Touch* touch, Event* event) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
        auto location = convertToUICoordinates(touch->getLocationInView());
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
        io.AddMouseButtonEvent(0, false);

        if (ImGui::GetIO().WantTextInput)
        {
            bd->KeyboardInputDelegate.attachWithIME();
            bd->Window->setIMEKeyboardState(true);
        }
        else
        {
            bd->KeyboardInputDelegate.detachWithIME();
            bd->Window->setIMEKeyboardState(false);
        }
    };

    touchListener->onTouchCancelled = [](Touch* touch, Event* /*event*/) {
        ImGuiIO& io = ImGui::GetIO();
        ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
        auto location = convertToUICoordinates(touch->getLocationInView());
        io.AddMousePosEvent(location.x, location.y);
        bd->LastValidMousePos = ImVec2(location.x, location.y);
        io.AddMouseButtonEvent(0, false);
    };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(touchListener, 1);

    return true;
}

IMGUI_IMPL_API void ImGui_ImplAx_RenderPlatform()
{
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }
}

void ImGui_ImplAndroid_Shutdown()
{
    ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
    IM_ASSERT(bd != NULL && "No platform backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplAndroid_ShutdownPlatformInterface();

    io.BackendPlatformName = NULL;
    io.BackendPlatformUserData = NULL;
    io.BackendRendererUserData = NULL;

    IM_DELETE(bd);
}

void ImGui_ImplAndroid_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplAndroid_Data* bd = ImGui_ImplAndroid_GetBackendData();
    IM_ASSERT(bd != NULL && "Did you call ImGui_ImplAndroid_InitForXXX()?");

    // Setup display size (every frame to accommodate for window resizing)
    int32_t window_width = bd->ViewResolution.width;
    int32_t window_height = bd->ViewResolution.height;
    int display_width = bd->Window->getFrameSize().width;
    int display_height = bd->Window->getFrameSize().height;

    io.DisplaySize = ImVec2((float)window_width, (float)window_height);
    if (window_width > 0 && window_height > 0)
        io.DisplayFramebufferScale = ImVec2((float)display_width / window_width, (float)display_height / window_height);

    // Setup time step
    struct timespec current_timespec;
    clock_gettime(CLOCK_MONOTONIC, &current_timespec);
    double current_time = (double)(current_timespec.tv_sec) + (current_timespec.tv_nsec / 1000000000.0);
    io.DeltaTime = bd->Time > 0.0 ? (float)(current_time - bd->Time) : (float)(1.0f / 60.0f);
    bd->Time = current_time;
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

// Helper structure we store in the void* RenderUserData field of each ImGuiViewport to easily retrieve our backend data.
struct ImGui_ImplAndroid_ViewportData
{
    GLView* Window;
    bool        WindowOwned;
    int         IgnoreWindowPosEventFrame;
    int         IgnoreWindowSizeEventFrame;

    ImGui_ImplAndroid_ViewportData()  { Window = NULL; WindowOwned = false; IgnoreWindowSizeEventFrame = IgnoreWindowPosEventFrame = -1; }
    ~ImGui_ImplAndroid_ViewportData() { IM_ASSERT(Window == NULL); }
};

static void ImGui_ImplAndroid_WindowCloseCallback(GLView* window)
{
    if (ImGuiViewport* viewport = ImGui::FindViewportByPlatformHandle(window))
        viewport->PlatformRequestClose = true;
}

static void ImGui_ImplAndroid_ShutdownPlatformInterface()
{
    ImGui::DestroyPlatformWindows();
}

#if defined(__clang__)
#pragma clang diagnostic pop
#endif


////////////////////////// axmol spec /////////////////////////

#define AX_PTR_CAST(v, pointer_type) reinterpret_cast<pointer_type>(v)


// fps macro
#define AX_IMGUI_DEFAULT_DELTA (1 / 60.f)
#define AX_IMGUI_MIN_DELTA (1 / 1000.f)
#define AX_IMGUI_MAX_DELTA (1 / 30.f)

enum
{
    GlfwClientApi_Axmol = 0xadee,
};

// axmol spec
bool ImGui_ImplAndroid_InitForAx(GLView* window, bool install_callbacks)
{
    return ImGui_ImplAndroid_Init(window, install_callbacks);
}

struct ImGui_ImplAx_Data
{
    // axmol spec data

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

static bool ImGui_ImplAx_CreateFontsTexture();
static void ImGui_ImplAx_DestroyFontsTexture();
static void ImGui_ImplAx_DestroyDeviceObjects();
static bool ImGui_ImplAx_CreateDeviceObjects();
static void ImGui_ImplAx_RenderWindow(ImGuiViewport* viewport, void*);
static void AddRendererCommand(const std::function<void()>& f);


static bool ImGui_ImplAx_createShaderPrograms();
static void ImGui_ImplAx_Renderer_RenderWindow(ImGuiViewport* viewport, void*);

static void ImGui_ImplAx_InitPlatformInterface()
{
    // Register platform interface (will be coupled with a renderer interface)
    ImGui_ImplAndroid_Data* bd           = ImGui_ImplAndroid_GetBackendData();
    ImGuiPlatformIO& platform_io      = ImGui::GetPlatformIO();

    platform_io.Renderer_RenderWindow = ImGui_ImplAx_Renderer_RenderWindow;
}

void ImGui_ImplAx_Init()
{
    auto bd                    = ImGui_ImplAndroid_GetBackendData();
    auto& io                   = ImGui::GetIO();
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName     = "imgui_impl_axmol";
    io.BackendFlags |=
        ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)
    // axmol spec: disable auto load and save
    io.IniFilename = nullptr;

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        ImGui_ImplAx_InitPlatformInterface();
}

void ImGui_ImplAx_Shutdown()
{
    ImGui_ImplAx_DestroyDeviceObjects();
}

IMGUI_IMPL_API void ImGui_ImplAx_NewFrame() {
    auto bd = ImGui_ImplAndroid_GetBackendData();
	//bd->CallbackCommands.clear();
    bd->CustomCommands.clear();
    bd->ProgramStates.clear();

    if (!bd->FontTexture)
        ImGui_ImplAx_CreateDeviceObjects();
    else if (bd->FontDeviceObjectsDirty)
    {  // recreate device objects, fonts also should be device objects
        ImGui_ImplAx_DestroyDeviceObjects();
        ImGui_ImplAx_CreateDeviceObjects();
    }
}

IMGUI_IMPL_API void ImGui_ImplAx_SetCustomFontLoader(ImGuiImplCocos2dxLoadFontFun fun, void* userdata)
{
    auto bd                    = ImGui_ImplAndroid_GetBackendData();
    bd->LoadCustomFont         = fun;
    bd->LoadCustomFontUserData = userdata;
}

IMGUI_IMPL_API void* ImGui_ImplAx_GetFontsTexture()
{
    auto bd = ImGui_ImplAndroid_GetBackendData();
    return bd->FontTexture;
}

IMGUI_IMPL_API void ImGui_ImplAx_SetDeviceObjectsDirty()
{
    auto bd                    = ImGui_ImplAndroid_GetBackendData();
    bd->FontDeviceObjectsDirty = true;
}

IMGUI_IMPL_API void ImGui_ImplAx_SetViewResolution(float width, float height)
{
    auto bd = ImGui_ImplAndroid_GetBackendData();
    bd->ViewResolution = Vec2(width, height);
}

static bool ImGui_ImplAx_CreateDeviceObjects()
{
    auto bd = ImGui_ImplAndroid_GetBackendData();
    if (bd->LoadCustomFont)
        bd->LoadCustomFont(bd->LoadCustomFontUserData);

    ImGui_ImplAx_createShaderPrograms();
    ImGui_ImplAx_CreateFontsTexture();

    bd->FontDeviceObjectsDirty = false;
    return true;
}

static void ImGui_ImplAx_DestroyDeviceObjects()
{
    auto bd = ImGui_ImplAndroid_GetBackendData();
    Director::getInstance()->getEventDispatcher()->removeEventListener(bd->TouchListener);
    AX_SAFE_RELEASE_NULL(bd->TouchListener);

    ImGui_ImplAx_DestroyFontsTexture();

    auto pm = ProgramManager::getInstance();
    pm->unloadProgram(bd->ProgramInfo.program);
    pm->unloadProgram(bd->ProgramFontInfo.program);
}

static bool ImGui_ImplAx_createShaderPrograms()
{
    auto bd = ImGui_ImplAndroid_GetBackendData();

    auto pm = ProgramManager::getInstance();

    bd->ProgramInfo.program     = pm->loadProgram("custom/imgui_sprite_vs"sv, ax::positionTextureColor_frag);
    bd->ProgramFontInfo.program = pm->loadProgram("custom/imgui_sprite_vs"sv, "custom/imgui_font_fs");

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
        layout.setAttrib("a_position", p->position, VertexFormat::FLOAT2, 0, false);
        layout.setAttrib("a_texCoord", p->uv, VertexFormat::FLOAT2, offsetof(ImDrawVert, uv), false);
        layout.setAttrib("a_color", p->color, VertexFormat::UBYTE4, offsetof(ImDrawVert, col), true);
        layout.setStride(sizeof(ImDrawVert));
    }

    return true;
}

bool ImGui_ImplAx_CreateFontsTexture()
{
    auto bd = ImGui_ImplAndroid_GetBackendData();
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
    // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small)
    // because it is more likely to be compatible with user's existing shaders.
    // If your ImTextureId represent a higher-level concept than just a GL texture id,
    // consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    AX_SAFE_RELEASE(bd->FontTexture);
    bd->FontTexture = new Texture2D();

    bd->FontTexture->setAntiAliasTexParameters();
    bd->FontTexture->initWithData(pixels, width * height, backend::PixelFormat::R8, width, height);
    io.Fonts->TexID = (ImTextureID)bd->FontTexture;
    return true;
}

IMGUI_IMPL_API void ImGui_ImplAx_DestroyFontsTexture()
{
    auto bd = ImGui_ImplAndroid_GetBackendData();
    if (bd->FontTexture)
    {
        ImGui::GetIO().Fonts->TexID = nullptr;
        AX_SAFE_RELEASE_NULL(bd->FontTexture);
    }
}

static void AddRendererCommand(const std::function<void()>& f)
{
    auto bd             = ImGui_ImplAndroid_GetBackendData();
    const auto renderer = Director::getInstance()->getRenderer();
    auto cmd            = renderer->nextCallbackCommand();
    cmd->init(0.f);
    cmd->func = f;
    renderer->addCommand(cmd);
    //bd->CallbackCommands.push_back(cmd);
}

static void ImGui_ImplAx_SaveRenderState(ax::Renderer* renderer)
{
    AddRendererCommand([renderer]() {
        auto bd                          = ImGui_ImplAndroid_GetBackendData();
        bd->SavedRenderState.cull        = renderer->getCullMode();
        bd->SavedRenderState.vp          = renderer->getViewport();
        bd->SavedRenderState.scissorTest = renderer->getScissorTest();
        bd->SavedRenderState.scissorRect = renderer->getScissorRect();
        bd->SavedRenderState.depthTest   = renderer->getDepthTest();
    });
}

static void ImGui_ImplAx_SetupRenderState(ax::Renderer* renderer,
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

    auto bd = ImGui_ImplAndroid_GetBackendData();
    Mat4::createOrthographicOffCenter(L, R, B, T, -1.f, 1.f, &bd->Projection);
}

static void ImGui_ImplAx_RestoreRenderState(ax::Renderer* renderer)
{
    AddRendererCommand([renderer]() {
        auto bd = ImGui_ImplAndroid_GetBackendData();
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

IMGUI_IMPL_API void ImGui_ImplAx_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays
    // (screen coordinates != framebuffer coordinates)
    int fb_width  = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    const auto renderer = Director::getInstance()->getRenderer();

    ImGui_ImplAx_SaveRenderState(renderer);

    ImGui_ImplAx_SetupRenderState(renderer, draw_data, fb_width, fb_height);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off   = draw_data->DisplayPos;        // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale;  // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];

        // Upload vertex/index buffers
        const auto vsize = cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
        IM_ASSERT(vsize > 0);
        auto vbuffer = backend::DriverBase::getInstance()->newBuffer(vsize, BufferType::VERTEX, BufferUsage::STATIC);
        vbuffer->autorelease();
        vbuffer->updateData(cmd_list->VtxBuffer.Data, vsize);
        const auto isize = cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
        IM_ASSERT(isize > 0);
        auto ibuffer = backend::DriverBase::getInstance()->newBuffer(isize, BufferType::INDEX, BufferUsage::STATIC);
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
                    ImGui_ImplAx_SetupRenderState(renderer, draw_data, fb_width, fb_height);
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

                    auto bd = ImGui_ImplAndroid_GetBackendData();

                    if (typeid(*((Object*)pcmd->TextureId)) == typeid(Texture2D))
                    {
                        auto tex = AX_PTR_CAST(pcmd->TextureId, Texture2D*);
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
                        desc.programState->setSharedVertexLayout(&pinfo->layout);
                        desc.programState->setUniform(pinfo->projection, &bd->Projection, sizeof(Mat4));
                        desc.programState->setTexture(pinfo->texture, 0, tex->getBackendTexture());
                        // In order to composite our output buffer we need to preserve alpha
                        desc.blendDescriptor.sourceAlphaBlendFactor = BlendFactor::ONE;
                        // set vertex/index buffer
                        cmd->setIndexBuffer(ibuffer, IMGUI_INDEX_FORMAT);
                        cmd->setVertexBuffer(vbuffer);
                        cmd->setDrawType(CustomCommand::DrawType::ELEMENT);
                        cmd->setPrimitiveType(PrimitiveType::TRIANGLE);
                        cmd->setIndexDrawInfo(pcmd->IdxOffset, pcmd->ElemCount);
                        renderer->addCommand(cmd.get());
                    }
                    else
                    {
                        auto node     = AX_PTR_CAST(pcmd->TextureId, Node*);
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
        }
    }

    ImGui_ImplAx_RestoreRenderState(renderer);
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports
// simultaneously. If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you
// completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

static void ImGui_ImplAx_Renderer_RenderWindow(ImGuiViewport* viewport, void*)
{
    if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
    {
        const auto renderer = Director::getInstance()->getRenderer();
        renderer->clear(ClearFlag::COLOR, {0, 0, 0, 1}, 1, 0, 0);
    }
    ImGui_ImplAx_RenderDrawData(viewport->DrawData);
}

