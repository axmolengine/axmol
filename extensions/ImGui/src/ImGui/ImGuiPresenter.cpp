/****************************************************************************
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "ImGuiPresenter.h"
#include <assert.h>
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#    include "backends/imgui_impl_android.h"
#else
#    include "backends/imgui_impl_glfw.h"
#endif

#include "backends/imgui_impl_axmol.h"

#include "imgui_internal.h"
#include "misc/freetype/imgui_freetype.h"

#include "xxhash.h"

#if defined(AX_PLATFORM_PC) && AX_RENDER_API == AX_RENDER_API_GL
#    define AX_IMGUI_ENABLE_MULTI_VIEWPORT 1
#else
#    define AX_IMGUI_ENABLE_MULTI_VIEWPORT 0
#endif

NS_AX_EXT_BEGIN

namespace
{
uint32_t fourccValue(std::string_view str)
{
    if (str.empty() || str[0] != '#')
        return (uint32_t)-1;
    uint32_t value = 0;
    memcpy(&value, str.data() + 1, std::min(sizeof(value), str.size() - 1));
    return value;
}

std::tuple<ImVec2, ImVec2> getTwoPointTextureUV(Sprite* sp)
{
    ImVec2 uv0, uv1;
    if (!sp || !sp->getTexture())
        return std::tuple{uv0, uv1};
    const auto& rect        = sp->getTextureRect();
    const auto tex          = sp->getTexture();
    const float atlasWidth  = (float)tex->getPixelsWide();
    const float atlasHeight = (float)tex->getPixelsHigh();
    uv0.x                   = rect.origin.x / atlasWidth;
    uv0.y                   = rect.origin.y / atlasHeight;
    uv1.x                   = (rect.origin.x + rect.size.width) / atlasWidth;
    uv1.y                   = (rect.origin.y + rect.size.height) / atlasHeight;
    return std::tuple{uv0, uv1};
}

std::tuple<ImVec2, ImVec2, ImVec2, ImVec2> getFourPointTextureUV(Texture2D* tex, const Rect& rectInPixels, bool rotated)
{
    ImVec2 uv0, uv1, uv2, uv3;

    const auto atlasWidth  = (float)tex->getPixelsWide();
    const auto atlasHeight = (float)tex->getPixelsHigh();

    float rw = rectInPixels.size.width;
    float rh = rectInPixels.size.height;

    if (rotated)
        std::swap(rw, rh);

#if AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL
    float left   = (2 * rectInPixels.origin.x + 1) / (2 * atlasWidth);
    float right  = left + (rw * 2 - 2) / (2 * atlasWidth);
    float top    = (2 * rectInPixels.origin.y + 1) / (2 * atlasHeight);
    float bottom = top + (rh * 2 - 2) / (2 * atlasHeight);
#else
    float left   = rectInPixels.origin.x / atlasWidth;
    float right  = (rectInPixels.origin.x + rw) / atlasWidth;
    float top    = rectInPixels.origin.y / atlasHeight;
    float bottom = (rectInPixels.origin.y + rh) / atlasHeight;
#endif  // AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL

    if (rotated)
    {
        uv0.x = right;
        uv0.y = top;
        uv1.x = right;
        uv1.y = bottom;
        uv2.x = left;
        uv2.y = bottom;
        uv3.x = left;
        uv3.y = top;
    }
    else
    {
        uv0.x = left;
        uv0.y = top;
        uv1.x = right;
        uv1.y = top;
        uv2.x = right;
        uv2.y = bottom;
        uv3.x = left;
        uv3.y = bottom;
    }

    return std::tuple{uv0, uv1, uv2, uv3};
}

std::tuple<ImVec2, ImVec2, ImVec2, ImVec2> getFourPointTextureUV(Sprite* sp)
{
    if (!sp || !sp->getTexture())
        return std::tuple{ImVec2(), ImVec2(), ImVec2(), ImVec2()};

    const auto rectInPixels = AX_RECT_POINTS_TO_PIXELS(sp->getTextureRect());
    return getFourPointTextureUV(sp->getTexture(), rectInPixels, sp->isTextureRectRotated());
}

std::tuple<ImVec2, ImVec2, ImVec2, ImVec2> getFourPointTextureUV(SpriteFrame* frame)
{
    if (!frame || !frame->getTexture())
        return std::tuple{ImVec2(), ImVec2(), ImVec2(), ImVec2()};

    const auto rectInPixels = frame->getRectInPixels();
    return getFourPointTextureUV(frame->getTexture(), rectInPixels, frame->isRotated());
}

ImVec2 operator+(const ImVec2& lhs, const ImVec2& rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)
{
    return {v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a};
}

ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
}  // namespace

class ImGuiEventTracker
{
public:
    virtual ~ImGuiEventTracker() = default;
};

// Track scene event and check whether routed to the scene graph
class ImGuiSceneEventTracker : public ImGuiEventTracker
{
public:
    bool initWithScene(Scene* scene)
    {
#if defined(AX_PLATFORM_PC) || defined(__EMSCRIPTEN__)
        _trackLayer = utils::newInstance<Node>(&Node::initLayer);

        // note: when at the first click to focus the window, this will not take effect
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);
        listener->onTouchBegan = [this](Touch* touch, Event*) -> bool { return ImGui::GetIO().WantCaptureMouse; };
        _trackLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, _trackLayer);

        // add by halx99
        auto stopAnyMouse  = [=](EventMouse* event) -> bool { return ImGui::GetIO().WantCaptureMouse; };
        auto mouseListener = EventListenerMouse::create();
        mouseListener->setSwallowMouse(true);
        mouseListener->onMouseDown = mouseListener->onMouseUp = stopAnyMouse;
        _trackLayer->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, _trackLayer);
        scene->addChild(_trackLayer, INT_MAX);
#endif
        // add an empty sprite to avoid render problem
        // const auto sp = Sprite::create();
        // sp->setGlobalZOrder(1);
        // sp->setOpacity(0);
        // addChild(sp, 1);

        /*
         * There a 3 choice for schedule frame for ImGui render loop
         * a. at visit/draw to call beginFrame/endFrame, but at ImGui loop, we can't game object and add to Scene
         * directly, will cause damage iterator b. scheduleUpdate at onEnter to call beginFrame, at visit/draw to call
         * endFrame, it's solve iterator damage problem, but when director is paused the director will stop call
         * 'update' function of Scheduler And need modify engine code to call _scheduler->update(_deltaTime) even
         * director is paused, pass 0 for update c. Director::EVENT_BEFORE_DRAW call beginFrame, EVENT_AFTER_VISIT call
         * endFrame
         */

        return true;
    }

    ~ImGuiSceneEventTracker() override
    {
#if defined(AX_PLATFORM_PC) || defined(__EMSCRIPTEN__)
        if (_trackLayer)
        {
            if (_trackLayer->getParent())
                _trackLayer->removeFromParent();
            _trackLayer->release();
        }
#endif
    }

