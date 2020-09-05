#include "imgui_impl_cocos2dx.h"
#include "cocos2d.h"
#include "renderer/backend/Backend.h"
#include <functional>

#ifdef CC_PLATFORM_PC
// GLFW
#ifdef _WIN32
	#include "glfw3.h"
	#undef APIENTRY
	#ifndef GLFW_EXPOSE_NATIVE_WIN32
		#define GLFW_EXPOSE_NATIVE_WIN32
	#endif
	#include "glfw3native.h"
#else
	#include <glfw3.h>
#endif // _WIN32

static_assert(
	GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3300,
	"glfw version should be 3.3+");

// 3.3.1+ Fixed: Resizing window repositions it on MacOS #1553
#define GLFW_HAS_OSX_WINDOW_POS_FIX (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 + GLFW_VERSION_REVISION * 10 >= 3310)
#ifdef GLFW_RESIZE_NESW_CURSOR
	#define GLFW_HAS_NEW_CURSORS (GLFW_VERSION_MAJOR * 1000 + GLFW_VERSION_MINOR * 100 >= 3400)
#else
	#define GLFW_HAS_NEW_CURSORS (0)
#endif

#endif // CC_PLATFORM_PC

using namespace cocos2d;
using namespace backend;

// GLFW data
static double               g_Time = 0.0;
static bool                 g_MouseJustPressed[5] = { false, false, false, false, false };
static ImVec2               g_CursorPos = ImVec2(-FLT_MAX, -FLT_MAX);

#ifdef CC_PLATFORM_PC
static GLFWcursor*          g_MouseCursors[ImGuiMouseCursor_COUNT] = { nullptr };
// Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
static GLFWmousebuttonfun   g_PrevUserCallbackMousebutton = nullptr;
static GLFWscrollfun        g_PrevUserCallbackScroll = nullptr;
static GLFWkeyfun           g_PrevUserCallbackKey = nullptr;
static GLFWcharfun          g_PrevUserCallbackChar = nullptr;

static GLFWmonitorfun       g_PrevUserCallbackMonitor = nullptr;

static bool                 g_WantUpdateMonitors = true;

// Forward Declarations
static void ImGui_ImplGlfw_InitPlatformInterface();
static void ImGui_ImplGlfw_ShutdownPlatformInterface();
static void ImGui_ImplGlfw_UpdateMonitors();

#endif // CC_PLATFORM_PC

// fps macro
#define CC_IMGUI_DEFAULT_DELTA (1 / 60.f)
#define CC_IMGUI_MIN_DELTA (1 / 1000.f)
#define CC_IMGUI_MAX_DELTA (1 / 30.f)

struct ProgramInfo
{
	Program* program = nullptr;
	// Uniforms location
	UniformLocation texture;
	UniformLocation projection;
	// Vertex attributes location
	int position = 0;
	int uv = 0;
	int color = 0;
	VertexLayout layout;
};
static ProgramInfo g_ProgramInfo;
static ProgramInfo g_ProgramFontInfo;
static Texture2D* g_FontTexture = nullptr;
static Mat4 g_Projection;
constexpr IndexFormat g_IndexFormat = sizeof(ImDrawIdx) == 2 ? IndexFormat::U_SHORT : IndexFormat::U_INT;

static std::vector<std::shared_ptr<CallbackCommand>> g_CallbackCommands;
static std::vector<std::shared_ptr<CustomCommand>> g_CustomCommands;
static Vector<ProgramState*> g_ProgramStates;

static void AddRendererCommand(const std::function<void()>& f)
{
	const auto renderer = Director::getInstance()->getRenderer();
	auto cmd = std::make_shared<CallbackCommand>();
	g_CallbackCommands.push_back(cmd);
	cmd->init(0.f);
	cmd->func = f;
	renderer->addCommand(cmd.get());
}

#ifdef CC_PLATFORM_PC
static GLFWwindow* ImGui_ImplCocos2dx_GetWindow()
{
	const auto glv = (GLViewImpl*)Director::getInstance()->getOpenGLView();
	return glv->getWindow();
}
#endif // CC_PLATFORM_PC

static void ImGui_ImplCocos2dx_SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height)
{
	const auto renderer = Director::getInstance()->getRenderer();
	// setup
	AddRendererCommand([=]()
	{
		renderer->setCullMode(backend::CullMode::NONE);
		renderer->setDepthTest(false);
		renderer->setScissorTest(true);
		renderer->setViewPort(0, 0, fb_width, fb_height);
	});

	const auto L = draw_data->DisplayPos.x;
    const auto R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
    const auto T = draw_data->DisplayPos.y;
    const auto B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
	Mat4::createOrthographicOffCenter(L, R, B, T, -1.f, 1.f, &g_Projection);
}

struct SavedRenderState
{
	backend::CullMode cull;
	Viewport vp;
	ScissorRect scissorRect;
	bool scissorTest;
	bool depthTest;
};
static SavedRenderState g_SavedRenderState;

