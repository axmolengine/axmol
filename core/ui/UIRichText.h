/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

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

#include "ui/UIWidget.h"
#include "ui/GUIExport.h"
#include "base/Value.h"

NS_AX_BEGIN
/**
 * @addtogroup ui
 * @{
 */

class Label;

namespace ui
{

/**
 *@brief Rich text element base class.
 * It defines the basic common properties for all rich text element.
 */
class AX_GUI_DLL RichElement : public Object
{
public:
    /**
     *@brief Rich element type.
     */
    enum class Type
    {
        TEXT,   /*!< RichElementText */
        IMAGE,  /*!< RichElementImage */
        CUSTOM, /*!< RichElementCustomNode */
        NEWLINE /*!< RichElementNewLine */
    };

    /**
     * @brief Initialize a rich element with different arguments.
     *
     * @param tag A integer tag value.
     * @param color A color in @see `Color3B`.
     * @param opacity A opacity value in `GLubyte`.
     * @return True if initialize success, false otherwise.
     */
    bool init(int tag, const Color3B& color, uint8_t opacity);

    bool equalType(Type type);
    void setColor(const Color3B& color);

protected:
    Type _type{};       /*!< Rich element type. */
    int _tag{};         /*!< A integer tag value. */
    Color3B _color{};   /*!< A color in `Color3B`. */
    uint8_t _opacity{}; /*!< A opacity value in `GLubyte`. */
    friend class RichText;
};

/**
 *@brief Rich element for displaying text.
 */
class AX_GUI_DLL RichElementText : public RichElement
{
public:
    /**
     *@brief Default constructor.
     * @js ctor
     * @lua new
     */
    RichElementText() : _fontSize(0), _flags(0), _outlineSize(0), _shadowBlurRadius(0) { _type = Type::TEXT; }

    enum
    {
        ITALICS_FLAG       = 1 << 0, /*!< italic text */
        BOLD_FLAG          = 1 << 1, /*!< bold text */
        UNDERLINE_FLAG     = 1 << 2, /*!< underline */
        STRIKETHROUGH_FLAG = 1 << 3, /*!< strikethrough */
        URL_FLAG           = 1 << 4, /*!< url of anchor */
        OUTLINE_FLAG       = 1 << 5, /*!< outline effect */
        SHADOW_FLAG        = 1 << 6, /*!< shadow effect */
        GLOW_FLAG          = 1 << 7  /*!< glow effect */
    };

    /**
     * @brief Initialize a RichElementText with various arguments.
     *
     * @param tag A integer tag value.
     * @param color A color in Color3B.
     * @param opacity A opacity in GLubyte.
     * @param text Content string.
     * @param fontName Content font name.
     * @param fontSize Content font size.
     * @param flags italics, bold, underline, strikethrough, url, outline, shadow or glow
     * @param url uniform resource locator
     * @param outlineColor the color of the outline
     * @param outlineSize the outline effect size value
     * @param shadowColor the shadow effect color value
     * @param shadowOffset shadow effect offset value
     * @param shadowBlurRadius the shadow effect blur radius
     * @param glowColor glow color
     * @param id ID of element
     * @return True if initialize success, false otherwise.
     */
    bool init(int tag,
              const Color3B& color,
              uint8_t opacity,
              std::string_view text,
              std::string_view fontName,
              float fontSize,
              uint32_t flags,
              std::string_view url,
              const Color3B& outlineColor = Color3B::WHITE,
              int outlineSize             = -1,
              const Color3B& shadowColor  = Color3B::BLACK,
              const Vec2& shadowOffset    = Vec2(2.0, -2.0),
              int shadowBlurRadius        = 0,
              const Color3B& glowColor    = Color3B::WHITE,
              std::string_view id         = ""sv);

