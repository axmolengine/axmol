#include "ImGuiPresenter.h"
#include <assert.h>
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#    include "imgui_impl_ax_android.h"
#else
#    include "imgui_impl_ax.h"
#endif
#include "imgui/imgui_internal.h"

// TODO: mac metal
#if defined(AX_USE_GL) && defined(AX_PLATFORM_PC)
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
    return { lhs.x + rhs.x, lhs.y + rhs.y };
}

ImVec2 ImRotate(const ImVec2& v, float cos_a, float sin_a)
{
    return { v.x * cos_a - v.y * sin_a, v.x * sin_a + v.y * cos_a };
}

ImVec2& operator+=(ImVec2& lhs, const ImVec2& rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}
}


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
        auto stopAnyMouse = [=](EventMouse* event) {
            if (ImGui::GetIO().WantCaptureMouse)
            {
                event->stopPropagation();
            }
        };
        auto mouseListener         = EventListenerMouse::create();
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
        auto stopAnyMouse = [=](EventMouse* event) {
            if (ImGui::GetIO().WantCaptureMouse)
            {
                event->stopPropagation();
            }
        };
        _mouseListener              = utils::newInstance<EventListenerMouse>();
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
std::function<void(ImGuiPresenter*)> ImGuiPresenter::_onInit;

ImGuiPresenter* ImGuiPresenter::getInstance()
{
    if (_instance == nullptr)
    {
        _instance = new ImGuiPresenter();
        _instance->init();
        if (_onInit)
            _onInit(_instance);
    }
    return _instance;
}

