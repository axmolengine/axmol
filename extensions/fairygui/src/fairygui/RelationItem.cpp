#include "RelationItem.h"
#include "GComponent.h"
#include "GGroup.h"
#include "event/UIEventType.h"
#include "utils/WeakPtr.h"

NS_FGUI_BEGIN
using namespace ax;

RelationItem::RelationItem(GObject* owner) : _target(nullptr)
{
    _owner = owner;
}

RelationItem::~RelationItem()
{
    releaseRefTarget(_target.ptr());
}

void RelationItem::setTarget(GObject* value)
{
    GObject* old = _target.ptr();
    if (old != value)
    {
        if (old)
            releaseRefTarget(old);
        _target = value;
        if (value)
            addRefTarget(value);
    }
}

void RelationItem::add(RelationType relationType, bool usePercent)
{
    if (relationType == RelationType::Size)
    {
        add(RelationType::Width, usePercent);
        add(RelationType::Height, usePercent);
        return;
    }

    for (auto& it : _defs)
    {
        if (it.type == relationType)
            return;
    }

    internalAdd(relationType, usePercent);
}

void RelationItem::internalAdd(RelationType relationType, bool usePercent)
{
    if (relationType == RelationType::Size)
    {
        internalAdd(RelationType::Width, usePercent);
        internalAdd(RelationType::Height, usePercent);
        return;
    }

    RelationDef info;
    info.percent = usePercent;
    info.type = relationType;
    info.axis = (relationType <= RelationType::Right_Right || relationType == RelationType::Width || (relationType >= RelationType::LeftExt_Left && relationType <= RelationType::RightExt_Right)) ? 0 : 1;
    _defs.push_back(info);

    if (usePercent || relationType == RelationType::Left_Center || relationType == RelationType::Center_Center || relationType == RelationType::Right_Center || relationType == RelationType::Top_Middle || relationType == RelationType::Middle_Middle || relationType == RelationType::Bottom_Middle)
        _owner->setPixelSnapping(true);
}

void RelationItem::remove(RelationType relationType)
{
    if (relationType == RelationType::Size)
    {
        remove(RelationType::Width);
        remove(RelationType::Height);
        return;
    }

    for (auto it = _defs.begin(); it != _defs.end(); ++it)
    {
        if (it->type == relationType)
        {
            _defs.erase(it);
            break;
        }
    }
}

void RelationItem::copyFrom(const RelationItem& source)
{
    setTarget(source._target.ptr());

    _defs.clear();
    for (auto& it : source._defs)
        _defs.push_back(it);
}

bool RelationItem::isEmpty() const
{
    return _defs.size() == 0;
}

void RelationItem::applyOnSelfSizeChanged(float dWidth, float dHeight, bool applyPivot)
{
    if (_target == nullptr || _defs.size() == 0)
        return;

    float ox = _owner->_position.x;
    float oy = _owner->_position.y;

    for (auto& it : _defs)
    {
        switch (it.type)
        {
        case RelationType::Center_Center:
            _owner->setX(_owner->_position.x - (0.5 - (applyPivot ? _owner->_pivot.x : 0)) * dWidth);
            break;

        case RelationType::Right_Center:
        case RelationType::Right_Left:
        case RelationType::Right_Right:
            _owner->setX(_owner->_position.x - (1 - (applyPivot ? _owner->_pivot.x : 0)) * dWidth);
            break;

        case RelationType::Middle_Middle:
            _owner->setY(_owner->_position.y - (0.5 - (applyPivot ? _owner->_pivot.y : 0)) * dHeight);
            break;

        case RelationType::Bottom_Middle:
        case RelationType::Bottom_Top:
        case RelationType::Bottom_Bottom:
            _owner->setY(_owner->_position.y - (1 - (applyPivot ? _owner->_pivot.y : 0)) * dHeight);
            break;

        default:
            break;
        }
    }

    if (ox != _owner->_position.x || oy != _owner->_position.y)
    {
        ox = _owner->_position.x - ox;
        oy = _owner->_position.y - oy;

        _owner->updateGearFromRelations(1, ox, oy);

        if (_owner->_parent != nullptr)
        {
            const Vector<Transition*>& arr = _owner->_parent->getTransitions();
            for (auto& it : arr)
                it->updateFromRelations(_owner->id, ox, oy);
        }
    }
}