    /**
     * @brief Create a RichElementText with various arguments.
     *
     * @param tag A integer tag value.
     * @param color A color in Color3B.
     * @param opacity A opacity in GLubyte.
     * @param text Content string.
     * @param fontName Content font name.
     * @param fontSize Content font size.
     * @param flags italics, bold, underline, strikethrough, url, outline, shadow or glow
     * @param url uniform resource locator
     * @param outlineColor the color of the outline
     * @param outlineSize the outline effect size value
     * @param shadowColor the shadow effect color value
     * @param shadowOffset shadow effect offset value
     * @param shadowBlurRadius the shadow effect blur radius
     * @param glowColor glow color
     * @param id ID of element
     * @return RichElementText instance.
     */
    static RichElementText* create(int tag,
                                   const Color3B& color,
                                   uint8_t opacity,
                                   std::string_view text,
                                   std::string_view fontName,
                                   float fontSize,
                                   uint32_t flags              = 0,
                                   std::string_view url        = "",
                                   const Color3B& outlineColor = Color3B::WHITE,
                                   int outlineSize             = -1,
                                   const Color3B& shadowColor  = Color3B::BLACK,
                                   const Vec2& shadowOffset    = Vec2(2.0, -2.0),
                                   int shadowBlurRadius        = 0,
                                   const Color3B& glowColor    = Color3B::WHITE,
                                   std::string_view id         = ""sv);

protected:
    std::string _text;
    std::string _fontName;
    float _fontSize;
    uint32_t _flags;
    std::string _url;
    Color3B _outlineColor; /*!< the color of the outline */
    int _outlineSize;      /*!< the outline effect size value */
    Color3B _shadowColor;  /*!< the shadow effect color value */
    Vec2 _shadowOffset;    /*!< shadow effect offset value */
    int _shadowBlurRadius; /*!< the shadow effect blur radius */
    Color3B _glowColor;    /*!< attributes of glow tag */
    std::string _id;       /*!< ID of this text field */
    friend class RichText;
};

/**
 *@brief Rich element for displaying images.
 */
class AX_GUI_DLL RichElementImage : public RichElement
{
public:
    /**
     * @brief Default constructor.
     * @js ctor
     * @lua new
     *
     */
    RichElementImage(): _textureType(), _width(0), _height(0), _scaleX(0), _scaleY(0) { _type = Type::IMAGE; }

    /**
     * @brief Initialize a RichElementImage with various arguments.
     *
     * @param tag A integer tag value.
     * @param color A color in Color3B.
     * @param opacity A opacity in GLubyte.
     * @param filePath A image file name.
     * @param url uniform resource locator
     * @param texType texture type, may be a valid file path, or a sprite frame name
     * @param id ID of element
     * @return True if initialize success, false otherwise.
     */
    bool init(int tag,
              const Color3B& color,
              uint8_t opacity,
              std::string_view filePath,
              std::string_view url           = "",
              Widget::TextureResType texType = Widget::TextureResType::LOCAL,
              std::string_view id            = ""sv);

    /**
     * @brief Create a RichElementImage with various arguments.
     *
     * @param tag A integer tag value.
     * @param color A color in Color3B.
     * @param opacity A opacity in GLubyte.
     * @param filePath A image file name.
     * @param url uniform resource locator
     * @param texType texture type, may be a valid file path, or a sprite frame name
     * @param id ID of element
     * @return A RichElementImage instance.
     */
    static RichElementImage* create(int tag,
                                    const Color3B& color,
                                    uint8_t opacity,
                                    std::string_view filePath,
                                    std::string_view url           = "",
                                    Widget::TextureResType texType = Widget::TextureResType::LOCAL,
                                    std::string_view id            = ""sv);

    void setWidth(int width);
    void setHeight(int height);
    void setScale(float scale) { _scaleX = _scaleY = scale; }
    void setScaleX(float scaleX) { _scaleX = scaleX; }
    void setScaleY(float scaleY) { _scaleY = scaleY; }
    void setUrl(std::string_view url);
    void setId(std::string_view id);

protected:
    std::string _filePath;
    Rect _textureRect;
    Widget::TextureResType _textureType;
    friend class RichText;
    int _width;
    int _height;
    float _scaleX;
    float _scaleY;
    std::string _url; /*!< attributes of anchor tag */
    std::string _id;  /*!< attributes of anchor tag */
};

/**
 *@brief Rich element for displaying custom node type.
 */
class AX_GUI_DLL RichElementCustomNode : public RichElement
{
public:
    /**
     * @brief Default constructor.
     * @js ctor
     * @lua new
     */
    RichElementCustomNode() { _type = Type::CUSTOM; }

