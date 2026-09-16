/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/platform/PlatformConfig.h"
#include "axmol/platform/Device.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
#    import <Cocoa/Cocoa.h>
#elif AX_TARGET_PLATFORM == AX_PLATFORM_IOS
#    import <CoreText/CoreText.h>
#    import <UIKit/UIKit.h>
#endif

#include <math.h>
#include <limits>
#include <stdlib.h>
#include <string.h>

#include "axmol/base/Types.h"
#include "axmol/platform/apple/Device-apple.h"

namespace FontUtils
{
NSMutableParagraphStyle* _calculateParagraphStyle(bool, int)
{
    auto* paragraphStyle         = [[NSMutableParagraphStyle alloc] init];
    paragraphStyle.lineBreakMode = NSLineBreakByWordWrapping;
    return paragraphStyle;
}

NSTextAlignment _calculateTextAlignment(ax::Device::TextAlign align)
{
    const auto horizontalAlignment = static_cast<unsigned>(align) & 0x0f;
    return horizontalAlignment == 2   ? NSTextAlignmentRight
           : horizontalAlignment == 3 ? NSTextAlignmentCenter
                                      : NSTextAlignmentLeft;
}

CGFloat _calculateTextDrawStartWidth(ax::Device::TextAlign align, CGSize textSize, CGSize canvasSize)
{
    switch (static_cast<unsigned>(align) & 0x0f)
    {
    case 2:
        return canvasSize.width - textSize.width;
    case 3:
        return (canvasSize.width - textSize.width) / 2.0f;
    default:
        return 0;
    }
}
}  // namespace FontUtils

