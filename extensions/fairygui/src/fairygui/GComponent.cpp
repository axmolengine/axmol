#include "GComponent.h"
#include "GButton.h"
#include "GGroup.h"
#include "Relations.h"
#include "TranslationHelper.h"
#include "UIObjectFactory.h"
#include "UIPackage.h"
#include "display/FUIContainer.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

using namespace std;

GComponent::GComponent() : _container(nullptr),
_scrollPane(nullptr),
_childrenRenderOrder(ChildrenRenderOrder::ASCENT),
_apexIndex(0),
_boundsChanged(false),
_trackBounds(false),
_opaque(false),
_sortingChildCount(0),
_applyingController(nullptr),
_buildingDisplayList(false),
_maskOwner(nullptr),
_hitArea(nullptr)
{
}

GComponent::~GComponent()
{
    for (auto& child : _children)
        child->_parent = nullptr;
    _children.clear();
    _controllers.clear();
    _transitions.clear();
    AX_SAFE_RELEASE(_maskOwner);
    AX_SAFE_RELEASE(_container);
    AX_SAFE_RELEASE(_scrollPane);
    AX_SAFE_DELETE(_hitArea);
    CALL_LATER_CANCEL(GComponent, doUpdateBounds);
    CALL_LATER_CANCEL(GComponent, buildNativeDisplayList);
}

void GComponent::handleInit()
{
    FUIContainer* c = FUIContainer::create();
    c->retain();
    c->gOwner = this;

    _displayObject = c;

    _container = FUIInnerContainer::create();
    _container->retain();
    _container->setCascadeOpacityEnabled(true);
    _displayObject->addChild(_container);
}

GObject* GComponent::addChild(GObject* child)
{
    addChildAt(child, (int)_children.size());
    return child;
}

GObject* GComponent::addChildAt(GObject* child, int index)
{
    AXASSERT(child != nullptr, "Argument must be non-nil");

    if (child->_parent == this)
    {
        setChildIndex(child, index);
    }
    else
    {
        child->retain();
        child->removeFromParent();
        child->_parent = this;

        int cnt = (int)_children.size();
        if (child->_sortingOrder != 0)
        {
            _sortingChildCount++;
            index = getInsertPosForSortingChild(child);
        }
        else if (_sortingChildCount > 0)
        {
            if (index > (cnt - _sortingChildCount))
                index = cnt - _sortingChildCount;
        }

        if (index == cnt)
            _children.pushBack(child);
        else
            _children.insert(index, child);

        child->release();

        childStateChanged(child);
        setBoundsChangedFlag();
    }
    return child;
}

int GComponent::getInsertPosForSortingChild(GObject* target)
{
    size_t cnt = _children.size();
    size_t i;
    for (i = 0; i < cnt; i++)
    {
        GObject* child = _children.at(i);
        if (child == target)
            continue;

        if (target->_sortingOrder < child->_sortingOrder)
            break;
    }
    return (int)i;
}

void GComponent::removeChild(GObject* child)
{
    AXASSERT(child != nullptr, "Argument must be non-nil");

    int childIndex = (int)_children.getIndex(child);
    if (childIndex != -1)
        removeChildAt(childIndex);
}

void GComponent::removeChildAt(int index)
{
    AXASSERT(index >= 0 && index < _children.size(), "Invalid child index");

    GObject* child = _children.at(index);

    child->_parent = nullptr;

    if (child->_sortingOrder != 0)
        _sortingChildCount--;

    child->setGroup(nullptr);
    if (child->_displayObject->getParent() != nullptr)
    {
        _container->removeChild(child->_displayObject, false);
        if (_childrenRenderOrder == ChildrenRenderOrder::ARCH)
            CALL_LATER(GComponent, buildNativeDisplayList);
    }

    _children.erase(index);
    setBoundsChangedFlag();
}

void GComponent::removeChildren(int beginIndex, int endIndex)
{
    if (endIndex < 0 || endIndex >= _children.size())
        endIndex = (int)_children.size() - 1;

    for (int i = beginIndex; i <= endIndex; ++i)
        removeChildAt(beginIndex);
}

GObject* GComponent::getChildAt(int index) const
{
    AXASSERT(index >= 0 && index < _children.size(), "Invalid child index");

    return _children.at(index);
}

