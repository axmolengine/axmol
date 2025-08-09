#include "imgui_impl_axmol.h"

#include "base/Director.h"
#include "base/Data.h"
#if !defined(__ANDROID__)
#    include "platform/RenderViewImpl.h"
#endif
#include "renderer/backend/Program.h"
#include "renderer/backend/ProgramState.h"
#include "renderer/backend/ProgramManager.h"
#include "renderer/Shaders.h"
#include "renderer/Renderer.h"
#include "renderer/CallbackCommand.h"
#include "renderer/backend/DriverBase.h"
#include "renderer/backend/Buffer.h"

using namespace ax;
using namespace ax::backend;

constexpr IndexFormat IMGUI_INDEX_FORMAT = sizeof(ImDrawIdx) == 2 ? IndexFormat::U_SHORT : IndexFormat::U_INT;

struct ProgramInfoData
{
    ProgramInfoData() { layout = DriverBase::getInstance()->createVertexLayout(); }
    ~ProgramInfoData() { AX_SAFE_DELETE(layout); }
    Program* program = nullptr;
    // Uniforms location
    UniformLocation texture{};
    UniformLocation projection{};
    // Vertex attributes location
    const VertexInputDesc* position{nullptr};
    const VertexInputDesc* uv{nullptr};
    const VertexInputDesc* color{nullptr};
    VertexLayout* layout{nullptr};
};

struct SavedRenderStateData
{
    backend::CullMode cull{};
    Viewport vp{};
    ScissorRect scissorRect{};
    bool scissorTest{};
    bool depthTest{};
};

struct ImGui_ImplAxmol_Data
{
    // axmol spec data, TODO: new type: ImGui_ImplAxmol_Data
    std::chrono::steady_clock::time_point LastFrameTime{};

    ImGuiImplAxmolUpdateFontsFn UpdateFontsFunc = nullptr;
    void* UpdateFontsFuncUserData               = nullptr;
    bool FontsDirty                             = false;

    ProgramInfoData ProgramInfo{};
    Mat4 Projection;

    // std::vector<std::shared_ptr<CallbackCommand>> CallbackCommands{};
    std::vector<std::shared_ptr<CustomCommand>> CustomCommands{};
    Vector<ProgramState*> ProgramStates{};

    SavedRenderStateData SavedRenderState{};
    ax::Data TempBuffer;

    Vec2 ViewResolution = Vec2(1920, 1080);
};

// Backend data stored in io.BackendRendererUserData to allow support for multiple Dear ImGui contexts
// It is STRONGLY preferred that you use docking branch with multi-viewports (== single Dear ImGui context + multiple
// windows) instead of multiple Dear ImGui contexts.
static ImGui_ImplAxmol_Data* ImGui_ImplAxmol_GetBackendData()
{
    return ImGui::GetCurrentContext() ? (ImGui_ImplAxmol_Data*)ImGui::GetIO().BackendRendererUserData : nullptr;
}

// Forward Declarations
static void ImGui_ImplAxmol_InitMultiViewportSupport();
static void ImGui_ImplAxmol_ShutdownMultiViewportSupport();

static void ImGui_ImplAxmol_DestroyTexture(ImTextureData* tex)
{
    auto texture = (Texture2D*)(intptr_t)tex->TexID;
    delete texture;

    // Clear identifiers and mark as destroyed (in order to allow e.g. calling InvalidateDeviceObjects while
    // running)
    tex->SetTexID(ImTextureID_Invalid);
    tex->SetStatus(ImTextureStatus_Destroyed);
}

