#ifndef __TWEENVALUE_H__
#define __TWEENVALUE_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class TweenValue
{
public:
    float x;
    float y;
    float z;
    float w;
    double d;

    TweenValue();

    ax::Vec2 getVec2() const;
    void setVec2(const ax::Vec2& value);
    ax::Vec3 getVec3() const;
    void setVec3(const ax::Vec3& value);
    ax::Vec4 getVec4() const;
    void setVec4(const ax::Vec4& value);
    ax::Color32 getColor() const;
    void setColor(const ax::Color32& value);
    float operator[] (int index) const;
    float& operator[] (int index);
    void setZero();
};

NS_FGUI_END

#endif