GObject* GComponent::getChild(const std::string& name) const
{
    for (const auto& child : _children)
    {
        if (child->name.compare(name) == 0)
            return child;
    }

    return nullptr;
}

GObject* GComponent::getChildByPath(const std::string& path) const
{
    const GComponent* gcom = this;
    GObject* obj = nullptr;

    FastSplitter fs;
    fs.start(path.data(), path.length(), '.');
    std::string str;
    while (fs.next())
    {
        if (gcom == nullptr)
        {
            gcom = dynamic_cast<GComponent*>(obj);
            if (gcom == nullptr)
            {
                obj = nullptr;
                break;
            }
        }

        str.append(fs.getText(), fs.getTextLength());
        obj = gcom->getChild(std::string(fs.getText(), fs.getTextLength()));
        if (!obj)
            break;

        gcom = nullptr;
    }

    return obj;
}

GObject* GComponent::getChildInGroup(const GGroup* group, const std::string& name) const
{
    AXASSERT(group != nullptr, "Argument must be non-nil");

    for (const auto& child : _children)
    {
        if (child->_group == group && child->name.compare(name) == 0)
            return child;
    }

    return nullptr;
}

GObject* GComponent::getChildById(const std::string& id) const
{
    for (const auto& child : _children)
    {
        if (child->id.compare(id) == 0)
            return child;
    }

    return nullptr;
}

int GComponent::getChildIndex(const GObject* child) const
{
    AXASSERT(child != nullptr, "Argument must be non-nil");

    return (int)_children.getIndex((GObject*)child);
}

void GComponent::setChildIndex(GObject* child, int index)
{
    AXASSERT(child != nullptr, "Argument must be non-nil");

    int oldIndex = (int)_children.getIndex(child);
    AXASSERT(oldIndex != -1, "Not a child of this container");

    if (child->_sortingOrder != 0) //no effect
        return;

    int cnt = (int)_children.size();
    if (_sortingChildCount > 0)
    {
        if (index > (cnt - _sortingChildCount - 1))
            index = cnt - _sortingChildCount - 1;
    }

    moveChild(child, oldIndex, index);
}

int GComponent::setChildIndexBefore(GObject* child, int index)
{
    AXASSERT(child != nullptr, "Argument must be non-nil");

    int oldIndex = (int)_children.getIndex(child);
    AXASSERT(oldIndex != -1, "Not a child of this container");

    if (child->_sortingOrder != 0) //no effect
        return oldIndex;

    int cnt = (int)_children.size();
    if (_sortingChildCount > 0)
    {
        if (index > (cnt - _sortingChildCount - 1))
            index = cnt - _sortingChildCount - 1;
    }

    if (oldIndex < index)
        return moveChild(child, oldIndex, index - 1);
    else
        return moveChild(child, oldIndex, index);
}

int GComponent::moveChild(GObject* child, int oldIndex, int index)
{
    int cnt = (int)_children.size();
    if (index > cnt)
        index = cnt;

    if (oldIndex == index)
        return oldIndex;

    child->retain();
    _children.erase(oldIndex);
    if (index >= cnt)
        _children.pushBack(child);
    else
        _children.insert(index, child);
    child->release();

    if (child->_displayObject->getParent() != nullptr)
    {
        if (_childrenRenderOrder == ChildrenRenderOrder::ASCENT)
        {
            int fromIndex = MIN(index, oldIndex);
            int toIndex = MIN(max(index, oldIndex), cnt - 1);
            for (int i = fromIndex; i <= toIndex; i++)
            {
                GObject* g = _children.at(i);
                if (g->_displayObject->getParent() != nullptr)
                    g->_displayObject->setLocalZOrder(i);
            }
        }
        else if (_childrenRenderOrder == ChildrenRenderOrder::DESCENT)
        {
            int fromIndex = MIN(index, oldIndex);
            int toIndex = MIN(max(index, oldIndex), cnt - 1);
            for (int i = fromIndex; i <= toIndex; i++)
            {
                GObject* g = _children.at(i);
                if (g->_displayObject->getParent() != nullptr)
                    g->_displayObject->setLocalZOrder(cnt - 1 - i);
            }
        }
        else
            CALL_LATER(GComponent, buildNativeDisplayList);

        setBoundsChangedFlag();
    }

    return index;
}

