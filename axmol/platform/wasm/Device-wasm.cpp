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
#    include "GLFW/glfw3.h"

#    include <map>
#    include <atomic>

namespace ax
{

// clang-format off

// Global pointer just to bridge the active callback
static std::map<uint64_t, std::function<void(std::string_view)>> g_clipboard_callbacks;
static std::atomic<uint64_t> g_next_clipboard_id{1};

extern "C" EMSCRIPTEN_KEEPALIVE void paste_bridge(uint64_t id, const char* text) {
    auto it = g_clipboard_callbacks.find(id);
    if (it != g_clipboard_callbacks.end()) {
        it->second(text ? text : "");
        g_clipboard_callbacks.erase(it);
    }
}

void Device::getClipboardText(std::function<void(std::string_view)> callback)
{
    if (!callback)
        return;

    // Save the callback pointer globally for the duration of the async event
    uint64_t id = g_next_clipboard_id++;
    g_clipboard_callbacks[id] = std::move(callback);

    EM_ASM({
        var handlerId = $0;
        navigator.clipboard.readText()
            .then(function(text) {
                // Pass directly to WASM using the runtime helper
                ccall('paste_bridge', null, ['uint64', 'string'], [handlerId, text]);
            })
            .catch(function(err) {
                ccall('paste_bridge', null, ['uint64', 'string'], [handlerId, ""]);
            });
    }, id);
}

void Device::setClipboardText(std::string_view text)
{
    EM_ASM_({
        const str = UTF8ToString($0, $1);
        navigator.clipboard.writeText(str).catch(err => {
            console.error("Clipboard write failed:", err);
        });
    }, text.data(), text.size());
}

void Device::clearClipboard()
{
    EM_ASM({
        navigator.clipboard.writeText("").catch(err => {
            console.error("Clipboard clear failed:", err);
        });
    });
}
// clang-format on

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
    char colorHex[10] = {0};
    auto& fillColor   = textDefinition._fontFillColor;
    fmt::format_to_z(colorHex, "#{:02x}{:02x}{:02x}{:02x}", fillColor.r, fillColor.g, fillColor.b, fillColor.a);

    char strokeColorHex[10] = {0};
    if (textDefinition._stroke._strokeEnabled)
    {
        auto& strokeColor = textDefinition._stroke._strokeColor;
        fmt::format_to_z(strokeColorHex, "#{:02x}{:02x}{:02x}{:02x}", strokeColor.r, strokeColor.g, strokeColor.b,
                         strokeColor.a);
    }

