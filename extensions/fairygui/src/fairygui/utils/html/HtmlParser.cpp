#include "HtmlParser.h"
#include "HtmlElement.h"
#include "utils/ToolSet.h"

#include <sstream>
#include <vector>
#include <locale>
#include <algorithm>

NS_FGUI_BEGIN
using namespace ax;

using namespace std;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)
#define strcasecmp _stricmp
#endif

static bool isWhitespace(char c) {
    return std::isspace(c, std::locale());
}

static void ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if_not(s.begin(),
        s.end(),
        isWhitespace));
}

static void rtrim(std::string& s) {
    s.erase(std::find_if_not(s.rbegin(),
        s.rend(),
        isWhitespace).base(),
        s.end());
}

Color3B HtmlParseOptions::defaultLinkColor(58, 103, 204);
bool HtmlParseOptions::defaultLinkUnderline = true;

HtmlParseOptions::HtmlParseOptions()
{
    linkColor = defaultLinkColor;
    linkUnderline = defaultLinkUnderline;
}

HtmlParser HtmlParser::defaultParser;

HtmlParser::HtmlParser()
{

}

HtmlParser::~HtmlParser()
{
}

void HtmlParser::pushTextFormat()
{
    if (_textFormatStack.size() <= _textFormatStackTop)
        _textFormatStack.push_back(_format);
    else
        _textFormatStack[_textFormatStackTop] = _format;
    _textFormatStackTop++;
}

void HtmlParser::popTextFormat()
{
    if (_textFormatStackTop > 0)
    {
        _format = _textFormatStack[_textFormatStackTop - 1];
        _textFormatStackTop--;
    }
}

void HtmlParser::addNewLine(bool check)
{
    HtmlElement* lastElement = _elements->empty() ? nullptr : _elements->back();
    if (lastElement && lastElement->type == HtmlElement::Type::TEXT)
    {
        if (!check || lastElement->text.back() != '\n')
            lastElement->text += "\n";
        return;
    }

    HtmlElement* element = new HtmlElement(HtmlElement::Type::TEXT);
    element->format = _format;
    element->text = "\n";
    _elements->push_back(element);
    if (!_linkStack.empty())
        element->link = _linkStack.back();
}

void HtmlParser::finishTextBlock()
{
    if (!_textBlock.empty())
    {
        HtmlElement* element = new HtmlElement(HtmlElement::Type::TEXT);
        element->format = _format;
        element->text = _textBlock;
        _textBlock.clear();
        _elements->push_back(element);
        if (!_linkStack.empty())
            element->link = _linkStack.back();
    }
}

#pragma warning(once:4307)
void HtmlParser::startElement(void* /*ctx*/, const char *elementName, const char **atts)
{
    if (_skipText == 0)
        finishTextBlock();
    else if (!_textBlock.empty())
        _textBlock.clear();

    if (strcasecmp(elementName, "b") == 0)
    {
        pushTextFormat();
        _format.bold = true;
    }
    else if (strcasecmp(elementName, "i") == 0)
    {
        pushTextFormat();
        _format.italics = true;
    }
    else if (strcasecmp(elementName, "u") == 0)
    {
        pushTextFormat();
        _format.underline = true;
    }
    else if (strcasecmp(elementName, "font") == 0)
    {
        pushTextFormat();
        ValueMap&& attrMap = parseAttrs(atts);
        _format.fontSize = attributeInt(attrMap, "size", _format.fontSize);

        auto it = attrMap.find("color");
        if (it != attrMap.end())
        {
            _format.color = (Color3B)ToolSet::hexToColor(it->second.asString().c_str());
            _format._hasColor = true;
        }
    }
    else if (strcasecmp(elementName, "br") == 0)
    {
        addNewLine(false);
    }
    else if (strcasecmp(elementName, "img") == 0)
    {
        HtmlElement* element = new HtmlElement(HtmlElement::Type::IMAGE);
        element->attrs = parseAttrs(atts);
        _elements->push_back(element);
        if (!_linkStack.empty())
            element->link = _linkStack.back();
    }
    else if (strcasecmp(elementName, "a") == 0)
    {
        pushTextFormat();

        HtmlElement* element = new HtmlElement(HtmlElement::Type::LINK);
        element->attrs = parseAttrs(atts);
        element->text = element->getString("href");
        _elements->push_back(element);
        _linkStack.push_back(element);

        if (_parseOptions.linkUnderline)
            _format.underline = true;
        if (!_format._hasColor)
            _format.color = _parseOptions.linkColor;
    }
    else if (strcasecmp(elementName, "p") == 0 || strcasecmp(elementName, "ui") == 0 || strcasecmp(elementName, "div") == 0
        || strcasecmp(elementName, "li") == 0)
    {
        addNewLine(true);
    }
    else if (strcasecmp(elementName, "html") == 0 || strcasecmp(elementName, "body") == 0)
    {
        //full html
        _ignoreWhiteSpace = true;
    }
    else if (strcasecmp(elementName, "head") == 0 || strcasecmp(elementName, "style") == 0 || strcasecmp(elementName, "script") == 0
        || strcasecmp(elementName, "form") == 0)
    {
        _skipText++;
    }
    else if (strcasecmp(elementName, "input") == 0)
    {
        HtmlElement* element = new HtmlElement(HtmlElement::Type::INPUT);
        element->attrs = parseAttrs(atts);
        _elements->push_back(element);
    }
    else if (strcasecmp(elementName, "select") == 0)
    {
        HtmlElement* element = new HtmlElement(HtmlElement::Type::SELECT);
        element->attrs = parseAttrs(atts);
        _elements->push_back(element);

        _currentSelect = element;
        _skipText++;
    }
    else if (strcasecmp(elementName, "option") == 0)
    {
        if (_currentSelect != nullptr)
        {
            auto attrMap = parseAttrs(atts);
            auto it = attrMap.find("value");
            if (it != attrMap.end())
                _currentSelect->getArray("values").push_back(Value(it->second.asString()));
            else
                _currentSelect->getArray("values").push_back(Value(STD_STRING_EMPTY));
        }
    }
    else if (strcasecmp(elementName, "object") == 0)
    {
        HtmlElement* element = new HtmlElement(HtmlElement::Type::OBJECT);
        element->attrs = parseAttrs(atts);
        _elements->push_back(element);

        _skipText++;
    }
}