void GComponent::swapChildren(GObject* child1, GObject* child2)
{
    AXASSERT(child1 != nullptr, "Argument1 must be non-nil");
    AXASSERT(child2 != nullptr, "Argument2 must be non-nil");

    int index1 = (int)_children.getIndex(child1);
    int index2 = (int)_children.getIndex(child2);

    AXASSERT(index1 != -1, "Not a child of this container");
    AXASSERT(index2 != -1, "Not a child of this container");

    swapChildrenAt(index1, index2);
}

void GComponent::swapChildrenAt(int index1, int index2)
{
    GObject* child1 = _children.at(index1);
    GObject* child2 = _children.at(index2);

    setChildIndex(child1, index2);
    setChildIndex(child2, index1);
}

int GComponent::numChildren() const
{
    return (int)_children.size();
}

bool GComponent::isAncestorOf(const GObject* obj) const
{
    if (obj == nullptr)
        return false;

    GComponent* p = obj->_parent;
    while (p != nullptr)
    {
        if (p == this)
            return true;

        p = p->_parent;
    }
    return false;
}

bool GComponent::isChildInView(GObject* child)
{
    if (_scrollPane != nullptr)
    {
        return _scrollPane->isChildInView(child);
    }
    else if (((FUIContainer*)_displayObject)->isClippingEnabled())
    {
        return child->_position.x + child->_size.width >= 0 && child->_position.x <= _size.width && child->_position.y + child->_size.height >= 0 && child->_position.y <= _size.height;
    }
    else
        return true;
}

int GComponent::getFirstChildInView()
{
    int i = 0;
    for (auto& child : _children)
    {

        if (isChildInView(child))
            return i;
        i++;
    }
    return -1;
}

GController* GComponent::getController(const std::string& name) const
{
    for (const auto& c : _controllers)
    {
        if (c->name.compare(name) == 0)
            return c;
    }

    return nullptr;
}

void GComponent::addController(GController* c)
{
    AXASSERT(c != nullptr, "Argument must be non-nil");

    _controllers.pushBack(c);
}

GController* GComponent::getControllerAt(int index) const
{
    AXASSERT(index >= 0 && index < _controllers.size(), "Invalid controller index");

    return _controllers.at(index);
}

void GComponent::removeController(GController* c)
{
    AXASSERT(c != nullptr, "Argument must be non-nil");

    ssize_t index = _controllers.getIndex(c);
    AXASSERT(index != -1, "controller not exists");

    c->setParent(nullptr);
    applyController(c);
    _controllers.erase(index);
}

void GComponent::applyController(GController* c)
{
    _applyingController = c;

    ssize_t count = _children.size();
    for (ssize_t i = 0; i < count; i++)
        _children.at(i)->handleControllerChanged(c);

    _applyingController = nullptr;

    c->runActions();
}

void GComponent::applyAllControllers()
{
    for (const auto& c : _controllers)
        applyController(c);
}

Transition* GComponent::getTransition(const std::string& name) const
{
    for (const auto& c : _transitions)
    {
        if (c->name.compare(name) == 0)
            return c;
    }

    return nullptr;
}

Transition* GComponent::getTransitionAt(int index) const
{
    AXASSERT(index >= 0 && index < _transitions.size(), "Invalid transition index");

    return _transitions.at(index);
}

void GComponent::adjustRadioGroupDepth(GObject* obj, GController* c)
{
    ssize_t cnt = (ssize_t)_children.size();
    ssize_t i;
    GObject* child;
    ssize_t myIndex = -1, maxIndex = -1;
    for (i = 0; i < cnt; i++)
    {
        child = _children.at(i);
        if (child == obj)
        {
            myIndex = i;
        }
        else if (dynamic_cast<GButton*>(child) && ((GButton*)child)->getRelatedController() == c)
        {
            if (i > maxIndex)
                maxIndex = i;
        }
    }
    if (myIndex < maxIndex)
    {
        if (_applyingController != nullptr)
            _children.at(maxIndex)->handleControllerChanged(_applyingController);
        swapChildrenAt((int)myIndex, (int)maxIndex);
    }
}

void GComponent::setOpaque(bool value)
{
    _opaque = value;
}

void GComponent::setMargin(const Margin& value)
{
    _margin = value;
}

void GComponent::setChildrenRenderOrder(ChildrenRenderOrder value)
{
    if (_childrenRenderOrder != value)
    {
        _childrenRenderOrder = value;
        CALL_LATER(GComponent, buildNativeDisplayList);
    }
}