static void ImGui_ImplAxmol_UpdateTexture(ImTextureData* tex)
{
    if (tex->Status == ImTextureStatus_WantCreate)
    {
        IM_ASSERT(tex->TexID == 0 && tex->BackendUserData == nullptr);
        IM_ASSERT(tex->Format == ImTextureFormat_RGBA32);
        const void* pixels = tex->GetPixels();

        auto texture = new Texture2D();
        texture->initWithData(pixels, tex->Width * tex->Height * 4, backend::PixelFormat::RGBA8, tex->Width,
                              tex->Height, true);

        backend::SamplerDescriptor descriptor(backend::SamplerFilter::LINEAR,              // magFilter
                                              backend::SamplerFilter::LINEAR,              // minFilter
                                              backend::SamplerAddressMode::CLAMP_TO_EDGE,  // sAddressMode
                                              backend::SamplerAddressMode::CLAMP_TO_EDGE   // tAddressMode
        );
        texture->getBackendTexture()->updateSamplerDescriptor(descriptor);

        // Store identifiers
        tex->SetTexID((ImTextureID)(intptr_t)texture);
        tex->SetStatus(ImTextureStatus_OK);
    }
    else if (tex->Status == ImTextureStatus_WantUpdates)
    {
        ImGui_ImplAxmol_Data* bd = ImGui_ImplAxmol_GetBackendData();
        for (ImTextureRect& r : tex->Updates)
        {
            const int src_pitch = r.w * tex->BytesPerPixel;
            bd->TempBuffer.resize(r.h * src_pitch);
            char* out_p = (char*)bd->TempBuffer.getBytes();
            for (int y = 0; y < r.h; y++, out_p += src_pitch)
                memcpy(out_p, tex->GetPixelsAt(r.x, r.y + y), src_pitch);
            auto texture = (Texture2D*)(intptr_t)tex->TexID;
            IM_ASSERT(texture != nullptr);

            texture->updateWithSubData(bd->TempBuffer.getBytes(), r.x, r.y, r.w, r.h);
        }

        tex->SetStatus(ImTextureStatus_OK);
    }
    else if (tex->Status == ImTextureStatus_WantDestroy && tex->UnusedFrames > 0)
        ImGui_ImplAxmol_DestroyTexture(tex);
}

static void ImGui_ImplAxmol_SetupRenderState(ax::Renderer* renderer, ImDrawData* draw_data, int fb_width, int fb_height)
{
    ImGui_ImplAxmol_PostCommand([=]() {
        renderer->setCullMode(backend::CullMode::NONE);
        renderer->setDepthTest(false);
        renderer->setScissorTest(true);
        renderer->setViewPort(0, 0, fb_width, fb_height);
    });

    // Catch up with texture updates. Most of the times, the list will have 1 element with an OK status, aka nothing to
    // do. (This almost always points to ImGui::GetPlatformIO().Textures[] but is part of ImDrawData to allow overriding
    // or disabling texture updates).
    if (draw_data->Textures != nullptr)
        for (ImTextureData* tex : *draw_data->Textures)
            if (tex->Status != ImTextureStatus_OK)
                ImGui_ImplAxmol_UpdateTexture(tex);

    const auto L = draw_data->DisplayPos.x;
    const auto R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    const auto T = draw_data->DisplayPos.y;
    const auto B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

    auto bd = ImGui_ImplAxmol_GetBackendData();
    Mat4::createOrthographicOffCenter(L, R, B, T, -1.f, 1.f, &bd->Projection);
}

