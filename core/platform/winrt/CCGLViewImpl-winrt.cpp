/****************************************************************************
Copyright (c) 2013 cocos2d-x.org
Copyright (c) Microsoft Open Technologies, Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

https://axmolengine.github.io/

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

#include "platform/winrt/CCGLViewImpl-winrt.h"
#include "base/ccMacros.h"
#include "base/CCDirector.h"
#include "base/CCTouch.h"
#include "base/CCIMEDispatcher.h"
#include "base/CCEventListenerKeyboard.h"
#include "platform/winrt/CCApplication-winrt.h"
#include "platform/winrt/CCWinRTUtils.h"
// #include "deprecated/CCNotificationCenter.h"
#include "base/CCEventDispatcher.h"
#include "base/CCEventMouse.h"

#include <map>

using namespace Platform;
using namespace Concurrency;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::Graphics::Display;
using namespace Windows::UI::Input;
using namespace Windows::UI::Core;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::System;
using namespace Windows::UI::ViewManagement;
using namespace Windows::ApplicationModel;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Platform;
using namespace Microsoft::WRL;

NS_AX_BEGIN

static GLViewImpl* s_pEglView = nullptr;

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
    , m_orientation(DisplayOrientations::Landscape)
    , m_appShouldExit(false)
    , _lastMouseButtonPressed(EventMouse::MouseButton::BUTTON_UNSET)
{
    s_pEglView = this;
    _viewName  = "axmol";
    m_keyboard = ref new KeyBoardWinRT();

    m_backButtonListener                = EventListenerKeyboard::create();
    m_backButtonListener->onKeyReleased = AX_CALLBACK_2(GLViewImpl::BackButtonListener, this);
    Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_backButtonListener, INT_MAX);
}

GLViewImpl::~GLViewImpl()
{
    AX_ASSERT(this == s_pEglView);
    s_pEglView = nullptr;

    // TODO: cleanup
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

void GLViewImpl::setDispatcher(Windows::UI::Core::CoreDispatcher ^ dispatcher)
{
    m_dispatcher = dispatcher;
}

void GLViewImpl::setPanel(Windows::UI::Xaml::Controls::Panel ^ panel)
{
    m_panel = panel;
}

void GLViewImpl::setIMEKeyboardState(bool bOpen)
{
    std::string str;
    setIMEKeyboardState(bOpen, str);
}

bool GLViewImpl::ShowMessageBox(Platform::String ^ title, Platform::String ^ message)
{
    if (m_dispatcher.Get())
    {
        m_dispatcher.Get()->RunAsync(Windows::UI::Core::CoreDispatcherPriority::Normal,
                                     ref new Windows::UI::Core::DispatchedHandler([title, message]() {
                                         // Show the message dialog
                                         auto msg = ref new Windows::UI::Popups::MessageDialog(message, title);
                                         // Set the command to be invoked when a user presses 'ESC'
                                         msg->CancelCommandIndex = 1;
                                         msg->ShowAsync();
                                     }));

        return true;
    }
    return false;
}

void GLViewImpl::setIMEKeyboardState(bool bOpen, std::string_view str)
{
    if (bOpen)
    {
        m_keyboard->ShowKeyboard(PlatformStringFromString(str));
    }
    else
    {
        m_keyboard->HideKeyboard(PlatformStringFromString(str));
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

void GLViewImpl::OnSuspending(Platform::Object ^ sender, SuspendingEventArgs ^ args) {}

void GLViewImpl::OnResuming(Platform::Object ^ sender, Platform::Object ^ args) {}

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
        AXLOG("*********************************************************************");
        AXLOG("GLViewImpl::BackButtonListener: Exiting application!");
        AXLOG("");
        AXLOG("If you want to listen for Windows Phone back button events,");
        AXLOG("add a listener for EventKeyboard::KeyCode::KEY_ESCAPE");
        AXLOG("Make sure you call stopPropagation() on the Event if you don't");
        AXLOG("want your app to exit when the back button is pressed.");
        AXLOG("");
        AXLOG("For example, add the following to your scene...");
        AXLOG("auto listener = EventListenerKeyboard::create();");
        AXLOG("listener->onKeyReleased = AX_CALLBACK_2(HelloWorld::onKeyReleased, this);");
        AXLOG("getEventDispatcher()->addEventListenerWithFixedPriority(listener, 1);");
        AXLOG("");
        AXLOG("void HelloWorld::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)");
        AXLOG("{");
        AXLOG("     if (keyCode == EventKeyboard::KeyCode::KEY_ESCAPE)");
        AXLOG("     {");
        AXLOG("         if (myAppShouldNotQuit) // or whatever logic you want...");
        AXLOG("         {");
        AXLOG("             event->stopPropagation();");
        AXLOG("         }");
        AXLOG("     }");
        AXLOG("}");
        AXLOG("");
        AXLOG("You MUST call event->stopPropagation() if you don't want your app to quit!");
        AXLOG("*********************************************************************");

        Director::getInstance()->end();
    }
}

bool GLViewImpl::AppShouldExit()
{
    return m_appShouldExit;
}

void GLViewImpl::OnPointerPressed(CoreWindow ^ sender, PointerEventArgs ^ args)
{
    OnPointerPressed(args);
}

void GLViewImpl::OnPointerPressed(PointerEventArgs ^ args)
{
    intptr_t id = args->CurrentPoint->PointerId;
    Vec2 pt     = GetPoint(args);
    handleTouchesBegin(1, &id, &pt.x, &pt.y);
}

void GLViewImpl::OnPointerWheelChanged(CoreWindow ^ sender, PointerEventArgs ^ args)
{
    float direction = (float)args->CurrentPoint->Properties->MouseWheelDelta;
    intptr_t id     = 0;
    Vec2 p(0.0f, 0.0f);
    handleTouchesBegin(1, &id, &p.x, &p.y);
    p.y += direction;
    handleTouchesMove(1, &id, &p.x, &p.y);
    handleTouchesEnd(1, &id, &p.x, &p.y);
}

void GLViewImpl::OnVisibilityChanged(CoreWindow ^ sender, VisibilityChangedEventArgs ^ args)
{
    m_windowVisible = args->Visible;
}

void GLViewImpl::OnWindowClosed(CoreWindow ^ sender, CoreWindowEventArgs ^ args)
{
    m_windowClosed = true;
}

void GLViewImpl::OnPointerMoved(CoreWindow ^ sender, PointerEventArgs ^ args)
{
    OnPointerMoved(args);
}

void GLViewImpl::OnPointerMoved(PointerEventArgs ^ args)
{
    auto currentPoint = args->CurrentPoint;
    if (currentPoint->IsInContact)
    {
        if (m_lastPointValid)
        {
            intptr_t id = args->CurrentPoint->PointerId;
            Vec2 p      = GetPoint(args);
            handleTouchesMove(1, &id, &p.x, &p.y);
        }
        m_lastPoint      = currentPoint->Position;
        m_lastPointValid = true;
    }
    else
    {
        m_lastPointValid = false;
    }
}

void GLViewImpl::OnPointerReleased(CoreWindow ^ sender, PointerEventArgs ^ args)
{
    OnPointerReleased(args);
}

void GLViewImpl::OnPointerReleased(PointerEventArgs ^ args)
{
    intptr_t id = args->CurrentPoint->PointerId;
    Vec2 pt     = GetPoint(args);
    handleTouchesEnd(1, &id, &pt.x, &pt.y);
}

void ax::GLViewImpl::OnMousePressed(Windows::UI::Core::PointerEventArgs ^ args)
{
    Vec2 mousePosition = GetPointMouse(args);

    // Emulated touch, if left mouse button
    if (args->CurrentPoint->Properties->IsLeftButtonPressed)
    {
        intptr_t id = 0;
        Vec2 pt     = GetPoint(args);
        handleTouchesBegin(1, &id, &pt.x, &pt.y);
    }

    if (_lastMouseButtonPressed != EventMouse::MouseButton::BUTTON_UNSET)
    {
        EventMouse event(EventMouse::MouseEventType::MOUSE_UP);

        event.setMouseButton(_lastMouseButtonPressed);
        event.setCursorPosition(mousePosition.x, mousePosition.y);
        Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
    }

    EventMouse event(EventMouse::MouseEventType::MOUSE_DOWN);
    // Set current button
    if (args->CurrentPoint->Properties->IsLeftButtonPressed)
    {
        _lastMouseButtonPressed = EventMouse::MouseButton::BUTTON_LEFT;
    }
    else if (args->CurrentPoint->Properties->IsRightButtonPressed)
    {
        _lastMouseButtonPressed = EventMouse::MouseButton::BUTTON_RIGHT;
    }
    else if (args->CurrentPoint->Properties->IsMiddleButtonPressed)
    {
        _lastMouseButtonPressed = EventMouse::MouseButton::BUTTON_MIDDLE;
    }
    event.setMouseButton(_lastMouseButtonPressed);
    event.setCursorPosition(mousePosition.x, mousePosition.y);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void ax::GLViewImpl::OnMouseMoved(Windows::UI::Core::PointerEventArgs ^ args)
{
    Vec2 mousePosition = GetPointMouse(args);

    // Emulated touch, if left mouse button
    if (args->CurrentPoint->Properties->IsLeftButtonPressed)
    {
        intptr_t id = 0;
        Vec2 pt     = GetPoint(args);
        handleTouchesMove(1, &id, &pt.x, &pt.y);
    }

    EventMouse event(EventMouse::MouseEventType::MOUSE_MOVE);
    // Set current button
    if (args->CurrentPoint->Properties->IsLeftButtonPressed)
    {
        event.setMouseButton(EventMouse::MouseButton::BUTTON_LEFT);
    }
    else if (args->CurrentPoint->Properties->IsRightButtonPressed)
    {
        event.setMouseButton(EventMouse::MouseButton::BUTTON_RIGHT);
    }
    else if (args->CurrentPoint->Properties->IsMiddleButtonPressed)
    {
        event.setMouseButton(EventMouse::MouseButton::BUTTON_MIDDLE);
    }
    event.setCursorPosition(mousePosition.x, mousePosition.y);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);
}

void ax::GLViewImpl::OnMouseReleased(Windows::UI::Core::PointerEventArgs ^ args)
{
    Vec2 mousePosition = GetPointMouse(args);

    // Emulated touch, if left mouse button
    if (_lastMouseButtonPressed == EventMouse::MouseButton::BUTTON_LEFT)
    {
        intptr_t id = 0;
        Vec2 pt     = GetPoint(args);
        handleTouchesEnd(1, &id, &pt.x, &pt.y);
    }

    EventMouse event(EventMouse::MouseEventType::MOUSE_UP);

    event.setMouseButton(_lastMouseButtonPressed);
    event.setCursorPosition(mousePosition.x, mousePosition.y);
    Director::getInstance()->getEventDispatcher()->dispatchEvent(&event);

    _lastMouseButtonPressed = EventMouse::MouseButton::BUTTON_UNSET;
}

void ax::GLViewImpl::OnMouseWheelChanged(Windows::UI::Core::PointerEventArgs ^ args)
{
    Vec2 mousePosition = GetPointMouse(args);
    EventMouse event(EventMouse::MouseEventType::MOUSE_SCROLL);
    // Because OpenGL and axmol uses different Y axis, we need to convert the coordinate here
    float cursorX = (mousePosition.x - _viewPortRect.origin.x) / _scaleX;
    float cursorY = (_viewPortRect.origin.y + _viewPortRect.size.height - mousePosition.y) / _scaleY;
    float delta   = args->CurrentPoint->Properties->MouseWheelDelta;
    if (args->CurrentPoint->Properties->IsHorizontalMouseWheel)
    {
        event.setScrollData(delta / WHEEL_DELTA, 0.0f);
    }
    else
    {
        event.setScrollData(0.0f, -delta / WHEEL_DELTA);
    }
    event.setCursorPosition(cursorX, cursorY);
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

GLViewImpl* GLViewImpl::sharedOpenGLView()
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
void GLViewImpl::UpdateOrientation(DisplayOrientations orientation)
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

    auto view = Director::getInstance()->getOpenGLView();
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

ax::Vec2 GLViewImpl::TransformToOrientation(Windows::Foundation::Point p)
{
    ax::Vec2 returnValue;

    float x     = p.X;
    float y     = p.Y;
    returnValue = Vec2(x, y);

    float zoomFactor = GLViewImpl::sharedOpenGLView()->getFrameZoomFactor();
    if (zoomFactor > 0.0f)
    {
        returnValue.x /= zoomFactor;
        returnValue.y /= zoomFactor;
    }

    // AXLOG("%.2f %.2f : %.2f %.2f", p.X, p.Y,returnValue.x, returnValue.y);

    return returnValue;
}

Vec2 GLViewImpl::GetPoint(PointerEventArgs ^ args)
{

    return TransformToOrientation(args->CurrentPoint->Position);
}

Vec2 GLViewImpl::GetPointMouse(PointerEventArgs ^ args)
{

    Vec2 position = TransformToOrientation(args->CurrentPoint->Position);

    // Because Windows and cocos2d-x uses different Y axis, we need to convert the coordinate here
    position.x = (position.x - _viewPortRect.origin.x) / _scaleX;
    position.y = (_viewPortRect.origin.y + _viewPortRect.size.height - position.y) / _scaleY;

    return position;
}

void GLViewImpl::QueueBackKeyPress()
{
    std::shared_ptr<BackButtonEvent> e(new BackButtonEvent());
    mInputEvents.push(e);
}

void GLViewImpl::QueuePointerEvent(PointerEventType type, PointerEventArgs ^ args)
{
    std::shared_ptr<PointerEvent> e(new PointerEvent(type, args));
    mInputEvents.push(e);
}

void GLViewImpl::QueueWinRTKeyboardEvent(WinRTKeyboardEventType type, KeyEventArgs ^ args)
{
    std::shared_ptr<WinRTKeyboardEvent> e(new WinRTKeyboardEvent(type, args));
    mInputEvents.push(e);
}

void GLViewImpl::OnWinRTKeyboardEvent(WinRTKeyboardEventType type, KeyEventArgs ^ args)
{
    m_keyboard->OnWinRTKeyboardEvent(type, args);
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

NS_AX_END
