#include "GearXY.h"
#include "GComponent.h"
#include "UIPackage.h"
#include "tween/GTween.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GearXY::GearXY(GObject* owner) 
    : GearBase(owner),
    positionsInPercent(false)
{
}

GearXY::~GearXY()
{
}

void GearXY::init()
{
    _default = Vec4(_owner->getX(),
                    _owner->getY(),
                    _owner->getX() / _owner->getParent()->getWidth(),
                    _owner->getY() / _owner->getParent()->getHeight());
    _storage.clear();
}

void GearXY::addStatus(const std::string& pageId, ByteBuffer* buffer)
{
    Vec4 gv;
    gv.x = buffer->readInt();
    gv.y = buffer->readInt();

    if (pageId.size() == 0)
        _default = gv;
    else
        _storage[pageId] = gv;
}

void GearXY::addExtStatus(const std::string& pageId, ByteBuffer* buffer)
{
    Vec4* gv;
    if (pageId.empty())
        gv = &_default;
    else
        gv = &_storage[pageId];
    gv->z = buffer->readFloat();
    gv->w = buffer->readFloat();
}

void GearXY::apply()
{
    Vec4 gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    Vec2 endPt;

    if (positionsInPercent && _owner->getParent())
    {
        endPt.x = gv.z * _owner->getParent()->getWidth();
        endPt.y = gv.w * _owner->getParent()->getHeight();
    }
    else
    {
        endPt.x = gv.x;
        endPt.y = gv.y;
    }

    if (_tweenConfig && _tweenConfig->tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        if (_tweenConfig->_tweener != nullptr)
        {
            if (_tweenConfig->_tweener->endValue.x != endPt.x || _tweenConfig->_tweener->endValue.y != endPt.y)
            {
                _tweenConfig->_tweener->kill(true);
                _tweenConfig->_tweener = nullptr;
            }
            else
                return;
        }
        Vec2 originPt(_owner->getX(), _owner->getY());

        if (originPt != endPt)
        {
            if (_owner->checkGearController(0, _controller))
                _tweenConfig->_displayLockToken = _owner->addDisplayLock();

            _tweenConfig->_tweener = GTween::to(originPt, endPt, _tweenConfig->duration)
                                         ->setDelay(_tweenConfig->delay)
                                         ->setEase(_tweenConfig->easeType)
                                         ->setTargetAny(this)
                                         ->onUpdate(AX_CALLBACK_1(GearXY::onTweenUpdate, this))
                                         ->onComplete(AX_CALLBACK_0(GearXY::onTweenComplete, this));
        }
    }
    else
    {
        _owner->_gearLocked = true;
        _owner->setPosition(endPt.x, endPt.y);
        _owner->_gearLocked = false;
    }
}

void GearXY::onTweenUpdate(GTweener* tweener)
{
    _owner->_gearLocked = true;
    _owner->setPosition(_tweenConfig->_tweener->value.x, _tweenConfig->_tweener->value.y);
    _owner->_gearLocked = false;
}

void GearXY::onTweenComplete()
{
    if (_tweenConfig->_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_tweenConfig->_displayLockToken);
        _tweenConfig->_displayLockToken = 0;
    }
    _tweenConfig->_tweener = nullptr;
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearXY::updateState()
{
    _storage[_controller->getSelectedPageId()] = Vec4(
        _owner->getX(),
        _owner->getY(),
        _owner->getX() / _owner->getParent()->getWidth(),
        _owner->getY() / _owner->getParent()->getHeight());
}

void GearXY::updateFromRelations(float dx, float dy)
{
    if (_controller != nullptr && !_storage.empty() && !positionsInPercent)
    {
        for (auto it = _storage.begin(); it != _storage.end(); ++it)
        {
            it->second = Vec4(it->second.x + dx, it->second.y + dy, it->second.z, it->second.w);
        }
        _default.x += dx;
        _default.y += dy;

        updateState();
    }
}

NS_FGUI_END