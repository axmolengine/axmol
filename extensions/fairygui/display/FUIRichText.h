#ifndef __FUIRICHTEXT_H__
#define __FUIRICHTEXT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "TextFormat.h"

NS_FGUI_BEGIN

class FUIXMLVisitor;
class GLoader;
class FUIRichElement;

class FUIRichText : public cocos2d::Node
{
public:
    FUIRichText();
    virtual ~FUIRichText();

    CREATE_FUNC(FUIRichText);

    const cocos2d::Size& getDimensions() const { return _dimensions; }
    void setDimensions(float width, float height);

    void setText(const std::string& value);

    TextFormat* getTextFormat() const { return _defaultTextFormat; }
    void applyTextFormat();

    cocos2d::Label::Overflow getOverflow()const { return _overflow; }
    void setOverflow(cocos2d::Label::Overflow overflow);

    bool isAnchorTextUnderline() { return _anchorTextUnderline; }
    void setAnchorTextUnderline(bool enable);

    const cocos2d::Color3B& getAnchorFontColor() { return _anchorFontColor; }
    void setAnchorFontColor(const cocos2d::Color3B& color);

    const char* hitTestLink(const cocos2d::Vec2& worldPoint);
    virtual void visit(cocos2d::Renderer *renderer, const cocos2d::Mat4 &parentTransform, uint32_t parentFlags) override;

    virtual const cocos2d::Size& getContentSize() const override;

protected:
    virtual bool init() override;
    void formatText();
    void formarRenderers();
    void handleTextRenderer(FUIRichElement* element, const TextFormat& format, const std::string& text);
    void handleImageRenderer(FUIRichElement* element);
    void addNewLine();
    int findSplitPositionForWord(cocos2d::Label* label, const std::string& text);
    void doHorizontalAlignment(const std::vector<cocos2d::Node*>& row, float rowWidth);

    std::vector<FUIRichElement*> _richElements;
    std::vector<std::vector<Node*>> _elementRenders;
    cocos2d::Vector<GLoader*> _imageLoaders;
    bool _formatTextDirty;
    bool _textChanged;
    cocos2d::Size _dimensions;
    float _leftSpaceWidth;
    float _textRectWidth;
    int _numLines;
    cocos2d::Label::Overflow _overflow;
    TextFormat* _defaultTextFormat;
    bool _anchorTextUnderline;
    cocos2d::Color3B _anchorFontColor;
    std::string _text;

    friend class FUIXMLVisitor;
};

NS_FGUI_END

#endif
