/****************************************************************************

Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmol.dev/

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

#include "math/Color.h"

#if defined(_WIN32)
#    pragma push_macro("TRANSPARENT")
#    undef TRANSPARENT
#endif

NS_AX_MATH_BEGIN

/**
 * Color3B
 */

bool Color3B::operator==(const Color3B& right) const
{
    return (r == right.r && g == right.g && b == right.b);
}

bool Color3B::operator==(const Color32& right) const
{
    return (r == right.r && g == right.g && b == right.b && 255 == right.a);
}

bool Color3B::operator==(const Color& right) const
{
    return (right.a == 1.0f && Color(*this) == right);
}

bool Color3B::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color3B::operator!=(const Color32& right) const
{
    return !(*this == right);
}

bool Color3B::operator!=(const Color& right) const
{
    return !(*this == right);
}

/**
 * Color32
 */

bool Color32::operator==(const Color32& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == right.a);
}

bool Color32::operator==(const Color3B& right) const
{
    return (r == right.r && g == right.g && b == right.b && a == 255);
}

bool Color32::operator==(const Color& right) const
{
    return (*this == Color32(right));
}

bool Color32::operator!=(const Color32& right) const
{
    return !(*this == right);
}

bool Color32::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color32::operator!=(const Color& right) const
{
    return !(*this == right);
}

/**
 * Color
 */

bool Color::operator==(const Color3B& right) const
{
    return (a == 1.0f && Color3B(*this) == right);
}

bool Color::operator==(const Color32& right) const
{
    return (*this == Color(right));
}

bool Color::operator!=(const Color3B& right) const
{
    return !(*this == right);
}