    /**
     * @brief Default destructor.
     * @js NA
     * @lua NA
     */
    ~RichElementCustomNode() override { AX_SAFE_RELEASE(_customNode); }

    /**
     * @brief Initialize a RichElementCustomNode with various arguments.
     *
     * @param tag A integer tag value.
     * @param color A color in Color3B.
     * @param opacity A opacity in GLubyte.
     * @param customNode A custom node pointer.
     * @param id ID of element
     * @return True if initialize success, false otherwise.
     */
    bool init(int tag, const Color3B& color, uint8_t opacity, Node* customNode, std::string_view id = ""sv);

    /**
     * @brief Create a RichElementCustomNode with various arguments.
     *
     * @param tag A integer tag value.
     * @param color A color in Color3B.
     * @param opacity A opacity in GLubyte.
     * @param customNode A custom node pointer.
     * @param id ID of element
     * @return A RichElementCustomNode instance.
     */
    static RichElementCustomNode* create(int tag,
                                         const Color3B& color,
                                         uint8_t opacity,
                                         Node* customNode,
                                         std::string_view id = ""sv);

protected:
    Node* _customNode{};
    std::string _id;

    friend class RichText;
};

/**
 *@brief Rich element for new line.
 */
class AX_GUI_DLL RichElementNewLine : public RichElement
{
public:
    /**
     * @brief Default constructor.
     * @js ctor
     * @lua new
     *
     */
    RichElementNewLine(int quantity = 1) : _quantity(quantity) { _type = Type::NEWLINE; }

    /**
     * @brief Default destructor.
     * @js NA
     * @lua NA
     */
    ~RichElementNewLine() override = default;

    /**
     * @brief Create a RichElementNewLine with various arguments.
     *
     * @param tag A integer tag value.
     * @param color A color in Color3B.
     * @param opacity A opacity in GLubyte.
     * @return A RichElementNewLine instance.
     */
    static RichElementNewLine* create(int tag, const Color3B& color, uint8_t opacity);

    /**
     * @brief Create a RichElementNewLine with various arguments.
     *
     * @param tag A integer tag value.
     * @param quantity Number of new lines to add
     * @param color A color in Color3B.
     * @param opacity A opacity in GLubyte.
     * @return A RichElementNewLine instance.
     */
    static RichElementNewLine* create(int tag, int quantity, const Color3B& color, uint8_t opacity);

protected:
    friend class RichText;
    int _quantity;
};

/**
 *@brief A container for displaying various RichElements.
 * We could use it to display texts with images easily.
 */
class AX_GUI_DLL RichText : public Widget
{
public:
    enum WrapMode
    {
        WRAP_PER_WORD,
        WRAP_PER_CHAR
    };

    enum class HorizontalAlignment
    {
        LEFT,
        CENTER,
        RIGHT,
    };

    enum class VerticalAlignment
    {
        TOP,
        CENTER,
        BOTTOM
    };

    /**
     * @brief call to open a resource specified by a URL
     * @param url a URL
     */
    typedef std::function<void(std::string_view url)> OpenUrlHandler;

    /**
     * @brief called on the specified tag
     * @param tagAttrValueMap the attributes of a tag
     * @result text attributes and RichElement
     */
    typedef std::function<std::pair<ValueMap, RichElement*>(const ValueMap& tagAttrValueMap)> VisitEnterHandler;
    typedef std::function<RichElement*()> VisitExitHandler;