void GComponent::setApexIndex(int value)
{
    if (_apexIndex != value)
    {
        _apexIndex = value;

        if (_childrenRenderOrder == ChildrenRenderOrder::ARCH)
            CALL_LATER(GComponent, buildNativeDisplayList);
    }
}

ax::Node* GComponent::getMask() const
{
    return ((FUIContainer*)_displayObject)->getStencil();
}

void GComponent::setMask(ax::Node* value, bool inverted)
{
    if (_maskOwner)
    {
        _maskOwner->_alignToBL = false;
        childStateChanged(_maskOwner);
        _maskOwner->handlePositionChanged();
        _maskOwner->release();
        _maskOwner = nullptr;
    }

    if (value)
    {
        for (auto& child : _children)
        {
            if (child->_displayObject == value)
            {
                _maskOwner = child;
                if (value->getParent())
                    value->getParent()->removeChild(value, false);
                _maskOwner->_alignToBL = true;
                _maskOwner->handlePositionChanged();
                _maskOwner->retain();
                break;
            }
        }
    }

    ((FUIContainer*)_displayObject)->setStencil(value);
    if (value)
    {
        ((FUIContainer*)_displayObject)->setAlphaThreshold(0.05f);
        ((FUIContainer*)_displayObject)->setInverted(inverted);
    }
}

void GComponent::setHitArea(IHitTest* value)
{
    if (_hitArea != value)
    {
        AX_SAFE_DELETE(_hitArea);
        _hitArea = value;
    }
}

float GComponent::getViewWidth() const
{
    if (_scrollPane != nullptr)
        return _scrollPane->getViewSize().width;
    else
        return _size.width - _margin.left - _margin.right;
}

void GComponent::setViewWidth(float value)
{
    if (_scrollPane != nullptr)
        _scrollPane->setViewWidth(value);
    else
        setWidth(value + _margin.left + _margin.right);
}

float GComponent::getViewHeight() const
{
    if (_scrollPane != nullptr)
        return _scrollPane->getViewSize().height;
    else
        return _size.height - _margin.top - _margin.bottom;
}

void GComponent::setViewHeight(float value)
{
    if (_scrollPane != nullptr)
        _scrollPane->setViewHeight(value);
    else
        setHeight(value + _margin.top + _margin.bottom);
}

void GComponent::setBoundsChangedFlag()
{
    if (_scrollPane == nullptr && !_trackBounds)
        return;

    _boundsChanged = true;
    CALL_LATER(GComponent, doUpdateBounds);
}

void GComponent::ensureBoundsCorrect()
{
    if (_boundsChanged)
        updateBounds();
}

void GComponent::updateBounds()
{
    float ax, ay, aw, ah;
    if (!_children.empty())
    {
        ax = FLT_MAX;
        ay = FLT_MAX;
        float ar = -FLT_MAX, ab = -FLT_MAX;
        float tmp;

        size_t cnt = _children.size();
        for (size_t i = 0; i < cnt; ++i)
        {
            GObject* child = _children.at(i);
            tmp = child->getX();
            if (tmp < ax)
                ax = tmp;
            tmp = child->getY();
            if (tmp < ay)
                ay = tmp;
            tmp = child->getX() + child->getWidth();
            if (tmp > ar)
                ar = tmp;
            tmp = child->getY() + child->getHeight();
            if (tmp > ab)
                ab = tmp;
        }
        aw = ar - ax;
        ah = ab - ay;
    }
    else
    {
        ax = 0;
        ay = 0;
        aw = 0;
        ah = 0;
    }
    setBounds(ax, ay, aw, ah);
}

void GComponent::setBounds(float ax, float ay, float aw, float ah)
{
    _boundsChanged = false;
    if (_scrollPane != nullptr)
        _scrollPane->setContentSize(ceil(ax + aw), ceil(ay + ah));
}

void GComponent::doUpdateBounds()
{
    if (_boundsChanged)
        updateBounds();
}