private:
    Node* _trackLayer = nullptr;
};

class ImGuiGlobalEventTracker : public ImGuiEventTracker
{
    static const int highestPriority = (std::numeric_limits<int>::min)();

public:
    bool init()
    {
#if defined(AX_PLATFORM_PC) || defined(__EMSCRIPTEN__)
        // note: when at the first click to focus the window, this will not take effect

        auto eventDispatcher = Director::getInstance()->getEventDispatcher();

        _touchListener = utils::newInstance<EventListenerTouchOneByOne>();
        _touchListener->setSwallowTouches(true);
        _touchListener->onTouchBegan = [this](Touch* touch, Event*) -> bool { return ImGui::GetIO().WantCaptureMouse; };
        eventDispatcher->addEventListenerWithFixedPriority(_touchListener, highestPriority);

        // add by halx99
        auto stopAnyMouse = [=](EventMouse* event) -> bool { return ImGui::GetIO().WantCaptureMouse; };
        _mouseListener    = utils::newInstance<EventListenerMouse>();
        _mouseListener->setSwallowMouse(true);
        _mouseListener->onMouseDown = _mouseListener->onMouseUp = stopAnyMouse;
        eventDispatcher->addEventListenerWithFixedPriority(_mouseListener, highestPriority);
#endif
        return true;
    }

