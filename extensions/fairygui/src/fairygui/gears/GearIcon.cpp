#include "GearIcon.h"
#include "GObject.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GearIcon::GearIcon(GObject * owner) :GearBase(owner)
{

}

GearIcon::~GearIcon()
{
}

void GearIcon::init()
{
    _default = _owner->getIcon();
    _storage.clear();
}

void GearIcon::addStatus(const std::string&  pageId, ByteBuffer* buffer)
{
    if (pageId.length() == 0)
        _default = buffer->readS();
    else
        _storage[pageId] = buffer->readS();
}

void GearIcon::apply()
{
    _owner->_gearLocked = true;

    auto it = _storage.find(_controller->getSelectedPageId());
    if (it != _storage.end())
        _owner->setIcon(it->second);
    else
        _owner->setIcon(_default);

    _owner->_gearLocked = false;
}

void GearIcon::updateState()
{
    _storage[_controller->getSelectedPageId()] = _owner->getIcon();
}

NS_FGUI_END