/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
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

#ifndef _COCOS2D_CCLABEL_H_
#define _COCOS2D_CCLABEL_H_

#include "2d/Node.h"
#include "renderer/CustomCommand.h"
#include "renderer/QuadCommand.h"
#include "2d/FontAtlas.h"
#include "2d/FontFreeType.h"
#include "base/Types.h"

NS_AX_BEGIN

/**
 * @addtogroup _2d
 * @{
 */

#define AX_DEFAULT_FONT_LABEL_SIZE 12

/**
 * @struct TTFConfig
 * @see `GlyphCollection`
 */
typedef struct _ttfConfig
{
    std::string fontFilePath;
    std::string customGlyphs;

    GlyphCollection glyphs;
    float fontSize; // The desired render font size
    int faceSize; // The original face size of font
    int outlineSize;

    bool distanceFieldEnabled;
    bool italics;
    bool bold;
    bool underline;
    bool strikethrough;

    _ttfConfig(std::string_view filePath              = {},
               float size                             = AX_DEFAULT_FONT_LABEL_SIZE,
               const GlyphCollection& glyphCollection = GlyphCollection::DYNAMIC,
               const char* customGlyphCollection      = nullptr, /* nullable */
               bool useDistanceField                  = FontFreeType::isShareDistanceFieldEnabled(),
               int outline                            = 0,
               bool useItalics                        = false,
               bool useBold                           = false,
               bool useUnderline                      = false,
               bool useStrikethrough                  = false)
        : fontFilePath(filePath)
        , fontSize(size)
        , faceSize(FontFreeType::DEFAULT_BASE_FONT_SIZE)
        , glyphs(glyphCollection)
        , customGlyphs(customGlyphCollection ? customGlyphCollection : "")
        , distanceFieldEnabled(useDistanceField)
        , outlineSize(outline)
        , italics(useItalics)
        , bold(useBold)
        , underline(useUnderline)
        , strikethrough(useStrikethrough)
    {
    }
} TTFConfig;

class Sprite;
class SpriteBatchNode;
class DrawNode;
class EventListenerCustom;
class TextureAtlas;

/**
 * @brief Label is a subclass of Node that knows how to render text labels.
 *
 * Label can be created with:
 * - A true type font file.
 * - A bitmap font file.
 * - A char map file.
 * - The built in system font.
 *
 * Bitmap Font supported editors:
 * - http://glyphdesigner.71squared.com/ (Commercial, Mac OS X)
 * - http://www.n4te.com/hiero/hiero.jnlp (Free, Java)
 * - http://slick.cokeandcode.com/demos/hiero.jnlp (Free, Java)
 * - http://www.angelcode.com/products/bmfont/ (Free, Windows only)
 * @js NA
 */
class AX_DLL Label : public Node, public LabelProtocol, public BlendProtocol
{
public:
    enum class Overflow
    {
        // In NONE mode, the dimensions is (0,0) and the content size will change dynamically to fit the label.
        NONE,
        /**
         *In CLAMP mode, when label content goes out of the bounding box, it will be clipped.
         */
        CLAMP,
        /**
         * In SHRINK mode, the font size will change dynamically to adapt the content size.
         */
        SHRINK,
        /**
         *In RESIZE_HEIGHT mode, you can only change the width of label and the height is changed automatically.
         */
        RESIZE_HEIGHT
    };

    enum class LabelType
    {
        TTF,
        BMFONT,
        CHARMAP,
        STRING_TEXTURE
    };

    /// @name Creators
    /// @{

    /**
     * Allocates and initializes a Label, with default settings.
     *
     * @return An automatically released Label object.
     */
    static Label* create();

    /**
     * Allocates and initializes a Label, base on platform-dependent API.
     *
     * @param text The initial text.
     * @param font A font file or a font family name.
     * @param fontSize The font size. This value must be > 0.
     * @param dimensions
     * @param hAlignment The text horizontal alignment.
     * @param vAlignment The text vertical alignment.
     *
     * @warning It will generate texture by the platform-dependent code.
     *
     * @return An automatically released Label object.
     */
    static Label* createWithSystemFont(std::string_view text,
                                       std::string_view font,
                                       float fontSize,
                                       const Vec2& dimensions    = Vec2::ZERO,
                                       TextHAlignment hAlignment = TextHAlignment::LEFT,
                                       TextVAlignment vAlignment = TextVAlignment::TOP);

