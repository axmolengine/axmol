#include "TextFormat.h"

NS_FGUI_BEGIN
using namespace ax;

TextFormat::TextFormat() :
    fontSize(12),
    color(Color3B::BLACK),
    bold(false),
    italics(false),
    underline(false),
    lineSpacing(3),
    letterSpacing(0),
    align(TextHAlignment::LEFT),
    verticalAlign(TextVAlignment::TOP),
    effect(0),
    outlineSize(1),
    shadowBlurRadius(0),
    _hasColor(false)
{
}

TextFormat::TextFormat(const TextFormat & other)
{
    *this = other;
}

TextFormat & TextFormat::operator=(const TextFormat & other)
{
    if (this != &other)
    {
        face = other.face;
        fontSize = other.fontSize;
        color = other.color;
        bold = other.bold;
        italics = other.italics;
        underline = other.underline;
        lineSpacing = other.lineSpacing;
        letterSpacing = other.letterSpacing;
        align = other.align;
        verticalAlign = other.verticalAlign;
        effect = other.effect;
        outlineColor = other.outlineColor;
        outlineSize = other.outlineSize;
        shadowColor = other.shadowColor;
        shadowOffset = other.shadowOffset;
        shadowBlurRadius = other.shadowBlurRadius;
        glowColor = other.glowColor;
        _hasColor = other._hasColor;
    }
    return *this;
}

void TextFormat::setFormat(const TextFormat & format)
{
    *this = format;
}

NS_FGUI_END