    ~ImGuiGlobalEventTracker() override
    {
#if defined(AX_PLATFORM_PC) || defined(__EMSCRIPTEN__)
        auto eventDispatcher = Director::getInstance()->getEventDispatcher();
        eventDispatcher->removeEventListener(_mouseListener);
        eventDispatcher->removeEventListener(_touchListener);

        _mouseListener->release();
        _touchListener->release();
#endif
    }

    EventListenerTouchOneByOne* _touchListener = nullptr;
    EventListenerMouse* _mouseListener         = nullptr;
};

static ImGuiPresenter* _instance = nullptr;
ImGuiPresenter* ImGuiPresenter::getInstance()
{
    if (_instance)
    {
        if (!_instance->_pendingDestroy)
            return _instance;
        return nullptr;
    }

    _instance = new ImGuiPresenter();
    _instance->init();
    return _instance;
}

void ImGuiPresenter::destroyInstance()
{
    if (_instance)
        _instance->_pendingDestroy = true;
}

void ImGuiPresenter::init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;  // Enable Docking

#if AX_IMGUI_ENABLE_MULTI_VIEWPORT
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;  // Enable Multi-Viewport / Platform Windows
#endif
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;
    // io.ConfigViewportsNoDefaultParent = true;
    // io.ConfigDockingAlwaysTabBar = true;
    // io.ConfigDockingTransparentPayload = true;
    // io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;     // FIXME-DPI: Experimental. THIS CURRENTLY DOESN'T
    // WORK AS EXPECTED. DON'T USE IN USER APP! io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports; //
    // FIXME-DPI: Experimental.

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular
    // ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding              = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ImGui_ImplAndroid_InitForAxmol(Director::getInstance()->getRenderView(), true);
#else
    auto window = static_cast<RenderViewImpl*>(Director::getInstance()->getRenderView())->getWindow();
    ImGui_ImplGlfw_InitForAxmol(window, true);
#endif
    ImGui_ImplAxmol_Init();

    ImGui_ImplAxmol_SetUpdateFontsFunc(&ImGuiPresenter::updateFonts, this);

    ImGui::StyleColorsClassic();

    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    eventDispatcher->addCustomEventListener(Director::EVENT_BEFORE_DRAW, [this](EventCustom*) { beginFrame(); });
    eventDispatcher->addCustomEventListener(Director::EVENT_AFTER_VISIT, [this](EventCustom*) { endFrame(); });
}

void ImGuiPresenter::cleanup()
{
    auto eventDispatcher = Director::getInstance()->getEventDispatcher();
    eventDispatcher->removeCustomEventListeners(Director::EVENT_AFTER_VISIT);
    eventDispatcher->removeCustomEventListeners(Director::EVENT_BEFORE_DRAW);

    ImGui_ImplAxmol_SetUpdateFontsFunc(nullptr, nullptr);
    ImGui_ImplAxmol_Shutdown();
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ImGui_ImplAndroid_Shutdown();
#else
    ImGui_ImplGlfw_Shutdown();
#endif

    ImGui::DestroyContext();

    if (!_renderLoops.empty())
    {
        for (auto item : _renderLoops)
        {
            delete item.second.tracker;
        }
        _renderLoops.clear();
    }
}

void ImGuiPresenter::updateFonts(void* ud)
{
    auto thiz  = (ImGuiPresenter*)ud;
    auto& io   = ImGui::GetIO();
    auto atlas = io.Fonts;

    atlas->Clear();
    atlas->Flags |= ImFontAtlasFlags_NoPowerOfTwoHeight;

    ImFontConfig fontCfg{};

    for (auto& [fontPath, fontSize] : thiz->_fontsInfoMap)
    {
        fontCfg.SizePixels = fontSize * thiz->_mainScale;

        auto data          = FileUtils::getInstance()->getDataFromFile(fontPath);
        ssize_t bufferSize = 0;
        void* buffer       = data.takeBuffer(&bufferSize);

        atlas->AddFontFromMemoryTTF(buffer, bufferSize, 0, &fontCfg);
    }
}

