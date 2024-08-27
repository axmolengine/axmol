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

#include "GRoot.h"
#include "AudioEngine.h"
#include "UIConfig.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
using namespace ax;

#if COCOS2D_VERSION < 0x00040000
using namespace ax::experimental;
#endif

GRoot* GRoot::_inst = nullptr;
bool GRoot::_soundEnabled = true;
float GRoot::_soundVolumeScale = 1.0f;
int GRoot::contentScaleLevel = 0;

GRoot* GRoot::create(Scene* scene, int zOrder)
{
    GRoot* pRet = new GRoot();
    if (pRet->initWithScene(scene, zOrder))
    {
        pRet->autorelease();
        return pRet;
    }
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

GRoot::GRoot() : _windowSizeListener(nullptr),
                 _inputProcessor(nullptr),
                 _modalLayer(nullptr),
                 _modalWaitPane(nullptr),
                 _tooltipWin(nullptr),
                 _defaultTooltipWin(nullptr)
{
}

GRoot::~GRoot()
{
    delete _inputProcessor;
    AX_SAFE_RELEASE(_modalWaitPane);
    AX_SAFE_RELEASE(_defaultTooltipWin);
    AX_SAFE_RELEASE(_modalLayer);
    CALL_LATER_CANCEL(GRoot, doShowTooltipsWin);

    if (_windowSizeListener)
        Director::getInstance()->getEventDispatcher()->removeEventListener(_windowSizeListener);
}

void GRoot::showWindow(Window* win)
{
    addChild(win);
    adjustModalLayer();
}

void GRoot::hideWindow(Window* win)
{
    win->hide();
}

void GRoot::hideWindowImmediately(Window* win)
{
    if (win->getParent() == this)
        removeChild(win);

    adjustModalLayer();
}

void GRoot::bringToFront(Window* win)
{
    int cnt = numChildren();
    int i;
    if (_modalLayer->getParent() != nullptr && !win->isModal())
        i = getChildIndex(_modalLayer) - 1;
    else
        i = cnt - 1;

    for (; i >= 0; i--)
    {
        GObject* g = getChildAt(i);
        if (g == win)
            return;
        if (dynamic_cast<Window*>(g))
            break;
    }

    if (i >= 0)
        setChildIndex(win, i);
}

void GRoot::closeAllExceptModals()
{
    Vector<GObject*> map(_children);

    for (const auto& child : map)
    {
        if (dynamic_cast<Window*>(child) && !((Window*)child)->isModal())
            hideWindowImmediately((Window*)child);
    }
}

void GRoot::closeAllWindows()
{
    Vector<GObject*> map(_children);

    for (const auto& child : map)
    {
        if (dynamic_cast<Window*>(child))
            hideWindowImmediately((Window*)child);
    }
}

Window* GRoot::getTopWindow()
{
    int cnt = numChildren();
    for (int i = cnt - 1; i >= 0; i--)
    {
        GObject* child = getChildAt(i);
        if (dynamic_cast<Window*>(child))
        {
            return (Window*)child;
        }
    }

    return nullptr;
}

GGraph* GRoot::getModalLayer()
{
    if (_modalLayer == nullptr)
        createModalLayer();

    return _modalLayer;
}

void GRoot::createModalLayer()
{
    _modalLayer = GGraph::create();
    _modalLayer->retain();
    _modalLayer->drawRect(getWidth(), getHeight(), 0, Color4F::WHITE, UIConfig::modalLayerColor);
    _modalLayer->addRelation(this, RelationType::Size);
}

void GRoot::adjustModalLayer()
{
    if (_modalLayer == nullptr)
        createModalLayer();

    int cnt = numChildren();

    if (_modalWaitPane != nullptr && _modalWaitPane->getParent() != nullptr)
        setChildIndex(_modalWaitPane, cnt - 1);

    for (int i = cnt - 1; i >= 0; i--)
    {
        GObject* child = getChildAt(i);
        if (dynamic_cast<Window*>(child) && ((Window*)child)->isModal())
        {
            if (_modalLayer->getParent() == nullptr)
                addChildAt(_modalLayer, i);
            else
                setChildIndexBefore(_modalLayer, i);
            return;
        }
    }

    if (_modalLayer->getParent() != nullptr)
        removeChild(_modalLayer);
}

bool GRoot::hasModalWindow()
{
    return _modalLayer != nullptr && _modalLayer->getParent() != nullptr;
}

void GRoot::showModalWait()
{
    getModalWaitingPane();
    if (_modalWaitPane)
        addChild(_modalWaitPane);
}

void GRoot::closeModalWait()
{
    if (_modalWaitPane != nullptr && _modalWaitPane->getParent() != nullptr)
        removeChild(_modalWaitPane);
}

GObject* GRoot::getModalWaitingPane()
{
    if (!UIConfig::globalModalWaiting.empty())
    {
        if (_modalWaitPane == nullptr)
        {
            _modalWaitPane = UIPackage::createObjectFromURL(UIConfig::globalModalWaiting);
            _modalWaitPane->setSortingOrder(INT_MAX);
            _modalWaitPane->retain();
        }

        _modalWaitPane->setSize(getWidth(), getHeight());
        _modalWaitPane->addRelation(this, RelationType::Size);

        return _modalWaitPane;
    }
    else
        return nullptr;
}

bool GRoot::isModalWaiting()
{
    return (_modalWaitPane != nullptr) && _modalWaitPane->onStage();
}

ax::Vec2 GRoot::getTouchPosition(int touchId)
{
    return _inputProcessor->getTouchPosition(touchId);
}

GObject* GRoot::getTouchTarget()
{
    return _inputProcessor->getRecentInput()->getTarget();
}

ax::Vec2 GRoot::worldToRoot(const ax::Vec2 &pt)
{
    ax::Vec2 pos = _displayObject->convertToNodeSpace(pt);
    pos.y = getHeight() - pos.y;
    return pos;
}

ax::Vec2 GRoot::rootToWorld(const ax::Vec2 &pt)
{
    ax::Vec2 pos = pt;
    pos.y = getHeight() - pos.y;
    pos = _displayObject->convertToWorldSpace(pos);
    return pos;
}

void GRoot::showPopup(GObject* popup)
{
    showPopup(popup, nullptr, PopupDirection::AUTO);
}

void GRoot::showPopup(GObject* popup, GObject* target, PopupDirection dir)
{
    if (!_popupStack.empty())
        hidePopup(popup);

    _popupStack.push_back(WeakPtr(popup));

    if (target != nullptr)
    {
        GObject* p = target;
        while (p != nullptr)
        {
            if (p->getParent() == this)
            {
                if (popup->getSortingOrder() < p->getSortingOrder())
                {
                    popup->setSortingOrder(p->getSortingOrder());
                }
                break;
            }
            p = p->getParent();
        }
    }

    addChild(popup);
    adjustModalLayer();

    if (dynamic_cast<Window*>(popup) && target == nullptr && dir == PopupDirection::AUTO)
        return;

    Vec2 pos = getPoupPosition(popup, target, dir);
    popup->setPosition(pos.x, pos.y);
}

void GRoot::togglePopup(GObject* popup)
{
    togglePopup(popup, nullptr, PopupDirection::AUTO);
}

void GRoot::togglePopup(GObject* popup, GObject* target, PopupDirection dir)
{
    if (std::find(_justClosedPopups.cbegin(), _justClosedPopups.cend(), popup) != _justClosedPopups.cend())
        return;

    showPopup(popup, target, dir);
}

void GRoot::hidePopup()
{
    hidePopup(nullptr);
}

void GRoot::hidePopup(GObject* popup)
{
    if (popup != nullptr)
    {
        auto it = std::find(_popupStack.cbegin(), _popupStack.cend(), popup);
        if (it != _popupStack.cend())
        {
            int k = (int)(it - _popupStack.cbegin());
            for (int i = (int)_popupStack.size() - 1; i >= k; i--)
            {
                closePopup(_popupStack.back().ptr());
                _popupStack.pop_back();
            }
        }
    }
    else
    {
        for (const auto& it : _popupStack)
            closePopup(it.ptr());
        _popupStack.clear();
    }
}

void GRoot::closePopup(GObject* target)
{
    if (target && target->getParent() != nullptr)
    {
        if (dynamic_cast<Window*>(target))
            ((Window*)target)->hide();
        else
            removeChild(target);
    }
}

void GRoot::checkPopups()
{
    _justClosedPopups.clear();
    if (!_popupStack.empty())
    {
        GObject* mc = _inputProcessor->getRecentInput()->getTarget();
        bool handled = false;
        while (mc != this && mc != nullptr)
        {
            auto it = std::find(_popupStack.cbegin(), _popupStack.cend(), mc);
            if (it != _popupStack.cend())
            {
                int k = (int)(it - _popupStack.cbegin());
                for (int i = (int)_popupStack.size() - 1; i > k; i--)
                {
                    closePopup(_popupStack.back().ptr());
                    _popupStack.pop_back();
                }
                handled = true;
                break;
            }
            mc = mc->findParent();
        }

        if (!handled)
        {
            for (int i = (int)_popupStack.size() - 1; i >= 0; i--)
            {
                GObject* popup = _popupStack[i].ptr();
                if (popup)
                {
                    _justClosedPopups.push_back(WeakPtr(popup));
                    closePopup(popup);
                }
            }
            _popupStack.clear();
        }
    }
}

bool GRoot::hasAnyPopup()
{
    return !_popupStack.empty();
}

ax::Vec2 GRoot::getPoupPosition(GObject* popup, GObject* target, PopupDirection dir)
{
    Vec2 pos;
    Vec2 size;
    if (target != nullptr)
    {
        pos = target->localToGlobal(Vec2::ZERO);
        pos = this->globalToLocal(pos);
        size = target->localToGlobal(target->getSize());
        size = this->globalToLocal(size);
        size -= pos;
    }
    else
    {
        pos = globalToLocal(_inputProcessor->getRecentInput()->getPosition());
    }
    float xx, yy;
    xx = pos.x;
    if (xx + popup->getWidth() > getWidth())
        xx = xx + size.x - popup->getWidth();
    yy = pos.y + size.y;
    if ((dir == PopupDirection::AUTO && yy + popup->getHeight() > getHeight()) || dir == PopupDirection::UP)
    {
        yy = pos.y - popup->getHeight() - 1;
        if (yy < 0)
        {
            yy = 0;
            xx += size.x / 2;
        }
    }

    return Vec2(round(xx), round(yy));
}

void GRoot::showTooltips(const std::string& msg)
{
    if (_defaultTooltipWin == nullptr)
    {
        const std::string& resourceURL = UIConfig::tooltipsWin;
        if (resourceURL.empty())
        {
            AXLOGW("FairyGUI: UIConfig.tooltipsWin not defined");
            return;
        }

        _defaultTooltipWin = UIPackage::createObjectFromURL(resourceURL);
        _defaultTooltipWin->setTouchable(false);
        _defaultTooltipWin->retain();
    }

    _defaultTooltipWin->setText(msg);
    showTooltipsWin(_defaultTooltipWin);
}

void GRoot::showTooltipsWin(GObject* tooltipWin)
{
    hideTooltips();

    _tooltipWin = tooltipWin;
    CALL_LATER(GRoot, doShowTooltipsWin, 0.1f);
}

void GRoot::doShowTooltipsWin()
{
    if (_tooltipWin == nullptr)
        return;

    Vec2 pt = _inputProcessor->getRecentInput()->getPosition();
    float xx = pt.x + 10;
    float yy = pt.y + 20;

    pt = globalToLocal(Vec2(xx, yy));
    xx = pt.x;
    yy = pt.y;

    if (xx + _tooltipWin->getWidth() > getWidth())
        xx = xx - _tooltipWin->getWidth();
    if (yy + _tooltipWin->getHeight() > getHeight())
    {
        yy = yy - _tooltipWin->getHeight() - 1;
        if (yy < 0)
            yy = 0;
    }

    _tooltipWin->setPosition(round(xx), round(yy));
    addChild(_tooltipWin);
}

void GRoot::hideTooltips()
{
    if (_tooltipWin != nullptr)
    {
        if (_tooltipWin->getParent() != nullptr)
            removeChild(_tooltipWin);
        _tooltipWin = nullptr;
    }
}

void GRoot::playSound(const std::string& url, float volumnScale)
{
    if (!_soundEnabled)
        return;

    PackageItem* pi = UIPackage::getItemByURL(url);
    if (pi)
        AudioEngine::play2d(pi->file, false, _soundVolumeScale * volumnScale);
}

void GRoot::setSoundEnabled(bool value)
{
    _soundEnabled = value;
}

void GRoot::setSoundVolumeScale(float value)
{
    _soundVolumeScale = value;
}

void GRoot::onTouchEvent(int eventType)
{
    if (eventType == UIEventType::TouchBegin)
    {
        if (_tooltipWin != nullptr)
            hideTooltips();

        checkPopups();
    }
}

void GRoot::handlePositionChanged()
{
    _displayObject->setPosition(0, _size.height);
}

void GRoot::onEnter()
{
    GComponent::onEnter();
    _inst = this;
}

void GRoot::onExit()
{
    GComponent::onExit();
    if (_inst == this)
        _inst = nullptr;
}

bool GRoot::initWithScene(ax::Scene* scene, int zOrder)
{
    if (!GComponent::init())
        return false;

    if (_inst == nullptr)
        _inst = this;

    _inputProcessor = new InputProcessor(this);
    _inputProcessor->setCaptureCallback(AX_CALLBACK_1(GRoot::onTouchEvent, this));

#if defined(AX_PLATFORM_PC) && AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
    _windowSizeListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(GLViewImpl::EVENT_WINDOW_RESIZED, AX_CALLBACK_0(GRoot::onWindowSizeChanged, this));
#endif
    onWindowSizeChanged();

    scene->addChild(_displayObject, zOrder);

    return true;
}

void GRoot::onWindowSizeChanged()
{
    const ax::Size& rs = Director::getInstance()->getGLView()->getDesignResolutionSize();
    setSize(rs.width, rs.height);

    updateContentScaleLevel();
}

void GRoot::updateContentScaleLevel()
{
    float ss = Director::getInstance()->getContentScaleFactor();
    if (ss >= 3.5f)
        contentScaleLevel = 3; //x4
    else if (ss >= 2.5f)
        contentScaleLevel = 2; //x3
    else if (ss >= 1.5f)
        contentScaleLevel = 1; //x2
    else
        contentScaleLevel = 0;
}

NS_FGUI_END