void GComponent::childStateChanged(GObject* child)
{
    if (_buildingDisplayList)
        return;

    int cnt = (int)_children.size();

    if (dynamic_cast<GGroup*>(child) != nullptr)
    {
        for (int i = 0; i < cnt; ++i)
        {
            GObject* g = _children.at(i);
            if (g->_group == child)
                childStateChanged(g);
        }
    }

    if (child->_displayObject == nullptr || child == _maskOwner)
        return;

    if (child->internalVisible())
    {
        if (child->_displayObject->getParent() == nullptr)
        {
            if (_childrenRenderOrder == ChildrenRenderOrder::ASCENT)
            {
                int index = (int)_children.getIndex(child);
                _container->addChild(child->_displayObject, index);
                size_t cnt = _children.size();
                for (size_t i = index + 1; i < cnt; i++)
                {
                    child = _children.at(i);
                    if (child->_displayObject->getParent() != nullptr)
                        child->_displayObject->setLocalZOrder((int)i);
                }
            }
            else if (_childrenRenderOrder == ChildrenRenderOrder::DESCENT)
            {
                ssize_t index = _children.getIndex(child);
                _container->addChild(child->_displayObject, (int)(cnt - 1 - index));
                for (ssize_t i = 0; i < index; i++)
                {
                    child = _children.at(i);
                    if (child->_displayObject->getParent() != nullptr)
                        child->_displayObject->setLocalZOrder((int)(cnt - 1 - i));
                }
            }
            else
            {
                CALL_LATER(GComponent, buildNativeDisplayList);
            }
        }
    }
    else
    {
        if (child->_displayObject->getParent() != nullptr)
        {
            _container->removeChild(child->_displayObject, false);
            if (_childrenRenderOrder == ChildrenRenderOrder::ARCH)
            {
                CALL_LATER(GComponent, buildNativeDisplayList);
            }
        }
    }
}

void GComponent::childSortingOrderChanged(GObject* child, int oldValue, int newValue)
{
    if (newValue == 0)
    {
        _sortingChildCount--;
        setChildIndex(child, (int)_children.size());
    }
    else
    {
        if (oldValue == 0)
            _sortingChildCount++;

        int oldIndex = (int)_children.getIndex(child);
        int index = getInsertPosForSortingChild(child);
        if (oldIndex < index)
            moveChild(child, oldIndex, index - 1);
        else
            moveChild(child, oldIndex, index);
    }
}

void GComponent::buildNativeDisplayList()
{
    int cnt = (int)_children.size();
    if (cnt == 0)
        return;

    switch (_childrenRenderOrder)
    {
    case ChildrenRenderOrder::ASCENT:
    {
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _children.at(i);
            if (child->_displayObject != nullptr && child != _maskOwner && child->internalVisible())
                _container->addChild(child->_displayObject, i);
        }
    }
    break;
    case ChildrenRenderOrder::DESCENT:
    {
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _children.at(i);
            if (child->_displayObject != nullptr && child != _maskOwner && child->internalVisible())
                _container->addChild(child->_displayObject, cnt - 1 - i);
        }
    }
    break;

    case ChildrenRenderOrder::ARCH:
    {
        int ai = MIN(_apexIndex, cnt);
        for (int i = 0; i < ai; i++)
        {
            GObject* child = _children.at(i);
            if (child->_displayObject != nullptr && child != _maskOwner && child->internalVisible())
            {
                if (child->_displayObject->getParent() == nullptr)
                    _container->addChild(child->_displayObject, i);
                else
                    child->_displayObject->setLocalZOrder((int)i);
            }
        }
        for (int i = cnt - 1; i >= ai; i--)
        {
            GObject* child = _children.at(i);
            if (child->_displayObject != nullptr && child != _maskOwner && child->internalVisible())
            {
                if (child->_displayObject->getParent() == nullptr)
                    _container->addChild(child->_displayObject, ai + cnt - 1 - i);
                else
                    child->_displayObject->setLocalZOrder(ai + cnt - 1 - i);
            }
        }
    }
    break;
    }
}

ax::Vec2 GComponent::getSnappingPosition(const ax::Vec2& pt)
{
    int cnt = (int)_children.size();
    if (cnt == 0)
        return pt;

    ensureBoundsCorrect();

    GObject* obj = nullptr;

    Vec2 ret = pt;

    int i = 0;
    if (ret.y != 0)
    {
        for (; i < cnt; i++)
        {
            obj = _children.at(i);
            if (ret.y < obj->getY())
            {
                if (i == 0)
                {
                    ret.y = 0;
                    break;
                }
                else
                {
                    GObject* prev = _children.at(i - 1);
                    if (ret.y < prev->getY() + prev->getHeight() / 2) //top half part
                        ret.y = prev->getY();
                    else //bottom half part
                        ret.y = obj->getY();
                    break;
                }
            }
        }

        if (i == cnt)
            ret.y = obj->getY();
    }

    if (ret.x != 0)
    {
        if (i > 0)
            i--;
        for (; i < cnt; i++)
        {
            obj = _children.at(i);
            if (ret.x < obj->getX())
            {
                if (i == 0)
                {
                    ret.x = 0;
                    break;
                }
                else
                {
                    GObject* prev = _children.at(i - 1);
                    if (ret.x < prev->getX() + prev->getWidth() / 2) // top half part
                        ret.x = prev->getX();
                    else //bottom half part
                        ret.x = obj->getX();
                    break;
                }
            }
        }
        if (i == cnt)
            ret.x = obj->getX();
    }

    return ret;
}

