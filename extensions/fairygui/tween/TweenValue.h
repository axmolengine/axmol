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

    axis::Vec2 getVec2() const;
    void setVec2(const axis::Vec2& value);
    axis::Vec3 getVec3() const;
    void setVec3(const axis::Vec3& value);
    axis::Vec4 getVec4() const;
    void setVec4(const axis::Vec4& value);
    axis::Color4B getColor() const;
    void setColor(const axis::Color4B& value);
    float operator[] (int index) const;
    float& operator[] (int index);
    void setZero();
};

NS_FGUI_END

#endif