void RelationItem::applyOnXYChanged(GObject* target, const RelationDef& info, float dx, float dy)
{
    float tmp;

    switch (info.type)
    {
    case RelationType::Left_Left:
    case RelationType::Left_Center:
    case RelationType::Left_Right:
    case RelationType::Center_Center:
    case RelationType::Right_Left:
    case RelationType::Right_Center:
    case RelationType::Right_Right:
        _owner->setX(_owner->_position.x + dx);
        break;

    case RelationType::Top_Top:
    case RelationType::Top_Middle:
    case RelationType::Top_Bottom:
    case RelationType::Middle_Middle:
    case RelationType::Bottom_Top:
    case RelationType::Bottom_Middle:
    case RelationType::Bottom_Bottom:
        _owner->setY(_owner->_position.y + dy);
        break;

    case RelationType::Width:
    case RelationType::Height:
        break;

    case RelationType::LeftExt_Left:
    case RelationType::LeftExt_Right:
        if (_owner != target->getParent())
        {
            tmp = _owner->getXMin();
            _owner->setWidth(_owner->_rawSize.width - dx);
            _owner->setXMin(tmp + dx);
        }
        else
            _owner->setWidth(_owner->_rawSize.width - dx);
        break;

    case RelationType::RightExt_Left:
    case RelationType::RightExt_Right:
        if (_owner != target->getParent())
        {
            tmp = _owner->getXMin();
            _owner->setWidth(_owner->_rawSize.width + dx);
            _owner->setXMin(tmp);
        }
        else
            _owner->setWidth(_owner->_rawSize.width + dx);
        break;

    case RelationType::TopExt_Top:
    case RelationType::TopExt_Bottom:
        if (_owner != target->getParent())
        {
            tmp = _owner->getYMin();
            _owner->setHeight(_owner->_rawSize.height - dy);
            _owner->setYMin(tmp + dy);
        }
        else
            _owner->setHeight(_owner->_rawSize.height - dy);
        break;

    case RelationType::BottomExt_Top:
    case RelationType::BottomExt_Bottom:
        if (_owner != target->getParent())
        {
            tmp = _owner->getYMin();
            _owner->setHeight(_owner->_rawSize.height + dy);
            _owner->setYMin(tmp);
        }
        else
            _owner->setHeight(_owner->_rawSize.height + dy);
        break;

    default:
        break;
    }
}

