#ifndef __TEXTFORMAT_H__
#define __TEXTFORMAT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class TextFormat
{
public:
    TextFormat();
    TextFormat(const TextFormat & other);
    TextFormat &operator =(const TextFormat & other);

	void setFormat(const TextFormat& format);
    void enableEffect(int effectFlag) { effect |= effectFlag; }
    void disableEffect(int effectFlag) { effect &= ~effectFlag; }
    bool hasEffect(int effectFlag) const { return (effect & effectFlag) != 0; }

    static const int OUTLINE = 1;
    static const int SHADOW = 2;
    static const int GLOW = 4;

    std::string face;
    float fontSize;
    cocos2d::Color3B color;
    bool bold;
    bool italics;
    bool underline;
    int lineSpacing;
    int letterSpacing;
    cocos2d::TextHAlignment align;
    cocos2d::TextVAlignment verticalAlign;

    int effect;
    cocos2d::Color3B outlineColor;
    int outlineSize;
    cocos2d::Color3B shadowColor;
    cocos2d::Size shadowOffset;
    int shadowBlurRadius;
    cocos2d::Color3B glowColor;

    //internal use
    bool _hasColor;
};

NS_FGUI_END

#endif
