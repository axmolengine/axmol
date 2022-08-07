#ifndef __HTMLPARSER_H__
#define __HTMLPARSER_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "display/TextFormat.h"

NS_FGUI_BEGIN

class HtmlElement;

class HtmlParseOptions
{
public:
    static bool defaultLinkUnderline;
    static ax::Color3B defaultLinkColor;

    HtmlParseOptions();

    bool linkUnderline;
    ax::Color3B linkColor;
};

class HtmlParser : public ax::SAXDelegator
{
public:
    static HtmlParser defaultParser;

    HtmlParser();
    virtual ~HtmlParser();

    void parse(const std::string& source, const TextFormat& format, std::vector<HtmlElement*>& elements, const HtmlParseOptions& parseOptions);

    void startElement(void *ctx, const char *name, const char **atts) override;
    void endElement(void *ctx, const char *name) override;
    void textHandler(void *ctx, const char *s, size_t len) override;

private:
    ax::ValueMap parseAttrs(const char ** attrs);
    int attributeInt(const  ax::ValueMap& vm, const std::string& key, int defaultValue);

    void pushTextFormat();
    void popTextFormat();
    void addNewLine(bool check);
    void finishTextBlock();

    std::vector<HtmlElement*>* _elements;
    std::vector<TextFormat> _textFormatStack;
    std::vector<HtmlElement*> _linkStack;
    HtmlElement* _currentSelect;
    TextFormat _format;
    size_t _textFormatStackTop;
    int _skipText;
    bool _ignoreWhiteSpace;
    std::string _textBlock;
    HtmlParseOptions _parseOptions;
};

NS_FGUI_END

#endif
