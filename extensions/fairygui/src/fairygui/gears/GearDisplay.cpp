#include "GearDisplay.h"
#include "Controller.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GearDisplay::GearDisplay(GObject* owner)
    : GearBase(owner),
      _visible(0),
      _displayLockToken(1)
{
}

GearDisplay::~GearDisplay()
{
}

void GearDisplay::apply()
{
    _displayLockToken++;
    if (_displayLockToken == 0)
        _displayLockToken = 1;

    if (pages.size() == 0)
        _visible = 1;
    else
    {
        auto iter = std::find(pages.begin(), pages.end(), _controller->getSelectedPageId());
        if (iter != pages.end())
            _visible = 1;
        else
            _visible = 0;
    }
}

void GearDisplay::updateState()
{
}

void GearDisplay::addStatus(const std::string& pageId, ByteBuffer* buffer)
{
}

void GearDisplay::init()
{
    pages.clear();
}

uint32_t GearDisplay::addLock()
{
    _visible++;
    return _displayLockToken;
}

void GearDisplay::releaseLock(uint32_t token)
{
    if (token == _displayLockToken)
        _visible--;
}

bool GearDisplay::isConnected()
{
    return _controller == nullptr || _visible > 0;
}

NS_FGUI_END