    static const std::string KEY_VERTICAL_SPACE;                   /*!< key of vertical space */
    static const std::string KEY_WRAP_MODE;                        /*!< key of per word, or per char */
    static const std::string KEY_HORIZONTAL_ALIGNMENT;             /*!< key of left, right, or center */
    static const std::string KEY_VERTICAL_ALIGNMENT;               /*!< key of left, right, or center */
    static const std::string KEY_FONT_COLOR_STRING;                /*!< key of font color */
    static const std::string KEY_FONT_SIZE;                        /*!< key of font size */
    static const std::string KEY_FONT_SMALL;                       /*!< key of font size small */
    static const std::string KEY_FONT_BIG;                         /*!< key of font size big */
    static const std::string KEY_FONT_FACE;                        /*!< key of font name */
    static const std::string KEY_TEXT_BOLD;                        /*!< key of text bold */
    static const std::string KEY_TEXT_ITALIC;                      /*!< key of text italic */
    static const std::string KEY_TEXT_LINE;                        /*!< key of line style */
    static const std::string VALUE_TEXT_LINE_NONE;                 /*!< value of none */
    static const std::string VALUE_TEXT_LINE_DEL;                  /*!< value of strikethrough line */
    static const std::string VALUE_TEXT_LINE_UNDER;                /*!< value of underline */
    static const std::string KEY_TEXT_STYLE;                       /*!< key of effect style */
    static const std::string VALUE_TEXT_STYLE_NONE;                /*!< value of none */
    static const std::string VALUE_TEXT_STYLE_OUTLINE;             /*!< value of outline */
    static const std::string VALUE_TEXT_STYLE_SHADOW;              /*!< value of shadow */
    static const std::string VALUE_TEXT_STYLE_GLOW;                /*!< value of glow */
    static const std::string KEY_TEXT_OUTLINE_COLOR;               /*!< key of outline color */
    static const std::string KEY_TEXT_OUTLINE_SIZE;                /*!< key of outline size */
    static const std::string KEY_TEXT_SHADOW_COLOR;                /*!< key of shadow color */
    static const std::string KEY_TEXT_SHADOW_OFFSET_WIDTH;         /*!< key of shadow offset (width) */
    static const std::string KEY_TEXT_SHADOW_OFFSET_HEIGHT;        /*!< key of shadow offset (height) */
    static const std::string KEY_TEXT_SHADOW_BLUR_RADIUS;          /*!< key of shadow blur radius */
    static const std::string KEY_TEXT_GLOW_COLOR;                  /*!< key of glow color */
    static const std::string KEY_URL;                              /*!< key of url */
    static const std::string KEY_ANCHOR_FONT_COLOR_STRING;         /*!< key of font color of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_BOLD;                 /*!< key of text bold of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_ITALIC;               /*!< key of text italic of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_LINE;                 /*!< key of line style of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_STYLE;                /*!< key of effect style of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_OUTLINE_COLOR;        /*!< key of outline color of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_OUTLINE_SIZE;         /*!< key of outline size of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_SHADOW_COLOR;         /*!< key of shadow color of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_SHADOW_OFFSET_WIDTH;  /*!< key of shadow offset (width) of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_SHADOW_OFFSET_HEIGHT; /*!< key of shadow offset (height) of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_SHADOW_BLUR_RADIUS;   /*!< key of shadow blur radius of anchor tag */
    static const std::string KEY_ANCHOR_TEXT_GLOW_COLOR;           /*!< key of glow color of anchor tag */
    static const std::string KEY_ID;                               /*!< key of id */

    /**
     * @brief Default constructor.
     * @js ctor
     * @lua new
     */
    RichText();

    /**
     * @brief Default destructor.
     * @js NA
     * @lua NA
     */
    ~RichText() override;

    /**
     * @brief Create a empty RichText.
     *
     * @return RichText instance.
     */
    static RichText* create();

    /**
     * @brief Create a RichText from an XML
     *
     * @return RichText instance.
     */
    static RichText* createWithXML(std::string_view xml,
                                   const ValueMap& defaults            = ValueMap(),
                                   const OpenUrlHandler& handleOpenUrl = nullptr);

    /**
     * @brief Insert a RichElement at a given index.
     *
     * @param element A RichElement type.
     * @param index A given index.
     */
    void insertElement(RichElement* element, int index);