GObject* GComponent::hitTest(const Vec2& worldPoint, const Camera* camera)
{
    if (_touchDisabled || !_touchable || !_displayObject->isVisible() || !_displayObject->getParent())
        return nullptr;

    GObject* target = nullptr;
    if (_maskOwner)
    {
        if (_maskOwner->hitTest(worldPoint, camera) != nullptr)
        {
            if (((FUIContainer*)_displayObject)->isInverted())
                return nullptr;
        }
        else
        {
            if (!((FUIContainer*)_displayObject)->isInverted())
                return nullptr;
        }
    }

    Rect rect;
    int flag = 0;

    if (_hitArea)
    {
        Rect rect;
        rect.size = _size;
        Vec2 localPoint = _displayObject->convertToNodeSpace(worldPoint);
        flag = rect.containsPoint(localPoint) ? 1 : 2;

        if (!_hitArea->hitTest(this, localPoint))
            return nullptr;
    }
    else
    {
        if (((FUIContainer*)_displayObject)->isClippingEnabled())
        {
            Rect rect;
            rect.size = _size;
            Vec2 localPoint = _displayObject->convertToNodeSpace(worldPoint);
            flag = rect.containsPoint(localPoint) ? 1 : 2;

            const Rect& clipRect = ((FUIContainer*)_displayObject)->getClippingRegion();
            if (!clipRect.containsPoint(localPoint))
                return nullptr;
        }
    }

    if (_scrollPane)
    {
        target = _scrollPane->hitTest(worldPoint, camera);
        if (!target)
            return nullptr;

        if (target != this)
            return target;
    }

    int cnt = (int)_children.size();

    switch (_childrenRenderOrder)
    {
    case ChildrenRenderOrder::ASCENT:
    {
        for (int i = cnt - 1; i >= 0; i--)
        {
            GObject* child = _children.at(i);
            if (!child->_displayObject || child == _maskOwner)
                continue;

            target = child->hitTest(worldPoint, camera);
            if (target)
                return target;
        }
    }
    break;
    case ChildrenRenderOrder::DESCENT:
    {
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _children.at(i);
            if (!child->_displayObject || child == _maskOwner)
                continue;

            target = child->hitTest(worldPoint, camera);
            if (target)
                return target;
        }
    }
    break;

    case ChildrenRenderOrder::ARCH:
    {
        int ai = MIN(_apexIndex, cnt);
        for (int i = ai; i < cnt; i++)
        {
            GObject* child = _children.at(i);
            if (!child->_displayObject || child == _maskOwner)
                continue;

            target = child->hitTest(worldPoint, camera);
            if (target)
                return target;
        }
        for (int i = ai - 1; i >= 0; i--)
        {
            GObject* child = _children.at(i);
            if (!child->_displayObject || child == _maskOwner)
                continue;

            target = child->hitTest(worldPoint, camera);
            if (target)
                return target;
        }
    }
    }

    if (_opaque)
    {
        if (flag == 0)
        {
            rect.size = _size;
            flag = rect.containsPoint(_displayObject->convertToNodeSpace(worldPoint)) ? 1 : 2;
        }

        if (flag == 1)
            return this;
        else
            return nullptr;
    }
    else
        return nullptr;
}

void GComponent::setupOverflow(OverflowType overflow)
{
    if (overflow == OverflowType::HIDDEN)
    {
        ((FUIContainer*)_displayObject)->setClippingEnabled(true);
        ((FUIContainer*)_displayObject)->setClippingRegion(Rect(_margin.left, _margin.top, _size.width - _margin.left - _margin.right, _size.height - _margin.top - _margin.bottom));
    }

    _container->setPosition2(_margin.left, _margin.top);
}