    /**
     * Allocates and initializes a Label, base on FreeType2.
     *
     * @param text The initial text.
     * @param fontFilePath A font file.
     * @param fontSize The font size. This value must be > 0.
     * @param dimensions
     * @param hAlignment The text horizontal alignment.
     * @param vAlignment The text vertical alignment.
     *
     * @return An automatically released Label object.
     */
    static Label* createWithTTF(std::string_view text,
                                std::string_view fontFilePath,
                                float fontSize,
                                const Vec2& dimensions    = Vec2::ZERO,
                                TextHAlignment hAlignment = TextHAlignment::LEFT,
                                TextVAlignment vAlignment = TextVAlignment::TOP);

    /**
     * Allocates and initializes a Label, base on FreeType2.
     *
     * @param ttfConfig A pointer to a TTFConfig object.
     * @param text The initial text.
     * @param hAlignment The text horizontal alignment.
     * @param maxLineWidth The max line width.
     *
     * @return An automatically released Label object.
     * @see TTFConfig setTTFConfig setMaxLineWidth
     */
    static Label* createWithTTF(const TTFConfig& ttfConfig,
                                std::string_view text,
                                TextHAlignment hAlignment = TextHAlignment::LEFT,
                                int maxLineWidth          = 0);

    /**
     * Allocates and initializes a Label, with a bitmap font file.
     *
     * @param bmfontPath A bitmap font file, it's a FNT format.
     * @param text The initial text.
     * @param hAlignment Text horizontal alignment.
     * @param maxLineWidth The max line width.
     *
     * @return An automatically released Label object.
     * @see setBMFontFilePath setMaxLineWidth
     */
    static Label* createWithBMFont(std::string_view bmfontPath,
                                   std::string_view text,
                                   const TextHAlignment& hAlignment = TextHAlignment::LEFT,
                                   int maxLineWidth                 = 0);

    /**
     * Allocates and initializes a Label, with a bitmap font file.
     *
     * @param bmfontPath A bitmap font file, it's a FNT format.
     * @param text The initial text.
     * @param hAlignment Text horizontal alignment.
     * @param maxLineWidth The max line width.
     * @param imageRect
     * @param imageRotated
     *
     * @return An automatically released Label object.
     * @see setBMFontFilePath setMaxLineWidth
     */
    static Label* createWithBMFont(std::string_view bmfontPath,
                                   std::string_view text,
                                   const TextHAlignment& hAlignment,
                                   int maxLineWidth,
                                   const Rect& imageRect,
                                   bool imageRotated);

    /**
     * Allocates and initializes a Label, with a bitmap font file.
     *
     * @param bmfontPath A bitmap font file, it's a FNT format.
     * @param text The initial text.
     * @param hAlignment Text horizontal alignment.
     * @param maxLineWidth The max line width.
     * @param subTextureKey Name of entry in PLIST texture atlas/sprite sheet
     *
     * @return An automatically released Label object.
     * @see setBMFontFilePath setMaxLineWidth
     */
    static Label* createWithBMFont(std::string_view bmfontPath,
                                   std::string_view text,
                                   const TextHAlignment& hAlignment,
                                   int maxLineWidth,
                                   std::string_view subTextureKey);

    /**
     * Allocates and initializes a Label, with a bitmap font file.
     *
     * @param bmfontPath A bitmap font file, it's a FNT format.
     * @param text The initial text.
     * @param hAlignment Text horizontal alignment.
     * @param maxLineWidth The max line width.
     * @param imageOffset Offset into larger texture
     *
     * @return An automatically released Label object.
     * @see setBMFontFilePath setMaxLineWidth
     */
    AX_DEPRECATED_ATTRIBUTE static Label* createWithBMFont(std::string_view bmfontPath,
                                                           std::string_view text,
                                                           const TextHAlignment& hAlignment,
                                                           int maxLineWidth,
                                                           const Vec2& imageOffset);

