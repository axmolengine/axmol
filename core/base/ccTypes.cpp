/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

 https://adxeproject.github.io/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "base/ccTypes.h"
#include "renderer/backend/Types.h"

NS_CC_BEGIN

const std::string STD_STRING_EMPTY("");
const ssize_t CC_INVALID_INDEX = -1;

/**
 * Color3B
 */

Color3B::Color3B() {}

Color3B::Color3B(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {}

Color3B::Color3B(const Color4B& color) : r(color.r), g(color.g), b(color.b) {}

Color3B::Color3B(const Color4F& color) : r(color.r * 255.0f), g(color.g * 255.0f), b(color.b * 255.0f) {}

bool Color3B::operator==(const Color3B& right) const
{
    return (r == right.r && g == right.g && b == right.b);
}

bool Color3B::operator==(const Color4B& right) const
{
    return (r == right.r && g == right.g && b == right.b && 255 == right.a);
}

bool Color3B::operator==(const Color4F& right) const
{
    return (right.a == 1.0f && Color4F(*this) == right);
}

bool Color3B::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color3B::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

bool Color3B::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

/**
 * Color4B
 */

Color4B::Color4B() {}

Color4B::Color4B(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

Color4B::Color4B(const Color3B& color, uint8_t _a) : r(color.r), g(color.g), b(color.b), a(_a) {}

Color4B::Color4B(const Color4F& color) : r(color.r * 255), g(color.g * 255), b(color.b * 255), a(color.a * 255) {}

bool Color4B::operator==(const Color4B& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == right.a);
}

bool Color4B::operator==(const Color3B& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == 255);
}

bool Color4B::operator==(const Color4F& right) const
{
    return (*this == Color4B(right));
}

bool Color4B::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

bool Color4B::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color4B::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

/**
 * Color4F
 */

Color4F::Color4F() {}

Color4F::Color4F(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}

Color4F::Color4F(const Color3B& color, float _a) : r(color.r / 255.0f), g(color.g / 255.0f), b(color.b / 255.0f), a(_a)
{}

Color4F::Color4F(const Color4B& color)
    : r(color.r / 255.0f), g(color.g / 255.0f), b(color.b / 255.0f), a(color.a / 255.0f)
{}

bool Color4F::operator==(const Color4F& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == right.a);
}

bool Color4F::operator==(const Color3B& right) const
{
    return (a == 1.0f && Color3B(*this) == right);
}

bool Color4F::operator==(const Color4B& right) const
{
    return (*this == Color4F(right));
}

bool Color4F::operator!=(const Color4F& right) const
{
    return !(*this == right);
}

bool Color4F::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color4F::operator!=(const Color4B& right) const
{
    return !(*this == right);
}

Color4F& operator+=(Color4F& lhs, const Color4F& rhs)
{
    lhs.r += rhs.r;
    lhs.g += rhs.g;
    lhs.b += rhs.b;
    lhs.a += rhs.a;
    return lhs;
}
Color4F operator+(Color4F lhs, const Color4F& rhs)
{
    return lhs += rhs;
}
Color4F& operator-=(Color4F& lhs, const Color4F& rhs)
{
    lhs.r -= rhs.r;
    lhs.g -= rhs.g;
    lhs.b -= rhs.b;
    lhs.a -= rhs.a;
    return lhs;
}
Color4F operator-(Color4F lhs, const Color4F& rhs)
{
    return lhs -= rhs;
}

Color4F& operator*=(Color4F& lhs, const Color4F& rhs)
{
    lhs.r *= rhs.r;
    lhs.g *= rhs.g;
    lhs.b *= rhs.b;
    lhs.a *= rhs.a;
    return lhs;
}
Color4F& operator*=(Color4F& lhs, float rhs)
{
    lhs.r *= rhs;
    lhs.g *= rhs;
    lhs.b *= rhs;
    lhs.a *= rhs;
    return lhs;
}
Color4F operator*(Color4F lhs, const Color4F& rhs)
{
    return lhs *= rhs;
}

Color4F operator*(Color4F lhs, float rhs)
{
    return lhs *= rhs;
}

Color4F& operator/=(Color4F& lhs, const Color4F& rhs)
{
    lhs.r /= rhs.r;
    lhs.g /= rhs.g;
    lhs.b /= rhs.b;
    lhs.a /= rhs.a;
    return lhs;
}
Color4F& operator/=(Color4F& lhs, float rhs)
{
    lhs.r /= rhs;
    lhs.g /= rhs;
    lhs.b /= rhs;
    lhs.a /= rhs;
    return lhs;
}
Color4F operator/(Color4F lhs, const Color4F& rhs)
{
    return lhs /= rhs;
}