void GComponent::setupScroll(ByteBuffer* buffer)
{
    _scrollPane = new ScrollPane(this);
    _scrollPane->setup(buffer);
}

void GComponent::handleSizeChanged()
{
    GObject::handleSizeChanged();

    if (_scrollPane != nullptr)
        _scrollPane->onOwnerSizeChanged();
    else
        _container->setPosition2(_margin.left, _margin.top);

    if (_maskOwner)
        _maskOwner->handlePositionChanged();

    if (((FUIContainer*)_displayObject)->isClippingEnabled())
        ((FUIContainer*)_displayObject)->setClippingRegion(Rect(_margin.left, _margin.top, _size.width - _margin.left - _margin.right, _size.height - _margin.top - _margin.bottom));

    if (_hitArea)
    {
        PixelHitTest* test = dynamic_cast<PixelHitTest*>(_hitArea);
        if (sourceSize.width != 0)
            test->scaleX = _size.width / sourceSize.width;
        if (sourceSize.height != 0)
            test->scaleY = _size.height / sourceSize.height;
    }
}

void GComponent::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

    GController* cc = getController("grayed");
    if (cc != nullptr)
        cc->setSelectedIndex(isGrayed() ? 1 : 0);
    else
    {
        for (auto& child : _children)
            child->handleGrayedChanged();
    }
}

void GComponent::handleControllerChanged(GController* c)
{
    GObject::handleControllerChanged(c);

    if (_scrollPane != nullptr)
        _scrollPane->handleControllerChanged(c);
}

void GComponent::onEnter()
{
    GObject::onEnter();

    if (!_transitions.empty())
    {
        for (auto& trans : _transitions)
            trans->onOwnerAddedToStage();
    }
}

void GComponent::onExit()
{
    GObject::onExit();

    if (!_transitions.empty())
    {
        for (auto& trans : _transitions)
            trans->onOwnerRemovedFromStage();
    }
}

void GComponent::constructFromResource()
{
    constructFromResource(nullptr, 0);
}