    /**
     * Allocates and initializes a Label, with char map configuration.
     *
     * @param charMapFile A char map file, it's a PNG format.
     * @param itemWidth The width in points of each element.
     * @param itemHeight The height in points of each element.
     * @param startCharMap The starting char of the char map.
     *
     * @return An automatically released Label object.
     */
    static Label* createWithCharMap(std::string_view charMapFile, int itemWidth, int itemHeight, int startCharMap);

    /**
     * Allocates and initializes a Label, with char map configuration.
     *
     * @param texture A pointer to an existing Texture2D object.
     * @param itemWidth The width in points of each element.
     * @param itemHeight The height in points of each element.
     * @param startCharMap The starting char of the char map.
     *
     * @return An automatically released Label object.
     */
    static Label* createWithCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);

    /**
     * Allocates and initializes a Label, with char map configuration.
     *
     * @param plistFile A configuration file of char map.
     *
     * @return An automatically released Label object.
     */
    static Label* createWithCharMap(std::string_view plistFile);

    //  end of creators group
    /// @}

    /// @{
    /// @name Font methods

    /**
     * Sets a new TTF configuration to Label.
     * @see `TTFConfig`
     */
    virtual bool setTTFConfig(const TTFConfig& ttfConfig);

    /**
     * Returns the TTF configuration object used by the Label.
     * @see `TTFConfig`
     */
    virtual const TTFConfig& getTTFConfig() const { return _fontConfig; }

    /** Sets a new bitmap font to Label */
    virtual bool setBMFontFilePath(std::string_view bmfontFilePath, float fontSize = 0);

    /** Sets a new bitmap font to Label */
    virtual bool setBMFontFilePath(std::string_view bmfontFilePath,
                                   const Rect& imageRect,
                                   bool imageRotated,
                                   float fontSize = 0);

    /** Sets a new bitmap font to Label */
    virtual bool setBMFontFilePath(std::string_view bmfontFilePath, std::string_view subTextureKey, float fontSize = 0);

    /** Sets a new bitmap font to Label */
    AX_DEPRECATED_ATTRIBUTE virtual bool setBMFontFilePath(std::string_view bmfontFilePath,
                                                           const Vec2& imageOffset,
                                                           float fontSize = 0);

    /** Returns the bitmap font used by the Label.*/
    std::string_view getBMFontFilePath() const { return _bmFontPath; }

    /**
     * Sets a new char map configuration to Label.
     *
     * @see `createWithCharMap(std::string_view,int,int,int)`
     */
    virtual bool setCharMap(std::string_view charMapFile, int itemWidth, int itemHeight, int startCharMap);

    /**
     * Sets a new char map configuration to Label.
     *
     * @see `createWithCharMap(Texture2D*,int,int,int)`
     */
    virtual bool setCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);

    /**
     * Sets a new char map configuration to Label.
     *
     * @see `createWithCharMap(std::string_view)`
     */
    virtual bool setCharMap(std::string_view plistFile);

    /**
     * Sets a new system font to Label.
     *
     * @param font A font file or a font family name.
     * @warning
     */
    virtual void setSystemFontName(std::string_view font);

    /** Returns the system font used by the Label.*/
    virtual std::string_view getSystemFontName() const { return _systemFont; }

    /* Sets the system font size of Label.*/
    virtual void setSystemFontSize(float fontSize);

    /** Returns the bitmap font path used by the Label.*/
    virtual float getSystemFontSize() const { return _systemFontSize; }

    /**
     * @warning This method is not recommended for game developers.
     */
    virtual void requestSystemFontRefresh() { _systemFontDirty = true; }

    //  end of font methods
    /// @}

    /** Sets the text that this Label is to display.*/
    virtual void setString(std::string_view text) override;

    /** Return the text the Label is currently displaying.*/
    virtual std::string_view getString() const override { return _utf8Text; }

    /**
     * Return the number of lines of text.
     */
    int getStringNumLines();

    /**
     * Return length of string.
     */
    int getStringLength();

    /**
     * Sets the text color of Label.
     *
     * The text color is different from the color of Node.
     *
     * @warning Limiting use to only when the Label created with true type font or system font.
     */
    virtual void setTextColor(const Color4B& color);

    /** Returns the text color of the Label.*/
    const Color4B& getTextColor() const { return _textColor; }

    /**
     * Enable shadow effect to Label.
     *
     * @todo Support blur for shadow effect.
     */
    virtual void enableShadow(const Color4B& shadowColor = Color4B::BLACK,
                              const Vec2& offset         = Vec2(2, -2),
                              int blurRadius             = 0);

    /**
     * Enable outline effect to Label.
     * @warning Limiting use to only when the Label created with true type font or system font.
     */
    virtual void enableOutline(const Color4B& outlineColor, int outlineSize = -1);

    /**
     * Enable glow effect to Label.
     * @warning Limiting use to only when the Label created with true type font.
     */
    virtual void enableGlow(const Color4B& glowColor);

    /**
     * Enable italics rendering
     */
    void enableItalics();

    /**
     * Enable bold rendering
     */
    void enableBold();

    /**
     * Enable underline
     */
    void enableUnderline();

    /**
     * Enables strikethrough.
     * Underline and Strikethrough cannot be enabled at the same time.
     * Strikethrough is like an underline but at the middle of the glyph
     */
    void enableStrikethrough();
    /**
     * Disable all effect applied to Label.
     * @warning Please use disableEffect(LabelEffect::ALL) instead of this API.
     */
    virtual void disableEffect();

    /**
     * Disable effect to Label.
     *
     * @see `LabelEffect`
     */
    virtual void disableEffect(LabelEffect effect);

    /**
     * Return whether the shadow effect is enabled.
     */
    bool isShadowEnabled() const { return _shadowEnabled; }

    /**
     * Return shadow effect offset value.
     */
    Vec2 getShadowOffset() const { return _shadowOffset; }

    /**
     * Return the shadow effect blur radius.
     */
    float getShadowBlurRadius() const { return _shadowBlurRadius; }

    /**
     * Return the shadow effect color value.
     */
    Color4F getShadowColor() const { return _shadowColor4F; }

    /**
     * Return the outline effect size value.
     */
    float getOutlineSize() const { return _outlineSize; }

    /**
     * Return current effect type.
     */
    LabelEffect getLabelEffectType() const { return _currLabelEffect; }

    /**
     * Return current effect color value.
     */
    Color4F getEffectColor() const { return _effectColorF; }

    /** Sets the Label's text horizontal alignment.*/
    void setAlignment(TextHAlignment hAlignment) { setAlignment(hAlignment, _vAlignment); }

    /** Returns the Label's text horizontal alignment.*/
    TextHAlignment getTextAlignment() const { return _hAlignment; }

    /** Sets the Label's text alignment.*/
    void setAlignment(TextHAlignment hAlignment, TextVAlignment vAlignment);

    /** Sets the Label's text horizontal alignment.*/
    void setHorizontalAlignment(TextHAlignment hAlignment) { setAlignment(hAlignment, _vAlignment); }

    /** Returns the Label's text horizontal alignment.*/
    TextHAlignment getHorizontalAlignment() const { return _hAlignment; }

    /** Sets the Label's text vertical alignment.*/
    void setVerticalAlignment(TextVAlignment vAlignment) { setAlignment(_hAlignment, vAlignment); }

    /** Returns the Label's text vertical alignment.*/
    TextVAlignment getVerticalAlignment() const { return _vAlignment; }

    /**
     * Specify what happens when a line is too long for Label.
     *
     * @param breakWithoutSpace Lines are automatically broken between words if this value is false.
     */
    void setLineBreakWithoutSpace(bool breakWithoutSpace);

    /**
     * Makes the Label at most this line untransformed width.
     * The Label's max line width be used for force line breaks if the value not equal zero.
     */
    void setMaxLineWidth(float maxLineWidth);
    float getMaxLineWidth() { return _maxLineWidth; }

    /**
     * Set the ttf/ttc font face size to determine how big to generate SDF bitmap
     * since axmol-2.1.0
     */
    void setTTFFaceSize(int faceSize);

    /** Gets ttf/ttc font face size, since axmol-2.1.0 */
    int getTTFFaceSize() const;

    /**
     * Change font size of label type BMFONT
     * Note: This function only scale the BMFONT letter to mimic the font size change effect.
     *
     * @param fontSize The desired font size in float.
     */
    void setBMFontSize(float fontSize);

    /**
     * Return the user define BMFont size.
     *
     * @return The BMFont size in float value.
     */
    float getBMFontSize() const;

    /**
     * Toggle wrap option of the label.
     * Note: System font doesn't support manually toggle wrap.
     *
     * @param enable Set true to enable wrap and false to disable wrap.
     */
    void enableWrap(bool enable);

    /**
     * Query the wrap is enabled or not.
     * Note: System font will always return true.
     */
    bool isWrapEnabled() const;

    /**
     * Change the label's Overflow type, currently only TTF and BMFont support all the valid Overflow type.
     * Char Map font supports all the Overflow type except for SHRINK, because we can't measure it's font size.
     * System font only support Overflow::Normal and Overflow::RESIZE_HEIGHT.
     *
     * @param overflow   see `Overflow`
     */
    void setOverflow(Overflow overflow);

    /**
     * Query the label's Overflow type.
     *
     * @return see `Overflow`
     */
    Overflow getOverflow() const;
    /**
     * Makes the Label exactly this untransformed width.
     *
     * The Label's width be used for text align if the value not equal zero.
     */
    void setWidth(float width) { setDimensions(width, _labelHeight); }
    float getWidth() const { return _labelWidth; }

    /**
     * Makes the Label exactly this untransformed height.
     *
     * The Label's height be used for text align if the value not equal zero.
     * The text will display incomplete if the size of Label is not large enough to display all text.
     */
    void setHeight(float height) { setDimensions(_labelWidth, height); }
    float getHeight() const { return _labelHeight; }

    /** Sets the untransformed size of the Label in a more efficient way. */
    void setDimensions(float width, float height);
    const Vec2& getDimensions() const { return _labelDimensions; }

    /** Update content immediately.*/
    virtual void updateContent();

    /**
     * Provides a way to treat each character like a Sprite.
     * @warning No support system font.
     */
    virtual Sprite* getLetter(int lettetIndex);

    /** Clips upper and lower margin to reduce height of Label.*/
    void setClipMarginEnabled(bool clipEnabled) { _clipEnabled = clipEnabled; }

    bool isClipMarginEnabled() const { return _clipEnabled; }

    /** Sets the line height of the Label.
     * @warning Not support system font.
     * @since v3.2.0
     */
    void setLineHeight(float height);

    /**
     * Returns the line height of this Label.
     * @warning Not support system font.
     * @since v3.2.0
     */
    float getLineHeight() const;

    void setLineSpacing(float height);
    float getLineSpacing() const;

    /**
     Returns type of label

     @warning Not support system font.
     @return the type of label
     @since v3.18.0
     */
    LabelType getLabelType() const { return _currentLabelType; }

    /**
     Returns font size
     */
    float getRenderingFontSize() const;

    /**
     * Sets the additional kerning of the Label.
     *
     * @warning Not support system font.
     * @since v3.2.0
     */
    void setAdditionalKerning(float space);

    /**
     * Returns the additional kerning of the Label.
     *
     * @warning Not support system font.
     * @since v3.2.0
     */
    float getAdditionalKerning() const;

    bool setProgramState(backend::ProgramState* programState, bool ownPS = false) override;

    FontAtlas* getFontAtlas() { return _fontAtlas; }

    virtual const BlendFunc& getBlendFunc() const override { return _blendFunc; }
    virtual void setBlendFunc(const BlendFunc& blendFunc) override;

    virtual bool isOpacityModifyRGB() const override { return _isOpacityModifyRGB; }
    virtual void setOpacityModifyRGB(bool isOpacityModifyRGB) override;
    virtual void updateDisplayedColor(const Color3B& parentColor) override;
    virtual void updateDisplayedOpacity(uint8_t parentOpacity) override;

    virtual std::string getDescription() const override;

    virtual const Vec2& getContentSize() const override;
    virtual Rect getBoundingBox() const override;

    virtual void visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags) override;
    virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;

    virtual void setCameraMask(unsigned short mask, bool applyChildren = true) override;

    virtual void removeAllChildrenWithCleanup(bool cleanup) override;
    virtual void removeChild(Node* child, bool cleanup = true) override;
    virtual void setGlobalZOrder(float globalZOrder) override;

    /**
     * Constructor of Label.
     * @js NA
     */
    Label(TextHAlignment hAlignment = TextHAlignment::LEFT, TextVAlignment vAlignment = TextVAlignment::TOP);

    /**
     * Destructor of Label.
     * @js NA
     * @lua NA
     */
    virtual ~Label();

    bool initWithTTF(std::string_view text,
                     std::string_view fontFilePath,
                     float fontSize,
                     const Vec2& dimensions    = Vec2::ZERO,
                     TextHAlignment hAlignment = TextHAlignment::LEFT,
                     TextVAlignment vAlignment = TextVAlignment::TOP);

    bool initWithTTF(const TTFConfig& ttfConfig,
                     std::string_view text,
                     TextHAlignment hAlignment = TextHAlignment::LEFT,
                     int maxLineWidth          = 0);