void HtmlParser::endElement(void* /*ctx*/, const char *elementName)
{
    if (_currentSelect != nullptr && strcasecmp(elementName, "option") == 0)
    {
        _currentSelect->getArray("items").push_back(Value(_textBlock));
        _textBlock.clear();
        return;
    }

    if (_skipText == 0)
        finishTextBlock();
    else if (!_textBlock.empty())
        _textBlock.clear();

    if (strcasecmp(elementName, "b") == 0 || strcasecmp(elementName, "i") == 0 || strcasecmp(elementName, "u") == 0
        || strcasecmp(elementName, "font") == 0)
    {
        popTextFormat();
    }
    else if (strcasecmp(elementName, "a") == 0)
    {
        popTextFormat();

        if (!_linkStack.empty())
            _linkStack.pop_back();
    }
    else if (strcasecmp(elementName, "head") == 0 || strcasecmp(elementName, "style") == 0 || strcasecmp(elementName, "script") == 0
        || strcasecmp(elementName, "form") == 0
        || strcasecmp(elementName, "object") == 0)
    {
        _skipText--;
    }
    else if (strcasecmp(elementName, "select") == 0)
    {
        _currentSelect = nullptr;
        _skipText--;
    }
}
#pragma warning(default:4307)

void HtmlParser::textHandler(void* /*ctx*/, const char *str, size_t len)
{
    if (_ignoreWhiteSpace)
    {
        string s(str, len);
        ltrim(s);
        rtrim(s);
        _textBlock += s;
    }
    else
        _textBlock += string(str, len);
}

ValueMap HtmlParser::parseAttrs(const char ** attrs)
{
    ValueMap attrMap;
    for (const char** attr = attrs; *attr != nullptr; attr = (attrs += 2)) {
        if (attr[0] && attr[1]) {
            attrMap[attr[0]] = attr[1];
        }
    }
    return attrMap;
}

int HtmlParser::attributeInt(const ValueMap& valueMap, const std::string& key, int defaultValue)
{
    auto it = valueMap.find(key);
    if (it != valueMap.end()) {
        string str = it->second.asString();
        if (!str.empty() && str.back() == '%')
            return ceil(atoi(str.substr(0, str.size() - 1).c_str()) / 100.0f*defaultValue);
        else
            return atoi(str.c_str());
    }
    else
        return defaultValue;
}

void HtmlParser::parse(const std::string& source, const TextFormat& format, std::vector<HtmlElement*>& elements, const HtmlParseOptions& parseOptions)
{
    _format = format;
    _elements = &elements;
    _parseOptions = parseOptions;
    _ignoreWhiteSpace = false;
    _textFormatStackTop = 0;
    _skipText = 0;

    string xmlText = "<dummy>" + source + "</dummy>";
    SAXParser parser;
    parser.setDelegator(this);
    parser.parseIntrusive(&xmlText.front(), xmlText.length(), SAXParser::ParseOption::HTML);
}

NS_FGUI_END
