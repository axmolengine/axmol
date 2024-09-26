#include "FUILabel.h"
#include "BitmapFont.h"
#include "UIConfig.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
using namespace ax;

static Color3B toGrayed(const Color3B& source)
{
    Color3B c = source;
    c.r = c.g = c.b = c.r * 0.299f + c.g * 0.587f + c.b * 0.114f;
    return c;
}

FUILabel::FUILabel() : _fontSize(-1),
_bmFontCanTint(false),
_textFormat(new TextFormat()),
_grayed(false)
{
}

FUILabel::~FUILabel()
{
    delete _textFormat;
}

void FUILabel::setText(std::string_view value)
{
    if (_fontSize < 0)
        applyTextFormat();

    setString(value);
}

void FUILabel::applyTextFormat()
{
    if (_fontSize < 0 /**first time**/ || _fontName != _textFormat->face)
    {
        _fontName = _textFormat->face;
        Label::LabelType oldType = _currentLabelType;

        if (_fontName.find("ui://") != -1)
        {
            this->setBMFontFilePath(_fontName);
        }
        else
        {
            bool ttf = false;
            const std::string& fontName = UIConfig::getRealFontName(_fontName, &ttf);
            if (ttf)
            {
                _fontConfig.fontFilePath = fontName;
                _fontConfig.fontSize = _textFormat->fontSize;
                setTTFConfig(_fontConfig);
            }
            else
            {
                setSystemFontName(fontName);
            }

            if (oldType == LabelType::BMFONT)
                setTextColor((Color4B)_textFormat->color);
        }
    }

    if (_fontSize != _textFormat->fontSize)
    {
        _fontSize = _textFormat->fontSize;
        if (_currentLabelType == LabelType::STRING_TEXTURE)
        {
            setSystemFontSize(_fontSize);
        }
        else if (_currentLabelType == LabelType::BMFONT)
        {
            BitmapFont* bmFont = (BitmapFont*)UIPackage::getItemAssetByURL(_bmFontPath, PackageItemType::FONT);
            if (bmFont && bmFont->isResizable())
                setBMFontSize(_fontSize);
        }
        else
        {
            _fontConfig.fontSize = _fontSize;
            setTTFConfig(_fontConfig);
        }
    }

    if (_currentLabelType != LabelType::BMFONT || _bmFontCanTint)
    {
        //setTextColor((Color4B)(_grayed ? toGrayed(_textFormat->color) : _textFormat->color));
        setColor(_grayed ? toGrayed(_textFormat->color) : _textFormat->color);
    }

    if (_textFormat->underline)
        enableUnderline();
    else
        disableEffect(LabelEffect::UNDERLINE);

    if (_textFormat->italics)
        enableItalics();
    //else //Cant call this, cocos will do setRotationSkew(0)!
    //    disableEffect(LabelEffect::ITALICS);

    if (_textFormat->bold && _currentLabelType != LabelType::STRING_TEXTURE)
        enableBold();
    else
        disableEffect(LabelEffect::BOLD);

    setLineSpacing(_textFormat->lineSpacing);
    setHorizontalAlignment(_textFormat->align);
    setVerticalAlignment(_textFormat->verticalAlign);

    if (_textFormat->hasEffect(TextFormat::OUTLINE))
        enableOutline((Color4B)(_grayed ? toGrayed(_textFormat->outlineColor) : _textFormat->outlineColor), _textFormat->outlineSize);
    else
        disableEffect(LabelEffect::OUTLINE);

    if (_textFormat->hasEffect(TextFormat::SHADOW))
        enableShadow((Color4B)(_grayed ? toGrayed(_textFormat->shadowColor) : _textFormat->shadowColor), _textFormat->shadowOffset);
    else if (!_textFormat->bold)
        disableEffect(LabelEffect::SHADOW);
}

#if defined(AX_VERSION)
bool FUILabel::setBMFontFilePath(std::string_view bmfontFilePath,
                                 const Rect& imageRect,
                                 bool imageRotated,
                                 float fontSize)
#else
bool FUILabel::setBMFontFilePath(std::string_view bmfontFilePath, const Vec2& imageOffset, float fontSize)
#endif
{
    BitmapFont* bmFont = (BitmapFont*)UIPackage::getItemAssetByURL(bmfontFilePath, PackageItemType::FONT);
    if (bmFont == nullptr)
    {
        reset();
        return false;
    }

    //assign the default fontSize
    if (std::abs(fontSize) < FLT_EPSILON)
    {
        float originalFontSize = bmFont->getOriginalFontSize();
        _bmFontSize = originalFontSize / AX_CONTENT_SCALE_FACTOR();
    }

    if (fontSize > 0.0f && (bmFont->isResizable() || getOverflow() >= Label::Overflow::SHRINK))
    {
        _bmFontSize = fontSize;
    }

    _bmFontPath = bmfontFilePath;
    _bmFontCanTint = bmFont->canTint();

    _currentLabelType = LabelType::BMFONT;
    setFontAtlas(bmFont->getFontAtlas());

    return true;
}

void FUILabel::setGrayed(bool value)
{
    if (_grayed != value)
    {
        _grayed = value;

        if (_currentLabelType != LabelType::BMFONT)
            setTextColor((Color4B)(_grayed ? toGrayed(_textFormat->color) : _textFormat->color));
        else if (_bmFontCanTint)
            setColor(_grayed ? toGrayed(_textFormat->color) : _textFormat->color);

        if (_textFormat->hasEffect(TextFormat::OUTLINE))
            enableOutline((Color4B)(_grayed ? toGrayed(_textFormat->outlineColor) : _textFormat->outlineColor), _textFormat->outlineSize);

        if (_textFormat->hasEffect(TextFormat::SHADOW))
            enableShadow((Color4B)(_grayed ? toGrayed(_textFormat->shadowColor) : _textFormat->shadowColor), _textFormat->shadowOffset);
    }
}

void FUILabel::updateFontScale()
{
    auto font = _fontAtlas->getFont();
    if (_currentLabelType == LabelType::BMFONT)
    {
        BitmapFont* bmFont = (BitmapFont*)font;
        float originalFontSize = bmFont->getOriginalFontSize();
        _fontScale             = _bmFontSize * AX_CONTENT_SCALE_FACTOR() / originalFontSize;
    }
    else
    {
        ax::Label::updateFontScale();
    }
}

void FUILabel::setUnderlineColor(const ax::Color3B& value)
{
    //NOT IMPLEMENTED
}

NS_FGUI_END