float ImGuiPresenter::enableDPIScale(float userScale)
{
    float devicePixelRatio = Device::getPixelRatio();

    auto mainScale = userScale * devicePixelRatio;

    if (mainScale != _mainScale)
    {
        _mainScale   = mainScale;
        auto imStyle = ImGui::GetStyle();
        imStyle.ScaleAllSizes(mainScale);
        imStyle.FontScaleDpi = mainScale;

        ImGui_ImplAxmol_MarkFontsDirty();
    }

    return mainScale;
}

void ImGuiPresenter::setViewResolution(float width, float height)
{
    ImGui_ImplAxmol_SetViewResolution(width, height);
}

void ImGuiPresenter::addFont(std::string_view fontFile, float fontSize)
{
    if (FileUtils::getInstance()->isFileExistInternal(fontFile))
    {
        bool isDirty = _fontsInfoMap.emplace(fontFile, fontSize).second;
        if (isDirty)
            ImGui_ImplAxmol_MarkFontsDirty();
    }
}

void ImGuiPresenter::removeFont(std::string_view fontFile)
{
    auto count = _fontsInfoMap.size();
    _fontsInfoMap.erase(fontFile);
    if (count != _fontsInfoMap.size())
        ImGui_ImplAxmol_MarkFontsDirty();
}

void ImGuiPresenter::clearFonts()
{
    bool haveCustomFonts = !_fontsInfoMap.empty();
    _fontsInfoMap.clear();
    if (haveCustomFonts)
        ImGui_ImplAxmol_MarkFontsDirty();
}

void ImGuiPresenter::end()
{
    _purgeNextLoop = true;
}

/*
 * begin ImGui frame and draw ImGui stubs
 */
void ImGuiPresenter::beginFrame()
{  // drived by event Director::EVENT_BEFORE_DRAW from engine mainLoop
    if (_purgeNextLoop)
    {
        Director::getInstance()->end();
        return;
    }
    if (_pendingDestroy)
    {
        cleanup();
        delete this;
        if (_instance == this)
            _instance = nullptr;
    }
    if (!_renderLoops.empty())
    {
        // create frame
        ImGui_ImplAxmol_NewFrame();
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
        ImGui_ImplAndroid_NewFrame();
#else
        ImGui_ImplGlfw_NewFrame();
#endif
        ImGui::NewFrame();

        // move to endFrame?
        // _fontsTexture = (Texture2D*)ImGui_ImplAx_GetFontsTexture();
        // assert(_fontsTexture != nullptr);
        // _fontsTexture->retain();

        // draw all gui
        this->update();

        ++_beginFrames;
    }
}

/*
 * flush ImGui draw data to engine
 */
void ImGuiPresenter::endFrame()
{
    if (_beginFrames > 0)
    {
        // render
        ImGui::Render();

        auto drawData = ImGui::GetDrawData();
        if (drawData)
            ImGui_ImplAxmol_RenderDrawData(drawData);

        ImGui_ImplAxmol_RenderPlatform();
        --_beginFrames;
    }
}

void ImGuiPresenter::update()
{
    // clear things from last frame
    _objsRefIdMap.clear();
    _usedObjs.clear();
    // drawing commands

    for (auto iter = _renderLoops.begin(); iter != _renderLoops.end();)
    {
        auto& imLoop = iter->second;
        if (imLoop.removing)
        {
            auto tracker = imLoop.tracker;
            iter         = _renderLoops.erase(iter);
            delete tracker;
            continue;
        }
        imLoop.func();  // invoke ImGui loop func
        ++iter;
    }

    // commands will be processed after update
}