void ImGui_ImplCocos2dx_RenderDrawData(ImDrawData* draw_data)
{
    // Avoid rendering when minimized, scale coordinates for retina displays
	// (screen coordinates != framebuffer coordinates)
    int fb_width = (int)(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
    int fb_height = (int)(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
    if (fb_width <= 0 || fb_height <= 0)
        return;

	const auto renderer = Director::getInstance()->getRenderer();
	// store
	AddRendererCommand([renderer]()
	{
		g_SavedRenderState.cull = renderer->getCullMode();
		g_SavedRenderState.vp = renderer->getViewport();
		g_SavedRenderState.scissorTest = renderer->getScissorTest();
		g_SavedRenderState.scissorRect = renderer->getScissorRect();
		g_SavedRenderState.depthTest = renderer->getDepthTest();
	});

    ImGui_ImplCocos2dx_SetupRenderState(draw_data, fb_width, fb_height);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        size_t ibuffer_offset = 0;

        // Upload vertex/index buffers
		const auto vsize = cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
		IM_ASSERT(vsize > 0);
		auto vbuffer = backend::Device::getInstance()->newBuffer(
			vsize, BufferType::VERTEX, BufferUsage::STATIC);
		vbuffer->autorelease();
		vbuffer->updateData(cmd_list->VtxBuffer.Data, vsize);
		const auto isize = cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
		IM_ASSERT(isize > 0);
		auto ibuffer = backend::Device::getInstance()->newBuffer(
			isize, BufferType::INDEX, BufferUsage::STATIC);
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
                    ImGui_ImplCocos2dx_SetupRenderState(draw_data, fb_width, fb_height);
                else
                {
					AddRendererCommand([=]()
					{
						pcmd->UserCallback(cmd_list, pcmd);
					});
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

                if (clip_rect.x < fb_width && clip_rect.y < fb_height &&
					clip_rect.z >= 0.0f && clip_rect.w >= 0.0f)
                {
					// Apply scissor/clipping rectangle
					AddRendererCommand([=]()
					{
						renderer->setScissorRect(
							clip_rect.x,
							fb_height - clip_rect.w,
							clip_rect.z - clip_rect.x,
							clip_rect.w - clip_rect.y);
					});

					if (typeid(*((Ref*)pcmd->TextureId)) == typeid(Texture2D))
					{
						auto tex = (Texture2D*)pcmd->TextureId;
                		auto cmd = std::make_shared<CustomCommand>();
						g_CustomCommands.push_back(cmd);
						cmd->init(0.f, BlendFunc::ALPHA_NON_PREMULTIPLIED);
						const auto pinfo = tex == g_FontTexture ? &g_ProgramFontInfo : &g_ProgramInfo;
						// create new ProgramState
						auto state = new ProgramState(pinfo->program);
						state->autorelease();
						g_ProgramStates.pushBack(state);
						auto& desc = cmd->getPipelineDescriptor();
						desc.programState = state;
						// setup attributes for ImDrawVert
						*desc.programState->getVertexLayout() = pinfo->layout;
						desc.programState->setUniform(pinfo->projection, &g_Projection, sizeof(Mat4));
						desc.programState->setTexture(pinfo->texture, 0, tex->getBackendTexture());
						// set vertex/index buffer
						cmd->setIndexBuffer(ibuffer, g_IndexFormat);
						cmd->setVertexBuffer(vbuffer);
						cmd->setDrawType(CustomCommand::DrawType::ELEMENT);
						cmd->setPrimitiveType(PrimitiveType::TRIANGLE);
						cmd->setIndexDrawInfo(ibuffer_offset, pcmd->ElemCount);
						renderer->addCommand(cmd.get());						
					}
					else
					{
						auto node = (Node*)pcmd->TextureId;
						const auto tr = node->getNodeToParentTransform();
						node->setVisible(true);
						node->setNodeToParentTransform(tr);
						const auto& proj = Director::getInstance()->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);
						node->visit(Director::getInstance()->getRenderer(), proj.getInversed() * g_Projection, 0);
						node->setVisible(false);
					}
                }
            }
			ibuffer_offset += pcmd->ElemCount;
        }
    }
	// restore
	AddRendererCommand([renderer]()
	{
		renderer->setCullMode(g_SavedRenderState.cull);
		auto& vp = g_SavedRenderState.vp;
		renderer->setViewPort(vp.x, vp.y, vp.w, vp.h);
		renderer->setScissorTest(g_SavedRenderState.scissorTest);
		auto& sc = g_SavedRenderState.scissorRect;
		renderer->setScissorRect(sc.x, sc.y, sc.width, sc.height);
		renderer->setDepthTest(g_SavedRenderState.depthTest);
	});
}

void ImGui_ImplCocos2dx_RenderPlatform()
{
#ifdef CC_PLATFORM_PC
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
		AddRendererCommand([=]()
		{
			glfwMakeContextCurrent(backup_current_context);
		});
	}
#endif
}

static const char* ImGui_ImplCocos2dx_GetClipboardText(void* user_data)
{
#ifdef CC_PLATFORM_PC
    return glfwGetClipboardString((GLFWwindow*)user_data);
#else
	return "";
#endif
}

static void ImGui_ImplCocos2dx_SetClipboardText(void* user_data, const char* text)
{
#ifdef CC_PLATFORM_PC
    glfwSetClipboardString((GLFWwindow*)user_data, text);
#else
#endif
}

#ifdef CC_PLATFORM_PC

void ImGui_ImplCocos2dx_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (g_PrevUserCallbackMousebutton != nullptr && window == ImGui_ImplCocos2dx_GetWindow())
        g_PrevUserCallbackMousebutton(window, button, action, mods);

    if (action == GLFW_PRESS && button >= 0 && button < IM_ARRAYSIZE(g_MouseJustPressed))
        g_MouseJustPressed[button] = true;
}

void ImGui_ImplCocos2dx_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (g_PrevUserCallbackScroll != nullptr && window == ImGui_ImplCocos2dx_GetWindow())
        g_PrevUserCallbackScroll(window, xoffset, yoffset);

    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheelH += (float)xoffset;
    io.MouseWheel += (float)yoffset;
}

void ImGui_ImplCocos2dx_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (g_PrevUserCallbackKey != nullptr && window == ImGui_ImplCocos2dx_GetWindow())
        g_PrevUserCallbackKey(window, key, scancode, action, mods);

	if (key < 0)
		return;
    ImGuiIO& io = ImGui::GetIO();
    if (action == GLFW_PRESS)
        io.KeysDown[key] = true;
    if (action == GLFW_RELEASE)
        io.KeysDown[key] = false;

    // Modifiers are not reliable across systems
    io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
    io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
    io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	io.KeySuper = false;
#else
	io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
#endif
}

void ImGui_ImplCocos2dx_CharCallback(GLFWwindow* window, unsigned int c)
{
    if (g_PrevUserCallbackChar != nullptr && window == ImGui_ImplCocos2dx_GetWindow())
        g_PrevUserCallbackChar(window, c);

    ImGuiIO& io = ImGui::GetIO();
    io.AddInputCharacter(c);
}

void ImGui_ImplGlfw_MonitorCallback(GLFWmonitor*, int)
{
	g_WantUpdateMonitors = true;
}

#endif

bool ImGui_ImplCocos2dx_CreateFontsTexture()
{
    // Build texture atlas
    ImGuiIO& io = ImGui::GetIO();
    unsigned char* pixels;
    int width, height;
	// Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small)
	// because it is more likely to be compatible with user's existing shaders.
	// If your ImTextureId represent a higher-level concept than just a GL texture id,
	// consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

	CC_SAFE_RELEASE(g_FontTexture);
	g_FontTexture = new Texture2D();

	g_FontTexture->setAntiAliasTexParameters();
	g_FontTexture->initWithData(pixels, width*height,
		backend::PixelFormat::A8, width, height, Size(width, height));
	io.Fonts->TexID = (ImTextureID)g_FontTexture;
    return true;
}