void RelationItem::applyOnSizeChanged(GObject* target, const RelationDef& info)
{
    float pos = 0, pivot = 0, delta = 0;
    if (info.axis == 0)
    {
        if (target != _owner->_parent)
        {
            pos = target->_position.x;
            if (target->_pivotAsAnchor)
                pivot = target->_pivot.x;
        }

        if (info.percent)
        {
            if (_targetData.z != 0)
                delta = target->_size.width / _targetData.z;
        }
        else
            delta = target->_size.width - _targetData.z;
    }
    else
    {
        if (target != _owner->_parent)
        {
            pos = target->_position.y;
            if (target->_pivotAsAnchor)
                pivot = target->_pivot.y;
        }

        if (info.percent)
        {
            if (_targetData.w != 0)
                delta = target->_size.height / _targetData.w;
        }
        else
            delta = target->_size.height - _targetData.w;
    }

    float v, tmp;

    switch (info.type)
    {
    case RelationType::Left_Left:
        if (info.percent)
            _owner->setXMin(pos + (_owner->getXMin() - pos) * delta);
        else if (pivot != 0)
            _owner->setX(_owner->_position.x + delta * (-pivot));
        break;
    case RelationType::Left_Center:
        if (info.percent)
            _owner->setXMin(pos + (_owner->getXMin() - pos) * delta);
        else
            _owner->setX(_owner->_position.x + delta * (0.5f - pivot));
        break;
    case RelationType::Left_Right:
        if (info.percent)
            _owner->setXMin(pos + (_owner->getXMin() - pos) * delta);
        else
            _owner->setX(_owner->_position.x + delta * (1 - pivot));
        break;
    case RelationType::Center_Center:
        if (info.percent)
            _owner->setXMin(pos + (_owner->getXMin() + _owner->_rawSize.width * 0.5f - pos) * delta - _owner->_rawSize.width * 0.5f);
        else
            _owner->setX(_owner->_position.x + delta * (0.5f - pivot));
        break;
    case RelationType::Right_Left:
        if (info.percent)
            _owner->setXMin(pos + (_owner->getXMin() + _owner->_rawSize.width - pos) * delta - _owner->_rawSize.width);
        else if (pivot != 0)
            _owner->setX(_owner->_position.x + delta * (-pivot));
        break;
    case RelationType::Right_Center:
        if (info.percent)
            _owner->setXMin(pos + (_owner->getXMin() + _owner->_rawSize.width - pos) * delta - _owner->_rawSize.width);
        else
            _owner->setX(_owner->_position.x + delta * (0.5f - pivot));
        break;
    case RelationType::Right_Right:
        if (info.percent)
            _owner->setXMin(pos + (_owner->getXMin() + _owner->_rawSize.width - pos) * delta - _owner->_rawSize.width);
        else
            _owner->setX(_owner->_position.x + delta * (1 - pivot));
        break;

    case RelationType::Top_Top:
        if (info.percent)
            _owner->setYMin(pos + (_owner->getYMin() - pos) * delta);
        else if (pivot != 0)
            _owner->setY(_owner->_position.y + delta * (-pivot));
        break;
    case RelationType::Top_Middle:
        if (info.percent)
            _owner->setYMin(pos + (_owner->getYMin() - pos) * delta);
        else
            _owner->setY(_owner->_position.y + delta * (0.5f - pivot));
        break;
    case RelationType::Top_Bottom:
        if (info.percent)
            _owner->setYMin(pos + (_owner->getYMin() - pos) * delta);
        else
            _owner->setY(_owner->_position.y + delta * (1 - pivot));
        break;
    case RelationType::Middle_Middle:
        if (info.percent)
            _owner->setYMin(pos + (_owner->getYMin() + _owner->_rawSize.height * 0.5f - pos) * delta - _owner->_rawSize.height * 0.5f);
        else
            _owner->setY(_owner->_position.y + delta * (0.5f - pivot));
        break;
    case RelationType::Bottom_Top:
        if (info.percent)
            _owner->setYMin(pos + (_owner->getYMin() + _owner->_rawSize.height - pos) * delta - _owner->_rawSize.height);
        else if (pivot != 0)
            _owner->setY(_owner->_position.y + delta * (-pivot));
        break;
    case RelationType::Bottom_Middle:
        if (info.percent)
            _owner->setYMin(pos + (_owner->getYMin() + _owner->_rawSize.height - pos) * delta - _owner->_rawSize.height);
        else
            _owner->setY(_owner->_position.y + delta * (0.5f - pivot));
        break;
    case RelationType::Bottom_Bottom:
        if (info.percent)
            _owner->setYMin(pos + (_owner->getYMin() + _owner->_rawSize.height - pos) * delta - _owner->_rawSize.height);
        else
            _owner->setY(_owner->_position.y + delta * (1 - pivot));
        break;

    case RelationType::Width:
        if (_owner->_underConstruct && _owner == target->_parent)
            v = _owner->sourceSize.width - target->initSize.width;
        else
            v = _owner->_rawSize.width - _targetData.z;
        if (info.percent)
            v = v * delta;
        if (_target == _owner->_parent)
        {
            if (_owner->_pivotAsAnchor)
            {
                tmp = _owner->getXMin();
                _owner->setSize(target->_size.width + v, _owner->_rawSize.height, true);
                _owner->setXMin(tmp);
            }
            else
                _owner->setSize(target->_size.width + v, _owner->_rawSize.height, true);
        }
        else
            _owner->setWidth(target->_size.width + v);
        break;
    case RelationType::Height:
        if (_owner->_underConstruct && _owner == target->_parent)
            v = _owner->sourceSize.height - target->initSize.height;
        else
            v = _owner->_rawSize.height - _targetData.w;
        if (info.percent)
            v = v * delta;
        if (_target == _owner->_parent)
        {
            if (_owner->_pivotAsAnchor)
            {
                tmp = _owner->getYMin();
                _owner->setSize(_owner->_rawSize.width, target->_size.height + v, true);
                _owner->setYMin(tmp);
            }
            else
                _owner->setSize(_owner->_rawSize.width, target->_size.height + v, true);
        }
        else
            _owner->setHeight(target->_size.height + v);
        break;

    case RelationType::LeftExt_Left:
        tmp = _owner->getXMin();
        if (info.percent)
            v = pos + (tmp - pos) * delta - tmp;
        else
            v = delta * (-pivot);
        _owner->setWidth(_owner->_rawSize.width - v);
        _owner->setXMin(tmp + v);
        break;
    case RelationType::LeftExt_Right:
        tmp = _owner->getXMin();
        if (info.percent)
            v = pos + (tmp - pos) * delta - tmp;
        else
            v = delta * (1 - pivot);
        _owner->setWidth(_owner->_rawSize.width - v);
        _owner->setXMin(tmp + v);
        break;
    case RelationType::RightExt_Left:
        tmp = _owner->getXMin();
        if (info.percent)
            v = pos + (tmp + _owner->_rawSize.width - pos) * delta - (tmp + _owner->_rawSize.width);
        else
            v = delta * (-pivot);
        _owner->setWidth(_owner->_rawSize.width + v);
        _owner->setXMin(tmp);
        break;
    case RelationType::RightExt_Right:
        tmp = _owner->getXMin();
        if (info.percent)
        {
            if (_owner == target->_parent)
            {
                if (_owner->_underConstruct)
                    _owner->setWidth(pos + target->_size.width - target->_size.width * pivot +
                                     (_owner->sourceSize.width - pos - target->initSize.width + target->initSize.width * pivot) * delta);
                else
                    _owner->setWidth(pos + (_owner->_rawSize.width - pos) * delta);
            }
            else
            {
                v = pos + (tmp + _owner->_rawSize.width - pos) * delta - (tmp + _owner->_rawSize.width);
                _owner->setWidth(_owner->_rawSize.width + v);
                _owner->setXMin(tmp);
            }
        }
        else
        {
            if (_owner == target->_parent)
            {
                if (_owner->_underConstruct)
                    _owner->setWidth(_owner->sourceSize.width + (target->_size.width - target->initSize.width) * (1 - pivot));
                else
                    _owner->setWidth(_owner->_rawSize.width + delta * (1 - pivot));
            }
            else
            {
                v = delta * (1 - pivot);
                _owner->setWidth(_owner->_rawSize.width + v);
                _owner->setXMin(tmp);
            }
        }
        break;
    case RelationType::TopExt_Top:
        tmp = _owner->getYMin();
        if (info.percent)
            v = pos + (tmp - pos) * delta - tmp;
        else
            v = delta * (-pivot);
        _owner->setHeight(_owner->_rawSize.height - v);
        _owner->setYMin(tmp + v);
        break;
    case RelationType::TopExt_Bottom:
        tmp = _owner->getYMin();
        if (info.percent)
            v = pos + (tmp - pos) * delta - tmp;
        else
            v = delta * (1 - pivot);
        _owner->setHeight(_owner->_rawSize.height - v);
        _owner->setYMin(tmp + v);
        break;
    case RelationType::BottomExt_Top:
        tmp = _owner->getYMin();
        if (info.percent)
            v = pos + (tmp + _owner->_rawSize.height - pos) * delta - (tmp + _owner->_rawSize.height);
        else
            v = delta * (-pivot);
        _owner->setHeight(_owner->_rawSize.height + v);
        _owner->setYMin(tmp);
        break;
    case RelationType::BottomExt_Bottom:
        tmp = _owner->getYMin();
        if (info.percent)
        {
            if (_owner == target->_parent)
            {
                if (_owner->_underConstruct)
                    _owner->setHeight(pos + target->_size.height - target->_size.height * pivot +
                                      (_owner->sourceSize.height - pos - target->initSize.height + target->initSize.height * pivot) * delta);
                else
                    _owner->setHeight(pos + (_owner->_rawSize.height - pos) * delta);
            }
            else
            {
                v = pos + (tmp + _owner->_rawSize.height - pos) * delta - (tmp + _owner->_rawSize.height);
                _owner->setHeight(_owner->_rawSize.height + v);
                _owner->setYMin(tmp);
            }
        }
        else
        {
            if (_owner == target->_parent)
            {
                if (_owner->_underConstruct)
                    _owner->setHeight(_owner->sourceSize.height + (target->_size.height - target->initSize.height) * (1 - pivot));
                else
                    _owner->setHeight(_owner->_rawSize.height + delta * (1 - pivot));
            }
            else
            {
                v = delta * (1 - pivot);
                _owner->setHeight(_owner->_rawSize.height + v);
                _owner->setYMin(tmp);
            }
        }
        break;
    default:
        break;
    }
}

