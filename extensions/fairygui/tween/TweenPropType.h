#ifndef __TWEENPROPTYPE_H__
#define __TWEENPROPTYPE_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class TweenValue;
class GObject;

enum class TweenPropType
{
    None,
    X,
    Y,
    Position,
    Width,
    Height,
    Size,
    ScaleX,
    ScaleY,
    Scale,
    Rotation,
    Alpha,
    Progress
};

class TweenPropTypeUtils
{
public:
    static void setProps(GObject* target, TweenPropType propType, const TweenValue& value);
    static void setProps(cocos2d::Node* target, TweenPropType propType, const TweenValue& value);
};


NS_FGUI_END

#endif