void ImGui_ImplCocos2dx_DestroyFontsTexture()
{
	if (g_FontTexture)
	{
		ImGui::GetIO().Fonts->TexID = nullptr;
		CC_SAFE_DELETE(g_FontTexture);
	}
}

bool ImGui_ImplCocos2dx_CreateDeviceObjects()
{
	static auto vertex_shader =
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
	static auto fragment_shader =
		"#ifdef GL_ES\n"
		"    precision mediump float;\n"
		"#endif\n"
		"uniform sampler2D u_texture;\n"
		"varying vec2 v_texCoord;\n"
		"varying vec4 v_fragmentColor;\n"
		"void main()\n"
		"{\n"
		"    gl_FragColor = v_fragmentColor * texture2D(u_texture, v_texCoord.st);\n"
		"}\n";
	static auto fragment_shader_font =
		"#ifdef GL_ES\n"
		"    precision mediump float;\n"
		"#endif\n"
		"uniform sampler2D u_texture;\n"
		"varying vec2 v_texCoord;\n"
		"varying vec4 v_fragmentColor;\n"
		"void main()\n"
		"{\n"
		"    float a = texture2D(u_texture, v_texCoord.st).a;\n"
		"    gl_FragColor = vec4(v_fragmentColor.rgb, v_fragmentColor.a * a);\n"
		"}\n";

	CC_SAFE_RELEASE(g_ProgramInfo.program);
	CC_SAFE_RELEASE(g_ProgramFontInfo.program);
	g_ProgramInfo.program = backend::Device::getInstance()->newProgram(
		vertex_shader, fragment_shader);
	g_ProgramFontInfo.program = backend::Device::getInstance()->newProgram(
		vertex_shader, fragment_shader_font);
	IM_ASSERT(g_ProgramInfo.program);
	IM_ASSERT(g_ProgramFontInfo.program);
	if (!g_ProgramInfo.program || !g_ProgramFontInfo.program)
		return false;

	for (auto& p : { &g_ProgramInfo,&g_ProgramFontInfo })
	{
		p->texture = p->program->getUniformLocation(TEXTURE);
		p->projection = p->program->getUniformLocation(MVP_MATRIX);
		p->position = p->program->getAttributeLocation(POSITION);
		p->uv = p->program->getAttributeLocation(TEXCOORD);
		p->color = p->program->getAttributeLocation(COLOR);
		IM_ASSERT(bool(p->texture));
		IM_ASSERT(bool(p->projection));
		IM_ASSERT(p->position >= 0);
		IM_ASSERT(p->uv >= 0);
		IM_ASSERT(p->color >= 0);
		auto& layout = p->layout;
		layout.setAttribute("a_position", p->position,
			VertexFormat::FLOAT2, 0, false);
		layout.setAttribute("a_texCoord", p->uv,
			VertexFormat::FLOAT2, offsetof(ImDrawVert, uv), false);
		layout.setAttribute("a_color", p->color,
			VertexFormat::UBYTE4, offsetof(ImDrawVert, col), true);
		layout.setLayout(sizeof(ImDrawVert));
	}

    ImGui_ImplCocos2dx_CreateFontsTexture();
    return true;
}

void ImGui_ImplCocos2dx_DestroyDeviceObjects()
{
	CC_SAFE_RELEASE_NULL(g_ProgramInfo.program);
	CC_SAFE_RELEASE_NULL(g_ProgramFontInfo.program);
    ImGui_ImplCocos2dx_DestroyFontsTexture();
}

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

static void ImGui_ImplOpenGL2_RenderWindow(ImGuiViewport* viewport, void*)
{
	if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
	{
		const auto renderer = Director::getInstance()->getRenderer();
		renderer->clear(ClearFlag::COLOR, { 0,0,0,1 }, 1, 0, 0);
	}
	ImGui_ImplCocos2dx_RenderDrawData(viewport->DrawData);
}

bool ImGui_ImplCocos2dx_Init(bool install_callbacks /*TODO: need check whether callbacks installed at shutdown*/)
{
    g_Time = 0.0;
    ImGui::CreateContext();

    // Setup back-end capabilities flags
    ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking

#ifdef CC_PLATFORM_PC
	// Enable Multi-Viewport / Platform Windows
    // TODO: mac osx have problem when create new window, the game scene will go to black
#if defined(_WIN32)
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
#endif
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;
    const auto window = ImGui_ImplCocos2dx_GetWindow();
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
	// backend
	io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.Renderer_RenderWindow = ImGui_ImplOpenGL2_RenderWindow;
#endif // CC_PLATFORM_PC

	io.BackendPlatformName = "imgui_impl_cocos";
	io.BackendRendererName = "imgui_impl_cocos";
	// disable auto load and save
	io.IniFilename = nullptr;

#ifdef CC_PLATFORM_PC
    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

    io.SetClipboardTextFn = ImGui_ImplCocos2dx_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplCocos2dx_GetClipboardText;
    io.ClipboardUserData = window;

	// Create mouse cursors
	// (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
	// GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
	// Missing cursors will return NULL and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
	GLFWerrorfun prev_error_callback = glfwSetErrorCallback(nullptr);
	g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
	g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
	g_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
#if GLFW_HAS_NEW_CURSORS
	g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
	g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
	g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
	g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
#else
	g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
	g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
#endif
	glfwSetErrorCallback(prev_error_callback);

    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
    g_PrevUserCallbackMousebutton = nullptr;
    g_PrevUserCallbackScroll = nullptr;
    g_PrevUserCallbackKey = nullptr;
    g_PrevUserCallbackChar = nullptr;
    if (install_callbacks)
    {
        g_PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, ImGui_ImplCocos2dx_MouseButtonCallback);
        g_PrevUserCallbackScroll = glfwSetScrollCallback(window, ImGui_ImplCocos2dx_ScrollCallback);
        g_PrevUserCallbackKey = glfwSetKeyCallback(window, ImGui_ImplCocos2dx_KeyCallback);
        g_PrevUserCallbackChar = glfwSetCharCallback(window, ImGui_ImplCocos2dx_CharCallback);
    }

	// Our mouse update function expect PlatformHandle to be filled for the main viewport
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	main_viewport->PlatformHandle = (void*)window;

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	main_viewport->PlatformHandleRaw = glfwGetWin32Window(window);
#endif

	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		ImGui_ImplGlfw_InitPlatformInterface();
	else {
		// Update monitors the first time (note: monitor callback are broken in GLFW 3.2 and earlier, see github.com/glfw/glfw/issues/784)
		ImGui_ImplGlfw_UpdateMonitors();
		g_PrevUserCallbackMonitor = glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);
	}
