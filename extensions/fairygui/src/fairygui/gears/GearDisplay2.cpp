#include "GearDisplay2.h"
#include "Controller.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GearDisplay2::GearDisplay2(GObject* owner)
    : GearBase(owner),
      _visible(0),
      condition(0)
{
}

GearDisplay2::~GearDisplay2()
{
}

void GearDisplay2::apply()
{
    if (_controller == nullptr || pages.size() == 0)
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

bool GearDisplay2::evaluate(bool connected)
{
    bool v = _controller == nullptr || _visible > 0;
    if (condition == 0)
        v = v && connected;
    else
        v = v || connected;
    return v;
}

void GearDisplay2::updateState()
{
}

void GearDisplay2::addStatus(const std::string& pageId, ByteBuffer* buffer)
{
}

void GearDisplay2::init()
{
    pages.clear();
}

NS_FGUI_END