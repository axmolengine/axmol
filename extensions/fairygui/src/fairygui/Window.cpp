#include "Window.h"
#include "GRoot.h"
#include "UIPackage.h"
#include "UIConfig.h"

NS_FGUI_BEGIN
using namespace ax;

Window::Window() :
    _requestingCmd(0),
    _frame(nullptr),
    _contentPane(nullptr),
    _modalWaitPane(nullptr),
    _closeButton(nullptr),
    _dragArea(nullptr),
    _contentArea(nullptr),
    _modal(false),
    _inited(false),
    _loading(false)
{
    _bringToFontOnClick = UIConfig::bringWindowToFrontOnClick;
}

Window::~Window()
{
    AX_SAFE_RELEASE(_contentPane);
    AX_SAFE_RELEASE(_frame);
    AX_SAFE_RELEASE(_closeButton);
    AX_SAFE_RELEASE(_dragArea);
    AX_SAFE_RELEASE(_modalWaitPane);
}

void Window::handleInit()
{
    GComponent::handleInit();

    addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(Window::onTouchBegin, this));
}

void Window::setContentPane(GComponent* value)
{
    if (_contentPane != value)
    {
        if (_contentPane != nullptr)
        {
            removeChild(_contentPane);

            AX_SAFE_RELEASE(_frame);
            _contentPane->release();
        }
        _contentPane = value;
        if (_contentPane != nullptr)
        {
            _contentPane->retain();

            addChild(_contentPane);
            setSize(_contentPane->getWidth(), _contentPane->getHeight());
            _contentPane->addRelation(this, RelationType::Size);
            _frame = dynamic_cast<GComponent*>(_contentPane->getChild("frame"));
            if (_frame != nullptr)
            {
                _frame->retain();

                setCloseButton(_frame->getChild("closeButton"));
                setDragArea(_frame->getChild("dragArea"));
                setContentArea(_frame->getChild("contentArea"));
            }
        }
        else
            _frame = nullptr;
    }
}

void Window::setCloseButton(GObject * value)
{
    if (_closeButton != nullptr)
    {
        _closeButton->removeClickListener(EventTag(this));
        _closeButton->release();
    }
    _closeButton = value;
    if (_closeButton != nullptr)
    {
        _closeButton->retain();
        _closeButton->addClickListener(AX_CALLBACK_1(Window::closeEventHandler, this), EventTag(this));
    }
}

void Window::setDragArea(GObject * value)
{
    if (_dragArea != value)
    {
        if (_dragArea != nullptr)
        {
            _dragArea->setDraggable(false);
            _dragArea->removeEventListener(UIEventType::DragStart, EventTag(this));
            _dragArea->release();
        }

        _dragArea = value;
        if (_dragArea != nullptr)
        {
            _dragArea->retain();
            if (dynamic_cast<GGraph*>(_dragArea) && ((GGraph*)_dragArea)->isEmpty())
                ((GGraph*)_dragArea)->drawRect(_dragArea->getWidth(), _dragArea->getHeight(), 0, Color4F(0, 0, 0, 0), Color4F(0, 0, 0, 0));
            _dragArea->setDraggable(true);
            _dragArea->addEventListener(UIEventType::DragStart, AX_CALLBACK_1(Window::onDragStart, this), EventTag(this));
        }
    }
}

void Window::show()
{
    UIRoot->showWindow(this);
}

void Window::hide()
{
    if (isShowing())
        doHideAnimation();
}

void Window::hideImmediately()
{
    UIRoot->hideWindowImmediately(this);
}

void Window::toggleStatus()
{
    if (isTop())
        hide();
    else
        show();
}

void Window::bringToFront()
{
    UIRoot->bringToFront(this);
}

bool Window::isTop() const
{
    return _parent != nullptr && _parent->getChildIndex(this) == _parent->numChildren() - 1;
}

void Window::showModalWait(int requestingCmd)
{
    if (requestingCmd != 0)
        _requestingCmd = requestingCmd;

    if (!UIConfig::windowModalWaiting.empty())
    {
        if (_modalWaitPane == nullptr)
        {
            _modalWaitPane = UIPackage::createObjectFromURL(UIConfig::windowModalWaiting);
            _modalWaitPane->retain();
        }

        layoutModalWaitPane();

        addChild(_modalWaitPane);
    }
}

void Window::layoutModalWaitPane()
{
    if (_contentArea != nullptr)
    {
        Vec2 pt = _frame->localToGlobal(Vec2::ZERO);
        pt = globalToLocal(pt);
        _modalWaitPane->setPosition((int)pt.x + _contentArea->getX(), (int)pt.y + _contentArea->getY());
        _modalWaitPane->setSize(_contentArea->getWidth(), _contentArea->getHeight());
    }
    else
        _modalWaitPane->setSize(_size.width, _size.height);
}

bool Window::closeModalWait(int requestingCmd)
{
    if (requestingCmd != 0)
    {
        if (_requestingCmd != requestingCmd)
            return false;
    }
    _requestingCmd = 0;

    if (_modalWaitPane != nullptr && _modalWaitPane->getParent() != nullptr)
        removeChild(_modalWaitPane);

    return true;
}

void Window::initWindow()
{
    if (_inited || _loading)
        return;

    if (!_uiSources.empty())
    {
        _loading = false;
        int cnt = (int)_uiSources.size();
        for (int i = 0; i < cnt; i++)
        {
            IUISource* lib = _uiSources.at(i);
            if (!lib->isLoaded())
            {
                lib->load(AX_CALLBACK_0(Window::onUILoadComplete, this));
                _loading = true;
            }
        }

        if (!_loading)
            _initWindow();
    }
    else
        _initWindow();
}

void Window::_initWindow()
{
    _inited = true;
    onInit();

    if (isShowing())
        doShowAnimation();
}

void Window::addUISource(IUISource * uiSource)
{
    _uiSources.pushBack(uiSource);
}

void Window::doShowAnimation()
{
    onShown();
}

void Window::doHideAnimation()
{
    hideImmediately();
}

void Window::closeEventHandler(EventContext * context)
{
    hide();
}

void Window::onUILoadComplete()
{
    int cnt = (int)_uiSources.size();
    for (int i = 0; i < cnt; i++)
    {
        IUISource* lib = _uiSources.at(i);
        if (!lib->isLoaded())
            return;
    }

    _loading = false;
    _initWindow();
}

void Window::onEnter()
{
    GComponent::onEnter();

    if (!_inited)
        initWindow();
    else
        doShowAnimation();
}

void Window::onExit()
{
    GComponent::onExit();

    closeModalWait();
    onHide();
}

void Window::onTouchBegin(EventContext * context)
{
    if (isShowing() && _bringToFontOnClick)
    {
        bringToFront();
    }
}

void Window::onDragStart(EventContext * context)
{
    context->preventDefault();

    startDrag(context->getInput()->getTouchId());
}


NS_FGUI_END