#else
	/*
	auto e = cocos2d::EventListenerMouse::create();
	e->onMouseDown = [](cocos2d::EventMouse* ev)
	{
		const auto b = (int)ev->getMouseButton();
		if (0 <= b && b < 5)
			g_MouseJustPressed[b] = true;
	};
	e->onMouseUp = [](cocos2d::EventMouse* ev)
	{
		const auto b = (int)ev->getMouseButton();
		if (0 <= b && b < 5)
			g_MouseJustPressed[b] = false;
	};
	e->onMouseMove = [](cocos2d::EventMouse* ev)
	{
		g_CursorPos.x = ev->getCursorX();
		g_CursorPos.y = ev->getCursorY();
	};
	e->onMouseScroll = [](cocos2d::EventMouse* ev)
	{
		auto& _io = ImGui::GetIO();
		_io.MouseWheelH += (float)ev->getScrollX();
		_io.MouseWheel += (float)ev->getScrollY();
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(e, 1);
	*/
	auto e1 = cocos2d::EventListenerTouchOneByOne::create();
	//e1->setSwallowTouches(true);
	e1->onTouchBegan = [](Touch* touch, Event*)
	{
		const auto loc = touch->getLocationInView();
		g_CursorPos.x = loc.x;
		g_CursorPos.y = loc.y;
		g_MouseJustPressed[0] = true;
		return true;
	};
	e1->onTouchMoved = [](Touch* touch, Event*)
	{
		const auto loc = touch->getLocationInView();
		g_CursorPos.x = loc.x;
		g_CursorPos.y = loc.y;
		g_MouseJustPressed[0] = true;
	};
	e1->onTouchEnded = [](Touch* touch, Event*)
	{
		const auto loc = touch->getLocationInView();
		g_CursorPos.x = loc.x;
		g_CursorPos.y = loc.y;
		g_MouseJustPressed[0] = false;
	};
	e1->onTouchCancelled = [&](Touch* touch, Event*)
	{
		g_CursorPos = ImVec2(-FLT_MAX, -FLT_MAX);
		g_MouseJustPressed[0] = false;
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(e1, 1);
	auto e2 = cocos2d::EventListenerKeyboard::create();
	using KeyCode = cocos2d::EventKeyboard::KeyCode;
	e2->onKeyPressed = [](KeyCode k, cocos2d::Event* ev)
	{
		auto& _io = ImGui::GetIO();
		_io.KeysDown[(int)k] = true;
		// Modifiers are not reliable across systems
		_io.KeyCtrl = _io.KeysDown[(int)KeyCode::KEY_LEFT_CTRL] || _io.KeysDown[(int)KeyCode::KEY_RIGHT_CTRL];
		_io.KeyShift = _io.KeysDown[(int)KeyCode::KEY_LEFT_SHIFT] || _io.KeysDown[(int)KeyCode::KEY_RIGHT_SHIFT];
		_io.KeyAlt = _io.KeysDown[(int)KeyCode::KEY_LEFT_ALT] || _io.KeysDown[(int)KeyCode::KEY_RIGHT_ALT];
		_io.KeySuper = _io.KeysDown[(int)KeyCode::KEY_HYPER];
	};
	e2->onKeyReleased = [](KeyCode k, cocos2d::Event* ev)
	{
		auto& _io = ImGui::GetIO();
		_io.KeysDown[(int)k] = false;
		// Modifiers are not reliable across systems
		_io.KeyCtrl = _io.KeysDown[(int)KeyCode::KEY_LEFT_CTRL] || _io.KeysDown[(int)KeyCode::KEY_RIGHT_CTRL];
		_io.KeyShift = _io.KeysDown[(int)KeyCode::KEY_LEFT_SHIFT] || _io.KeysDown[(int)KeyCode::KEY_RIGHT_SHIFT];
		_io.KeyAlt = _io.KeysDown[(int)KeyCode::KEY_LEFT_ALT] || _io.KeysDown[(int)KeyCode::KEY_RIGHT_ALT];
		_io.KeySuper = _io.KeysDown[(int)KeyCode::KEY_HYPER];
	};
	cocos2d::Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(e2, 1);
#endif // CC_PLATFORM_PC
	return true;
}

void ImGui_ImplCocos2dx_Shutdown()
{
	const auto window = ImGui_ImplCocos2dx_GetWindow();

	ImGui::DestroyPlatformWindows();

#ifdef CC_PLATFORM_PC
	glfwSetMonitorCallback(g_PrevUserCallbackMonitor);
	g_PrevUserCallbackMonitor = nullptr;

	// Restore mouse and char callback
	glfwSetMouseButtonCallback(window, g_PrevUserCallbackMousebutton);
	glfwSetScrollCallback(window, g_PrevUserCallbackScroll);
	glfwSetKeyCallback(window, g_PrevUserCallbackKey);
	glfwSetCharCallback(window, g_PrevUserCallbackChar);
	g_PrevUserCallbackMousebutton = nullptr;
	g_PrevUserCallbackScroll = nullptr;
	g_PrevUserCallbackKey = nullptr;
	g_PrevUserCallbackChar = nullptr;

	// Restore monitor callback
	if (g_PrevUserCallbackMonitor) {
		glfwSetMonitorCallback(g_PrevUserCallbackMonitor);
		g_PrevUserCallbackChar = nullptr;
	}

	// Destroy cursors
    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
    {
        glfwDestroyCursor(g_MouseCursors[cursor_n]);
        g_MouseCursors[cursor_n] = nullptr;
    }
#endif // CC_PLATFORM_PC

    ImGui_ImplCocos2dx_DestroyDeviceObjects();
	ImGui::DestroyContext();
}