void RelationItem::addRefTarget(GObject* target)
{
    if (!target)
        return;

    if (target != _owner->_parent)
        target->addEventListener(UIEventType::PositionChange, AX_CALLBACK_1(RelationItem::onTargetXYChanged, this), EventTag(this));
    target->addEventListener(UIEventType::SizeChange, AX_CALLBACK_1(RelationItem::onTargetSizeChanged, this), EventTag(this));

    _targetData.x = target->_position.x;
    _targetData.y = target->_position.y;
    _targetData.z = target->_size.width;
    _targetData.w = target->_size.height;
}

void RelationItem::releaseRefTarget(GObject* target)
{
    if (!target)
        return;

    target->removeEventListener(UIEventType::PositionChange, EventTag(this));
    target->removeEventListener(UIEventType::SizeChange, EventTag(this));
}

void RelationItem::onTargetXYChanged(EventContext* context)
{
    GObject* target = (GObject*)context->getSender();
    if (_owner->relations()->handling != nullptr || (_owner->_group != nullptr && _owner->_group->_updating != 0))
    {
        _targetData.x = target->_position.x;
        _targetData.y = target->_position.y;
        return;
    }

    _owner->relations()->handling = target;

    float ox = _owner->_position.x;
    float oy = _owner->_position.y;
    float dx = target->_position.x - _targetData.x;
    float dy = target->_position.y - _targetData.y;

    for (auto& it : _defs)
        applyOnXYChanged(target, it, dx, dy);

    _targetData.x = target->_position.x;
    _targetData.y = target->_position.y;

    if (ox != _owner->_position.x || oy != _owner->_position.y)
    {
        ox = _owner->_position.x - ox;
        oy = _owner->_position.y - oy;

        _owner->updateGearFromRelations(1, ox, oy);

        if (_owner->_parent != nullptr)
        {
            const Vector<Transition*>& arr = _owner->_parent->getTransitions();
            for (auto& it : arr)
                it->updateFromRelations(_owner->id, ox, oy);
        }
    }

    _owner->relations()->handling = nullptr;
}