bool Color::operator!=(const Color32& right) const
{
    return !(*this == right);
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

const Color32 Color32::WHITE(255, 255, 255, 255);
const Color32 Color32::YELLOW(255, 255, 0, 255);
const Color32 Color32::GREEN(0, 255, 0, 255);
const Color32 Color32::BLUE(0, 0, 255, 255);
const Color32 Color32::RED(255, 0, 0, 255);
const Color32 Color32::MAGENTA(255, 0, 255, 255);
const Color32 Color32::BLACK(0, 0, 0, 255);
const Color32 Color32::ORANGE(255, 127, 0, 255);
const Color32 Color32::GRAY(166, 166, 166, 255);
const Color32 Color32::TRANSPARENT(0, 0, 0, 0);

const Color Color::WHITE(1, 1, 1, 1);
const Color Color::YELLOW(1, 1, 0, 1);
const Color Color::GREEN(0, 1, 0, 1);
const Color Color::BLUE(0, 0, 1, 1);
const Color Color::RED(1, 0, 0, 1);
const Color Color::MAGENTA(1, 0, 1, 1);
const Color Color::BLACK(0, 0, 0, 1);
const Color Color::ORANGE(1, 0.5f, 0, 1);
const Color Color::GRAY(0.65f, 0.65f, 0.65f, 1);
const Color Color::TRANSPARENT(0, 0, 0, 0);

HSV::HSV() {}

HSV::HSV(float _h, float _s, float _v, float _a) : Vec4Adapter(_h, _s, _v, _a) {}

HSV::HSV(const Color3B& c)
{
    fromRgba(Color(c));
};

HSV::HSV(const Color32& c)
{
    fromRgba(Color(c));
}

HSV::HSV(const Color& c)
{
    fromRgba(c);
}

void HSV::fromRgba(const Color& rgba)
{
    float fcmax = MAX(MAX(rgba.r, rgba.g), rgba.b);
    float fcmin = MIN(MIN(rgba.r, rgba.g), rgba.b);
    float fdel  = fcmax - fcmin;

    if (fdel > 0)
    {
        if (fcmax == rgba.r)
        {
            h = 60 * (fmod(((rgba.g - rgba.b) / fdel), 6));
        }
        else if (fcmax == rgba.g)
        {
            h = 60 * (((rgba.b - rgba.r) / fdel) + 2);
        }
        else if (fcmax == rgba.b)
        {
            h = 60 * (((rgba.r - rgba.g) / fdel) + 4);
        }

        if (fcmax > 0)
        {
            s = fdel / fcmax;
        }
        else
        {
            s = 0;
        }

        v = fcmax;
    }
    else
    {
        h = 0;
        s = 0;
        v = fcmax;
    }

    if (h < 0)
    {
        h = 360 + h;
    }

    this->a = rgba.a;
}

Color HSV::toRgba() const
{
    auto rgba = Color(0, 0, 0, a);

    float hue = remainder(std::fabs(h), 360);
    hue += 360;

    float fc      = v * s;
    float fhprime = fmod(hue / 60.0, 6);
    float fx      = fc * (1 - fabs(fmod(fhprime, 2) - 1));
    float fm      = v - fc;

    if (0 <= fhprime && fhprime < 1)
    {
        rgba.r = fc;
        rgba.g = fx;
        rgba.b = 0;
    }
    else if (1 <= fhprime && fhprime < 2)
    {
        rgba.r = fx;
        rgba.g = fc;
        rgba.b = 0;
    }
    else if (2 <= fhprime && fhprime < 3)
    {
        rgba.r = 0;
        rgba.g = fc;
        rgba.b = fx;
    }
    else if (3 <= fhprime && fhprime < 4)
    {
        rgba.r = 0;
        rgba.g = fx;
        rgba.b = fc;
    }
    else if (4 <= fhprime && fhprime < 5)
    {
        rgba.r = fx;
        rgba.g = 0;
        rgba.b = fc;
    }
    else if (5 <= fhprime && fhprime < 6)
    {
        rgba.r = fc;
        rgba.g = 0;
        rgba.b = fx;
    }
    else
    {
        rgba.r = 0;
        rgba.g = 0;
        rgba.b = 0;
    }

    rgba.r += fm;
    rgba.g += fm;
    rgba.b += fm;

    return rgba;
}

Color3B HSV::toColor3B() const
{
    return Color3B(toRgba());
}

Color32 HSV::toColor32() const
{
    return Color32(toRgba());
}

HSL::HSL() {}
HSL::HSL(float _h, float _s, float _l, float _a) : Vec4Adapter(_h, _s, _l, _a) {}

HSL::HSL(const Color3B& c)
{
    fromRgba(Color(c));
}

HSL::HSL(const Color32& c)
{
    fromRgba(Color(c));
}

HSL::HSL(const Color& c)
{
    fromRgba(c);
}

void HSL::fromRgba(const Color& rgba)
{
    float max = MAX(MAX(rgba.r, rgba.g), rgba.b);
    float min = MIN(MIN(rgba.r, rgba.g), rgba.b);

    h = s = l = (max + min) / 2;

    if (max == min)
    {
        h = s = 0;  // achromatic
    }
    else
    {
        float d = max - min;
        s       = (l > 0.5) ? d / (2 - max - min) : d / (max + min);

        if (max == rgba.r)
        {
            h = (rgba.g - rgba.b) / d + (rgba.g < rgba.b ? 6 : 0);
        }
        else if (max == rgba.g)
        {
            h = (rgba.b - rgba.r) / d + 2;
        }
        else if (max == rgba.b)
        {
            h = (rgba.r - rgba.g) / d + 4;
        }

        h /= 6;
    }

    this->a = rgba.a;
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

Color HSL::toRgba() const
{
    auto rgba = Color(0, 0, 0, a);

    float hue = remainder(std::fabs(h), 360);
    hue += 360;
    hue /= 360.0F;

    if (0 == s)
    {
        rgba.r = rgba.g = rgba.b = l;
    }
    else
    {
        float q = l < 0.5 ? l * (1 + s) : l + s - l * s;
        float p = 2 * l - q;
        rgba.r  = hue2rgb(p, q, hue + 1. / 3);
        rgba.g  = hue2rgb(p, q, hue);
        rgba.b  = hue2rgb(p, q, hue - 1. / 3);
    }

    return rgba;
}

Color3B HSL::toColor3B() const
{
    return Color3B(toRgba());
}

Color32 HSL::toColor32() const
{
    return Color32(toRgba());
}

NS_AX_MATH_END

#if defined(_WIN32)
#    pragma pop_macro("TRANSPARENT")
#endif