    // clang-format off
    unsigned char* ptr = (unsigned char*)EM_ASM_PTR({
        var lines = UTF8ToString($0).split("\n");
        var fontName = UTF8ToString($1);
        var finalFontSize = $2;
        var color = UTF8ToString($3);
        var dimWidth = $4;
        var dimHeight = $5;
        var align = $6;
        var strokeEnabled = $7;
        var strokeSize = $8;
        var strokeColor = UTF8ToString($9);
        var overflow = $10;

        // use axmol offscreen canvas to render text
        var canvas = Module.axmol_offscreen_canvas = Module.axmol_offscreen_canvas || document.createElement("canvas");
        var context = canvas.getContext('2d', { willReadFrequently: true });
        // use alphabetic baseline for text rendering
        context.textBaseline = "alphabetic";

        var linesWidth = [];
        var linesAscent = [];
        var linesDescent = [];
        var textWidth = 0;
        var textHeight = 0;
        var defaultAscent = 0;
        var defaultDescent = 0;

        var resetMetrics = ()=> {
            linesWidth.length = 0;
            linesAscent.length = 0;
            linesDescent.length = 0;
            textWidth = 0;
            textHeight = 0;
            defaultAscent = 0;
            defaultDescent = 0;
        };

        var measureLayout = (fontSize, shouldCollectMetrics) => {
            context.font = fontSize + "px " + fontName;

            let metrics = context.measureText('M'); // or Hg
            defaultAscent = (typeof metrics.actualBoundingBoxAscent === "number") ? metrics.actualBoundingBoxAscent : fontSize * 0.8;
            defaultDescent = (typeof metrics.actualBoundingBoxDescent === "number") ? metrics.actualBoundingBoxDescent : fontSize * 0.2;

            // compute per line metrics
            for (var i = 0; i < lines.length; i++) {
                metrics = context.measureText(lines[i]);
                let lineWidth = metrics.width;
                // if browser not support actualBoundingBoxAscent/descent, use fallback values
                let ascent = (typeof metrics.actualBoundingBoxAscent === "number") ? metrics.actualBoundingBoxAscent : fontSize * 0.8;
                let descent = (typeof metrics.actualBoundingBoxDescent === "number") ? metrics.actualBoundingBoxDescent : fontSize * 0.2;
                let lineHeight = ascent + descent;
                // if the line text only contains white space chars, the lineHeight will be 0,
                // so we re-calculate lineHeight by representative text, i.e. 'M' or 'Hg'
                if (lineHeight == 0) {
                    ascent = defaultAscent;
                    descent = defaultDescent;
                    lineHeight = ascent + descent;
                }
                if (shouldCollectMetrics) {
                    linesWidth.push(lineWidth);
                    linesAscent.push(ascent);
                    linesDescent.push(descent);
                }
                if (lineWidth > textWidth) {
                    textWidth = lineWidth;
                }
                textHeight += lineHeight;
            }
        };

        // SHRINK mode: binary search for best fitting font size
        if (overflow === 2 && dimWidth > 0 && dimHeight > 0) {
            let low = 1;
            let high = finalFontSize;
            let bestFit = finalFontSize;
            while (low <= high) {
                let mid = Math.floor((low + high) / 2);
                measureLayout(mid, false);
                if (textWidth <= dimWidth && textHeight <= dimHeight) {
                    bestFit = mid;
                    low = mid + 1;
                } else {
                    high = mid - 1;
                }
                resetMetrics();
            }
            finalFontSize = bestFit;
        }

        measureLayout(finalFontSize, true);

        var renderWidth = dimWidth > 0 ? Math.ceil(dimWidth) : Math.ceil(textWidth);
        var renderHeight = dimHeight > 0 ? Math.ceil(dimHeight) : Math.ceil(textHeight);

        canvas.width = renderWidth;
        canvas.height = renderHeight;

        context.clearRect(0, 0, renderWidth, renderHeight);

        // after setting canvas width/height, the context state will be reset, so we need to set them again
        context.font = finalFontSize + "px " + fontName;
        context.fillStyle = color;
        context.textBaseline = "alphabetic";

        strokeEnabled = strokeEnabled && strokeSize > 0;
        if (strokeEnabled) {
            context.lineWidth = strokeSize;
            context.strokeStyle = strokeColor;
        }

        // vertical top
        var offsetY = 0;
        if ((align & 0xf0) === 0x30) {
            // vertical center align
            offsetY = (renderHeight - textHeight) / 2;
        } else if ((align & 0xf0) === 0x20) {
            // bottom align
            offsetY = renderHeight - textHeight;
        }

        // draw each line of text
        for (var i = 0; i < lines.length; i++) {
            var lineH = linesAscent[i] + linesDescent[i];
            // horizontal left
            var offsetX = 0;
            if ((align & 0x0f) === 0x03) {
                offsetX = (renderWidth - linesWidth[i]) / 2;
            } else if ((align & 0x0f) === 0x02) {
                offsetX = renderWidth - linesWidth[i];
            }
            // use align left by default, offsetX remains 0
            var baselineY = offsetY + linesAscent[i];
            if (strokeEnabled) {
                context.strokeText(lines[i], offsetX, baselineY);
            }
            context.fillText(lines[i], offsetX, baselineY);
            offsetY += lineH;
        }

        // get pixel data from the canvas
        var data = context.getImageData(0, 0, renderWidth, renderHeight).data;
        var ptr = _malloc(data.byteLength);
        var buffer = new Uint8Array(Module.HEAPU8.buffer, ptr, data.byteLength);
        buffer.set(data);
        return ptr;
    }, text.data(), textDefinition._fontName.c_str(), textDefinition._fontSize, colorHex,
       textDefinition._dimensions.width, textDefinition._dimensions.height, align,
       textDefinition._stroke._strokeEnabled, textDefinition._stroke._strokeSize, strokeColorHex,
       textDefinition._overflow);

    width = EM_ASM_INT({
        return Module.axmol_offscreen_canvas.width;
    });
    height = EM_ASM_INT({
        return Module.axmol_offscreen_canvas.height;
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