protected:
    struct LetterInfo
    {
        char32_t utf32Char;
        bool valid;
        float positionX;
        float positionY;
        int atlasIndex;
        int lineIndex;
    };

    struct BatchCommand
    {
        BatchCommand();
        BatchCommand(const BatchCommand& rhs) = delete;
        BatchCommand(BatchCommand&& rhs)      = default;
        ~BatchCommand();

        BatchCommand& operator=(const BatchCommand& rhs) = delete;
        BatchCommand& operator=(BatchCommand&& rhs) = default;

        void setProgramState(backend::ProgramState* state);

        std::array<CustomCommand*, 3> getCommandArray();

        CustomCommand textCommand;
        CustomCommand outLineCommand;
        CustomCommand shadowCommand;
    };

    virtual void setFontAtlas(FontAtlas* atlas, bool distanceFieldEnabled = false, bool useA8Shader = false);
    bool getFontLetterDef(char32_t character, FontLetterDefinition& letterDef) const;

    void computeStringNumLines();

    void drawSelf(bool visibleByCamera, Renderer* renderer, uint32_t flags);

    bool multilineTextWrapByChar();
    bool multilineTextWrapByWord();
    bool multilineTextWrap(const std::function<int(const std::u32string&, int, int)>& lambda);
    void shrinkLabelToContentSize(const std::function<bool(void)>& lambda);
    bool isHorizontalClamp();
    bool isVerticalClamp();
    void rescaleWithOriginalFontSize();

    void updateLabelLetters();
    virtual bool alignText();
    void computeAlignmentOffset();
    bool computeHorizontalKernings(const std::u32string& stringToRender);

    void recordLetterInfo(const ax::Vec2& point, char32_t utf32Char, int letterIndex, int lineIndex);
    void recordPlaceholderInfo(int letterIndex, char32_t utf16Char);

    bool updateQuads();

    void createSpriteForSystemFont(const FontDefinition& fontDef);
    void createShadowSpriteForSystemFont(const FontDefinition& fontDef);

    virtual void updateShaderProgram();
    virtual void updateFontScale();
    /* DEPRECATED: use updateFontScale instead */
    AX_DEPRECATED_ATTRIBUTE virtual void updateBMFontScale() { updateFontScale(); }
    void scaleFontSize(float fontSize);
    bool setTTFConfigInternal(const TTFConfig& ttfConfig);
    bool updateTTFConfigInternal();
    void setBMFontSizeInternal(float fontSize);
    bool isHorizontalClamped(float letterPositionX, int lineIndex);
    void restoreFontSize();
    void updateLetterSpriteScale(Sprite* sprite);
    int getFirstCharLen(const std::u32string& utf32Text, int startIndex, int textLen) const;
    int getFirstWordLen(const std::u32string& utf32Text, int startIndex, int textLen) const;

    void reset();

    FontDefinition _getFontDefinition() const;

    virtual void updateColor() override;

    void updateUniformLocations();
    void setVertexLayout();
    void updateBlendState();
    void updateEffectUniforms(BatchCommand& batch,
                              TextureAtlas* textureAtlas,
                              Renderer* renderer,
                              const Mat4& transform);
    void updateBuffer(TextureAtlas* textureAtlas, CustomCommand& customCommand);

    void updateBatchCommand(BatchCommand& batch);
    
    bool _contentDirty;
    bool _useDistanceField;
    bool _useA8Shader;
    bool _shadowDirty;

    bool _shadowEnabled;
    bool _bmRotated;
    bool _systemFontDirty;
    bool _clipEnabled;

    bool _blendFuncDirty;
    /// whether or not the label was inside bounds the previous frame
    bool _insideBounds;
    bool _isOpacityModifyRGB;
    bool _enableWrap;

    bool _boldEnabled;
    bool _strikethroughEnabled;
    bool _lineBreakWithoutSpaces;
    uint8_t _shadowOpacity;

    Color3B _shadowColor3B;

    LabelType _currentLabelType;
    int _numberOfLines;
    float _outlineSize;
    float _systemFontSize;

    int _lengthOfString;
    int _uniformEffectColor;
    int _uniformEffectType;  // 0: None, 1: Outline, 2: Shadow; Only used when outline is enabled.
    int _uniformTextColor;

    // layout relevant properties.
    float _lineHeight;
    float _lineSpacing;
    float _additionalKerning;
    float _maxLineWidth;

    float _labelWidth;
    float _labelHeight;
    TextHAlignment _hAlignment;
    TextVAlignment _vAlignment;

    float _textDesiredHeight;
    float _letterOffsetY;
    float _tailoredTopY;
    float _tailoredBottomY;

    LabelEffect _currLabelEffect;
    float _shadowBlurRadius;
    float _bmFontSize;
    float _fontScale; // The bmFontScale or ttfFontScale(SDF rendering mode)

    Overflow _overflow;
    float _originalFontSize;
    Color4B _textColor;

    BlendFunc _blendFunc;
   
    Vec2 _labelDimensions;
    Vec2 _shadowOffset;
    mutable Vec2 _scaledContentSize;  // !SDF scale

    Sprite* _textSprite;
    Sprite* _shadowNode;
    int* _horizontalKernings;
    FontAtlas* _fontAtlas;
    //! used for optimization
    Sprite* _reusedLetter;
    DrawNode* _underlineNode;

    Rect _bmRect;
    Rect _reusedRect;

    Color4F _effectColorF;
    Color4F _textColorF;
    Color4F _shadowColor4F;
    Mat4 _shadowTransform;

    std::u32string _utf32Text;
    std::string _utf8Text;

    std::string _bmFontPath;
    std::string _bmSubTextureKey;
    std::string _systemFont;

    TTFConfig _fontConfig;

    Vector<SpriteBatchNode*> _batchNodes;
    std::vector<LetterInfo> _lettersInfo;

    std::vector<float> _linesWidth;
    std::vector<float> _linesOffsetX;
    
    QuadCommand _quadCommand;

    std::vector<BatchCommand> _batchCommands;

    std::unordered_map<int, Sprite*> _letters;

    EventListenerCustom* _purgeTextureListener;
    EventListenerCustom* _resetTextureListener;

#if AX_LABEL_DEBUG_DRAW
    DrawNode* _debugDrawNode;
#endif

    backend::UniformLocation _mvpMatrixLocation;
    backend::UniformLocation _textureLocation;
    backend::UniformLocation _textColorLocation;
    backend::UniformLocation _effectColorLocation;
    backend::UniformLocation _effectTypeLocation;

private:
    AX_DISALLOW_COPY_AND_ASSIGN(Label);
};

// end group
/// @}

NS_AX_END

#endif /*__COCOS2D_CCLABEL_H */
