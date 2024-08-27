#include "GearLook.h"
#include "GObject.h"
#include "UIPackage.h"
#include "tween/GTween.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GearLook::GearLookValue::GearLookValue() : alpha(0), rotation(0), grayed(false), touchable(false)
{
}

GearLook::GearLookValue::GearLookValue(float alpha, float rotation, bool grayed, bool touchable)
{
    this->alpha = alpha;
    this->rotation = rotation;
    this->grayed = grayed;
    this->touchable = touchable;
}

GearLook::GearLook(GObject* owner) : GearBase(owner)
{
}

GearLook::~GearLook()
{
}

void GearLook::init()
{
    _default = GearLookValue(_owner->getAlpha(), _owner->getRotation(),
                             _owner->isGrayed(), _owner->isTouchable());
    _storage.clear();
}

void GearLook::addStatus(const std::string& pageId, ByteBuffer* buffer)
{
    GearLookValue gv;
    gv.alpha = buffer->readFloat();
    gv.rotation = buffer->readFloat();
    gv.grayed = buffer->readBool();
    gv.touchable = buffer->readBool();

    if (pageId.size() == 0)
        _default = gv;
    else
        _storage[pageId] = gv;
}

void GearLook::apply()
{
    GearLookValue gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    if (_tweenConfig && _tweenConfig->tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        _owner->_gearLocked = true;
        _owner->setGrayed(gv.grayed);
        _owner->setTouchable(gv.touchable);
        _owner->_gearLocked = false;

        if (_tweenConfig->_tweener != nullptr)
        {
            if (_tweenConfig->_tweener->endValue.x != gv.alpha || _tweenConfig->_tweener->endValue.y != gv.rotation)
            {
                _tweenConfig->_tweener->kill(true);
                _tweenConfig->_tweener = nullptr;
            }
            else
                return;
        }

        bool a = gv.alpha != _owner->getAlpha();
        bool b = gv.rotation != _owner->getRotation();
        if (a || b)
        {
            if (_owner->checkGearController(0, _controller))
                _tweenConfig->_displayLockToken = _owner->addDisplayLock();

            _tweenConfig->_tweener = GTween::to(Vec2(_owner->getAlpha(), _owner->getRotation()), Vec2(gv.alpha, gv.rotation), _tweenConfig->duration)
                                         ->setDelay(_tweenConfig->delay)
                                         ->setEase(_tweenConfig->easeType)
                                         ->setTargetAny(this)
                                         ->setUserData(Value((a ? 1 : 0) + (b ? 2 : 0)))
                                         ->onUpdate(AX_CALLBACK_1(GearLook::onTweenUpdate, this))
                                         ->onComplete(AX_CALLBACK_0(GearLook::onTweenComplete, this));
        }
    }
    else
    {
        _owner->_gearLocked = true;
        _owner->setAlpha(gv.alpha);
        _owner->setRotation(gv.rotation);
        _owner->setGrayed(gv.grayed);
        _owner->setTouchable(gv.touchable);
        _owner->_gearLocked = false;
    }
}

void GearLook::onTweenUpdate(GTweener* tweener)
{
    int flag = _tweenConfig->_tweener->getUserData().asInt();
    _owner->_gearLocked = true;
    if ((flag & 1) != 0)
        _owner->setAlpha(_tweenConfig->_tweener->value.x);
    if ((flag & 2) != 0)
        _owner->setRotation(_tweenConfig->_tweener->value.y);
    _owner->_gearLocked = false;
}

void GearLook::onTweenComplete()
{
    if (_tweenConfig->_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_tweenConfig->_displayLockToken);
        _tweenConfig->_displayLockToken = 0;
    }
    _tweenConfig->_tweener = nullptr;
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearLook::updateState()
{
    _storage[_controller->getSelectedPageId()] = GearLookValue(_owner->getAlpha(), _owner->getRotation(),
                                                               _owner->isGrayed(), _owner->isTouchable());
}

NS_FGUI_END
