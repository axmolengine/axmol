#include "GearBase.h"
#include "GComponent.h"
#include "GearDisplay.h"
#include "gears/GearAnimation.h"
#include "gears/GearColor.h"
#include "gears/GearDisplay2.h"
#include "gears/GearFontSize.h"
#include "gears/GearIcon.h"
#include "gears/GearLook.h"
#include "gears/GearSize.h"
#include "gears/GearText.h"
#include "gears/GearXY.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

bool GearBase::disableAllTweenEffect = false;

GearBase* GearBase::create(GObject* owner, int index)
{
    GearBase* gear = nullptr;
    switch (index)
    {
    case 0:
        gear = new GearDisplay(owner);
        break;
    case 1:
        gear = new GearXY(owner);
        break;
    case 2:
        gear = new GearSize(owner);
        break;
    case 3:
        gear = new GearLook(owner);
        break;
    case 4:
        gear = new GearColor(owner);
        break;
    case 5:
        gear = new GearAnimation(owner);
        break;
    case 6:
        gear = new GearText(owner);
        break;
    case 7:
        gear = new GearIcon(owner);
        break;
    case 8:
        gear = new GearDisplay2(owner);
        break;
    case 9:
        gear = new GearFontSize(owner);
        break;
    }

    return gear;
}

GearTweenConfig::GearTweenConfig()
    : tween(true),
      easeType(EaseType::QuadOut),
      duration(0.3f),
      delay(0),
      _tweener(nullptr),
      _displayLockToken(0)
{
}

GearBase::GearBase(GObject* owner) : _owner(owner), _tweenConfig(nullptr)
{
}

GearBase::~GearBase()
{
    if (_tweenConfig && _tweenConfig->_tweener)
        _tweenConfig->_tweener->kill();
    AX_SAFE_DELETE(_tweenConfig);
}

void GearBase::setController(GController* value)
{
    if (value != _controller)
    {
        _controller = value;
        if (_controller != nullptr)
            init();
    }
}

GearTweenConfig* GearBase::getTweenConfig()
{
    if (!_tweenConfig)
        _tweenConfig = new GearTweenConfig();

    return _tweenConfig;
}

void GearBase::init()
{
}

void GearBase::addStatus(const std::string& pageId, ByteBuffer* buffer)
{
}

void GearBase::apply()
{
}

void GearBase::updateState()
{
}

void GearBase::updateFromRelations(float dx, float dy)
{
}

void GearBase::setup(ByteBuffer* buffer)
{
    _controller = _owner->getParent()->getControllerAt(buffer->readShort());
    init();

    int cnt = buffer->readShort();
    GearDisplay* g0 = dynamic_cast<GearDisplay*>(this);
    GearDisplay2* g1 = dynamic_cast<GearDisplay2*>(this);
    GearXY* g2 = nullptr;
    g0 = dynamic_cast<GearDisplay*>(this);
    if (g0)
        buffer->readSArray(g0->pages, cnt);
    else if (g1)
        buffer->readSArray(g1->pages, cnt);
    else
    {
        for (int i = 0; i < cnt; i++)
        {
            const std::string& page = buffer->readS();
            if (page.empty())
                continue;

            addStatus(page, buffer);
        }

        if (buffer->readBool())
            addStatus(STD_STRING_EMPTY, buffer);
    }

    if (buffer->readBool())
    {
        _tweenConfig = new GearTweenConfig();
        _tweenConfig->tween = true;
        _tweenConfig->easeType = (EaseType)buffer->readByte();
        _tweenConfig->duration = buffer->readFloat();
        _tweenConfig->delay = buffer->readFloat();
    }

    if (buffer->version >= 2)
    {
        if ((g2 = dynamic_cast<GearXY*>(this)) != nullptr)
        {
            if (buffer->readBool())
            {
                if (g2)
                {
                    g2->positionsInPercent = true;
                    for (int i = 0; i < cnt; i++)
                    {
                        const std::string& page = buffer->readS();
                        if (page.empty())
                            continue;

                        g2->addExtStatus(page, buffer);
                    }

                    if (buffer->readBool())
                        g2->addExtStatus(STD_STRING_EMPTY, buffer);
                }
            }
        }
        else if (g1 != nullptr)
            g1->condition = buffer->readByte();
    }
}

NS_FGUI_END