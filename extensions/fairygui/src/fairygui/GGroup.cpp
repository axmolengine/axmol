#include "GGroup.h"
#include "GComponent.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GGroup::GGroup() : _layout(GroupLayoutType::NONE),
                   _lineGap(0),
                   _columnGap(0),
                   _excludeInvisibles(false),
                   _autoSizeDisabled(false),
                   _mainGridIndex(-1),
                   _mainGridMinSize(10),
                   _mainChildIndex(-1),
                   _totalSize(0),
                   _numChildren(0),
                   _percentReady(false),
                   _boundsChanged(false),
                   _updating(false)
{
    _touchDisabled = true;
}

GGroup::~GGroup()
{
    CALL_LATER_CANCEL(GGroup, ensureBoundsCorrect);
}

void GGroup::setLayout(GroupLayoutType value)
{
    if (_layout != value)
    {
        _layout = value;
        setBoundsChangedFlag(true);
    }
}

void GGroup::setColumnGap(int value)
{
    if (_columnGap != value)
    {
        _columnGap = value;
        setBoundsChangedFlag();
    }
}

void GGroup::setLineGap(int value)
{
    if (_lineGap != value)
    {
        _lineGap = value;
        setBoundsChangedFlag();
    }
}

void GGroup::setExcludeInvisibles(bool value)
{
    if (_excludeInvisibles != value)
    {
        _excludeInvisibles = value;
        setBoundsChangedFlag();
    }
}
void GGroup::setAutoSizeDisabled(bool value)
{
    if (_autoSizeDisabled != value)
    {
        _autoSizeDisabled = value;
        setBoundsChangedFlag();
    }
}

void GGroup::setMainGridIndex(int value)
{
    if (_mainGridIndex != value)
    {
        _mainGridIndex = value;
        setBoundsChangedFlag();
    }
}

void GGroup::setMainGridMinSize(int value)
{
    if (_mainGridMinSize != value)
    {
        _mainGridMinSize = value;
        setBoundsChangedFlag();
    }
}

void GGroup::setBoundsChangedFlag(bool positionChangedOnly)
{
    if (_updating == 0 && _parent != nullptr)
    {
        if (!positionChangedOnly)
            _percentReady = false;

        if (!_boundsChanged)
        {
            _boundsChanged = true;

            if (_layout != GroupLayoutType::NONE)
                CALL_LATER(GGroup, ensureBoundsCorrect);
        }
    }
}

void GGroup::ensureBoundsCorrect()
{
    if (_parent == nullptr || !_boundsChanged)
        return;

    CALL_LATER_CANCEL(GGroup, ensureBoundsCorrect);
    _boundsChanged = false;

    if (_autoSizeDisabled)
        resizeChildren(0, 0);
    else
    {
        handleLayout();
        updateBounds();
    }
}

void GGroup::updateBounds()
{
    int cnt = _parent->numChildren();
    int i;
    GObject* child;
    float ax = FLT_MAX, ay = FLT_MAX;
    float ar = FLT_MIN, ab = FLT_MIN;
    float tmp;
    bool empty = true;

    for (i = 0; i < cnt; i++)
    {
        child = _parent->getChildAt(i);
        if (child->_group != this || (_excludeInvisibles && !child->internalVisible3()))
            continue;

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

        empty = false;
    }

    float w;
    float h;
    if (!empty)
    {
        _updating |= 1;
        setPosition(ax, ay);
        _updating &= 2;

        w = ar - ax;
        h = ab - ay;
    }
    else
        w = h = 0;

    if ((_updating & 2) == 0)
    {
        _updating |= 2;
        setSize(w, h);
        _updating &= 1;
    }
    else
    {
        _updating &= 1;
        resizeChildren(getWidth() - w, getHeight() - h);
    }
}

void GGroup::handleLayout()
{
    _updating |= 1;

    if (_layout == GroupLayoutType::HORIZONTAL)
    {
        float curX = getX();
        int cnt = _parent->numChildren();
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;
            if (_excludeInvisibles && !child->internalVisible3())
                continue;

            child->setXMin(curX);
            if (child->getWidth() != 0)
                curX += child->getWidth() + _columnGap;
        }
    }
    else if (_layout == GroupLayoutType::VERTICAL)
    {
        float curY = getY();
        int cnt = _parent->numChildren();
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;
            if (_excludeInvisibles && !child->internalVisible3())
                continue;

            child->setYMin(curY);
            if (child->getHeight() != 0)
                curY += child->getHeight() + _lineGap;
        }
    }

    _updating &= 2;
}

void GGroup::moveChildren(float dx, float dy)
{
    if ((_updating & 1) != 0 || _parent == nullptr)
        return;

    _updating |= 1;

    int cnt = _parent->numChildren();
    int i;
    GObject* child;
    for (i = 0; i < cnt; i++)
    {
        child = _parent->getChildAt(i);
        if (child->_group == this)
        {
            child->setPosition(child->getX() + dx, child->getY() + dy);
        }
    }

    _updating &= 2;
}