bool ImGuiPresenter::addRenderLoop(std::string_view id, std::function<void()> func, Scene* target)
{

    auto tracker = target ? static_cast<ImGuiEventTracker*>(utils::newInstance<ImGuiSceneEventTracker>(
                                &ImGuiSceneEventTracker::initWithScene, target))
                          : static_cast<ImGuiEventTracker*>(utils::newInstance<ImGuiGlobalEventTracker>());

    auto fourccId = fourccValue(id);
    auto iter     = _renderLoops.find(fourccId);
    if (iter == _renderLoops.end())
    {
        _renderLoops.emplace(fourccId, ImGuiLoop{tracker, std::move(func)});
        return true;
    }

    // allow reuse imLoop, update func, tracker, removing status
    auto& imLoop   = iter->second;
    imLoop.func    = std::move(func);
    imLoop.tracker = tracker;

    if (imLoop.removing)
        imLoop.removing = false;

    return true;
}

void ImGuiPresenter::removeRenderLoop(std::string_view id)
{
    auto fourccId   = fourccValue(id);
    const auto iter = _renderLoops.find(fourccId);
    if (iter != _renderLoops.end())
        iter->second.removing = true;
}

void ImGuiPresenter::image(Texture2D* tex,
                           const ImVec2& size,
                           const ImVec2& uv0,
                           const ImVec2& uv1,
                           const ImVec4& /*tint_col*/,
                           const ImVec4& /*border_col*/)
{
    if (!tex)
        return;
    auto size_ = size;
    if (size_.x <= 0.f)
        size_.x = tex->getPixelsWide();
    if (size_.y <= 0.f)
        size_.y = tex->getPixelsHigh();
    ImGui::PushID(objectRef(tex));
    ImGui::Image((ImTextureID)tex, size_, uv0, uv1);
    ImGui::PopID();
}

void ImGuiPresenter::image(Sprite* sprite,
                           const ImVec2& size,
                           bool keepAspectRatio,
                           const ImVec4& tint_col,
                           const ImVec4& border_col)
{
    if (!sprite || !sprite->getTexture())
        return;

    auto size_       = size;
    const auto& rect = sprite->getTextureRect();
    if (size_.x <= 0.f)
        size_.x = rect.size.width;
    if (size_.y <= 0.f)
        size_.y = rect.size.height;

    if (keepAspectRatio)
    {
        const auto rectInPixels = AX_RECT_POINTS_TO_PIXELS(rect);
        const auto scale =
            std::min(1.0f, std::min(size_.x / rectInPixels.size.width, size_.y / rectInPixels.size.height));
        size_ = ImVec2(rectInPixels.size.width * scale, rectInPixels.size.height * scale);
    }

    auto [uv0, uv1, uv2, uv3] = getFourPointTextureUV(sprite);

    const auto* window = ImGui::GetCurrentWindow();

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size_);
    if (border_col.w > 0.0f)
        bb.Max += ImVec2(2, 2);

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0))
        return;

    const ImVec2 pos[4] = {ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Max.x, bb.Min.y), ImVec2(bb.Max.x, bb.Max.y),
                           ImVec2(bb.Min.x, bb.Max.y)};

    ImGui::PushID(objectRef(sprite));
    if (border_col.w > 0.0f)
    {
        window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(border_col), 0.0f);
        window->DrawList->AddImageQuad((ImTextureID)sprite->getTexture(), pos[0] + ImVec2(1, 1), pos[1] + ImVec2(-1, 1),
                                       pos[2] + ImVec2(1, -1), pos[3] + ImVec2(-1, -1), uv0, uv1, uv2, uv3,
                                       ImGui::GetColorU32(tint_col));
    }
    else
    {

        window->DrawList->AddImageQuad((ImTextureID)sprite->getTexture(), pos[0], pos[1], pos[2], pos[3], uv0, uv1, uv2,
                                       uv3, ImGui::GetColorU32(tint_col));
    }
    ImGui::PopID();
}