void ImGuiPresenter::destroyInstance()
{
    if (_instance)
    {
        _instance->cleanup();
        delete _instance;
        _instance = nullptr;
    }
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
    ImGui_ImplAndroid_InitForAx(Director::getInstance()->getGLView(), true);
#else
    auto window = static_cast<GLViewImpl*>(Director::getInstance()->getGLView())->getWindow();
    ImGui_ImplGlfw_InitForAx(window, true);
#endif
    ImGui_ImplAx_Init();

    ImGui_ImplAx_SetCustomFontLoader(&ImGuiPresenter::loadCustomFonts, this);

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

    ImGui_ImplAx_SetCustomFontLoader(nullptr, nullptr);
    ImGui_ImplAx_Shutdown();
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
    ImGui_ImplAndroid_Shutdown();
#else
    ImGui_ImplGlfw_Shutdown();
#endif

    AX_SAFE_RELEASE_NULL(_fontsTexture);

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

void ImGuiPresenter::setOnInit(const std::function<void(ImGuiPresenter*)>& callBack)
{
    _onInit = callBack;
}

void ImGuiPresenter::loadCustomFonts(void* ud)
{
    auto thiz = (ImGuiPresenter*)ud;

    auto imFonts = ImGui::GetIO().Fonts;
    imFonts->Clear();

    auto contentZoomFactor = thiz->_contentZoomFactor;
    for (auto& fontInfo : thiz->_fontsInfoMap)
    {
        const ImWchar* imChars = nullptr;
        switch (fontInfo.second.glyphRange)
        {
        case CHS_GLYPH_RANGE::GENERAL:
            imChars = imFonts->GetGlyphRangesChineseSimplifiedCommon();
            break;
        case CHS_GLYPH_RANGE::FULL:
            imChars = imFonts->GetGlyphRangesChineseFull();
            break;
        default:;
        }

        auto fontData = FileUtils::getInstance()->getDataFromFile(fontInfo.first);
        AXASSERT(!fontData.isNull(), "Cannot load font for IMGUI");

        ssize_t bufferSize = 0;
        auto* buffer       = fontData.takeBuffer(&bufferSize);  // Buffer automatically freed by IMGUI

        imFonts->AddFontFromMemoryTTF(buffer, bufferSize, fontInfo.second.fontSize * contentZoomFactor, nullptr,
                                      imChars);
    }
}

float ImGuiPresenter::enableDPIScale(float userScale)
{
    float devicePixelRatio = Device::getPixelRatio();

    auto zoomFactor = userScale * devicePixelRatio;

    auto imFonts = ImGui::GetIO().Fonts;

    // clear before add new font
    auto fontConf = imFonts->ConfigData;  // copy font config data

    if (zoomFactor != _contentZoomFactor)
    {
        for (auto& fontConf : imFonts->ConfigData)
        {
            fontConf.SizePixels = (fontConf.SizePixels / _contentZoomFactor) * zoomFactor;
        }

        // Destroy font information, let ImplAx recreate at newFrame
        ImGui_ImplAx_SetDeviceObjectsDirty();

        ImGui::GetStyle().ScaleAllSizes(zoomFactor);

        _contentZoomFactor = zoomFactor;
    }

    return zoomFactor;
}

void ImGuiPresenter::setViewResolution(float width, float height)
{
    ImGui_ImplAx_SetViewResolution(width, height);
}

void ImGuiPresenter::addFont(std::string_view fontFile, float fontSize, CHS_GLYPH_RANGE glyphRange)
{
    if (FileUtils::getInstance()->isFileExistInternal(fontFile))
    {
        if (_fontsInfoMap.emplace(fontFile, FontInfo{fontSize, glyphRange}).second)
            ImGui_ImplAx_SetDeviceObjectsDirty();
    }
}

void ImGuiPresenter::removeFont(std::string_view fontFile)
{
    auto count = _fontsInfoMap.size();
    _fontsInfoMap.erase(fontFile);
    if (count != _fontsInfoMap.size())
        ImGui_ImplAx_SetDeviceObjectsDirty();
}

void ImGuiPresenter::clearFonts()
{
    bool haveCustomFonts = !_fontsInfoMap.empty();
    _fontsInfoMap.clear();
    if (haveCustomFonts)
        ImGui_ImplAx_SetDeviceObjectsDirty();

    // auto drawData = ImGui::GetDrawData();
    // if(drawData) drawData->Clear();
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
    if (!_renderLoops.empty())
    {
        // create frame
        ImGui_ImplAx_NewFrame();
#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
        ImGui_ImplAndroid_NewFrame();
#else
        ImGui_ImplGlfw_NewFrame();
#endif
        ImGui::NewFrame();

        // move to endFrame?
        _fontsTexture = (Texture2D*)ImGui_ImplAx_GetFontsTexture();
        assert(_fontsTexture != nullptr);
        _fontsTexture->retain();

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
            ImGui_ImplAx_RenderDrawData(drawData);

        ImGui_ImplAx_RenderPlatform();
        --_beginFrames;

        AX_SAFE_RELEASE_NULL(_fontsTexture);
    }
}

void ImGuiPresenter::update()
{
    // clear things from last frame
    usedCCRefIdMap.clear();
    usedCCRef.clear();
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
                           const ImVec4& tint_col,
                           const ImVec4& border_col)
{
    if (!tex)
        return;
    auto size_ = size;
    if (size_.x <= 0.f)
        size_.x = tex->getPixelsWide();
    if (size_.y <= 0.f)
        size_.y = tex->getPixelsHigh();
    ImGui::PushID(getCCRefId(tex));
    ImGui::Image((ImTextureID)tex, size_, uv0, uv1, tint_col, border_col);
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

    ImGui::PushID(getCCRefId(sprite));
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

    ImGui::PushID(getCCRefId(spriteFrame));
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
                                 int frame_padding,
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
    ImGui::PushID(getCCRefId(tex));
    const auto ret = ImGui::ImageButton((ImTextureID)tex, size_, uv0, uv1, frame_padding, bg_col, tint_col);
    ImGui::PopID();
    return ret;
}

bool ImGuiPresenter::imageButton(Sprite* sprite,
                                 const ImVec2& size,
                                 int frame_padding,
                                 const ImVec4& bg_col,
                                 const ImVec4& tint_col)
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
    ImGui::PushID(getCCRefId(sprite));
    const auto ret =
        ImGui::ImageButton((ImTextureID)sprite->getTexture(), size_, uv0, uv1, frame_padding, bg_col, tint_col);
    ImGui::PopID();
    return ret;
}

void ImGuiPresenter::node(Node* node, const ImVec4& tint_col, const ImVec4& border_col)
{
    if (!node)
        return;
    const auto size = node->getContentSize();
    const auto pos  = ImGui::GetCursorScreenPos();
    Mat4 tr;
    tr.m[5]  = -1;
    tr.m[12] = pos.x;
    tr.m[13] = pos.y + size.height;
    if (border_col.w > 0.f)
    {
        tr.m[12] += 1;
        tr.m[13] += 1;
    }
    node->setNodeToParentTransform(tr);
    ImGui::PushID(getCCRefId(node));
    ImGui::Image((ImTextureID)node, ImVec2(size.width, size.height), ImVec2(0, 0), ImVec2(1, 1), tint_col, border_col);
    ImGui::PopID();
}

