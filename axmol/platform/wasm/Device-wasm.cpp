/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "axmol/platform/PlatformConfig.h"
#if AX_TARGET_PLATFORM == AX_PLATFORM_WASM

#    include "axmol/platform/Device.h"
#    include "axmol/platform/FileUtils.h"

#    include <emscripten/emscripten.h>
#    include <emscripten/html5.h>

namespace ax
{

int Device::getDPI()
{
    return static_cast<int>(160 * Device::getPixelRatio());
}

float Device::getPixelRatio()
{
    return EM_ASM_DOUBLE(return window.devicePixelRatio);
}

void Device::setAccelerometerEnabled(bool isEnabled)
{
    // TODO: https://emscripten.org/docs/api_reference/html5.h.html?
}

void Device::setAccelerometerInterval(float interval)
{
    // TODO: https://emscripten.org/docs/api_reference/html5.h.html?
}

Data Device::getTextureDataForText(std::string_view text,
                                   const FontDefinition& textDefinition,
                                   TextAlign align,
                                   int& width,
                                   int& height,
                                   bool& hasPremultipliedAlpha)
{
    char color[10];
    fmt::format_to_z(color, "#{:02x}{:02x}{:02x}{:02x}", textDefinition._fontFillColor.r, textDefinition._fontFillColor.g,
            textDefinition._fontFillColor.b, textDefinition._fontFillColor.a);
    // clang-format off
    unsigned char* ptr = (unsigned char*)EM_ASM_PTR({
        var lines = UTF8ToString($0).split("\n");
        var fontName = UTF8ToString($1);
        var fontSize = $2;
        var color = UTF8ToString($3);
        var dimWidth = $4;
        var dimHeight = $5;
        var align = $6;

        // use shared canvas
        var canvas = Module.axmolSharedCanvas = Module.axmolSharedCanvas || document.createElement("canvas");
        var context = canvas.getContext('2d', { willReadFrequently: true });
        context.font = fontSize + "px " + fontName;
        // use alphabetic baseline for text rendering
        context.textBaseline = "alphabetic";

        var linesWidth = [];
        var linesAscent = [];
        var linesDescent = [];
        var totalHeight = 0;
        var maxWidth = dimWidth > 0 ? dimWidth : 0;

        var defaultAscent = 0;
        var defaultDescent = 0;

        var measureDefault = ()=> {
            if (defaultAscent == 0 && defaultDescent == 0) {
                var metrics = context.measureText('M'); // or Hg
                defaultAscent = (typeof metrics.actualBoundingBoxAscent === "number") ? metrics.actualBoundingBoxAscent : fontSize * 0.8;
                defaultDescent = (typeof metrics.actualBoundingBoxDescent === "number") ? metrics.actualBoundingBoxDescent : fontSize * 0.2;
            }
        };

        // compute per line metrics
        for (var i = 0; i < lines.length; i++) {
            var metrics = context.measureText(lines[i]);
            var lineWidth = metrics.width;
            // if browser not support actualBoundingBoxAscent/descent, use fallback values
            var ascent = (typeof metrics.actualBoundingBoxAscent === "number") ? metrics.actualBoundingBoxAscent : fontSize * 0.8;
            var descent = (typeof metrics.actualBoundingBoxDescent === "number") ? metrics.actualBoundingBoxDescent : fontSize * 0.2;
            var lineHeight = ascent + descent;
			// if the line text only contains white space chars, the lineHeight will be 0, 
			// so we re-calculate lineHeight by representative text, i.e. 'M' or 'Hg'
            if (lineHeight == 0) {
                measureDefault();
                ascent = defaultAscent;
                descent = defaultDescent;
                lineHeight = ascent + descent;
            }
            linesWidth.push(lineWidth);
            linesAscent.push(ascent);
            linesDescent.push(descent);
            if (dimWidth <= 0 && lineWidth > maxWidth) {
                maxWidth = lineWidth;
            }
            totalHeight += lineHeight;
        }

        // if dimensions are specified, use them to limit the height
        if (dimHeight > 0) {
            totalHeight = dimHeight;
        }

        var canvasWidth = Math.ceil(maxWidth);
        var canvasHeight = Math.ceil(totalHeight);

        canvas.width = canvasWidth;
        canvas.height = canvasHeight;

        context.clearRect(0, 0, canvasWidth, canvasHeight);
        context.font = fontSize + "px " + fontName;
        context.fillStyle = color;
        context.textBaseline = "alphabetic";

        // vertical top
        var offsetY = 0;
        if ((align & 0xf0) === 0x30) {
            // vertical center align
            offsetY = (canvasHeight - totalHeight) / 2;
        } else if ((align & 0xf0) === 0x20) {
            // bottom align
            offsetY = canvasHeight - totalHeight;
        }

        // draw each line of text
        for (var i = 0; i < lines.length; i++) {
            var lineH = linesAscent[i] + linesDescent[i];
            // horizontal left
            var offsetX = 0;
            if ((align & 0x0f) === 0x03) {
                offsetX = (canvasWidth - linesWidth[i]) / 2;
            } else if ((align & 0x0f) === 0x02) {
                offsetX = canvasWidth - linesWidth[i];
            }
            // use align left by default, offsetX remains 0
            var baselineY = offsetY + linesAscent[i];
            context.fillText(lines[i], offsetX, baselineY);
            offsetY += lineH;
        }

        // get pixel data from the canvas
        var data = context.getImageData(0, 0, canvasWidth, canvasHeight).data;
        var ptr = _malloc(data.byteLength);
        var buffer = new Uint8Array(Module.HEAPU8.buffer, ptr, data.byteLength);
        buffer.set(data);
        return ptr;
    }, text.data(), textDefinition._fontName.c_str(), textDefinition._fontSize, color, textDefinition._dimensions.width, textDefinition._dimensions.height, align);

    width = EM_ASM_INT({
        return Module.axmolSharedCanvas.width;
    });
    height = EM_ASM_INT({
        return Module.axmolSharedCanvas.height;
    });
    // clang-format on

    hasPremultipliedAlpha = false;

    Data ret;
    ret.fastSet(ptr, width * height * 4);
    return ret;
}

void Device::setKeepScreenOn(bool value)
{
    // Impossible in browser
}

void Device::vibrate(float duration)
{
    emscripten_vibrate(duration * 1000);
}

void Device::prepareImpactFeedbackGenerator(ImpactFeedbackStyle style) {}

void Device::impactOccurred(ImpactFeedbackStyle style) {}

void Device::prepareNotificationFeedbackGenerator() {}

void Device::notificationOccurred(NotificationFeedbackType type) {}

void Device::prepareSelectionFeedbackGenerator() {}

void Device::selectionChanged() {}

}  // namespace ax

#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_WASM
