#include "TweenPropType.h"
#include "TweenValue.h"
#include "GObject.h"
#include "GProgressBar.h"

NS_FGUI_BEGIN
using namespace ax;

void TweenPropTypeUtils::setProps(GObject * target, TweenPropType propType, const TweenValue & value)
{
    if (target == nullptr)
        return;

    switch (propType)
    {
    case TweenPropType::X:
        target->setX(value.x);
        break;

    case TweenPropType::Y:
        target->setY(value.x);
        break;

    case TweenPropType::Position:
        target->setPosition(value.x, value.y);
        break;

    case TweenPropType::Width:
        target->setWidth(value.x);
        break;

    case TweenPropType::Height:
        target->setHeight(value.x);
        break;

    case TweenPropType::Size:
        target->setSize(value.x, value.y);
        break;

    case TweenPropType::ScaleX:
        target->setScaleX(value.x);
        break;

    case TweenPropType::ScaleY:
        target->setScaleY(value.x);
        break;

    case TweenPropType::Scale:
        target->setScale(value.x, value.y);
        break;

    case TweenPropType::Rotation:
        target->setRotation(value.x);
        break;

    case TweenPropType::Alpha:
        target->setAlpha(value.x);
        break;

    case TweenPropType::Progress:
        target->as<GProgressBar>()->update(value.d);
        break;
    default:
        break;
    }
}

void TweenPropTypeUtils::setProps(ax::Node * target, TweenPropType propType, const TweenValue & value)
{
    switch (propType)
    {
    case TweenPropType::X:
        target->setPositionX(value.x);
        break;

    case TweenPropType::Y:
        target->setPositionY(value.x);
        break;

    case TweenPropType::Position:
        target->setPosition(value.x, value.y);
        break;

    case TweenPropType::ScaleX:
        target->setScaleX(value.x);
        break;

    case TweenPropType::ScaleY:
        target->setScaleY(value.x);
        break;

    case TweenPropType::Scale:
        target->setScale(value.x, value.y);
        break;

    case TweenPropType::Rotation:
        target->setRotation(value.x);
        break;

    case TweenPropType::Alpha:
        target->setOpacity(value.x);
        break;

    default:
        break;
    }
}

NS_FGUI_END