void GComponent::constructFromResource(std::vector<GObject*>* objectPool, int poolIndex)
{
    PackageItem* contentItem = _packageItem->getBranch();

    if (!contentItem->translated)
    {
        contentItem->translated = true;
        TranslationHelper::translateComponent(contentItem);
    }

    ByteBuffer* buffer = contentItem->rawData;
    buffer->seek(0, 0);

    _underConstruct = true;

    sourceSize.width = buffer->readInt();
    sourceSize.height = buffer->readInt();
    initSize = sourceSize;

    setSize(sourceSize.width, sourceSize.height);

    if (buffer->readBool())
    {
        minSize.width = buffer->readInt();
        maxSize.width = buffer->readInt();
        minSize.height = buffer->readInt();
        maxSize.height = buffer->readInt();
    }

    if (buffer->readBool())
    {
        float f1 = buffer->readFloat();
        float f2 = buffer->readFloat();
        setPivot(f1, f2, buffer->readBool());
    }

    if (buffer->readBool())
    {
        _margin.top = buffer->readInt();
        _margin.bottom = buffer->readInt();
        _margin.left = buffer->readInt();
        _margin.right = buffer->readInt();
    }

    OverflowType overflow = (OverflowType)buffer->readByte();
    if (overflow == OverflowType::SCROLL)
    {
        int savedPos = buffer->getPos();
        buffer->seek(0, 7);
        setupScroll(buffer);
        buffer->setPos(savedPos);
    }
    else
        setupOverflow(overflow);

    if (buffer->readBool()) //clipsoft
        buffer->skip(8);

    _buildingDisplayList = true;

    buffer->seek(0, 1);

    int controllerCount = buffer->readShort();
    for (int i = 0; i < controllerCount; i++)
    {
        int nextPos = buffer->readUshort();
        nextPos += buffer->getPos();

        GController* controller = new GController();
        _controllers.pushBack(controller);
        controller->setParent(this);
        controller->setup(buffer);
        controller->release();

        buffer->setPos(nextPos);
    }

    buffer->seek(0, 2);

    GObject* child;
    int childCount = buffer->readShort();
    for (int i = 0; i < childCount; i++)
    {
        int dataLen = buffer->readUshort();
        int curPos = buffer->getPos();

        if (objectPool != nullptr)
            child = (*objectPool)[poolIndex + i];
        else
        {
            buffer->seek(curPos, 0);

            ObjectType type = (ObjectType)buffer->readByte();
            const string& src = buffer->readS();
            const string& pkgId = buffer->readS();

            PackageItem* pi = nullptr;
            if (!src.empty())
            {
                UIPackage* pkg;
                if (!pkgId.empty())
                    pkg = UIPackage::getById(pkgId);
                else
                    pkg = contentItem->owner;

                pi = pkg != nullptr ? pkg->getItem(src) : nullptr;
            }

            if (pi != nullptr)
            {
                child = UIObjectFactory::newObject(pi);
                child->constructFromResource();
            }
            else
                child = UIObjectFactory::newObject(type);
        }

        child->_underConstruct = true;
        child->setup_beforeAdd(buffer, curPos);
        child->_parent = this;
        _children.pushBack(child);

        buffer->setPos(curPos + dataLen);
    }

    buffer->seek(0, 3);
    _relations->setup(buffer, true);

    buffer->seek(0, 2);
    buffer->skip(2);

    for (int i = 0; i < childCount; i++)
    {
        int nextPos = buffer->readUshort();
        nextPos += buffer->getPos();

        buffer->seek(buffer->getPos(), 3);
        _children.at(i)->relations()->setup(buffer, false);

        buffer->setPos(nextPos);
    }

    buffer->seek(0, 2);
    buffer->skip(2);

    for (int i = 0; i < childCount; i++)
    {
        int nextPos = buffer->readUshort();
        nextPos += buffer->getPos();

        child = _children.at(i);
        child->setup_afterAdd(buffer, buffer->getPos());
        child->_underConstruct = false;

        buffer->setPos(nextPos);
    }

    buffer->seek(0, 4);

    buffer->skip(2); //customData
    _opaque = buffer->readBool();
    int maskId = buffer->readShort();
    if (maskId != -1)
    {
        bool inverted = buffer->readBool();
        setMask(getChildAt(maskId)->displayObject(), inverted);
    }

    const string& hitTestId = buffer->readS();
    int i1 = buffer->readInt();
    int i2 = buffer->readInt();
    if (!hitTestId.empty())
    {
        PackageItem* pi = contentItem->owner->getItem(hitTestId);
        if (pi != nullptr && pi->pixelHitTestData != nullptr)
            setHitArea(new PixelHitTest(pi->pixelHitTestData, i1, i2));
    }
    else if (i1 != 0 && i2 != -1)
    {
        //setHitArea(new ChildHitArea(getChildAt(i2)));
    }

    buffer->seek(0, 5);

    int transitionCount = buffer->readShort();
    for (int i = 0; i < transitionCount; i++)
    {
        int nextPos = buffer->readUshort();
        nextPos += buffer->getPos();

        Transition* trans = new Transition(this);
        trans->setup(buffer);
        _transitions.pushBack(trans);
        trans->release();

        buffer->setPos(nextPos);
    }

    applyAllControllers();

    _buildingDisplayList = false;
    _underConstruct = false;

    buildNativeDisplayList();
    setBoundsChangedFlag();

    if (contentItem->objectType != ObjectType::COMPONENT)
        constructExtension(buffer);

    onConstruct();
}

void GComponent::constructExtension(ByteBuffer* buffer)
{
}

void GComponent::onConstruct()
{
}

void GComponent::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_afterAdd(buffer, beginPos);

    buffer->seek(beginPos, 4);

    int pageController = buffer->readShort();
    if (pageController != -1 && _scrollPane != nullptr && _scrollPane->isPageMode())
        _scrollPane->setPageController(_parent->getControllerAt(pageController));

    int cnt = buffer->readShort();
    for (int i = 0; i < cnt; i++)
    {
        GController* cc = getController(buffer->readS());
        const string& pageId = buffer->readS();
        if (cc)
            cc->setSelectedPageId(pageId);
    }

    if (buffer->version >= 2)
    {
        cnt = buffer->readShort();
        for (int i = 0; i < cnt; i++)
        {
            std::string target = buffer->readS();
            ObjectPropID propId = (ObjectPropID)buffer->readShort();
            std::string value = buffer->readS();
            GObject* obj = getChildByPath(target);
            if (obj != nullptr)
                obj->setProp(propId, Value(value));
        }
    }
}

NS_FGUI_END