    /**
     * @brief Add a RichElement at the end of RichText.
     *
     * @param element A RichElement instance.
     */
    void pushBackElement(RichElement* element);

    /**
     * @brief Remove a RichElement at a given index.
     *
     * @param index A integer index value.
     */
    void removeElement(int index);

    /**
     * @brief Remove specific RichElement.
     *
     * @param element A RichElement type.
     */
    void removeElement(RichElement* element);

    /**
     * @brief Set vertical space between each RichElement.
     *
     * @param space Point in float.
     */
    void setVerticalSpace(float space);

    /**
     * @brief Rearrange all RichElement in the RichText.
     * @param force Force the formatting of the contents
     * It's usually called internally.
     */
    void formatText(bool force = false);

    // override functions.
    void ignoreContentAdaptWithSize(bool ignore) override;
    std::string getDescription() const override;

    void setWrapMode(WrapMode wrapMode); /*!< sets the wrapping mode: WRAP_PER_CHAR or WRAP_PER_WORD */
    WrapMode getWrapMode() const;        /*!< returns the current wrapping mode */
    void setHorizontalAlignment(
        HorizontalAlignment a); /*!< sets the horizontal alignment mode: LEFT, CENTER, or RIGHT */
    HorizontalAlignment getHorizontalAlignment() const; /*!< returns the current horizontal alignment mode */
    void setVerticalAlignment(VerticalAlignment a); /*!< sets the vertical alignment mode: TOP, CENTER, or BOTTOM */
    VerticalAlignment getVerticalAlignment() const;     /*!< returns the current vertical alignment mode */
    void setFontColor(
        std::string_view color);             /*!< Set the font color. @param color the #RRGGBB hexadecimal notation. */
    std::string getFontColor();              /*!< return the current font color */
    Color3B getFontColor3B();                /*!< return the current font color */
    void setFontSize(float size);            /*!< Set the font size. @param size the font size. */
    float getFontSize();                     /*!< return the current font size */
    void setFontFace(std::string_view face); /*!< Set the font face. @param face the font face. */
    std::string getFontFace();               /*!< return the current font face */
    void setAnchorFontColor(std::string_view color); /*!< Set the font color of a-tag. @param color the font color. */
    std::string getAnchorFontColor();                /*!< return the current font color of a-tag */
    ax::Color3B getAnchorFontColor3B();              /*!< return the current font color of a-tag */
    void setAnchorTextBold(bool enable);             /*!< enable bold text of a-tag */
    bool isAnchorTextBoldEnabled();                  /*!< valid style is bold text of a-tag? */
    void setAnchorTextItalic(bool enable);           /*!< enable italic text of a-tag */
    bool isAnchorTextItalicEnabled();                /*!< valid style is italic text of a-tag? */
    void setAnchorTextDel(bool enable);              /*!< enable the strikethrough of a-tag */
    bool isAnchorTextDelEnabled();                   /*!< valid strikethrough of a-tag? */
    void setAnchorTextUnderline(bool enable);        /*!< enable the underline of a-tag */
    bool isAnchorTextUnderlineEnabled();             /*!< valid underline of a-tag? */
    /** @brief enable the outline of a-tag */
    void setAnchorTextOutline(bool enable, const Color3B& outlineColor = Color3B::WHITE, int outlineSize = -1);
    bool isAnchorTextOutlineEnabled();     /*!< valid outline of a-tag? */
    Color3B getAnchorTextOutlineColor3B(); /*!< return the current text outline color of a-tag */
    int getAnchorTextOutlineSize();        /*!< return the current text outline size of a-tag */
    /** @brief enable the shadow of a-tag */
    void setAnchorTextShadow(bool enable,
                             const Color3B& shadowColor = Color3B::BLACK,
                             const Vec2& offset         = Vec2(2.0, -2.0),
                             int blurRadius             = 0);
    bool isAnchorTextShadowEnabled();     /*!< valid shadow of a-tag? */
    Color3B getAnchorTextShadowColor3B(); /*!< return the current text shadow color of a-tag */
    Vec2 getAnchorTextShadowOffset();     /*!< return the current text shadow offset of a-tag */
    int getAnchorTextShadowBlurRadius();  /*!< return the current text shadow blur radius of a-tag */
    void setAnchorTextGlow(bool enable, const Color3B& glowColor = Color3B::WHITE); /*!< enable the glow of a-tag */
    bool isAnchorTextGlowEnabled();                                                 /*!< valid glow of a-tag? */
    Color3B getAnchorTextGlowColor3B();         /*!< return the current text glow color of a-tag */
    void setDefaults(const ValueMap& defaults); /*!< set the default values */
    ValueMap getDefaults() const;               /*!< returns the default values */

