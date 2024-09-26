#include "GearSize.h"
#include "GObject.h"
#include "UIPackage.h"
#include "tween/GTween.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GearSize::GearSize(GObject* owner) : GearBase(owner)
{
}

GearSize::~GearSize()
{
}

void GearSize::init()
{
    _default = Vec4(_owner->getWidth(), _owner->getHeight(),
                    _owner->getScaleX(), _owner->getScaleY());
    _storage.clear();
}

void GearSize::addStatus(const std::string& pageId, ByteBuffer* buffer)
{
    Vec4 gv;
    gv.x = buffer->readInt();
    gv.y = buffer->readInt();
    gv.z = buffer->readFloat();
    gv.w = buffer->readFloat();

    if (pageId.size() == 0)
        _default = gv;
    else
        _storage[pageId] = gv;
}

void GearSize::apply()
{
    Vec4 gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    if (_tweenConfig && _tweenConfig->tween && UIPackage::_constructing == 0 && !disableAllTweenEffect)
    {
        if (_tweenConfig->_tweener != nullptr)
        {
            if (_tweenConfig->_tweener->endValue.getVec4() != gv)
            {
                _tweenConfig->_tweener->kill(true);
                _tweenConfig->_tweener = nullptr;
            }
            else
                return;
        }

        bool a = gv.x != _owner->getWidth() || gv.y != _owner->getHeight();
        bool b = gv.z != _owner->getScaleX() || gv.w != _owner->getScaleY();
        if (a || b)
        {
            if (_owner->checkGearController(0, _controller))
                _tweenConfig->_displayLockToken = _owner->addDisplayLock();

            _tweenConfig->_tweener = GTween::to(Vec4(_owner->getWidth(), _owner->getHeight(), _owner->getScaleX(), _owner->getScaleY()), gv, _tweenConfig->duration)
                                         ->setDelay(_tweenConfig->delay)
                                         ->setEase(_tweenConfig->easeType)
                                         ->setTargetAny(this)
                                         ->setUserData(Value((a ? 1 : 0) + (b ? 2 : 0)))
                                         ->onUpdate(AX_CALLBACK_1(GearSize::onTweenUpdate, this))
                                         ->onComplete(AX_CALLBACK_0(GearSize::onTweenComplete, this));
        }
    }
    else
    {
        _owner->_gearLocked = true;
        _owner->setSize(gv.x, gv.y, _owner->checkGearController(1, _controller));
        _owner->setScale(gv.z, gv.w);
        _owner->_gearLocked = false;
    }
}

void GearSize::onTweenUpdate(GTweener* tweener)
{
    int flag = tweener->getUserData().asInt();
    _owner->_gearLocked = true;
    if ((flag & 1) != 0)
        _owner->setSize(tweener->value.x, tweener->value.y, _owner->checkGearController(1, _controller));
    if ((flag & 2) != 0)
        _owner->setScale(tweener->value.z, tweener->value.w);
    _owner->_gearLocked = false;
}

void GearSize::onTweenComplete()
{
    if (_tweenConfig->_displayLockToken != 0)
    {
        _owner->releaseDisplayLock(_tweenConfig->_displayLockToken);
        _tweenConfig->_displayLockToken = 0;
    }
    _tweenConfig->_tweener = nullptr;
    _owner->dispatchEvent(UIEventType::GearStop);
}

void GearSize::updateState()
{
    _storage[_controller->getSelectedPageId()] = Vec4(_owner->getWidth(), _owner->getHeight(),
                                                      _owner->getScaleX(), _owner->getScaleY());
}

void GearSize::updateFromRelations(float dx, float dy)
{
    if (_controller != nullptr && !_storage.empty())
    {
        for (auto it = _storage.begin(); it != _storage.end(); ++it)
        {
            it->second = Vec4(it->second.x + dx, it->second.y + dy,
                              it->second.z, it->second.w);
        }
        _default.x += dx;
        _default.y += dy;

        updateState();
    }
}

NS_FGUI_END