static void ImGui_ImplCocos2dx_UpdateMousePosAndButtons()
{
#ifdef CC_PLATFORM_PC
	const auto g_Window = ImGui_ImplCocos2dx_GetWindow();
    // Update buttons
    ImGuiIO& io = ImGui::GetIO();
    for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
    {
        // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
        io.MouseDown[i] = g_MouseJustPressed[i] || glfwGetMouseButton(g_Window, i) != 0;
        g_MouseJustPressed[i] = false;
    }

    // Update mouse position
    const ImVec2 mouse_pos_backup = io.MousePos;
    io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	io.MouseHoveredViewport = 0;
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	for (int n = 0; n < platform_io.Viewports.Size; n++)
	{
		ImGuiViewport* viewport = platform_io.Viewports[n];
		GLFWwindow* window = (GLFWwindow*)viewport->PlatformHandle;
		IM_ASSERT(window != NULL);
#ifdef __EMSCRIPTEN__
	    const bool focused = true;
		IM_ASSERT(platform_io.Viewports.Size == 1);
#else
	    const bool focused = glfwGetWindowAttrib(window, GLFW_FOCUSED) != 0;
#endif
	    if (focused)
	    {
	        if (io.WantSetMousePos)
	        {
	            glfwSetCursorPos(window, (double)mouse_pos_backup.x, (double)mouse_pos_backup.y);
	        }
	        else
	        {
	            double mouse_x, mouse_y;
	            glfwGetCursorPos(window, &mouse_x, &mouse_y);
				if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
				{
					// Multi-viewport mode: mouse position in OS absolute coordinates (io.MousePos is (0,0) when the mouse is on the upper-left of the primary monitor)
					int window_x, window_y;
					glfwGetWindowPos(window, &window_x, &window_y);
					io.MousePos = ImVec2((float)mouse_x + window_x, (float)mouse_y + window_y);
				}
				else
				{
					// Single viewport mode: mouse position in client window coordinates (io.MousePos is (0,0) when the mouse is on the upper-left corner of the app window)
					io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
				}
	        }
			for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
				io.MouseDown[i] |= glfwGetMouseButton(window, i) != 0;
	    }

		// (Optional) When using multiple viewports: set io.MouseHoveredViewport to the viewport the OS mouse cursor is hovering.
		// Important: this information is not easy to provide and many high-level windowing library won't be able to provide it correctly, because
		// - This is _ignoring_ viewports with the ImGuiViewportFlags_NoInputs flag (pass-through windows).
		// - This is _regardless_ of whether another viewport is focused or being dragged from.
		// If ImGuiBackendFlags_HasMouseHoveredViewport is not set by the back-end, imgui will ignore this field and infer the information by relying on the
		// rectangles and last focused time of every viewports it knows about. It will be unaware of other windows that may be sitting between or over your windows.
		// [GLFW] FIXME: This is currently only correct on Win32. See what we do below with the WM_NCHITTEST, missing an equivalent for other systems.
		// See https://github.com/glfw/glfw/issues/1236 if you want to help in making this a GLFW feature.
#if GLFW_HAS_GLFW_HOVERED && CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
		if (glfwGetWindowAttrib(window, GLFW_HOVERED) && !(viewport->Flags & ImGuiViewportFlags_NoInputs))
			io.MouseHoveredViewport = viewport->ID;
#endif
	}
#else
	// Update buttons
	ImGuiIO& io = ImGui::GetIO();
	for (int i = 0; i < IM_ARRAYSIZE(io.MouseDown); i++)
	{
		// g_MouseJustPressed represents touch state on mobile platforms
		io.MouseDown[i] = g_MouseJustPressed[i];
	}
	// Update mouse position
	const ImVec2 mouse_pos_backup = io.MousePos;
	io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
	if (io.WantSetMousePos)
	{
		io.MousePos = mouse_pos_backup;
	}
	else
	{
		if (g_CursorPos.x != -FLT_MAX && g_CursorPos.y != -FLT_MAX)
		{
			// convert g_CursorPos
			const auto glv = cocos2d::Director::getInstance()->getOpenGLView();
			const auto rect = glv->getViewPortRect();
			io.MousePos.x = g_CursorPos.x * glv->getScaleX() + rect.origin.x;
			io.MousePos.y = g_CursorPos.y * glv->getScaleY() + rect.origin.y;
		}
	}
#endif // CC_PLATFORM_PC
}

static void ImGui_ImplCocos2dx_UpdateMouseCursor()
{
#ifdef CC_PLATFORM_PC
	const auto g_Window = ImGui_ImplCocos2dx_GetWindow();
	ImGuiIO& io = ImGui::GetIO();
    if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) || glfwGetInputMode(g_Window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
        return;

	ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	for (int n = 0; n < platform_io.Viewports.Size; n++)
	{
		GLFWwindow* window = (GLFWwindow*)platform_io.Viewports[n]->PlatformHandle;
		if (imgui_cursor == ImGuiMouseCursor_None || io.MouseDrawCursor)
		{
			// Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		else
		{
			// Show OS mouse cursor
			// FIXME-PLATFORM: Unfocused windows seems to fail changing the mouse cursor with GLFW 3.2, but 3.3 works here.
			glfwSetCursor(window, g_MouseCursors[imgui_cursor] ? g_MouseCursors[imgui_cursor] : g_MouseCursors[ImGuiMouseCursor_Arrow]);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
	}
#endif // CC_PLATFORM_PC
}

static void ImGui_ImplCocos2dx_UpdateGamepads()
{
    ImGuiIO& io = ImGui::GetIO();
    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
        return;

    // Update gamepad inputs
    int axes_count = 0, buttons_count = 0;
#ifdef CC_PLATFORM_PC
    #define MAP_BUTTON(NAV_NO, BUTTON_NO)       {\
    	if (buttons_count > BUTTON_NO && buttons[BUTTON_NO] == GLFW_PRESS) io.NavInputs[NAV_NO] = 1.0f; }
    #define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1) {\
    	float v = (axes_count > AXIS_NO) ? axes[AXIS_NO] : V0;\
    	v = (v - V0) / (V1 - V0);\
    	if (v > 1.0f) v = 1.0f;\
    	if (io.NavInputs[NAV_NO] < v) io.NavInputs[NAV_NO] = v; }
    const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axes_count);
    const unsigned char* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttons_count);
    MAP_BUTTON(ImGuiNavInput_Activate,   0);     // Cross / A
    MAP_BUTTON(ImGuiNavInput_Cancel,     1);     // Circle / B
    MAP_BUTTON(ImGuiNavInput_Menu,       2);     // Square / X
    MAP_BUTTON(ImGuiNavInput_Input,      3);     // Triangle / Y
    MAP_BUTTON(ImGuiNavInput_DpadLeft,   13);    // D-Pad Left
    MAP_BUTTON(ImGuiNavInput_DpadRight,  11);    // D-Pad Right
    MAP_BUTTON(ImGuiNavInput_DpadUp,     10);    // D-Pad Up
    MAP_BUTTON(ImGuiNavInput_DpadDown,   12);    // D-Pad Down
    MAP_BUTTON(ImGuiNavInput_FocusPrev,  4);     // L1 / LB
    MAP_BUTTON(ImGuiNavInput_FocusNext,  5);     // R1 / RB
    MAP_BUTTON(ImGuiNavInput_TweakSlow,  4);     // L1 / LB
    MAP_BUTTON(ImGuiNavInput_TweakFast,  5);     // R1 / RB
    MAP_ANALOG(ImGuiNavInput_LStickLeft, 0,  -0.3f,  -0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickRight,0,  +0.3f,  +0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickUp,   1,  +0.3f,  +0.9f);
    MAP_ANALOG(ImGuiNavInput_LStickDown, 1,  -0.3f,  -0.9f);
    #undef MAP_BUTTON
    #undef MAP_ANALOG