Color4F operator/(Color4F lhs, float rhs)
{
    return lhs /= rhs;
}

/**
 * Color constants
 */

const Color3B Color3B::WHITE(255, 255, 255);
const Color3B Color3B::YELLOW(255, 255, 0);
const Color3B Color3B::GREEN(0, 255, 0);
const Color3B Color3B::BLUE(0, 0, 255);
const Color3B Color3B::RED(255, 0, 0);
const Color3B Color3B::MAGENTA(255, 0, 255);
const Color3B Color3B::BLACK(0, 0, 0);
const Color3B Color3B::ORANGE(255, 127, 0);
const Color3B Color3B::GRAY(166, 166, 166);

const Color4B Color4B::WHITE(255, 255, 255, 255);
const Color4B Color4B::YELLOW(255, 255, 0, 255);
const Color4B Color4B::GREEN(0, 255, 0, 255);
const Color4B Color4B::BLUE(0, 0, 255, 255);
const Color4B Color4B::RED(255, 0, 0, 255);
const Color4B Color4B::MAGENTA(255, 0, 255, 255);
const Color4B Color4B::BLACK(0, 0, 0, 255);
const Color4B Color4B::ORANGE(255, 127, 0, 255);
const Color4B Color4B::GRAY(166, 166, 166, 255);

const Color4F Color4F::WHITE(1, 1, 1, 1);
const Color4F Color4F::YELLOW(1, 1, 0, 1);
const Color4F Color4F::GREEN(0, 1, 0, 1);
const Color4F Color4F::BLUE(0, 0, 1, 1);
const Color4F Color4F::RED(1, 0, 0, 1);
const Color4F Color4F::MAGENTA(1, 0, 1, 1);
const Color4F Color4F::BLACK(0, 0, 0, 1);
const Color4F Color4F::ORANGE(1, 0.5f, 0, 1);
const Color4F Color4F::GRAY(0.65f, 0.65f, 0.65f, 1);

HSV::HSV() {}

HSV::HSV(float _h, float _s, float _v, float _a) : h(_h), s(_s), v(_v), a(_a) {}

HSV::HSV(const Color3B& c)
{
    float r = c.r / 255.0F;
    float g = c.g / 255.0F;
    float b = c.b / 255.0F;
    set(r, g, b, 1.0F);
}

HSV::HSV(const Color4B& c)
{
    float r = c.r / 255.0F;
    float g = c.g / 255.0F;
    float b = c.b / 255.0F;
    float a = c.a / 255.0F;
    set(r, g, b, a);
}

HSV::HSV(const Color4F& c)
{
    float r = c.r;
    float g = c.g;
    float b = c.b;
    float a = c.a;
    set(r, g, b, a);
}

bool HSV::operator==(const HSV& right) const
{
    return (h == right.h && s == right.s && v == right.v && a == right.a);
}

bool HSV::operator!=(const HSV& right) const
{
    return !(h != right.h || s != right.s || v != right.v || a == right.a);
}

HSV& operator+=(HSV& lhs, const HSV& rhs)
{
    lhs.h += rhs.h;
    lhs.s += rhs.s;
    lhs.v += rhs.v;
    lhs.a += rhs.a;
    return lhs;
}
HSV operator+(HSV lhs, const HSV& rhs)
{
    return lhs += rhs;
}
HSV& operator-=(HSV& lhs, const HSV& rhs)
{
    lhs.h -= rhs.h;
    lhs.s -= rhs.s;
    lhs.v -= rhs.v;
    lhs.a -= rhs.a;
    return lhs;
}
HSV operator-(HSV lhs, const HSV& rhs)
{
    return lhs -= rhs;
}

HSV& operator*=(HSV& lhs, const HSV& rhs)
{
    lhs.h *= rhs.h;
    lhs.s *= rhs.s;
    lhs.v *= rhs.v;
    lhs.a *= rhs.a;
    return lhs;
}
HSV& operator*=(HSV& lhs, float rhs)
{
    lhs.h *= rhs;
    lhs.s *= rhs;
    lhs.v *= rhs;
    lhs.a *= rhs;
    return lhs;
}
HSV operator*(HSV lhs, const HSV& rhs)
{
    return lhs *= rhs;
}

HSV operator*(HSV lhs, float rhs)
{
    return lhs *= rhs;
}

