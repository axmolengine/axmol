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

namespace ax
{

inline Color3B::Color3B() {}

inline Color3B::Color3B(uint8_t _r, uint8_t _g, uint8_t _b) : r(_r), g(_g), b(_b) {}

inline Color3B::Color3B(const Color4B& color) : r(color.r), g(color.g), b(color.b) {}

inline Color3B::Color3B(const Color4F& color) : r(color.r * 255.0f), g(color.g * 255.0f), b(color.b * 255.0f) {}


inline Color4B::Color4B() {}

inline Color4B::Color4B(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a) : r(_r), g(_g), b(_b), a(_a) {}

inline Color4B::Color4B(const Color3B& color, uint8_t _a) : r(color.r), g(color.g), b(color.b), a(_a) {}

inline Color4B::Color4B(const Color4F& color) : r(color.r * 255), g(color.g * 255), b(color.b * 255), a(color.a * 255) {}


inline Color4F::Color4F() {}

inline Color4F::Color4F(float _r, float _g, float _b, float _a) : Vec4Base(_r, _g, _b, _a) {}

inline Color4F::Color4F(const Color3B& color, float _a)
    : Vec4Base(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, _a)
{}

inline Color4F::Color4F(const Color4B& color)
    : Vec4Base(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f)
{}

}
