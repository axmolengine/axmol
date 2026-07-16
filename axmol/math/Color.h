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

#pragma once

#include "axmol/math/Vec4.h"

NS_AX_MATH_BEGIN

struct Color32;
struct Color;
struct HSV;

/**
 * RGBA color composed of 4 bytes.
 * @since v3.0
 */
struct AX_DLL Color32
{
    constexpr Color32() : value(0) {}
    constexpr Color32(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

    constexpr Color32(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b), a(255) {}

    template <class _Other,
              typename = std::enable_if_t<std::is_unsigned_v<decltype(_Other{}.r)> &&
                                          std::is_same_v<decltype(_Other{}.r), decltype(_Other{}.g)> &&
                                          std::is_same_v<decltype(_Other{}.r), decltype(_Other{}.b)> &&
                                          std::is_same_v<decltype(_Other{}.r), decltype(_Other{}.a)>>,
              typename = void>
    explicit Color32(const _Other& other) noexcept : r(other.r), g(other.g), b(other.b), a(other.a)
    {}

    template <class _Other,
              typename = std::enable_if_t<std::is_floating_point_v<decltype(_Other{}.r)> &&
                                          std::is_same_v<decltype(_Other{}.r), decltype(_Other{}.g)> &&
                                          std::is_same_v<decltype(_Other{}.r), decltype(_Other{}.b)> &&
                                          std::is_same_v<decltype(_Other{}.r), decltype(_Other{}.a)>>>
    explicit Color32(const _Other& other) noexcept
        : r(static_cast<uint8_t>(other.r * 255.f + 0.5f))
        , g(static_cast<uint8_t>(other.g * 255.f + 0.5f))
        , b(static_cast<uint8_t>(other.b * 255.f + 0.5f))
        , a(static_cast<uint8_t>(other.a * 255.f + 0.5f))
    {}

    // from normalized color, comment to refactor all low performance assign code
    template <class _Other,
              typename = std::enable_if_t<std::is_floating_point_v<decltype(_Other{}.r)> &&
                                          std::is_same_v<decltype(_Other{}.r), decltype(_Other{}.g)> &&
                                          std::is_same_v<decltype(_Other{}.r), decltype(_Other{}.b)> &&
                                          std::is_same_v<decltype(_Other{}.r), decltype(_Other{}.a)>>>
    Color32& operator=(const _Other & other) noexcept
    {
        set(static_cast<uint8_t>(other.r * 255.f + 0.5f), static_cast<uint8_t>(other.g * 255.f + 0.5f),
            static_cast<uint8_t>(other.b * 255.f + 0.5f), static_cast<uint8_t>(other.a * 255.f + 0.5f));
        return *this;
    }

    operator Color() const;

    template <typename T>
    Color32 withAlpha(T alpha) const
    {
        static_assert(std::is_floating_point_v<T> || std::is_unsigned_v<T>,
                      "withAlpha: alpha must be float (0~1) or unsigned integer (0~255)");
        if constexpr (std::is_floating_point_v<float>)
            return Color32{r, g, b, static_cast<uint8_t>(alpha * 255)};
        else
            return Color32{r, g, b, static_cast<uint8_t>(alpha)};
    }

    void set(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a)
    {
        r = _r;
        g = _g;
        b = _b;
        a = _a;
    }

    bool operator==(const Color32& right) const;
    bool operator==(const Color& right) const;
    bool operator!=(const Color32& right) const;
    bool operator!=(const Color& right) const;

    union
    {
        struct
        {
            uint8_t r;
            uint8_t g;
            uint8_t b;
            uint8_t a;
        };
        uint32_t value;
    };

    static const Color32 white;
    static const Color32 yellow;
    static const Color32 blue;
    static const Color32 green;
    static const Color32 red;
    static const Color32 magenta;
    static const Color32 black;
    static const Color32 orange;
    static const Color32 gray;
    static const Color32 clear;
};

#if !(defined(AX_DLLEXPORT) || defined(AX_DLLIMPORT))
inline constexpr Color32 Color32::white{255, 255, 255, 255};
inline constexpr Color32 Color32::yellow{255, 255, 0, 255};
inline constexpr Color32 Color32::blue{0, 0, 255, 255};
inline constexpr Color32 Color32::green{0, 255, 0, 255};
inline constexpr Color32 Color32::red{255, 0, 0, 255};
inline constexpr Color32 Color32::magenta{255, 0, 255, 255};
inline constexpr Color32 Color32::black{0, 0, 0, 255};
inline constexpr Color32 Color32::orange{255, 127, 0, 255};
inline constexpr Color32 Color32::gray{166, 166, 166, 255};
inline constexpr Color32 Color32::clear{0, 0, 0, 0};
#endif

/**
 * RGBA color composed of 4 floats.
 * @since v3.0, renamed from Color4F
 */
struct AX_DLL Color : public Vec4Adapter<Color>
{
    constexpr Color() {}
    constexpr Color(float _r, float _g, float _b, float _a) : Vec4Adapter(_r, _g, _b, _a) {}
    constexpr Color(float _r, float _g, float _b) : Vec4Adapter(_r, _g, _b, 1.0f) {}
    explicit Color(const Color32& color)
        : Vec4Adapter(color.r / 255.f, color.g / 255.f, color.b / 255.f, color.a / 255.f)
    {}
    template <typename _Other>
    explicit Color(const _Other& color) : Vec4Adapter(color.r, color.g, color.b, color.a)
    {}

