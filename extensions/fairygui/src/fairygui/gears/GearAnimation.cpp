#include "GearAnimation.h"
#include "GObject.h"
#include "UIPackage.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GearAnimation::GearAnimationValue::GearAnimationValue() : frame(0), playing(false)
{
}

GearAnimation::GearAnimationValue::GearAnimationValue(bool playing, int frame)
{
    this->playing = playing;
    this->frame = frame;
}

GearAnimation::GearAnimation(GObject* owner) : GearBase(owner)
{
}

GearAnimation::~GearAnimation()
{
}

void GearAnimation::init()
{
    _default = GearAnimationValue(_owner->getProp(ObjectPropID::Playing).asBool(), _owner->getProp(ObjectPropID::Frame).asInt());
    _storage.clear();
}

void GearAnimation::addStatus(const std::string& pageId, ByteBuffer* buffer)
{
    GearAnimationValue gv;
    gv.playing = buffer->readBool();
    gv.frame = buffer->readInt();
    if (pageId.size() == 0)
        _default = gv;
    else
        _storage[pageId] = gv;
}

void GearAnimation::apply()
{
    _owner->_gearLocked = true;

    GearAnimationValue gv;
    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        gv = it->second;
    else
        gv = _default;

    _owner->setProp(ObjectPropID::Playing, Value(gv.playing));
    _owner->setProp(ObjectPropID::Frame, Value(gv.frame));

    _owner->_gearLocked = false;
}

void GearAnimation::updateState()
{
    _storage[_controller->getSelectedPageId()] = GearAnimationValue(
        _owner->getProp(ObjectPropID::Playing).asBool(), _owner->getProp(ObjectPropID::Frame).asInt());
}

NS_FGUI_END