HSV& operator/=(HSV& lhs, const HSV& rhs)
{
    lhs.h /= rhs.h;
    lhs.s /= rhs.s;
    lhs.v /= rhs.v;
    lhs.a /= rhs.a;
    return lhs;
}
HSV& operator/=(HSV& lhs, float rhs)
{
    lhs.h /= rhs;
    lhs.s /= rhs;
    lhs.v /= rhs;
    lhs.a /= rhs;
    return lhs;
}
HSV operator/(HSV lhs, const HSV& rhs)
{
    return lhs /= rhs;
}

HSV operator/(HSV lhs, float rhs)
{
    return lhs /= rhs;
}

void HSV::set(float r, float g, float b, float a)
{
    float fCMax  = MAX(MAX(r, g), b);
    float fCMin  = MIN(MIN(r, g), b);
    float fDelta = fCMax - fCMin;

    if (fDelta > 0)
    {
        if (fCMax == r)
        {
            h = 60 * (fmod(((g - b) / fDelta), 6));
        }
        else if (fCMax == g)
        {
            h = 60 * (((b - r) / fDelta) + 2);
        }
        else if (fCMax == b)
        {
            h = 60 * (((r - g) / fDelta) + 4);
        }

        if (fCMax > 0)
        {
            s = fDelta / fCMax;
        }
        else
        {
            s = 0;
        }

        v = fCMax;
    }
    else
    {
        h = 0;
        s = 0;
        v = fCMax;
    }

    if (h < 0)
    {
        h = 360 + h;
    }

    this->a = a;
}

void HSV::get(float& r, float& g, float& b) const
{
    float hue = -(remainder(std::fabs(h), 360));
    hue += 360;

    float fC      = v * s;
    float fHPrime = fmod(hue / 60.0, 6);
    float fX      = fC * (1 - fabs(fmod(fHPrime, 2) - 1));
    float fM      = v - fC;

    if (0 <= fHPrime && fHPrime < 1)
    {
        r = fC;
        g = fX;
        b = 0;
    }
    else if (1 <= fHPrime && fHPrime < 2)
    {
        r = fX;
        g = fC;
        b = 0;
    }
    else if (2 <= fHPrime && fHPrime < 3)
    {
        r = 0;
        g = fC;
        b = fX;
    }
    else if (3 <= fHPrime && fHPrime < 4)
    {
        r = 0;
        g = fX;
        b = fC;
    }
    else if (4 <= fHPrime && fHPrime < 5)
    {
        r = fX;
        g = 0;
        b = fC;
    }
    else if (5 <= fHPrime && fHPrime < 6)
    {
        r = fC;
        g = 0;
        b = fX;
    }
    else
    {
        r = 0;
        g = 0;
        b = 0;
    }

    r += fM;
    g += fM;
    b += fM;
}

Color3B HSV::toColor3B()
{
    float r, g, b;
    get(r, g, b);
    return Color3B(r * 255.0F, g * 255.0F, b * 255.0F);
}

Color4B HSV::toColor4B()
{
    float r, g, b;
    get(r, g, b);
    return Color4B(r * 255.0F, g * 255.0F, b * 255.0F, a * 255.0F);
}

Color4F HSV::toColor4F()
{
    float r, g, b;
    get(r, g, b);
    return Color4F(r, g, b, a);
}

HSL::HSL() {}
HSL::HSL(float _h, float _s, float _l, float _a) : h(_h), s(_s), l(_l), a(_a) {}

HSL::HSL(const Color3B& c)
{
    float r = c.r / 255.0F;
    float g = c.g / 255.0F;
    float b = c.b / 255.0F;
    set(r, g, b, 1.0F);
}

HSL::HSL(const Color4B& c)
{
    float r = c.r / 255.0F;
    float g = c.g / 255.0F;
    float b = c.b / 255.0F;
    float a = c.a / 255.0F;
    set(r, g, b, a);
}

HSL::HSL(const Color4F& c)
{
    float r = c.r;
    float g = c.g;
    float b = c.b;
    float a = c.a;
    set(r, g, b, a);
}

bool HSL::operator==(const HSL& right) const
{
    return (h == right.h && s == right.s && l == right.l && a == right.a);
}

bool HSL::operator!=(const HSL& right) const
{
    return !(h != right.h || s != right.s || l != right.l || a == right.a);
}