    static Color fromHex(unsigned int v)
    {
        auto r = (v >> 24) & 0xff;
        auto g = (v >> 16) & 0xff;
        auto b = (v >> 8) & 0xff;
        auto a = v & 0xff;
        return Color{r / 255.f, g / 255.f, b / 255.f, a / 255.f};
    }

    inline Color& premultiplyAlpha()
    {
        r *= a;
        g *= a;
        b *= a;
        return *this;
    }

    template <typename T>
    Color withAlpha(T alpha) const
    {
        static_assert(std::is_floating_point_v<T> || std::is_unsigned_v<T>,
                      "withAlpha: alpha must be float (0~1) or unsigned integer (0~255)");

        if constexpr (std::is_floating_point_v<T>)
            return Color{r, g, b, static_cast<float>(alpha)};
        else
            return Color{r, g, b, static_cast<float>(alpha) / 255.0f};
    }

    static Color random() { return Color(AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1(), AXRANDOM_0_1()); }

    bool operator==(const Color32& rhs) const;
    bool operator!=(const Color32& rhs) const;

    bool equals(const Color& other) const { return (*this == other); }

    static const Color white;
    static const Color yellow;
    static const Color blue;
    static const Color green;
    static const Color red;
    static const Color magenta;
    static const Color black;
    static const Color orange;
    static const Color gray;
    static const Color clear;
};

#if !(defined(AX_DLLEXPORT) || defined(AX_DLLIMPORT))
inline constexpr Color Color::white{1, 1, 1, 1};
inline constexpr Color Color::yellow{1, 1, 0, 1};
inline constexpr Color Color::blue{0, 0, 1, 1};
inline constexpr Color Color::green{0, 1, 0, 1};
inline constexpr Color Color::red{1, 0, 0, 1};
inline constexpr Color Color::magenta{1, 0, 1, 1};
inline constexpr Color Color::black{0, 0, 0, 1};
inline constexpr Color Color::orange{1, 0.5f, 0, 1};
inline constexpr Color Color::gray{0.65f, 0.65f, 0.65f, 1};
inline constexpr Color Color::clear{0, 0, 0, 0};
#endif

/**
 * Hue Saturation Value color space composed of 4 floats.
 * @since axmol-1.0.0b7
 *
 * Implementation source: https://gist.github.com/fairlight1337/4935ae72bcbcc1ba5c72
 */
struct AX_DLL HSV : public Vec4Adapter<HSV>
{
    HSV();
    HSV(float _h, float _s, float _v, float _a = 1.0F);
    explicit HSV(const Color32& c);
    explicit HSV(const Color& c);

    bool equals(const HSV& other) const { return (*this == other); }

    void fromRgba(const Color& rgba);
    Color toRgba() const;
    Color32 toColor32() const;
};

/**
 * Hue Saturation Luminance color space composed of 4 floats.
 * @since axmol-1.0.0b7
 *
 * Implementation source: https://gist.github.com/ciembor/1494530
 */
struct AX_DLL HSL : public Vec4Adapter<HSL>
{
    HSL();
    HSL(float _h, float _s, float _l, float _a = 1.0F);

    explicit HSL(const Color32& c);
    explicit HSL(const Color& c);

    bool equals(const HSL& other) const { return (*this == other); }

    void fromRgba(const Color& rgba);
    Color toRgba() const;

    static float hue2rgb(float p, float q, float t);

    Color32 toColor32() const;
};

inline Color32::operator Color() const
{
    return Color{r / 255.f, g / 255.f, b / 255.f, a / 255.f};
}

NS_AX_MATH_END