#else
	//TODO:
#endif // CC_PLATFORM_PC
    if (axes_count > 0 && buttons_count > 0)
        io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
    else
        io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
}

static void ImGui_ImplGlfw_UpdateMonitors()
{
#ifdef CC_PLATFORM_PC
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	int monitors_count = 0;
	GLFWmonitor** glfw_monitors = glfwGetMonitors(&monitors_count);
	platform_io.Monitors.resize(0);
	for (int n = 0; n < monitors_count; n++)
	{
		ImGuiPlatformMonitor monitor;
		int x, y;
		glfwGetMonitorPos(glfw_monitors[n], &x, &y);
		const GLFWvidmode* vid_mode = glfwGetVideoMode(glfw_monitors[n]);
#if GLFW_HAS_MONITOR_WORK_AREA
		monitor.MainPos = ImVec2((float)x, (float)y);
		monitor.MainSize = ImVec2((float)vid_mode->width, (float)vid_mode->height);
		int w, h;
		glfwGetMonitorWorkarea(glfw_monitors[n], &x, &y, &w, &h);
		monitor.WorkPos = ImVec2((float)x, (float)y);;
		monitor.WorkSize = ImVec2((float)w, (float)h);
#else
		monitor.MainPos = monitor.WorkPos = ImVec2((float)x, (float)y);
		monitor.MainSize = monitor.WorkSize = ImVec2((float)vid_mode->width, (float)vid_mode->height);
#endif
#if GLFW_HAS_PER_MONITOR_DPI
		// Warning: the validity of monitor DPI information on Windows depends on the application DPI awareness settings, which generally needs to be set in the manifest or at runtime.
		float x_scale, y_scale;
		glfwGetMonitorContentScale(glfw_monitors[n], &x_scale, &y_scale);
		monitor.DpiScale = x_scale;
#endif
		platform_io.Monitors.push_back(monitor);
	}
	g_WantUpdateMonitors = false;
#endif // CC_PLATFORM_PC
}

void ImGui_ImplCocos2dx_NewFrame()
{
	static std::chrono::steady_clock::time_point s_lastFrameTime;

	// Calculate deltaTime by self to avoid error when pause cocos2d::Director
	auto now = std::chrono::steady_clock::now();
	auto deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(now - s_lastFrameTime).count() / 1000000.0f;
	deltaTime = cocos2d::clampf(deltaTime, CC_IMGUI_MIN_DELTA, CC_IMGUI_MAX_DELTA);
	s_lastFrameTime = now;

	g_CallbackCommands.clear();
	g_CustomCommands.clear();
	g_ProgramStates.clear();

    if (!g_FontTexture)
        ImGui_ImplCocos2dx_CreateDeviceObjects();

    ImGuiIO& io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() &&
		"Font atlas not built! Missing call to renderer _NewFrame() function?");

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int buffer_w, buffer_h;
#ifdef CC_PLATFORM_PC
	const auto g_Window = ImGui_ImplCocos2dx_GetWindow();
	glfwGetWindowSize(g_Window, &w, &h);
    glfwGetFramebufferSize(g_Window, &buffer_w, &buffer_h);
#else
	auto glv = cocos2d::Director::getInstance()->getOpenGLView();
	const auto frame_size = glv->getFrameSize();
	//const auto buffer_size = glv->getDesignResolutionSize();
	w = buffer_w = frame_size.width;
	h = buffer_h = frame_size.height;
#endif // CC_PLATFORM_PC
    io.DisplaySize = ImVec2((float)w, (float)h);
    if (w > 0 && h > 0)
        io.DisplayFramebufferScale = ImVec2((float)buffer_w / w, (float)buffer_h / h);
#ifdef CC_PLATFORM_PC
	if (g_WantUpdateMonitors)
		ImGui_ImplGlfw_UpdateMonitors();
#endif // CC_PLATFORM_PC

    // Setup time step
	io.DeltaTime = deltaTime;

    ImGui_ImplCocos2dx_UpdateMousePosAndButtons();
    ImGui_ImplCocos2dx_UpdateMouseCursor();

    // Update game controllers (if enabled and available)
    ImGui_ImplCocos2dx_UpdateGamepads();

    ImGui::NewFrame();
}

#ifdef CC_PLATFORM_PC

//--------------------------------------------------------------------------------------------------------
// MULTI-VIEWPORT / PLATFORM INTERFACE SUPPORT
// This is an _advanced_ and _optional_ feature, allowing the back-end to create and handle multiple viewports simultaneously.
// If you are new to dear imgui or creating a new binding for dear imgui, it is recommended that you completely ignore this section first..
//--------------------------------------------------------------------------------------------------------

// Helper structure we store in the void* RenderUserData field of each ImGuiViewport to easily retrieve our backend data.
struct ImGuiViewportDataGlfw
{
	GLFWwindow* Window;
	bool        WindowOwned;
	int         IgnoreWindowPosEventFrame;
	int         IgnoreWindowSizeEventFrame;