    ax::Color3B color3BWithString(std::string_view color);     /*!< convert a color string into a Color3B. */
    std::string stringWithColor3B(const ax::Color3B& color3b); /*!< convert a Color3B into a color string. */
    std::string stringWithColor4B(const ax::Color4B& color4b); /*!< convert a Color4B into a color string. */

    /**
     * @brief add a callback to own tag.
     * @param tag tag's name
     * @param isFontElement use attributes of text tag
     * @param handleVisitEnter callback at opening tag
     * @param handleVisitExit callback at closing tag
     */
    static void setTagDescription(std::string_view tag,
                                  bool isFontElement,
                                  VisitEnterHandler handleVisitEnter,
                                  VisitExitHandler handleVisitExit = nullptr);

    /**
     * @brief remove a callback to own tag.
     * @param tag tag's name
     */
    static void removeTagDescription(std::string_view tag);

    void openUrl(std::string_view url);

    /**
     * @brief Asks the callback to open a resource specified by a URL.
     * @discussion If you set up your own URL in the anchor tag, it is used to intercept the URL open.
     * @param handleOpenUrl the callback
     */
    void setOpenUrlHandler(const OpenUrlHandler& handleOpenUrl);

    bool init() override;

    bool initWithXML(std::string_view xml,
                     const ValueMap& defaults            = ValueMap(),
                     const OpenUrlHandler& handleOpenUrl = nullptr);

    bool setString(std::string_view text);

protected:
    void adaptRenderers() override;

    void initRenderer() override;
    void pushToContainer(Node* renderer);
    void handleTextRenderer(std::string_view text,
                            std::string_view fontName,
                            float fontSize,
                            const Color3B& color,
                            uint8_t opacity,
                            uint32_t flags,
                            std::string_view url        = "",
                            const Color3B& outlineColor = Color3B::WHITE,
                            int outlineSize             = -1,
                            const Color3B& shadowColor  = Color3B::BLACK,
                            const Vec2& shadowOffset    = Vec2(2.0, -2.0),
                            int shadowBlurRadius        = 0,
                            const Color3B& glowColor    = Color3B::WHITE,
                            std::string_view id         = ""sv);
    void handleImageRenderer(std::string_view filePath,
                             Widget::TextureResType textureType,
                             const Color3B& color,
                             uint8_t opacity,
                             int width,
                             int height,
                             std::string_view url,
                             float scaleX        = 1.f,
                             float scaleY        = 1.f,
                             std::string_view id = ""sv);
    void handleCustomRenderer(Node* renderer, std::string_view id = ""sv);
    void formatRenderers();
    void addNewLine(int quantity = 1);
    void doHorizontalAlignment(const Vector<Node*>& row, float rowWidth);
    float stripTrailingWhitespace(const Vector<Node*>& row);

    bool _formatTextDirty;
    Vector<RichElement*> _richElements;
    std::vector<Vector<Node*>> _elementRenders;
    std::vector<float> _lineHeights;
    float _leftSpaceWidth;

    ValueMap _defaults;            /*!< default values */
    OpenUrlHandler _handleOpenUrl; /*!< the callback for open URL */

    std::string _text;
    std::string _xmlText;
};

}  // namespace ui

// end of ui group
/// @}
NS_AX_END