void RelationItem::onTargetSizeChanged(EventContext* context)
{
    GObject* target = (GObject*)context->getSender();
    if (_owner->relations()->handling != nullptr || (_owner->_group != nullptr && _owner->_group->_updating != 0))
    {
        _targetData.z = target->_size.width;
        _targetData.w = target->_size.height;
        return;
    }

    _owner->relations()->handling = target;

    float ox = _owner->_position.x;
    float oy = _owner->_position.y;
    float ow = _owner->_rawSize.width;
    float oh = _owner->_rawSize.height;

    for (auto& it : _defs)
        applyOnSizeChanged(target, it);

    _targetData.z = target->_size.width;
    _targetData.w = target->_size.height;

    if (ox != _owner->_position.x || oy != _owner->_position.y)
    {
        ox = _owner->_position.x - ox;
        oy = _owner->_position.y - oy;

        _owner->updateGearFromRelations(1, ox, oy);

        if (_owner->_parent != nullptr)
        {
            const Vector<Transition*>& arr = _owner->_parent->getTransitions();
            for (auto& it : arr)
                it->updateFromRelations(_owner->id, ox, oy);
        }
    }

    if (ow != _owner->_rawSize.width || oh != _owner->_rawSize.height)
    {
        ow = _owner->_rawSize.width - ow;
        oh = _owner->_rawSize.height - oh;

        _owner->updateGearFromRelations(2, ow, oh);
    }

    _owner->relations()->handling = nullptr;
}

NS_FGUI_END