static void ImGui_ImplAxmol_RestoreRenderState(ax::Renderer* renderer)
{
    ImGui_ImplAxmol_PostCommand([renderer]() {
        auto bd = ImGui_ImplAxmol_GetBackendData();
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

static void ImGui_ImplAxmol_SaveRenderState(ax::Renderer* renderer)
{
    ImGui_ImplAxmol_PostCommand([renderer]() {
        auto bd                          = ImGui_ImplAxmol_GetBackendData();
        bd->SavedRenderState.cull        = renderer->getCullMode();
        bd->SavedRenderState.vp          = renderer->getViewport();
        bd->SavedRenderState.scissorTest = renderer->getScissorTest();
        bd->SavedRenderState.scissorRect = renderer->getScissorRect();
        bd->SavedRenderState.depthTest   = renderer->getDepthTest();
    });
}

IMGUI_IMPL_API void ImGui_ImplAxmol_Init()
{
    ImGuiIO& io = ImGui::GetIO();
    IMGUI_CHECKVERSION();
    IM_ASSERT(io.BackendRendererUserData == nullptr && "Already initialized a renderer backend!");

    // Setup backend capabilities flags
    ImGui_ImplAxmol_Data* bd   = IM_NEW(ImGui_ImplAxmol_Data)();
    io.BackendRendererUserData = (void*)bd;
    io.BackendRendererName     = "imgui_impl_axmol";

#if AX_RENDER_API == AX_RENDER_API_GL && (!defined(AX_GLES_PROFILE) || AX_GLES_PROFILE >= 300)
    io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;  // We can honor the ImDrawCmd::VtxOffset field,
                                                                // allowing for large meshes.
#endif
    io.BackendFlags |=
        ImGuiBackendFlags_RendererHasTextures;  // We can honor ImGuiPlatformIO::Textures[] requests during render.
    io.BackendFlags |=
        ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

    ImGuiPlatformIO& platform_io         = ImGui::GetPlatformIO();
    platform_io.Renderer_TextureMaxWidth = platform_io.Renderer_TextureMaxHeight =
        backend::DriverBase::getInstance()->getMaxTextureSize();

    io.IniFilename = nullptr;

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        ImGui_ImplAxmol_InitMultiViewportSupport();
}

IMGUI_IMPL_API void ImGui_ImplAxmol_Shutdown()
{
    ImGui_ImplAxmol_Data* bd = ImGui_ImplAxmol_GetBackendData();
    IM_ASSERT(bd != nullptr && "No renderer backend to shutdown, or already shutdown?");
    ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplAxmol_ShutdownMultiViewportSupport();
    ImGui_ImplAxmol_DestroyDeviceObjects();
    io.BackendRendererName     = nullptr;
    io.BackendRendererUserData = nullptr;
    io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasTextures |
                         ImGuiBackendFlags_RendererHasViewports);
    IM_DELETE(bd);
}

IMGUI_IMPL_API void ImGui_ImplAxmol_NewFrame()
{
    auto bd = ImGui_ImplAxmol_GetBackendData();
    // bd->CallbackCommands.clear();
    bd->CustomCommands.clear();
    bd->ProgramStates.clear();

    if (!bd->ProgramInfo.program)
        ImGui_ImplAxmol_CreateDeviceObjects();
    if (bd->FontsDirty)
    {
        // since imgui-1.92.0, rebuild font atlas at here
        if (bd->UpdateFontsFunc)
            bd->UpdateFontsFunc(bd->UpdateFontsFuncUserData);

        bd->FontsDirty = false;
    }
}

IMGUI_IMPL_API void ImGui_ImplAxmol_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays
    // (screen coordinates != framebuffer coordinates)
    int fb_width  = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

    const auto renderer = Director::getInstance()->getRenderer();

    ImGui_ImplAxmol_SaveRenderState(renderer);

    ImGui_ImplAxmol_SetupRenderState(renderer, draw_data, fb_width, fb_height);

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
        auto vbuffer = backend::DriverBase::getInstance()->createBuffer(vsize, BufferType::VERTEX, BufferUsage::STATIC);
        vbuffer->autorelease();
        vbuffer->updateData(cmd_list->VtxBuffer.Data, vsize);
        const auto isize = cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
        IM_ASSERT(isize > 0);
        auto ibuffer = backend::DriverBase::getInstance()->createBuffer(isize, BufferType::INDEX, BufferUsage::STATIC);
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
                    ImGui_ImplAxmol_SetupRenderState(renderer, draw_data, fb_width, fb_height);
                else
                {
                    ImGui_ImplAxmol_PostCommand([=]() { pcmd->UserCallback(cmd_list, pcmd); });
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
                    ImGui_ImplAxmol_PostCommand([=]() {
                        renderer->setScissorRect(clip_rect.x, fb_height - clip_rect.w, clip_rect.z - clip_rect.x,
                                                 clip_rect.w - clip_rect.y);
                    });

                    auto bd = ImGui_ImplAxmol_GetBackendData();

                    if (typeid(*((Object*)pcmd->TexRef.GetTexID())) == typeid(Texture2D))
                    {
                        auto tex = (Texture2D*)(uintptr_t)(pcmd->TexRef.GetTexID());
                        auto cmd = std::make_shared<CustomCommand>();
                        bd->CustomCommands.push_back(cmd);
                        cmd->init(0.f, BlendFunc::ALPHA_NON_PREMULTIPLIED);
                        const auto pinfo =
                            &bd->ProgramInfo;  // tex == bd->FontTexture ? &bd->ProgramFontInfo : &bd->ProgramInfo;
                        // create new ProgramState
                        auto state = new ProgramState(pinfo->program);
                        state->autorelease();
                        bd->ProgramStates.pushBack(state);
                        auto& desc        = cmd->getPipelineDescriptor();
                        desc.programState = state;
                        // setup attributes for ImDrawVert
                        desc.programState->setSharedVertexLayout(pinfo->layout);
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
                        auto node     = (Node*)(uintptr_t)pcmd->TexRef.GetTexID();
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

    ImGui_ImplAxmol_RestoreRenderState(renderer);
}

IMGUI_IMPL_API void ImGui_ImplAxmol_RenderPlatform()
{
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();

#if AX_RENDER_API == AX_RENDER_API_GL && !defined(__ANDROID__)
        // restore context
        GLFWwindow* prev_current_context = glfwGetCurrentContext();
        ImGui_ImplAxmol_PostCommand([=]() { ImGui_ImplAxmol_MakeCurrent(prev_current_context); });
#endif
    }
}

IMGUI_IMPL_API void ImGui_ImplAxmol_MakeCurrent(GLFWwindow* window)
{
#if !defined(__ANDROID__)
    glfwMakeContextCurrent(window);

#    if AX_RENDER_API == AX_RENDER_API_GL
    auto p = glfwGetWindowUserPointer(window);
    if (!p)
    {
        p = new OpenGLState();
        glfwSetWindowUserPointer(window, p);
#        if AX_GLES_PROFILE != 200
        // this is a new OpenGLContext, create default VAO for it when core profile enabled
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
#        endif
    }

    if (backend::__gl != p)
    {
        backend::__gl->resetVAO();
        backend::__gl = (OpenGLState*)p;
    }
#    endif
#endif
}

IMGUI_IMPL_API void ImGui_ImplAxmol_PostCommand(std::function<void()>&& func)
{
    const auto renderer = Director::getInstance()->getRenderer();
    auto cmd            = renderer->nextCallbackCommand();
    cmd->init(0.f);
    cmd->func = std::forward<std::function<void()>>(func);
    renderer->addCommand(cmd);
}

IMGUI_IMPL_API bool ImGui_ImplAxmol_CreateDeviceObjects()
{
    auto bd = ImGui_ImplAxmol_GetBackendData();

    auto pm = ProgramManager::getInstance();

    bd->ProgramInfo.program = pm->loadProgram("custom/imgui_sprite_vs"sv, ax::positionTextureColor_frag);

    IM_ASSERT(bd->ProgramInfo.program);

    if (!bd->ProgramInfo.program)
        return false;

    auto& info      = bd->ProgramInfo;
    info.texture    = info.program->getUniformLocation(TEXTURE);
    info.projection = info.program->getUniformLocation(MVP_MATRIX);
    info.position   = info.program->getVertexInputDesc(POSITION);
    info.uv         = info.program->getVertexInputDesc(TEXCOORD);
    info.color      = info.program->getVertexInputDesc(COLOR);
    IM_ASSERT(bool(info.texture));
    IM_ASSERT(bool(info.projection));
    IM_ASSERT(!!info.position);
    IM_ASSERT(!!info.uv);
    IM_ASSERT(!!info.color);
    auto layout = info.layout;
    layout->setAttrib("a_position", info.position, VertexFormat::FLOAT2, 0, false);
    layout->setAttrib("a_texCoord", info.uv, VertexFormat::FLOAT2, offsetof(ImDrawVert, uv), false);
    layout->setAttrib("a_color", info.color, VertexFormat::UBYTE4, offsetof(ImDrawVert, col), true);
    layout->setStride(sizeof(ImDrawVert));

    return true;
}

IMGUI_IMPL_API void ImGui_ImplAxmol_DestroyDeviceObjects()
{
    auto pm = ProgramManager::getInstance();

    auto bd = ImGui_ImplAxmol_GetBackendData();

    pm->unloadProgram(bd->ProgramInfo.program);

    // Destroy all textures
    for (ImTextureData* tex : ImGui::GetPlatformIO().Textures)
        if (tex->RefCount == 1)
            ImGui_ImplAxmol_DestroyTexture(tex);
}

IMGUI_IMPL_API void ImGui_ImplAxmol_SetUpdateFontsFunc(ImGuiImplAxmolUpdateFontsFn func, void* userdata)
{
    auto bd                     = ImGui_ImplAxmol_GetBackendData();
    bd->UpdateFontsFunc         = func;
    bd->UpdateFontsFuncUserData = userdata;
}

IMGUI_IMPL_API void ImGui_ImplAxmol_MarkFontsDirty()
{
    auto bd        = ImGui_ImplAxmol_GetBackendData();
    bd->FontsDirty = true;
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the backend to create and handle multiple viewports
// simultaneously. If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you
// completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

static void ImGui_ImplAxmol_RenderWindow(ImGuiViewport* viewport, void*)
{
    if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
    {
        const auto renderer = Director::getInstance()->getRenderer();
        renderer->clear(ClearFlag::COLOR, {0, 0, 0, 1}, 1, 0, 0);
    }
    ImGui_ImplAxmol_RenderDrawData(viewport->DrawData);
}

static void ImGui_ImplAxmol_InitMultiViewportSupport()
{
    ImGuiPlatformIO& platform_io      = ImGui::GetPlatformIO();
    platform_io.Renderer_RenderWindow = ImGui_ImplAxmol_RenderWindow;
}

static void ImGui_ImplAxmol_ShutdownMultiViewportSupport()
{
    ImGui::DestroyPlatformWindows();
}

#if !defined(__ANDROID__)
IMGUI_IMPL_API void ImGui_ImplAxmol_SetViewResolution(float width, float height)
{
    // Resize (expand) window
    auto* view = (ax::RenderViewImpl*)ax::Director::getInstance()->getRenderView();
    view->setWindowed(width, height);
}
#endif

//-----------------------------------------------------------------------------