void ImGuiPresenter::image(SpriteFrame* spriteFrame,
                           const ImVec2& size,
                           bool keepAspectRatio,
                           const ImVec4& tint_col,
                           const ImVec4& border_col)
{
    if (!spriteFrame || !spriteFrame->getTexture())
        return;

    auto size_       = size;
    const auto& rect = spriteFrame->getOriginalSizeInPixels();
    if (size_.x <= 0.f)
        size_.x = rect.width;
    if (size_.y <= 0.f)
        size_.y = rect.height;

    if (keepAspectRatio)
    {
        const auto scale = std::min(1.0f, std::min(size_.x / rect.width, size_.y / rect.height));
        size_            = ImVec2(rect.width * scale, rect.height * scale);
    }

    auto [uv0, uv1, uv2, uv3] = getFourPointTextureUV(spriteFrame);

    const auto* window = ImGui::GetCurrentWindow();

    ImRect bb(window->DC.CursorPos, window->DC.CursorPos + size_);
    if (border_col.w > 0.0f)
        bb.Max += ImVec2(2, 2);

    ImGui::ItemSize(bb);
    if (!ImGui::ItemAdd(bb, 0))
        return;

    const ImVec2 pos[4] = {ImVec2(bb.Min.x, bb.Min.y), ImVec2(bb.Max.x, bb.Min.y), ImVec2(bb.Max.x, bb.Max.y),
                           ImVec2(bb.Min.x, bb.Max.y)};

    ImGui::PushID(objectRef(spriteFrame));
    if (border_col.w > 0.0f)
    {
        window->DrawList->AddRect(bb.Min, bb.Max, ImGui::GetColorU32(border_col), 0.0f);
        window->DrawList->AddImageQuad((ImTextureID)spriteFrame->getTexture(), pos[0] + ImVec2(1, 1),
                                       pos[1] + ImVec2(-1, 1), pos[2] + ImVec2(1, -1), pos[3] + ImVec2(-1, -1), uv0,
                                       uv1, uv2, uv3, ImGui::GetColorU32(tint_col));
    }
    else
    {

        window->DrawList->AddImageQuad((ImTextureID)spriteFrame->getTexture(), pos[0], pos[1], pos[2], pos[3], uv0, uv1,
                                       uv2, uv3, ImGui::GetColorU32(tint_col));
    }
    ImGui::PopID();
}

bool ImGuiPresenter::imageButton(Texture2D* tex,
                                 const ImVec2& size,
                                 const ImVec2& uv0,
                                 const ImVec2& uv1,
                                 const ImVec4& bg_col,
                                 const ImVec4& tint_col)
{
    if (!tex)
        return false;
    auto size_ = size;
    if (size_.x <= 0.f)
        size_.x = tex->getPixelsWide();
    if (size_.y <= 0.f)
        size_.y = tex->getPixelsHigh();
    ImGui::PushID(objectRef(tex));
    const auto ret = ImGui::ImageButton("###", (ImTextureID)tex, size_, uv0, uv1, bg_col, tint_col);
    ImGui::PopID();
    return ret;
}

bool ImGuiPresenter::imageButton(Sprite* sprite, const ImVec2& size, const ImVec4& bg_col, const ImVec4& tint_col)
{
    if (!sprite || !sprite->getTexture())
        return false;
    auto size_       = size;
    const auto& rect = sprite->getTextureRect();
    if (size_.x <= 0.f)
        size_.x = rect.size.width;
    if (size_.y <= 0.f)
        size_.y = rect.size.height;
    auto [uv0, uv1] = getTwoPointTextureUV(sprite);
    ImGui::PushID(objectRef(sprite));
    const auto ret = ImGui::ImageButton("###", (ImTextureID)sprite->getTexture(), size_, uv0, uv1, bg_col, tint_col);
    ImGui::PopID();
    return ret;
}

void ImGuiPresenter::node(Node* node, const ImVec4& /*tint_col*/, const ImVec4& /*border_col*/)
{
    if (!node)
        return;
    const auto size = node->getContentSize();
    const auto pos  = ImGui::GetCursorScreenPos();
    Mat4 tr;
    tr.m[5]  = -1;
    tr.m[12] = pos.x;
    tr.m[13] = pos.y + size.height;
    /*if (border_col.w > 0.f)
    {
        tr.m[12] += 1;
        tr.m[13] += 1;
    }*/
    node->setNodeToParentTransform(tr);
    ImGui::PushID(objectRef(node));
    ImGui::Image((ImTextureID)node, ImVec2(size.width, size.height), ImVec2(0, 0), ImVec2(1, 1));
    ImGui::PopID();
}