void GGroup::resizeChildren(float dw, float dh)
{
    if (_layout == GroupLayoutType::NONE || (_updating & 2) != 0 || _parent == nullptr)
        return;

    _updating |= 2;

    if (_boundsChanged)
    {
        _boundsChanged = false;
        if (!_autoSizeDisabled)
        {
            updateBounds();
            return;
        }
    }

    int cnt = _parent->numChildren();

    if (!_percentReady)
    {
        _percentReady = true;
        _numChildren = 0;
        _totalSize = 0;
        _mainChildIndex = -1;

        int j = 0;
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            if (!_excludeInvisibles || child->internalVisible3())
            {
                if (j == _mainGridIndex)
                    _mainChildIndex = i;

                _numChildren++;

                if (_layout == GroupLayoutType::HORIZONTAL)
                    _totalSize += child->getWidth();
                else
                    _totalSize += child->getHeight();
            }

            j++;
        }

        if (_mainChildIndex != -1)
        {
            if (_layout == GroupLayoutType::HORIZONTAL)
            {
                GObject* child = _parent->getChildAt(_mainChildIndex);
                _totalSize += _mainGridMinSize - child->getWidth();
                child->_sizePercentInGroup = _mainGridMinSize / _totalSize;
            }
            else
            {
                GObject* child = _parent->getChildAt(_mainChildIndex);
                _totalSize += _mainGridMinSize - child->getHeight();
                child->_sizePercentInGroup = _mainGridMinSize / _totalSize;
            }
        }

        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            if (i == _mainChildIndex)
                continue;

            if (_totalSize > 0)
                child->_sizePercentInGroup = (_layout == GroupLayoutType::HORIZONTAL ? child->getWidth() : child->getHeight()) / _totalSize;
            else
                child->_sizePercentInGroup = 0;
        }
    }

    float remainSize = 0;
    float remainPercent = 1;
    bool priorHandled = false;

    if (_layout == GroupLayoutType::HORIZONTAL)
    {
        remainSize = getWidth() - (_numChildren - 1) * _columnGap;
        if (_mainChildIndex != -1 && remainSize >= _totalSize)
        {
            GObject* child = _parent->getChildAt(_mainChildIndex);
            child->setSize(remainSize - (_totalSize - _mainGridMinSize), child->_rawSize.height + dh, true);
            remainSize -= child->getWidth();
            remainPercent -= child->_sizePercentInGroup;
            priorHandled = true;
        }

        float curX = getX();
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            if (_excludeInvisibles && !child->internalVisible3())
            {
                child->setSize(child->_rawSize.width, child->_rawSize.height + dh, true);
                continue;
            }

            if (!priorHandled || i != _mainChildIndex)
            {
                child->setSize(round(child->_sizePercentInGroup / remainPercent * remainSize), child->_rawSize.height + dh, true);
                remainPercent -= child->_sizePercentInGroup;
                remainSize -= child->getWidth();
            }

            child->setXMin(curX);
            if (child->getWidth() != 0)
                curX += child->getWidth() + _columnGap;
        }
    }
    else
    {
        remainSize = getHeight() - (_numChildren - 1) * _lineGap;
        if (_mainChildIndex != -1 && remainSize >= _totalSize)
        {
            GObject* child = _parent->getChildAt(_mainChildIndex);
            child->setSize(child->_rawSize.width + dw, remainSize - (_totalSize - _mainGridMinSize), true);
            remainSize -= child->getHeight();
            remainPercent -= child->_sizePercentInGroup;
            priorHandled = true;
        }

        float curY = getY();
        for (int i = 0; i < cnt; i++)
        {
            GObject* child = _parent->getChildAt(i);
            if (child->_group != this)
                continue;

            if (_excludeInvisibles && !child->internalVisible3())
            {
                child->setSize(child->_rawSize.width + dw, child->_rawSize.height, true);
                continue;
            }

            if (!priorHandled || i != _mainChildIndex)
            {
                child->setSize(child->_rawSize.width + dw, round(child->_sizePercentInGroup / remainPercent * remainSize), true);
                remainPercent -= child->_sizePercentInGroup;
                remainSize -= child->getHeight();
            }

            child->setYMin(curY);
            if (child->getHeight() != 0)
                curY += child->getHeight() + _lineGap;
        }
    }

    _updating &= 1;
}

void GGroup::handleAlphaChanged()
{
    GObject::handleAlphaChanged();

    if (_underConstruct)
        return;

    int cnt = _parent->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        GObject* child = _parent->getChildAt(i);
        if (child->_group == this)
            child->setAlpha(_alpha);
    }
}

void GGroup::handleVisibleChanged()
{
    if (!_parent)
        return;

    int cnt = _parent->numChildren();
    for (int i = 0; i < cnt; i++)
    {
        GObject* child = _parent->getChildAt(i);
        if (child->_group == this)
            child->handleVisibleChanged();
    }
}

void GGroup::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 5);

    _layout = (GroupLayoutType)buffer->readByte();
    _lineGap = buffer->readInt();
    _columnGap = buffer->readInt();
    if (buffer->version >= 2)
    {
        _excludeInvisibles = buffer->readBool();
        _autoSizeDisabled = buffer->readBool();
        _mainGridIndex = buffer->readShort();
    }
}

void GGroup::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_afterAdd(buffer, beginPos);

    if (!_visible)
        handleVisibleChanged();
}

NS_FGUI_END