namespace ax
{
namespace
{
#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
using PlatformFont  = NSFont;
using PlatformColor = NSColor;
#else
using PlatformFont  = UIFont;
using PlatformColor = UIColor;
#endif

struct TextLayout
{
    NSAttributedString* fillText   = nil;
    NSAttributedString* strokeText = nil;
    CGRect textRect                = CGRectZero;
    int bitmapWidth                = 0;
    int bitmapHeight               = 0;
};

static CGFloat maxMeasureDimension()
{
#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    return CGFLOAT_MAX;
#else
    return 10000.0f;
#endif
}

static PlatformFont* createFont(const char* fontName, CGFloat fontSize)
{
    NSString* name = [NSString stringWithUTF8String:fontName];
    name           = [[name lastPathComponent] stringByDeletingPathExtension];

#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    PlatformFont* font = [NSFont fontWithName:name size:fontSize];
    return font ?: [NSFont systemFontOfSize:fontSize];
#else
    PlatformFont* font = [UIFont fontWithName:name size:fontSize];
    return font ?: [UIFont systemFontOfSize:fontSize];
#endif
}

static PlatformFont* resizeFont(PlatformFont* font, CGFloat fontSize)
{
#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    return [[NSFontManager sharedFontManager] convertFont:font toSize:fontSize];
#else
    return [font fontWithSize:fontSize];
#endif
}

static PlatformColor* createColor(const Color32& color)
{
#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    return [NSColor colorWithDeviceRed:color.r / 255.0f
                                 green:color.g / 255.0f
                                  blue:color.b / 255.0f
                                 alpha:color.a / 255.0f];
#else
    return [UIColor colorWithRed:color.r / 255.0f green:color.g / 255.0f blue:color.b / 255.0f alpha:color.a / 255.0f];
#endif
}

static CGFloat calculateTextDrawStartHeight(Device::TextAlign align, CGSize textSize, CGSize canvasSize)
{
    const auto verticalAlignment = (static_cast<unsigned>(align) >> 4) & 0x0f;
#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    if (verticalAlignment == 1)
        return canvasSize.height - textSize.height;
    if (verticalAlignment == 2)
        return 0;
#else
    if (verticalAlignment == 2)
        return canvasSize.height - textSize.height;
    if (verticalAlignment == 1)
        return 0;
#endif
    return verticalAlignment == 3 ? (canvasSize.height - textSize.height) / 2.0f : 0;
}

static NSAttributedString* attributedStringWithFontSize(NSAttributedString* string, CGFloat fontSize)
{
    NSMutableAttributedString* resizedString = [string mutableCopy];
    [resizedString beginEditing];
    [resizedString enumerateAttribute:NSFontAttributeName
                              inRange:NSMakeRange(0, resizedString.length)
                              options:0
                           usingBlock:^(id value, NSRange range, BOOL*) {
                             auto* font = static_cast<PlatformFont*>(value);
                             [resizedString removeAttribute:NSFontAttributeName range:range];
                             [resizedString addAttribute:NSFontAttributeName
                                                   value:resizeFont(font, fontSize)
                                                   range:range];
                           }];
    [resizedString endEditing];
    return [resizedString copy];
}

static CGSize measureText(NSAttributedString* string, CGSize constrainSize, bool enableWrap, int overflow)
{
    CGSize measureSize = CGSizeMake(constrainSize.width > 0 ? constrainSize.width : maxMeasureDimension(),
                                    constrainSize.height > 0 ? constrainSize.height : maxMeasureDimension());
    if (overflow == 1)
    {
        if (!enableWrap)
            measureSize.width = maxMeasureDimension();
        measureSize.height = maxMeasureDimension();
    }

    auto size =
        [string boundingRectWithSize:measureSize options:NSStringDrawingUsesLineFragmentOrigin context:nil].size;
    return CGSizeMake(ceil(size.width), ceil(size.height));
}

static CGSize measureShrunkText(NSAttributedString* string, CGSize constrainSize, bool enableWrap)
{
    if (enableWrap)
    {
        return [string boundingRectWithSize:CGSizeMake(constrainSize.width, maxMeasureDimension())
                                    options:NSStringDrawingUsesLineFragmentOrigin
                                    context:nil]
            .size;
    }

#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS
    CTFramesetterRef framesetter = CTFramesetterCreateWithAttributedString((__bridge CFAttributedStringRef)string);
    if (!framesetter)
        return CGSizeZero;
    const auto size =
        CTFramesetterSuggestFrameSizeWithConstraints(framesetter, CFRangeMake(0, string.length), nullptr,
                                                     CGSizeMake(maxMeasureDimension(), maxMeasureDimension()), nullptr);
    CFRelease(framesetter);
    return size;
#else
    return [string boundingRectWithSize:CGSizeMake(maxMeasureDimension(), maxMeasureDimension())
                                options:NSStringDrawingUsesLineFragmentOrigin
                                context:nil]
        .size;
#endif
}

static CGSize shrinkTextToFit(NSAttributedString* __strong& string,
                              PlatformFont* font,
                              CGSize constrainSize,
                              bool enableWrap)
{
    CGSize actualSize = CGSizeMake(constrainSize.width + 1, constrainSize.height + 1);
    int fontSize      = static_cast<int>(font.pointSize) + 1;

    while (actualSize.width > constrainSize.width || actualSize.height > constrainSize.height)
    {
        if (--fontSize < 0)
            return CGSizeZero;

        string          = attributedStringWithFontSize(string, fontSize);
        const auto size = measureShrunkText(string, constrainSize, enableWrap);
        if (size.width == 0 || size.height == 0)
            continue;

        actualSize = size;
        if (constrainSize.width <= 0)
            constrainSize.width = size.width;
        if (constrainSize.height <= 0)
            constrainSize.height = size.height;
        if (fontSize == 0)
            break;
    }

#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    return actualSize;
#else
    return CGSizeMake(ceil(actualSize.width), ceil(actualSize.height));
#endif
}

static bool prepareTextLayout(std::string_view text,
                              const FontDefinition& definition,
                              Device::TextAlign align,
                              TextLayout& layout)
{
    if (text.empty())
        return false;

    auto* string = [[NSString alloc] initWithBytes:text.data() length:text.size() encoding:NSUTF8StringEncoding];
    if (!string)
        return false;

    auto* font = createFont(definition._fontName.c_str(), definition._fontSize);
    if (!font)
        return false;

    auto* paragraphStyle     = FontUtils::_calculateParagraphStyle(definition._enableWrap, definition._overflow);
    paragraphStyle.alignment = FontUtils::_calculateTextAlignment(align);
    auto* foregroundColor    = createColor(definition._fontFillColor);
    NSDictionary* attributes = @{
        NSForegroundColorAttributeName : foregroundColor,
        NSFontAttributeName : font,
        NSParagraphStyleAttributeName : paragraphStyle
    };
    layout.fillText = [[NSAttributedString alloc] initWithString:string attributes:attributes];

    CGSize canvasSize = CGSizeMake(definition._dimensions.width, definition._dimensions.height);
    CGSize textSize;
    if (definition._overflow == 2)
        textSize = shrinkTextToFit(layout.fillText, font, canvasSize, definition._enableWrap);
    else
        textSize = measureText(layout.fillText, canvasSize, definition._enableWrap, definition._overflow);

    if (textSize.width <= 0 || textSize.height <= 0)
        return false;
    if (canvasSize.width <= 0)
        canvasSize.width = textSize.width;
    if (canvasSize.height <= 0)
        canvasSize.height = textSize.height;

    layout.bitmapWidth  = static_cast<int>(canvasSize.width);
    layout.bitmapHeight = static_cast<int>(canvasSize.height);
    if (layout.bitmapWidth <= 0 || layout.bitmapHeight <= 0)
        return false;

    const auto x = FontUtils::_calculateTextDrawStartWidth(align, textSize, canvasSize);
    const auto y = calculateTextDrawStartHeight(align, textSize, canvasSize);
#if AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    layout.textRect = CGRectMake(x, layout.bitmapHeight - canvasSize.height + y, textSize.width, textSize.height);
#else
    layout.textRect = CGRectMake(x, y, textSize.width, textSize.height);
#endif

    const auto& stroke = definition._stroke;
    if (stroke._strokeEnabled && stroke._strokeSize > 0)
    {
        auto* finalFont = static_cast<PlatformFont*>([layout.fillText attribute:NSFontAttributeName
                                                                        atIndex:0
                                                                 effectiveRange:nil]);
        if (finalFont.pointSize > 0)
        {
            NSDictionary* strokeAttributes = @{
                NSForegroundColorAttributeName : foregroundColor,
                NSFontAttributeName : finalFont,
                NSParagraphStyleAttributeName : paragraphStyle,
                NSStrokeWidthAttributeName : @(stroke._strokeSize / finalFont.pointSize * 100.0f),
                NSStrokeColorAttributeName : createColor(stroke._strokeColor),
            };
            layout.strokeText = [[NSAttributedString alloc] initWithString:string attributes:strokeAttributes];
        }
    }

    return true;
}

static bool calculateBitmapSize(const TextLayout& layout, size_t& bytesPerRow, size_t& dataSize)
{
    if (layout.bitmapWidth <= 0 || layout.bitmapHeight <= 0)
        return false;

    const auto width  = static_cast<size_t>(layout.bitmapWidth);
    const auto height = static_cast<size_t>(layout.bitmapHeight);
    if (width > std::numeric_limits<size_t>::max() / 4)
        return false;

    bytesPerRow = width * 4;
    if (height > std::numeric_limits<size_t>::max() / bytesPerRow)
        return false;

    dataSize = bytesPerRow * height;
    return true;
}

#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS
static Data drawTextLayout(const TextLayout& layout)
{
    size_t bytesPerRow;
    size_t dataSize;
    if (!calculateBitmapSize(layout, bytesPerRow, dataSize))
        return {};

    auto* buffer = static_cast<unsigned char*>(calloc(dataSize, 1));
    if (!buffer)
        return {};

    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    if (!colorSpace)
    {
        free(buffer);
        return {};
    }

    const auto bitmapInfo =
        static_cast<CGBitmapInfo>(kCGImageAlphaPremultipliedLast) | static_cast<CGBitmapInfo>(kCGBitmapByteOrder32Big);
    CGContextRef context =
        CGBitmapContextCreate(buffer, layout.bitmapWidth, layout.bitmapHeight, 8, bytesPerRow, colorSpace, bitmapInfo);
    CGColorSpaceRelease(colorSpace);
    if (!context)
    {
        free(buffer);
        return {};
    }

    CGContextTranslateCTM(context, 0.0f, layout.bitmapHeight);
    CGContextScaleCTM(context, 1.0f, -1.0f);
    CGContextSetShouldSubpixelQuantizeFonts(context, false);
    UIGraphicsPushContext(context);
    CGContextBeginTransparencyLayerWithRect(context, layout.textRect, nullptr);
    if (layout.strokeText)
    {
        CGContextSetTextDrawingMode(context, kCGTextStroke);
        [layout.strokeText drawInRect:layout.textRect];
    }
    CGContextSetTextDrawingMode(context, kCGTextFill);
    [layout.fillText drawInRect:layout.textRect];
    CGContextEndTransparencyLayer(context);
    UIGraphicsPopContext();
    CGContextRelease(context);

    Data data;
    data.fastSet(buffer, static_cast<ssize_t>(dataSize));
    return data;
}
#else
static Data drawTextLayout(const TextLayout& layout)
{
    size_t bytesPerRow;
    size_t dataSize;
    if (!calculateBitmapSize(layout, bytesPerRow, dataSize))
        return {};

    auto* bitmap  = [[NSBitmapImageRep alloc] initWithBitmapDataPlanes:nullptr
                                                           pixelsWide:layout.bitmapWidth
                                                           pixelsHigh:layout.bitmapHeight
                                                        bitsPerSample:8
                                                      samplesPerPixel:4
                                                             hasAlpha:YES
                                                             isPlanar:NO
                                                       colorSpaceName:NSDeviceRGBColorSpace
                                                         bitmapFormat:0
                                                          bytesPerRow:bytesPerRow
                                                         bitsPerPixel:32];
    auto* context = bitmap ? [NSGraphicsContext graphicsContextWithBitmapImageRep:bitmap] : nil;
    if (!context)
        return {};

    [NSGraphicsContext saveGraphicsState];
    [NSGraphicsContext setCurrentContext:context];
    if (layout.strokeText)
        [layout.strokeText drawInRect:layout.textRect];
    [layout.fillText drawInRect:layout.textRect];
    [NSGraphicsContext restoreGraphicsState];

    const auto* source = bitmap.bitmapData;
    auto* buffer       = static_cast<unsigned char*>(malloc(dataSize));
    if (!source || !buffer)
    {
        free(buffer);
        return {};
    }
    memcpy(buffer, source, dataSize);

    Data data;
    data.fastSet(buffer, static_cast<ssize_t>(dataSize));
    return data;
}
#endif
}  // namespace

Data Device::getTextureDataForText(std::string_view text,
                                   const FontDefinition& textDefinition,
                                   TextAlign align,
                                   int& width,
                                   int& height,
                                   bool& hasPremultipliedAlpha)
{
    @autoreleasepool
    {
        TextLayout layout;
        if (!prepareTextLayout(text, textDefinition, align, layout))
            return {};

        auto data = drawTextLayout(layout);
        if (data.isNull())
            return {};

        width                 = layout.bitmapWidth;
        height                = layout.bitmapHeight;
        hasPremultipliedAlpha = true;
        return data;
    }
}
}  // namespace ax