bool ImGuiPresenter::nodeButton(Node* node, const ImVec4& bg_col, const ImVec4& tint_col)
{
    if (!node)
        return false;
    const auto size = node->getContentSize();
    const auto pos  = ImGui::GetCursorScreenPos();
    Mat4 tr;
    tr.m[5]  = -1;
    tr.m[12] = pos.x;
    tr.m[13] = pos.y + size.height;
    tr.m[12] += ImGui::GetStyle().FramePadding.x;
    tr.m[13] += ImGui::GetStyle().FramePadding.y;

    node->setNodeToParentTransform(tr);
    ImGui::PushID(objectRef(node));
    const auto ret = ImGui::ImageButton("###", (ImTextureID)node, ImVec2(size.width, size.height), ImVec2(0, 0),
                                        ImVec2(1, 1), bg_col, tint_col);
    ImGui::PopID();
    return ret;
}

std::tuple<ImTextureID, int> ImGuiPresenter::useTexture(Texture2D* texture)
{
    if (!texture)
        return std::tuple<ImTextureID, int>{0, 0};
    return std::tuple{(ImTextureID)texture, objectRef(texture)};
}

std::tuple<ImTextureID, ImVec2, ImVec2, int> ImGuiPresenter::useSprite(Sprite* sprite)
{
    if (!sprite || !sprite->getTexture())
        return std::tuple<ImTextureID, ImVec2, ImVec2, int>{0, {}, {}, 0};
    auto [uv0, uv1] = getTwoPointTextureUV(sprite);
    return std::tuple{(ImTextureID)sprite->getTexture(), uv0, uv1, objectRef(sprite)};
}

std::tuple<ImTextureID, ImVec2, ImVec2, int> ImGuiPresenter::useNode(Node* node, const ImVec2& pos)
{
    if (!node)
        return std::tuple<ImTextureID, ImVec2, ImVec2, int>{0, {}, {}, 0};
    const auto size = node->getContentSize();
    Mat4 tr;
    tr.m[5]  = -1;
    tr.m[12] = pos.x;
    tr.m[13] = pos.y + size.height;
    node->setNodeToParentTransform(tr);
    return std::tuple{(ImTextureID)node, pos, ImVec2(pos.x + size.width, pos.y + size.height), objectRef(node)};
}

void ImGuiPresenter::setNodeColor(Node* node, const ImVec4& col)
{
    if (node)
    {
        node->setColor({uint8_t(col.x * 255), uint8_t(col.y * 255), uint8_t(col.z * 255), uint8_t(col.w * 255)});
    }
}

void ImGuiPresenter::setNodeColor(Node* node, ImGuiCol col)
{
    if (node && 0 <= col && col < ImGuiCol_COUNT)
        setNodeColor(node, ImGui::GetStyleColorVec4(col));
}

void ImGuiPresenter::setLabelColor(Label* label, const ImVec4& col)
{
    if (label)
    {
        label->setTextColor({uint8_t(col.x * 255), uint8_t(col.y * 255), uint8_t(col.z * 255), uint8_t(col.w * 255)});
    }
}

void ImGuiPresenter::setLabelColor(Label* label, bool disabled)
{
    if (label)
        setLabelColor(label, ImGui::GetStyleColorVec4(disabled ? ImGuiCol_TextDisabled : ImGuiCol_Text));
}

void ImGuiPresenter::setLabelColor(Label* label, ImGuiCol col)
{
    if (label && 0 <= col && col < ImGuiCol_COUNT)
        setLabelColor(label, ImGui::GetStyleColorVec4(col));
}

int ImGuiPresenter::objectRef(Object* p)
{
    int id        = 0;
    const auto it = _objsRefIdMap.find(p);
    if (it == _objsRefIdMap.end())
    {
        _objsRefIdMap[p] = 0;
        _usedObjs.pushBack(p);
    }
    else
        id = ++it->second;

    return XXH32(&p, sizeof(p), id);
}

NS_AX_EXT_END