HSL& operator+=(HSL& lhs, const HSL& rhs)
{
    lhs.h += rhs.h;
    lhs.s += rhs.s;
    lhs.l += rhs.l;
    lhs.a += rhs.a;
    return lhs;
}
HSL operator+(HSL lhs, const HSL& rhs)
{
    return lhs += rhs;
}
HSL& operator-=(HSL& lhs, const HSL& rhs)
{
    lhs.h -= rhs.h;
    lhs.s -= rhs.s;
    lhs.l -= rhs.l;
    lhs.a -= rhs.a;
    return lhs;
}
HSL operator-(HSL lhs, const HSL& rhs)
{
    return lhs -= rhs;
}

HSL& operator*=(HSL& lhs, const HSL& rhs)
{
    lhs.h *= rhs.h;
    lhs.s *= rhs.s;
    lhs.l *= rhs.l;
    lhs.a *= rhs.a;
    return lhs;
}
HSL& operator*=(HSL& lhs, float rhs)
{
    lhs.h *= rhs;
    lhs.s *= rhs;
    lhs.l *= rhs;
    lhs.a *= rhs;
    return lhs;
}
HSL operator*(HSL lhs, const HSL& rhs)
{
    return lhs *= rhs;
}

HSL operator*(HSL lhs, float rhs)
{
    return lhs *= rhs;
}

HSL& operator/=(HSL& lhs, const HSL& rhs)
{
    lhs.h /= rhs.h;
    lhs.s /= rhs.s;
    lhs.l /= rhs.l;
    lhs.a /= rhs.a;
    return lhs;
}
HSL& operator/=(HSL& lhs, float rhs)
{
    lhs.h /= rhs;
    lhs.s /= rhs;
    lhs.l /= rhs;
    lhs.a /= rhs;
    return lhs;
}
HSL operator/(HSL lhs, const HSL& rhs)
{
    return lhs /= rhs;
}

HSL operator/(HSL lhs, float rhs)
{
    return lhs /= rhs;
}

void HSL::set(float r, float g, float b, float a)
{
    float max = MAX(MAX(r, g), b);
    float min = MIN(MIN(r, g), b);

    h = s = l = (max + min) / 2;

    if (max == min)
    {
        h = s = 0;  // achromatic
    }
    else
    {
        float d  = max - min;
        s = (l > 0.5) ? d / (2 - max - min) : d / (max + min);

        if (max == r)
        {
            h = (g - b) / d + (g < b ? 6 : 0);
        }
        else if (max == g)
        {
            h = (b - r) / d + 2;
        }
        else if (max == b)
        {
            h = (r - g) / d + 4;
        }

        h /= 6;
    }
    
    this->a = a;
}

float HSL::hue2rgb(float p, float q, float t)
{
    if (t < 0)
        t += 1;
    if (t > 1)
        t -= 1;
    if (t < 1. / 6)
        return p + (q - p) * 6 * t;
    if (t < 1. / 2)
        return q;
    if (t < 2. / 3)
        return p + (q - p) * (2. / 3 - t) * 6;

    return p;
}

void HSL::get(float& r, float& g, float& b) const
{
    float hue = -(remainder(std::fabs(h), 360));
    hue += 360;
    hue /= 360.0F;

    if (0 == s)
    {
        r = g = b = l;  // achromatic
    }
    else
    {
        float q  = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p  = 2 * l - q;
        r       = hue2rgb(p, q, hue + 1. / 3);
        g       = hue2rgb(p, q, hue);
        b       = hue2rgb(p, q, hue - 1. / 3);
    }
}

Color3B HSL::toColor3B()
{
    float r, g, b;
    get(r, g, b);
    return Color3B(r * 255.0F, g * 255.0F, b * 255.0F);
}

Color4B HSL::toColor4B()
{
    float r, g, b;
    get(r, g, b);
    return Color4B(r * 255.0F, g * 255.0F, b * 255.0F, a * 255.0F);
}

Color4F HSL::toColor4F()
{
    float r, g, b;
    get(r, g, b);
    return Color4F(r, g, b, a);
}

const BlendFunc BlendFunc::DISABLE             = {backend::BlendFactor::ONE, backend::BlendFactor::ZERO};
const BlendFunc BlendFunc::ALPHA_PREMULTIPLIED = {backend::BlendFactor::ONE, backend::BlendFactor::ONE_MINUS_SRC_ALPHA};
const BlendFunc BlendFunc::ALPHA_NON_PREMULTIPLIED = {backend::BlendFactor::SRC_ALPHA,
                                                      backend::BlendFactor::ONE_MINUS_SRC_ALPHA};
const BlendFunc BlendFunc::ADDITIVE                = {backend::BlendFactor::SRC_ALPHA, backend::BlendFactor::ONE};

NS_CC_END

