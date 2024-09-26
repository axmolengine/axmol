/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "platform/winrt/GLViewImpl-winrt.h"
#include "base/Macros.h"
#include "base/Director.h"
#include "base/Touch.h"
#include "base/IMEDispatcher.h"
#include "base/EventListenerKeyboard.h"
#include "platform/winrt/Application-winrt.h"
#include "platform/winrt/WinRTUtils.h"
#include "base/EventDispatcher.h"
#include "base/EventMouse.h"
#include <map>

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Popups.h>
#include <winrt/Windows.UI.Input.h>

namespace ax
{

static GLViewImpl* s_pEglView = nullptr;

static EventMouse::MouseButton checkMouseButton(Windows::UI::Core::PointerEventArgs const& args)
{
    if (args.CurrentPoint().Properties().IsLeftButtonPressed())
    {
        return EventMouse::MouseButton::BUTTON_LEFT;
    }
    else if (args.CurrentPoint().Properties().IsRightButtonPressed())
    {
        return EventMouse::MouseButton::BUTTON_RIGHT;
    }
    else if (args.CurrentPoint().Properties().IsMiddleButtonPressed())
    {
        return EventMouse::MouseButton::BUTTON_MIDDLE;
    }
    return EventMouse::MouseButton::BUTTON_UNSET;
}

GLViewImpl* GLViewImpl::create(std::string_view viewName)
{
    auto ret = new GLViewImpl;
    if (ret && ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }

    return nullptr;
}

GLViewImpl* GLViewImpl::createWithRect(std::string_view viewName,
                                       const Rect& rect, float frameZoomFactor, bool /*resizable*/)
{
    auto ret = new GLViewImpl;
    if (ret && ret->initWithRect(viewName, rect, frameZoomFactor))
    {
        ret->autorelease();
        return ret;
    }

    return nullptr;
}

GLViewImpl* GLViewImpl::createWithFullScreen(std::string_view viewName)
{
    auto ret = new GLViewImpl();
    if (ret->initWithFullScreen(viewName))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

GLViewImpl::GLViewImpl()
    : _frameZoomFactor(1.0f)
    , _supportTouch(true)
    , _isRetina(false)
    , _isCursorVisible(true)
    , m_lastPointValid(false)
    , m_running(false)
    , m_initialized(false)
    , m_windowClosed(false)
    , m_windowVisible(true)
    , m_width(0)
    , m_height(0)
    , m_orientation(Windows::Graphics::Display::DisplayOrientations::Landscape)
    , m_appShouldExit(false)
    , _lastMouseButtonPressed(EventMouse::MouseButton::BUTTON_UNSET)
{
    s_pEglView = this;
    _viewName  = "axmol2";
    m_keyboard = KeyBoardWinRT();

    m_backButtonListener                = EventListenerKeyboard::create();
    m_backButtonListener->onKeyReleased = AX_CALLBACK_2(GLViewImpl::BackButtonListener, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_backButtonListener, INT_MAX);
}

GLViewImpl::~GLViewImpl()
{
    AX_ASSERT(this == s_pEglView);
    s_pEglView = nullptr;
}

bool GLViewImpl::initWithRect(std::string_view viewName, const Rect& rect, float frameZoomFactor)
{
    setViewName(viewName);
    setFrameSize(rect.size.width, rect.size.height);
    setFrameZoomFactor(frameZoomFactor);
    UpdateForWindowSizeChange(rect.size.width, rect.size.height);
    return true;
}

bool GLViewImpl::initWithFullScreen(std::string_view viewName)
{
    return initWithRect(viewName, Rect(0, 0, m_width, m_height), 1.0f);
}

void ax::GLViewImpl::setCursorVisible(bool isVisible)
{
    _isCursorVisible = isVisible;
}

void GLViewImpl::setDispatcher(winrt::agile_ref<Windows::UI::Core::CoreDispatcher> dispatcher)
{
    m_dispatcher = dispatcher;
}

void GLViewImpl::setPanel(winrt::agile_ref<Windows::UI::Xaml::Controls::Panel> panel)
{
    m_panel = panel;
}

void GLViewImpl::setIMEKeyboardState(bool bOpen)
{
    std::string str;
    setIMEKeyboardState(bOpen, str);
}

bool GLViewImpl::ShowMessageBox(const winrt::hstring& title, const winrt::hstring& message)
{
    if (m_dispatcher)
    {
        m_dispatcher.get().RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                                     Windows::UI::Core::DispatchedHandler([title, message]() {
                                         // Show the message dialog
                                         auto msg = Windows::UI::Popups::MessageDialog(message, title);
                                         // Set the command to be invoked when a user presses 'ESC'
                                         msg.CancelCommandIndex(1);
                                         msg.ShowAsync();
                                     }));

        return true;
    }
    return false;
}

void GLViewImpl::setIMEKeyboardState(bool bOpen, std::string_view str)
{
    if (bOpen)
    {
        m_keyboard.ShowKeyboard(PlatformStringFromString(str));
    }
    else
    {
        m_keyboard.HideKeyboard(PlatformStringFromString(str));
    }
}

void GLViewImpl::swapBuffers() {}

bool GLViewImpl::isOpenGLReady()
{
    return true;
}

void GLViewImpl::end()
{
    m_windowClosed  = true;
    m_appShouldExit = true;
}

void GLViewImpl::OnSuspending(Windows::Foundation::IInspectable const& sender,
                              Windows::ApplicationModel::SuspendingEventArgs const& args)
{}

void GLViewImpl::OnResuming(Windows::Foundation::IInspectable const& sender) {}

// user pressed the Back Key on the phone
void GLViewImpl::OnBackKeyPress()
{
    ax::EventKeyboard::KeyCode cocos2dKey = EventKeyboard::KeyCode::KEY_ESCAPE;
    ax::EventKeyboard event(cocos2dKey, false);
    ax::Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GLViewImpl::BackButtonListener(EventKeyboard::KeyCode keyCode, Event* event)
{
    if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)
    {
        AXLOGD("*********************************************************************");
        AXLOGD("GLViewImpl::BackButtonListener: Exiting application!");
        AXLOGD("");
        AXLOGD("If you want to listen for Windows Phone back button events,");
        AXLOGD("add a listener for EventKeyboard::KeyCode::KEY_ESCAPE");
        AXLOGD("Make sure you call stopPropagation() on the Event if you don't");
        AXLOGD("want your app to exit when the back button is pressed.");
        AXLOGD("");
        AXLOGD("For example, add the following to your scene...");
        AXLOGD("auto listener = EventListenerKeyboard::create();");
        AXLOGD("listener->onKeyReleased = AX_CALLBACK_2(HelloWorld::onKeyReleased, this);");
        AXLOGD("getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);");
        AXLOGD("");
        AXLOGD("void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)");
        AXLOGD("{{");
        AXLOGD("     if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)");
        AXLOGD("     {{");
        AXLOGD("         if (myAppShouldNotQuit) // or whatever logic you want...");
        AXLOGD("         {{");
        AXLOGD("             event->stopPropagation();");
        AXLOGD("         }}");
        AXLOGD("     }}");
        AXLOGD("}}");
        AXLOGD("");
        AXLOGD("You MUST call event->stopPropagation() if you don't want your app to quit!");
        AXLOGD("*********************************************************************");

        Director::getInstance()->end();
    }
}

bool GLViewImpl::AppShouldExit()
{
    return m_appShouldExit;
}

void GLViewImpl::OnPointerPressed(Windows::UI::Core::CoreWindow const& sender,
                                  Windows::UI::Core::PointerEventArgs const& args)
{
    OnPointerPressed(args);
}

void GLViewImpl::OnPointerPressed(Windows::UI::Core::PointerEventArgs const& args)
{
    intptr_t id = args.CurrentPoint().PointerId();
    Vec2 pt     = GetPoint(args);
    handleTouchesBegin(1, &id, &pt.x, &pt.y);
}

void GLViewImpl::OnPointerWheelChanged(Windows::UI::Core::CoreWindow const& sender,
                                       Windows::UI::Core::PointerEventArgs const& args)
{
    float direction = (float)args.CurrentPoint().Properties().MouseWheelDelta();
    intptr_t id     = 0;
    Vec2 p(0.0f, 0.0f);
    handleTouchesBegin(1, &id, &p.x, &p.y);
    p.y += direction;
    handleTouchesMove(1, &id, &p.x, &p.y);
    handleTouchesEnd(1, &id, &p.x, &p.y);
}

void GLViewImpl::OnVisibilityChanged(Windows::UI::Core::CoreWindow const& sender,
                                     Windows::UI::Core::VisibilityChangedEventArgs const& args)
{
    m_windowVisible = args.Visible();
}

void GLViewImpl::OnWindowClosed(Windows::UI::Core::CoreWindow const& sender,
                                Windows::UI::Core::CoreWindowEventArgs const& args)
{
    m_windowClosed = true;
}

void GLViewImpl::OnPointerMoved(Windows::UI::Core::CoreWindow const& sender,
                                Windows::UI::Core::PointerEventArgs const& args)
{
    OnPointerMoved(args);
}

void GLViewImpl::OnPointerMoved(Windows::UI::Core::PointerEventArgs const& args)
{
    auto currentPoint = args.CurrentPoint();
    if (currentPoint.IsInContact())
    {
        if (m_lastPointValid)
        {
            intptr_t id = args.CurrentPoint().PointerId();
            Vec2 p      = GetPoint(args);
            handleTouchesMove(1, &id, &p.x, &p.y);
        }
        m_lastPoint      = currentPoint.Position();
        m_lastPointValid = true;
    }
    else
    {
        m_lastPointValid = false;
    }
}

void GLViewImpl::OnPointerReleased(Windows::UI::Core::CoreWindow const& sender,
                                   Windows::UI::Core::PointerEventArgs const& args)
{
    OnPointerReleased(args);
}

void GLViewImpl::OnPointerReleased(Windows::UI::Core::PointerEventArgs const& args)
{
    intptr_t id = args.CurrentPoint().PointerId();
    Vec2 pt     = GetPoint(args);
    handleTouchesEnd(1, &id, &pt.x, &pt.y);
}

void ax::GLViewImpl::OnMousePressed(Windows::UI::Core::PointerEventArgs const& args)
{
    Vec2 pt = GetPoint(args);

    // Emulated touch, if left mouse button
    if (args.CurrentPoint().Properties().IsLeftButtonPressed())
    {
        intptr_t id = 0;
        handleTouchesBegin(1, &id, &pt.x, &pt.y);
    }

    float x = transformInputX(pt.x);
    float y = transformInputY(pt.y);
    if (_lastMouseButtonPressed != EventMouse::MouseButton::BUTTON_UNSET)
    {
        EventMouse event(EventMouse::MouseEventType::MOUSE_UP);

        event.setMouseInfo(x, y, _lastMouseButtonPressed);
        Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }

    EventMouse event(EventMouse::MouseEventType::MOUSE_DOWN);
    // Set current button
    if (args.CurrentPoint().Properties().IsLeftButtonPressed())
    {
        _lastMouseButtonPressed = EventMouse::MouseButton::BUTTON_LEFT;
    }
    else if (args.CurrentPoint().Properties().IsRightButtonPressed())
    {
        _lastMouseButtonPressed = EventMouse::MouseButton::BUTTON_RIGHT;
    }
    else if (args.CurrentPoint().Properties().IsMiddleButtonPressed())
    {
        _lastMouseButtonPressed = EventMouse::MouseButton::BUTTON_MIDDLE;
    }
    event.setMouseInfo(x, y, _lastMouseButtonPressed);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void ax::GLViewImpl::OnMouseMoved(Windows::UI::Core::PointerEventArgs const& args)
{
    Vec2 pt = GetPoint(args);

    // Emulated touch, if left mouse button
    if (args.CurrentPoint().Properties().IsLeftButtonPressed())
    {
        intptr_t id = 0;
        handleTouchesMove(1, &id, &pt.x, &pt.y);
    }

    EventMouse event(EventMouse::MouseEventType::MOUSE_MOVE);

    event.setMouseInfo(transformInputX(pt.x), transformInputY(pt.y), checkMouseButton(args));
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void ax::GLViewImpl::OnMouseReleased(Windows::UI::Core::PointerEventArgs const& args)
{
    Vec2 pt = GetPoint(args);

    // Emulated touch, if left mouse button
    if (_lastMouseButtonPressed == EventMouse::MouseButton::BUTTON_LEFT)
    {
        intptr_t id = 0;
        handleTouchesEnd(1, &id, &pt.x, &pt.y);
    }

    EventMouse event(EventMouse::MouseEventType::MOUSE_UP);

    event.setMouseInfo(transformInputX(pt.x), transformInputY(pt.y), _lastMouseButtonPressed);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);

    _lastMouseButtonPressed = EventMouse::MouseButton::BUTTON_UNSET;
}

void ax::GLViewImpl::OnMouseWheelChanged(Windows::UI::Core::PointerEventArgs const& args)
{
    Vec2 pt = GetPoint(args);
    EventMouse event(EventMouse::MouseEventType::MOUSE_SCROLL);
    // Because OpenGL and axmol uses different Y axis, we need to convert the coordinate here
    float delta   = static_cast<float>(args.CurrentPoint().Properties().MouseWheelDelta());
    if (args.CurrentPoint().Properties().IsHorizontalMouseWheel())
    {
        event.setScrollData(delta / WHEEL_DELTA, 0.0f);
    }
    else
    {
        event.setScrollData(0.0f, -delta / WHEEL_DELTA);
    }
    event.setMouseInfo(transformInputX(pt.x), transformInputY(pt.y), checkMouseButton(args));
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void GLViewImpl::resize(int width, int height) {}

void GLViewImpl::setFrameZoomFactor(float fZoomFactor)
{
    _frameZoomFactor = fZoomFactor;
    Director::getInstance()->setProjection(Director::getInstance()->getProjection());
    // resize(m_obScreenSize.width * fZoomFactor, m_obScreenSize.height * fZoomFactor);
}

float GLViewImpl::getFrameZoomFactor()
{
    return _frameZoomFactor;
}

void GLViewImpl::centerWindow()
{
    // not implemented in WinRT. Window is always full screen
}

GLViewImpl* GLViewImpl::sharedGLView()
{
    return s_pEglView;
}

int GLViewImpl::Run()
{
    // XAML version does not have a run loop
    m_running = true;
    return 0;
};

void GLViewImpl::Render()
{
    OnRendering();
}

void GLViewImpl::OnRendering()
{
    if (m_running && m_initialized)
    {
        Director::getInstance()->mainLoop();
    }
}

// called by orientation change from WP8 XAML
void GLViewImpl::UpdateOrientation(Windows::Graphics::Display::DisplayOrientations orientation)
{
    if (m_orientation != orientation)
    {
        m_orientation = orientation;
        UpdateWindowSize();
    }
}

// called by size change from WP8 XAML
void GLViewImpl::UpdateForWindowSizeChange(float width, float height)
{
    if (width != m_width || height != m_height)
    {
        m_width  = width;
        m_height = height;
        UpdateWindowSize();
    }
}

void GLViewImpl::UpdateWindowSize()
{
    float width, height;

    width  = m_width;
    height = m_height;

    // CCSize designSize = getDesignResolutionSize();
    if (!m_initialized)
    {
        m_initialized = true;
        GLView::setFrameSize(width, height);
    }

    auto view = Director::getInstance()->getGLView();
    if (view && view->getResolutionPolicy() != ResolutionPolicy::UNKNOWN)
    {
        Size resSize               = view->getDesignResolutionSize();
        ResolutionPolicy resPolicy = view->getResolutionPolicy();
        view->setFrameSize(width, height);
        view->setDesignResolutionSize(resSize.width, resSize.height, resPolicy);
        auto director = Director::getInstance();
        director->setViewport();
        director->setProjection(director->getProjection());
    }
}

ax::Vec2 GLViewImpl::TransformToOrientation(Windows::Foundation::Point const& p)
{
    ax::Vec2 returnValue;

    float x     = p.X;
    float y     = p.Y;
    returnValue = Vec2(x, y);

    float zoomFactor = GLViewImpl::sharedGLView()->getFrameZoomFactor();
    if (zoomFactor > 0.0f)
    {
        returnValue.x /= zoomFactor;
        returnValue.y /= zoomFactor;
    }

    // AXLOGD("{:.2f} {:.2f} : {:.2f} {:.2f}", p.X, p.Y,returnValue.x, returnValue.y);

    return returnValue;
}

Vec2 GLViewImpl::GetPoint(Windows::UI::Core::PointerEventArgs const& args)
{
    return TransformToOrientation(args.CurrentPoint().Position());
}

void GLViewImpl::QueueBackKeyPress()
{
    std::shared_ptr<BackButtonEvent> e(new BackButtonEvent());
    mInputEvents.push(e);
}

void GLViewImpl::QueuePointerEvent(PointerEventType type, Windows::UI::Core::PointerEventArgs const& args)
{
    std::shared_ptr<PointerEvent> e(new PointerEvent(type, args));
    mInputEvents.push(e);
}

void GLViewImpl::QueueWinRTKeyboardEvent(WinRTKeyboardEventType type, Windows::UI::Core::KeyEventArgs const& args)
{
    std::shared_ptr<WinRTKeyboardEvent> e(new WinRTKeyboardEvent(type, args));
    mInputEvents.push(e);
}

void GLViewImpl::OnWinRTKeyboardEvent(WinRTKeyboardEventType type, Windows::UI::Core::KeyEventArgs const& args)
{
    m_keyboard.OnWinRTKeyboardEvent(type, args);
}

void GLViewImpl::QueueEvent(std::shared_ptr<InputEvent>& event)
{
    mInputEvents.push(event);
}

void GLViewImpl::ProcessEvents()
{
    std::shared_ptr<InputEvent> e;
    while (mInputEvents.try_pop(e))
    {
        e->execute();
    }
}

void GLViewImpl::SetQueueOperationCb(std::function<void(AsyncOperation, void*)> cb)
{
    mQueueOperationCb = std::move(cb);
}

void GLViewImpl::queueOperation(AsyncOperation op, void* param)
{
    if (mQueueOperationCb)
        mQueueOperationCb(std::move(op), param);
}

}