	ImGuiViewportDataGlfw() { Window = NULL; WindowOwned = false; IgnoreWindowSizeEventFrame = IgnoreWindowPosEventFrame = -1; }
	~ImGuiViewportDataGlfw() { IM_ASSERT(Window == NULL); }
};

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
		if (ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData)
		{
			bool ignore_event = (ImGui::GetFrameCount() <= data->IgnoreWindowPosEventFrame + 1);
			//data->IgnoreWindowPosEventFrame = -1;
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
		if (ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData)
		{
			bool ignore_event = (ImGui::GetFrameCount() <= data->IgnoreWindowSizeEventFrame + 1);
			//data->IgnoreWindowSizeEventFrame = -1;
			if (ignore_event)
				return;
		}
		viewport->PlatformRequestResize = true;
	}
}

static void ImGui_ImplGlfw_CreateWindow(ImGuiViewport* viewport)
{
	ImGuiViewportDataGlfw* data = IM_NEW(ImGuiViewportDataGlfw)();
	viewport->PlatformUserData = data;

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
#ifdef CC_USE_GL
	GLFWwindow* share_window = ImGui_ImplCocos2dx_GetWindow();
#else
	GLFWwindow* share_window = nullptr;
#endif
	data->Window = glfwCreateWindow((int)viewport->Size.x, (int)viewport->Size.y, "No Title Yet", nullptr, share_window);
	data->WindowOwned = true;
	viewport->PlatformHandle = (void*)data->Window;
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
	viewport->PlatformHandleRaw = glfwGetWin32Window(data->Window);
#endif
	glfwSetWindowPos(data->Window, (int)viewport->Pos.x, (int)viewport->Pos.y);

	// Install GLFW callbacks for secondary viewports
	glfwSetMouseButtonCallback(data->Window, ImGui_ImplCocos2dx_MouseButtonCallback);
	glfwSetScrollCallback(data->Window, ImGui_ImplCocos2dx_ScrollCallback);
	glfwSetKeyCallback(data->Window, ImGui_ImplCocos2dx_KeyCallback);
	glfwSetCharCallback(data->Window, ImGui_ImplCocos2dx_CharCallback);
	glfwSetWindowCloseCallback(data->Window, ImGui_ImplGlfw_WindowCloseCallback);
	glfwSetWindowPosCallback(data->Window, ImGui_ImplGlfw_WindowPosCallback);
	glfwSetWindowSizeCallback(data->Window, ImGui_ImplGlfw_WindowSizeCallback);
#ifdef CC_USE_GL
	const auto window = data->Window;
	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
	AddRendererCommand([=]()
	{
		glfwMakeContextCurrent(window);
	});
#endif
}

static void ImGui_ImplGlfw_DestroyWindow(ImGuiViewport* viewport)
{
	if (ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData)
	{
		if (data->WindowOwned)
		{
			const auto window = data->Window;
			const auto PlatformHandleRaw = viewport->PlatformHandleRaw;
			AddRendererCommand([=]()
			{
#if GLFW_HAS_GLFW_HOVERED && CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
				HWND hwnd = PlatformHandleRaw;
				::RemovePropA(hwnd, "IMGUI_VIEWPORT");
#endif
				glfwDestroyWindow(window);
			});
		}
		data->Window = NULL;
		IM_DELETE(data);
	}
	viewport->PlatformUserData = viewport->PlatformHandle = NULL;
}

// We have submitted https://github.com/glfw/glfw/pull/1568 to allow GLFW to support "transparent inputs".
// In the meanwhile we implement custom per-platform workarounds here (FIXME-VIEWPORT: Implement same work-around for Linux/OSX!)
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32 && GLFW_HAS_GLFW_HOVERED
static WNDPROC g_GlfwWndProc = NULL;
static LRESULT CALLBACK WndProcNoInputs(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCHITTEST)
	{
		// Let mouse pass-through the window. This will allow the back-end to set io.MouseHoveredViewport properly (which is OPTIONAL).
		// The ImGuiViewportFlags_NoInputs flag is set while dragging a viewport, as want to detect the window behind the one we are dragging.
		// If you cannot easily access those viewport flags from your windowing/event code: you may manually synchronize its state e.g. in
		// your main loop after calling UpdatePlatformWindows(). Iterate all viewports/platform windows and pass the flag to your windowing system.
		ImGuiViewport* viewport = (ImGuiViewport*)::GetPropA(hWnd, "IMGUI_VIEWPORT");
		if (viewport->Flags & ImGuiViewportFlags_NoInputs)
			return HTTRANSPARENT;
	}
	return ::CallWindowProc(g_GlfwWndProc, hWnd, msg, wParam, lParam);
}
#endif

static void ImGui_ImplGlfw_ShowWindow(ImGuiViewport* viewport)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;

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
#if GLFW_HAS_GLFW_HOVERED && defined(_WIN32)
	::SetPropA(hwnd, "IMGUI_VIEWPORT", viewport);
	if (g_GlfwWndProc == NULL)
		g_GlfwWndProc = (WNDPROC)::GetWindowLongPtr(hwnd, GWLP_WNDPROC);
	::SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProcNoInputs);
#endif

#if !GLFW_HAS_FOCUS_ON_SHOW
	// GLFW hack: GLFW 3.2 has a bug where glfwShowWindow() also activates/focus the window.
	// The fix was pushed to GLFW repository on 2018/01/09 and should be included in GLFW 3.3 via a GLFW_FOCUS_ON_SHOW window attribute.
	// See https://github.com/glfw/glfw/issues/1189
	// FIXME-VIEWPORT: Implement same work-around for Linux/OSX in the meanwhile.
	if (viewport->Flags & ImGuiViewportFlags_NoFocusOnAppearing)
	{
		::ShowWindow(hwnd, SW_SHOWNA);
		return;
	}
#endif
#endif

	glfwShowWindow(data->Window);
}

static ImVec2 ImGui_ImplGlfw_GetWindowPos(ImGuiViewport* viewport)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
	int x = 0, y = 0;
	glfwGetWindowPos(data->Window, &x, &y);
	return ImVec2((float)x, (float)y);
}

static void ImGui_ImplGlfw_SetWindowPos(ImGuiViewport* viewport, ImVec2 pos)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
	glfwSetWindowPos(data->Window, (int)pos.x, (int)pos.y);
}

static ImVec2 ImGui_ImplGlfw_GetWindowSize(ImGuiViewport* viewport)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
	int w = 0, h = 0;
	glfwGetWindowSize(data->Window, &w, &h);
	return ImVec2((float)w, (float)h);
}

