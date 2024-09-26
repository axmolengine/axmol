#include "GearColor.h"
#include "GObject.h"
#include "GTextField.h"
#include "UIPackage.h"
#include "tween/GTween.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

GearColor::GearColorValue::GearColorValue()
{
}

GearColor::GearColorValue::GearColorValue(const Color3B& color, const Color3B& strokeColor)
{
    this->color = color;
    this->outlineColor = strokeColor;
}

GearColor::GearColor(GObject* owner) : GearBase(owner)
{
}

GearColor::~GearColor()
{
}

void GearColor::init()
{
    _default = GearColorValue(ToolSet::intToColor(_owner->getProp(ObjectPropID::Color).asUnsignedInt()),
                              ToolSet::intToColor(_owner->getProp(ObjectPropID::OutlineColor).asUnsignedInt()));
    _storage.clear();
}

void GearColor::addStatus(const std::string& pageId, ByteBuffer* buffer)
{
    GearColorValue gv;
    gv.color = (Color3B)buffer->readColor();
    gv.outlineColor = (Color3B)buffer->readColor();
    if (pageId.size() == 0)
        _default = gv;
    else
        _storage[pageId] = gv;
}

void GearColor::apply()
{
    GearColorValue gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    if (_tweenConfig && _tweenConfig->tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        Color3B curColor = ToolSet::intToColor(_owner->getProp(ObjectPropID::Color).asUnsignedInt());
        Color3B curStrokeColor = ToolSet::intToColor(_owner->getProp(ObjectPropID::OutlineColor).asUnsignedInt());

        if (gv.outlineColor != curStrokeColor)
        {
            _owner->_gearLocked = true;
            _owner->setProp(ObjectPropID::OutlineColor, Value(ToolSet::colorToInt(gv.outlineColor)));
            _owner->_gearLocked = false;
        }

        if (_tweenConfig->_tweener != nullptr)
        {
            if (_tweenConfig->_tweener->endValue.getColor() != gv.color)
            {
                _tweenConfig->_tweener->kill(true);
                _tweenConfig->_tweener = nullptr;
            }
            else
                return;
        }

        if (gv.color != curColor)
        {
            if (_owner->checkGearController(0, _controller))
                _tweenConfig->_displayLockToken = _owner->addDisplayLock();

            _tweenConfig->_tweener = GTween::to((Color4B)curColor, (Color4B)gv.color, _tweenConfig->duration)
                                         ->setDelay(_tweenConfig->delay)
                                         ->setEase(_tweenConfig->easeType)
                                         ->setTargetAny(this)
                                         ->onUpdate(AX_CALLBACK_1(GearColor::onTweenUpdate, this))
                                         ->onComplete(AX_CALLBACK_0(GearColor::onTweenComplete, this));
        }
    }
    else
    {
        _owner->_gearLocked = true;
        _owner->setProp(ObjectPropID::Color, Value(ToolSet::colorToInt(gv.color)));
        _owner->setProp(ObjectPropID::OutlineColor, Value(ToolSet::colorToInt(gv.outlineColor)));
        _owner->_gearLocked = false;
    }
}

void GearColor::onTweenUpdate(GTweener* tweener)
{
    _owner->_gearLocked = true;
    _owner->setProp(ObjectPropID::Color, Value(ToolSet::colorToInt((Color3B)_tweenConfig->_tweener->value.getColor())));
    _owner->_gearLocked = false;
}

void GearColor::onTweenComplete()
{
    if (_tweenConfig->_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_tweenConfig->_displayLockToken);
        _tweenConfig->_displayLockToken = 0;
    }
    _tweenConfig->_tweener = nullptr;
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearColor::updateState()
{
    _storage[_controller->getSelectedPageId()] = GearColorValue(
        ToolSet::intToColor(_owner->getProp(ObjectPropID::Color).asUnsignedInt()),
        ToolSet::intToColor(_owner->getProp(ObjectPropID::OutlineColor).asUnsignedInt()));
}

NS_FGUI_END
