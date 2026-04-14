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

#include <d2d1.h>
#include <dwrite.h>
#include <wincodec.h>
#include <string_view>
#include <vector>
#include "axmol/math/Color.h"
#include "axmol/tlx/byte_buffer.hpp"

namespace ax
{

/**
 * Internal helper class for rendering text using DirectWrite and Direct2D.
 */
class DWriteTextRenderer
{
public:
    static DWriteTextRenderer& sharedTextRenderer();

    DWriteTextRenderer();
    ~DWriteTextRenderer();

    /**
     * @brief Render UTF-8 text into a WIC bitmap and return its pixel data.
     *
     * This function uses DirectWrite/Direct2D to draw text into a WIC bitmap,
     * applying font settings, alignment, wrapping, overflow rules, stroke and shadow effects.
     * The rendered bitmap is then copied into the provided output buffer in 32-bit BGRA format
     * with premultiplied alpha.
     *
     * @param text        UTF-8 encoded text string to render. Line breaks ('\n') are supported.
     * @param textDefinition FontDefinition structure specifying font family, size, color,
     *                       stroke and shadow options, wrapping and overflow behavior.
     * @param align       Alignment flags (compatible with BitmapDC):
     *                    - Horizontal: bits 0–3 (1 = left, 2 = right, 3 = center).
     *                    - Vertical: bits 4–7 (1 = top, 2 = bottom, 3 = middle).
     * @param premultipliedAlpha Output flag indicating whether the pixel data uses premultiplied alpha.
     * @param extent      Input: maximum width/height constraints.
     *                    Output: actual rendered text size in pixels.
     * @param outData     Output buffer containing raw pixel data (BGRA, premultiplied alpha).
     *
     *
     * @note '&' characters are escaped by doubling ('&&') to avoid mnemonic interpretation.
     * @note If overflow == 2, the font size may be reduced iteratively until the text fits.
     * @note When stroke is enabled, glyph outlines are converted into geometry and drawn with
     *       both stroke and fill.
     * @note Alignment is handled by DirectWrite layout; origin is adjusted only for bounding box placement.
     */
    bool drawText(std::string_view text,
                  const FontDefinition& textDefinition,
                  unsigned int align,
                  bool& premultipliedAlpha,
                  SIZE& extent,
                  tlx::byte_buffer& outData);

private:
    bool initialize();
    bool setFont(std::wstring_view fontName, int nSize = 0, bool enableBold = false);
    bool createRenderTarget(int width, int height, const FontDefinition& textDefinition, bool& premultipliedAlpha);
    void releaseRenderTarget();

    // COM interfaces
    IDWriteFactory* _dwriteFactory  = nullptr;
    ID2D1Factory* _d2dFactory       = nullptr;
    IWICImagingFactory* _wicFactory = nullptr;

    // Current font resources
    IDWriteTextFormat* _textFormat = nullptr;
    float _fontSize                = 16.0f;
    bool _bold                     = false;
    std::wstring _fontName;

    // Render target resources
    ID2D1RenderTarget* _renderTarget = nullptr;
    ID2D1SolidColorBrush* _textBrush = nullptr;
    IWICBitmap* _wicBitmap           = nullptr;
    int _bitmapWidth                 = 0;
    int _bitmapHeight                = 0;

    bool _initialized = false;
};

}  // namespace ax