static void ImGui_ImplGlfw_SetWindowSize(ImGuiViewport* viewport, ImVec2 size)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
#if CC_TARGET_PLATFORM == CC_PLATFORM_MAC && !GLFW_HAS_OSX_WINDOW_POS_FIX
	// Native OS windows are positioned from the bottom-left corner on macOS, whereas on other platforms they are
	// positioned from the upper-left corner. GLFW makes an effort to convert macOS style coordinates, however it
	// doesn't handle it when changing size. We are manually moving the window in order for changes of size to be based
	// on the upper-left corner.
	int x, y, width, height;
	glfwGetWindowPos(data->Window, &x, &y);
	glfwGetWindowSize(data->Window, &width, &height);
	glfwSetWindowPos(data->Window, x, y - height + size.y);
#endif
	data->IgnoreWindowSizeEventFrame = ImGui::GetFrameCount();
	glfwSetWindowSize(data->Window, (int)size.x, (int)size.y);
}

static void ImGui_ImplGlfw_SetWindowTitle(ImGuiViewport* viewport, const char* title)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
	glfwSetWindowTitle(data->Window, title);
}

static void ImGui_ImplGlfw_SetWindowFocus(ImGuiViewport* viewport)
{
#if GLFW_HAS_FOCUS_WINDOW
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
	glfwFocusWindow(data->Window);
#else
	// FIXME: What are the effect of not having this function? At the moment imgui doesn't actually call SetWindowFocus - we set that up ahead, will answer that question later.
	(void)viewport;
#endif
}

static bool ImGui_ImplGlfw_GetWindowFocus(ImGuiViewport* viewport)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
	return glfwGetWindowAttrib(data->Window, GLFW_FOCUSED) != 0;
}

static bool ImGui_ImplGlfw_GetWindowMinimized(ImGuiViewport* viewport)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
	return glfwGetWindowAttrib(data->Window, GLFW_ICONIFIED) != 0;
}

#if GLFW_HAS_WINDOW_ALPHA
static void ImGui_ImplGlfw_SetWindowAlpha(ImGuiViewport* viewport, float alpha)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
	glfwSetWindowOpacity(data->Window, alpha);
}
#endif

static void ImGui_ImplGlfw_RenderWindow(ImGuiViewport* viewport, void*)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
#ifdef CC_USE_GL
	const auto window = data->Window;
	glfwMakeContextCurrent(window);
	AddRendererCommand([=]()
	{
		glfwMakeContextCurrent(window);
	});
#endif
}

static void ImGui_ImplGlfw_SwapBuffers(ImGuiViewport* viewport, void*)
{
	ImGuiViewportDataGlfw* data = (ImGuiViewportDataGlfw*)viewport->PlatformUserData;
#ifdef CC_USE_GL
	const auto window = data->Window;
	glfwMakeContextCurrent(window);
	AddRendererCommand([=]()
	{
		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	});
#endif 
}

//--------------------------------------------------------------------------------------------------------
// IME (Input Method Editor) basic support for e.g. Asian language users
//--------------------------------------------------------------------------------------------------------

#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32

#include <imm.h>
#ifdef _MSC_VER
#pragma comment(lib, "imm32")
#endif

static void ImGui_ImplCocos2dx_SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos)
{
	const auto hwnd = (HWND)glfwGetWin32Window(ImGui_ImplCocos2dx_GetWindow());
	if (hwnd)
		if (HIMC himc = ::ImmGetContext(hwnd))
		{
			COMPOSITIONFORM cf = {
				CFS_FORCE_POSITION,
				{ (LONG)(pos.x - viewport->Pos.x), (LONG)(pos.y - viewport->Pos.y) },
				{ 0, 0, 0, 0 } };
			::ImmSetCompositionWindow(himc, &cf);
			::ImmReleaseContext(hwnd, himc);
		}
}

#else

static void ImGui_ImplCocos2dx_SetImeInputPos(ImGuiViewport* viewport, ImVec2 pos) {}

#endif

static void ImGui_ImplGlfw_InitPlatformInterface()
{
	// Register platform interface (will be coupled with a renderer interface)
	ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
	platform_io.Platform_CreateWindow = ImGui_ImplGlfw_CreateWindow;
	platform_io.Platform_DestroyWindow = ImGui_ImplGlfw_DestroyWindow;
	platform_io.Platform_ShowWindow = ImGui_ImplGlfw_ShowWindow;
	platform_io.Platform_SetWindowPos = ImGui_ImplGlfw_SetWindowPos;
	platform_io.Platform_GetWindowPos = ImGui_ImplGlfw_GetWindowPos;
	platform_io.Platform_SetWindowSize = ImGui_ImplGlfw_SetWindowSize;
	platform_io.Platform_GetWindowSize = ImGui_ImplGlfw_GetWindowSize;
	platform_io.Platform_SetWindowFocus = ImGui_ImplGlfw_SetWindowFocus;
	platform_io.Platform_GetWindowFocus = ImGui_ImplGlfw_GetWindowFocus;
	platform_io.Platform_GetWindowMinimized = ImGui_ImplGlfw_GetWindowMinimized;
	platform_io.Platform_SetWindowTitle = ImGui_ImplGlfw_SetWindowTitle;
	platform_io.Platform_RenderWindow = ImGui_ImplGlfw_RenderWindow;
	platform_io.Platform_SwapBuffers = ImGui_ImplGlfw_SwapBuffers;
#if GLFW_HAS_WINDOW_ALPHA
	platform_io.Platform_SetWindowAlpha = ImGui_ImplGlfw_SetWindowAlpha;
#endif
	platform_io.Platform_SetImeInputPos = ImGui_ImplCocos2dx_SetImeInputPos;

	// Note: monitor callback are broken GLFW 3.2 and earlier (see github.com/glfw/glfw/issues/784)
	ImGui_ImplGlfw_UpdateMonitors();
	g_PrevUserCallbackMonitor = glfwSetMonitorCallback(ImGui_ImplGlfw_MonitorCallback);

	// Register main window handle (which is owned by the main application, not by us)
	// This is mostly for simplicity and consistency, so that our code (e.g. mouse handling etc.) can use same logic for main and secondary viewports.
	ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGuiViewportDataGlfw* data = IM_NEW(ImGuiViewportDataGlfw)();
	const auto window = ImGui_ImplCocos2dx_GetWindow();
	data->Window = window;
	data->WindowOwned = false;
	main_viewport->PlatformUserData = data;
	main_viewport->PlatformHandle = (void*)window;
}

static void ImGui_ImplGlfw_ShutdownPlatformInterface()
{
}

#endif // CC_PLATFORM_PC
