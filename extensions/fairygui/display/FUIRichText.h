#ifndef __FUIRICHTEXT_H__
#define __FUIRICHTEXT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "TextFormat.h"
#include "utils/html/HtmlParser.h"

NS_FGUI_BEGIN

class HtmlObject;

class FUIRichText : public ax::Node
{
public:
    FUIRichText();
    virtual ~FUIRichText();

    CREATE_FUNC(FUIRichText);

    const ax::Size& getDimensions() const { return _dimensions; }
    void setDimensions(float width, float height);

    void setText(const std::string& value);

    TextFormat* getTextFormat() { return &_textFormat; }
    void applyTextFormat();

    ax::Label::Overflow getOverflow()const { return _overflow; }
    void setOverflow(ax::Label::Overflow overflow);

    bool isAnchorTextUnderline();
    void setAnchorTextUnderline(bool enable);

    const ax::Color3B& getAnchorFontColor();
    void setAnchorFontColor(const ax::Color3B& color);

    void setObjectFactory(const std::function<HtmlObject*(HtmlElement*)>& value) { _objectFactory = value; }
    HtmlParseOptions& parseOptions() { return _parseOptions; }

    const std::vector<HtmlObject*>& getControls() const { return _controls; }
    HtmlObject* getControl(const std::string& name) const;

    const char* hitTestLink(const ax::Vec2& worldPoint);
    virtual void visit(ax::Renderer *renderer, const ax::Mat4 &parentTransform, uint32_t parentFlags) override;

    virtual const ax::Size& getContentSize() const override;

protected:

    virtual bool init() override;
    void formatText();
    void formarRenderers();
    void handleTextRenderer(HtmlElement* element, const std::string& text);
    void handleRichRenderer(HtmlElement* element);
    void addNewLine();
    int findSplitPositionForWord(ax::Label* label, const std::string& text);
    void doHorizontalAlignment(const std::vector<ax::Node*>& row, float rowWidth);

    std::vector<HtmlElement*> _elements;
    std::vector<HtmlObject*> _controls;
    std::vector<std::vector<ax::Node*>> _renderers;
    bool _formatTextDirty;
    bool _textChanged;
    ax::Size _dimensions;
    float _leftSpaceWidth;
    float _textRectWidth;
    int _numLines;
    ax::Label::Overflow _overflow;
    TextFormat _textFormat;
    std::string _text;
    HtmlParseOptions _parseOptions;
    std::function<HtmlObject*(HtmlElement*)> _objectFactory;
};

NS_FGUI_END

#endif
