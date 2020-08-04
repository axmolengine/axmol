#ifndef __MARGIN_H__
#define __MARGIN_H__

#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

//copy from cocos2d::ui::Margin
class Margin
{
public:
    /**
    * Left margin.
    */
    float left;
    /**
    * Top margin.
    */
    float top;
    /**
    * Right margin.
    */
    float right;
    /**
    * Bottom margin.
    */
    float bottom;

public:
    /**
    * Default constructor.
    */
    Margin();

    /**
    * Construct a Margin instance with left, top, right and bottom margins.
    *@param l Left margin in float.
    *@param t Top margin in float.
    *@param r Right margin in float.
    *@param b Bottom margin in float.
    */
    Margin(float l, float t, float r, float b);

    /**
    * Copy constructor.
    */
    Margin(const Margin& other);

    /**
    * Copy assignment operator.
    */
    Margin& operator= (const Margin& other);

    /**
    * Change margin with left, top, right and bottom margin.
    *@param l Left margin in float.
    *@param t Top margin in float.
    *@param r Right margin in float.
    *@param b Bottom margin in float.
    */
    void setMargin(float l, float t, float r, float b);

    /**
    * Test equality of two margins.
    *@param target A Margin instance.
    *@return True if two margins are equal, false otherwise.
    */
    bool equals(const Margin& target) const;

    /**
    * A margin constant with all margins equal zero.
    */
    static const Margin ZERO;
};

NS_FGUI_END

#endif