bool ImGuiPresenter::nodeButton(Node* node, int frame_padding, const ImVec4& bg_col, const ImVec4& tint_col)
{
    if (!node)
        return false;
    const auto size = node->getContentSize();
    const auto pos  = ImGui::GetCursorScreenPos();
    Mat4 tr;
    tr.m[5]  = -1;
    tr.m[12] = pos.x;
    tr.m[13] = pos.y + size.height;
    if (frame_padding >= 0)
    {
        tr.m[12] += (float)frame_padding;
        tr.m[13] += (float)frame_padding;
    }
    else
    {
        tr.m[12] += ImGui::GetStyle().FramePadding.x;
        tr.m[13] += ImGui::GetStyle().FramePadding.y;
    }
    node->setNodeToParentTransform(tr);
    ImGui::PushID(getCCRefId(node));
    const auto ret = ImGui::ImageButton((ImTextureID)node, ImVec2(size.width, size.height), ImVec2(0, 0), ImVec2(1, 1),
                                        frame_padding, bg_col, tint_col);
    ImGui::PopID();
    return ret;
}

std::tuple<ImTextureID, int> ImGuiPresenter::useTexture(Texture2D* texture)
{
    if (!texture)
        return std::tuple<ImTextureID, int>{nullptr, 0};
    return std::tuple{(ImTextureID)texture, getCCRefId(texture)};
}

std::tuple<ImTextureID, ImVec2, ImVec2, int> ImGuiPresenter::useSprite(Sprite* sprite)
{
    if (!sprite || !sprite->getTexture())
        return std::tuple<ImTextureID, ImVec2, ImVec2, int>{nullptr, {}, {}, 0};
    auto [uv0, uv1] = getTwoPointTextureUV(sprite);
    return std::tuple{(ImTextureID)sprite->getTexture(), uv0, uv1, getCCRefId(sprite)};
}

std::tuple<ImTextureID, ImVec2, ImVec2, int> ImGuiPresenter::useNode(Node* node, const ImVec2& pos)
{
    if (!node)
        return std::tuple<ImTextureID, ImVec2, ImVec2, int>{nullptr, {}, {}, 0};
    const auto size = node->getContentSize();
    Mat4 tr;
    tr.m[5]  = -1;
    tr.m[12] = pos.x;
    tr.m[13] = pos.y + size.height;
    node->setNodeToParentTransform(tr);
    return std::tuple{(ImTextureID)node, pos, ImVec2(pos.x + size.width, pos.y + size.height), getCCRefId(node)};
}

void ImGuiPresenter::setNodeColor(Node* node, const ImVec4& col)
{
    if (node)
    {
        node->setColor({uint8_t(col.x * 255), uint8_t(col.y * 255), uint8_t(col.z * 255)});
        node->setOpacity(uint8_t(col.w * 255));
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

ImWchar* ImGuiPresenter::addGlyphRanges(std::string_view key, const std::vector<ImWchar>& ranges)
{
    auto it = glyphRanges.find(key);
    // the pointer must be persistant, do not replace
    if (it != glyphRanges.end())
        return it->second.data();
    it = glyphRanges.emplace(key, ranges).first;  // glyphRanges[key] = ranges;
    if (ranges.empty())
        it->second.push_back(0);
    return it->second.data();
}

void ImGuiPresenter::mergeFontGlyphs(ImFont* dst, ImFont* src, ImWchar start, ImWchar end)
{
    if (!dst || !src || start > end)
        return;
    for (auto i = start; i <= end; ++i)
    {
        const auto g = src->FindGlyphNoFallback(i);
        if (g)
        {
            dst->AddGlyph(nullptr, g->Codepoint, g->X0, g->Y0, g->X1, g->Y1, g->U0, g->V0, g->U1, g->V1, g->AdvanceX);
        }
    }
    dst->BuildLookupTable();
}

int ImGuiPresenter::getCCRefId(Object* p)
{
    int id        = 0;
    const auto it = usedCCRefIdMap.find(p);
    if (it == usedCCRefIdMap.end())
    {
        usedCCRefIdMap[p] = 0;
        usedCCRef.pushBack(p);
    }
    else
        id = ++it->second;
    // BKDR hash
    constexpr unsigned int seed = 131;
    unsigned int hash           = 0;
    for (auto i = 0u; i < sizeof(void*); ++i)
        hash = hash * seed + ((const char*)&p)[i];
    for (auto i = 0u; i < sizeof(int); ++i)
        hash = hash * seed + ((const char*)&id)[i];
    return (int)hash;
}

NS_AX_EXT_END
