#include "FUIRichText.h"

#include <sstream>
#include <vector>
#include <locale>
#include <algorithm>

#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
USING_NS_CC;

using namespace std;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
#define strcasecmp _stricmp
#endif

static const int GUTTER_X = 2;
static const int GUTTER_Y = 2;

static int getPrevWord(const std::string& text, int idx)
{
    // start from idx-1
    for (int i = idx - 1; i >= 0; --i)
    {
        if (!std::isalnum(text[i], std::locale()))
            return i;
    }
    return -1;
}

static bool isWrappable(const std::string& text)
{
    for (size_t i = 0, size = text.length(); i < size; ++i)
    {
        if (!std::isalnum(text[i], std::locale()))
            return true;
    }
    return false;
}

static float getPaddingAmount(TextHAlignment alignment, const float leftOver) {
    switch (alignment) {
    case TextHAlignment::CENTER:
        return leftOver / 2.f;
    case TextHAlignment::RIGHT:
        return leftOver;
    default:
        CCASSERT(false, "invalid horizontal alignment!");
        return 0.f;
    }
}

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

static float stripTrailingWhitespace(const std::vector<cocos2d::Node*>& row) {
    if (!row.empty()) {
        if (auto label = dynamic_cast<Label*>(row.back())) {
            const auto width = label->getContentSize().width;
            auto str = label->getString();
            rtrim(str);
            if (label->getString() != str) {
                label->setString(str);
                return label->getContentSize().width - width;
            }
        }
    }
    return 0.0f;
}

static std::string getSubStringOfUTF8String(const std::string& str, std::string::size_type start, std::string::size_type length)
{
    std::u32string utf32;
    if (!StringUtils::UTF8ToUTF32(str, utf32)) {
        CCLOGERROR("Can't convert string to UTF-32: %s", str.c_str());
        return "";
    }
    if (utf32.size() < start) {
        CCLOGERROR("'start' is out of range: %ld, %s", static_cast<long>(start), str.c_str());
        return "";
    }
    std::string result;
    if (!StringUtils::UTF32ToUTF8(utf32.substr(start, length), result)) {
        CCLOGERROR("Can't convert internal UTF-32 string to UTF-8: %s", str.c_str());
        return "";
    }
    return result;
}

class FUIRichElement
{
public:
    enum class Type
    {
        TEXT,
        IMAGE,
        LINK
    };

    FUIRichElement(Type type);
    virtual ~FUIRichElement() {};

    Type _type;
    std::string text;
    TextFormat textFormat;
    int width;
    int height;
    FUIRichElement* link;
};

FUIRichElement::FUIRichElement(Type type) :
    _type(type),
    width(0),
    height(0),
    link(nullptr)
{
};

class FUIXMLVisitor : public SAXDelegator
{
public:
    explicit FUIXMLVisitor(FUIRichText* richText);
    virtual ~FUIXMLVisitor();

    void startElement(void *ctx, const char *name, const char **atts) override;
    void endElement(void *ctx, const char *name) override;
    void textHandler(void *ctx, const char *s, size_t len) override;

private:
    ValueMap tagAttrMapWithXMLElement(const char ** attrs);
    int attributeInt(const ValueMap& vm, const std::string& key, int defaultValue);

    void pushTextFormat();
    void popTextFormat();
    void addNewLine(bool check);
    void finishTextBlock();

    FUIRichText* _richText;
    std::vector<TextFormat> _textFormatStack;
    std::vector<FUIRichElement*> _linkStack;
    TextFormat _format;
    size_t _textFormatStackTop;
    int _skipText;
    bool _ignoreWhiteSpace;
    std::string _textBlock;
};

FUIXMLVisitor::FUIXMLVisitor(FUIRichText* richText)
    : _richText(richText),
    _textFormatStackTop(0),
    _skipText(0),
    _ignoreWhiteSpace(false)
{
    _format = *_richText->_defaultTextFormat;
}

FUIXMLVisitor::~FUIXMLVisitor()
{
}

void FUIXMLVisitor::pushTextFormat()
{
    if (_textFormatStack.size() <= _textFormatStackTop)
        _textFormatStack.push_back(_format);
    else
        _textFormatStack[_textFormatStackTop] = _format;
    _textFormatStackTop++;
}

void FUIXMLVisitor::popTextFormat()
{
    if (_textFormatStackTop > 0)
    {
        _format = _textFormatStack[_textFormatStackTop - 1];
        _textFormatStackTop--;
    }
}

void FUIXMLVisitor::addNewLine(bool check)
{
    FUIRichElement* lastElement = _richText->_richElements.empty() ? nullptr : _richText->_richElements.back();
    if (lastElement && lastElement->_type == FUIRichElement::Type::TEXT)
    {
        if (!check || lastElement->text.back() != '\n')
            lastElement->text += "\n";
        return;
    }

    FUIRichElement* element = new FUIRichElement(FUIRichElement::Type::TEXT);
    element->textFormat = _format;
    element->text = "\n";
    _richText->_richElements.push_back(element);
    if (!_linkStack.empty())
        element->link = _linkStack.back();
}

void FUIXMLVisitor::finishTextBlock()
{
    if (!_textBlock.empty())
    {
        FUIRichElement* element = new FUIRichElement(FUIRichElement::Type::TEXT);
        element->textFormat = _format;
        element->text = _textBlock;
        _textBlock.clear();
        _richText->_richElements.push_back(element);
        if (!_linkStack.empty())
            element->link = _linkStack.back();
    }
}

#pragma warning(once:4307)
void FUIXMLVisitor::startElement(void* /*ctx*/, const char *elementName, const char **atts)
{
    finishTextBlock();

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
        ValueMap&& tagAttrValueMap = tagAttrMapWithXMLElement(atts);
        _format.fontSize = attributeInt(tagAttrValueMap, "size", _format.fontSize);

        auto it = tagAttrValueMap.find("color");
        if (it != tagAttrValueMap.end())
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
        std::string src;
        ValueMap&& tagAttrValueMap = tagAttrMapWithXMLElement(atts);

        int width = 0;
        int height = 0;

        auto it = tagAttrValueMap.find("src");
        if (it != tagAttrValueMap.end()) {
            src = it->second.asString();
        }

        if (!src.empty()) {
            PackageItem* pi = UIPackage::getItemByURL(src);
            if (pi)
            {
                width = pi->width;
                height = pi->height;
            }
        }

        width = attributeInt(tagAttrValueMap, "width", width);
        height = attributeInt(tagAttrValueMap, "height", height);
        if (width == 0)
            width = 5;
        if (height == 0)
            height = 10;

        FUIRichElement* element = new FUIRichElement(FUIRichElement::Type::IMAGE);
        element->width = width;
        element->height = height;
        element->text = src;
        _richText->_richElements.push_back(element);
        if (!_linkStack.empty())
            element->link = _linkStack.back();
    }
    else if (strcasecmp(elementName, "a") == 0)
    {
        pushTextFormat();

        std::string href;
        ValueMap&& tagAttrValueMap = tagAttrMapWithXMLElement(atts);
        auto it = tagAttrValueMap.find("href");
        if (it != tagAttrValueMap.end())
            href = it->second.asString();

        FUIRichElement* element = new FUIRichElement(FUIRichElement::Type::LINK);
        element->text = href;
        _richText->_richElements.push_back(element);
        _linkStack.push_back(element);

        if (_richText->_anchorTextUnderline)
            _format.underline = true;
        if (!_format._hasColor)
            _format.color = _richText->_anchorFontColor;
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
}

void FUIXMLVisitor::endElement(void* /*ctx*/, const char *elementName)
{
    finishTextBlock();

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
        || strcasecmp(elementName, "form") == 0)
    {
        _skipText--;
    }
}
#pragma warning(default:4307)

void FUIXMLVisitor::textHandler(void* /*ctx*/, const char *str, size_t len)
{
    if (_skipText != 0)
        return;

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

ValueMap FUIXMLVisitor::tagAttrMapWithXMLElement(const char ** attrs)
{
    ValueMap tagAttrValueMap;
    for (const char** attr = attrs; *attr != nullptr; attr = (attrs += 2)) {
        if (attr[0] && attr[1]) {
            tagAttrValueMap[attr[0]] = attr[1];
        }
    }
    return tagAttrValueMap;
}

int FUIXMLVisitor::attributeInt(const ValueMap& valueMap, const std::string& key, int defaultValue)
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

FUIRichText::FUIRichText() :
    _formatTextDirty(true),
    _textChanged(false),
    _leftSpaceWidth(0.0f),
    _textRectWidth(0.0f),
    _numLines(0),
    _overflow(Label::Overflow::NONE),
    _anchorTextUnderline(true),
    _anchorFontColor(Color3B::BLUE),
    _defaultTextFormat(new TextFormat())
{
}

FUIRichText::~FUIRichText()
{
    for (auto &it : _richElements)
        delete it;
}

bool FUIRichText::init()
{
    if (!Node::init())
        return false;

    return true;
}

void FUIRichText::setDimensions(float width, float height)
{
    if ((_numLines > 1 && width != _dimensions.width) || width < _contentSize.width)
        _formatTextDirty = true;
    _dimensions.setSize(width, height);
}

void FUIRichText::setText(const std::string & value)
{
    _formatTextDirty = true;
    _textChanged = true;
    _text = value;
}

void FUIRichText::applyTextFormat()
{
    _textChanged = true;
    _formatTextDirty = true;
}

void FUIRichText::setOverflow(cocos2d::Label::Overflow overflow)
{
    if (_overflow != overflow)
    {
        _overflow = overflow;
        _formatTextDirty = true;
    }
}

const Size & FUIRichText::getContentSize() const
{
    if (_formatTextDirty)
        const_cast<FUIRichText*>(this)->formatText();

    return Node::getContentSize();
}

void FUIRichText::setAnchorTextUnderline(bool enable)
{
    if (_anchorTextUnderline != enable)
    {
        _anchorTextUnderline = enable;
        _formatTextDirty = true;
    }
}

void FUIRichText::setAnchorFontColor(const cocos2d::Color3B & color)
{
    _anchorFontColor = color;
    _formatTextDirty = true;
}

const char*  FUIRichText::hitTestLink(const cocos2d::Vec2 & worldPoint)
{
    Rect rect;
    for (auto &child : _children)
    {
        FUIRichElement* element = (FUIRichElement*)child->getUserData();
        if (!element || !element->link)
            continue;

        rect.size = child->getContentSize();
        if (rect.containsPoint(child->convertToNodeSpace(worldPoint)))
            return element->link->text.c_str();
    }
    return nullptr;
}

void FUIRichText::visit(cocos2d::Renderer * renderer, const cocos2d::Mat4 & parentTransform, uint32_t parentFlags)
{
    if (_visible)
        formatText();

    Node::visit(renderer, parentTransform, parentFlags);
}

void FUIRichText::formatText()
{
    if (!_formatTextDirty)
        return;

    if (_textChanged)
    {
        _textChanged = false;
        _richElements.clear();
        _numLines = 0;

        if (!_text.empty())
        {
            string xmlText = "<dummy>" + _text + "</dummy>";
            FUIXMLVisitor visitor(this);
            SAXParser parser;
            parser.setDelegator(&visitor);
            parser.parseIntrusive(&xmlText.front(), xmlText.length());
        }
    }

    removeAllChildrenWithCleanup(true);
    _elementRenders.clear();
    _imageLoaders.clear();
    if (_overflow == Label::Overflow::NONE)
        _textRectWidth = FLT_MAX;
    else
        _textRectWidth = _dimensions.width - GUTTER_X * 2;

    int size = (int)_richElements.size();
    if (size == 0)
    {
        formarRenderers();
        _formatTextDirty = false;
        return;
    }

    addNewLine();
    for (int i = 0; i < size; ++i)
    {
        FUIRichElement* element = static_cast<FUIRichElement*>(_richElements.at(i));
        switch (element->_type)
        {
        case FUIRichElement::Type::TEXT:
        {
            FastSplitter fs;
            fs.start(element->text.c_str(), (int)element->text.size(), '\n');
            bool first = true;
            while (fs.next())
            {
                if (!first)
                    addNewLine();
                if (fs.getTextLength() > 0)
                    handleTextRenderer(element, element->textFormat, string(fs.getText(), fs.getTextLength()));
                first = false;
            }
            break;
        }
        case FUIRichElement::Type::IMAGE:
            handleImageRenderer(element);
            break;
        default:
            break;
        }
    }
    formarRenderers();
    _formatTextDirty = false;
}

void FUIRichText::addNewLine()
{
    _leftSpaceWidth = _textRectWidth;
    _elementRenders.emplace_back();
    _numLines++;
}

void FUIRichText::handleTextRenderer(FUIRichElement* element, const TextFormat& format, const std::string& text)
{
    FUILabel* textRenderer = FUILabel::create();
    textRenderer->setCascadeOpacityEnabled(true);
    textRenderer->getTextFormat()->setFormat(format);
    textRenderer->applyTextFormat();
    textRenderer->setString(text);
    textRenderer->setUserData(element);

    float textRendererWidth = textRenderer->getContentSize().width;
    _leftSpaceWidth -= textRendererWidth;
    if (_leftSpaceWidth >= 0)
    {
        _elementRenders.back().push_back(textRenderer);
        return;
    }

    int leftLength = findSplitPositionForWord(textRenderer, text);

    //The minimum cut length is 1, otherwise will cause the infinite loop.
    if (0 == leftLength) leftLength = 1;
    std::string leftWords = getSubStringOfUTF8String(text, 0, leftLength);
    int rightStart = leftLength;
    if (std::isspace(text[rightStart], std::locale()))
        rightStart++;
    std::string cutWords = getSubStringOfUTF8String(text, rightStart, text.length() - leftLength);
    if (leftLength > 0)
    {
        FUILabel* leftRenderer = FUILabel::create();
        leftRenderer->setCascadeOpacityEnabled(true);
        leftRenderer->getTextFormat()->setFormat(format);
        leftRenderer->applyTextFormat();
        leftRenderer->setString(getSubStringOfUTF8String(leftWords, 0, leftLength));
        leftRenderer->setUserData(element);
        _elementRenders.back().push_back(leftRenderer);
    }

    if (cutWords.length() > 0)
    {
        addNewLine();
        handleTextRenderer(element, format, cutWords);
    }
}

int FUIRichText::findSplitPositionForWord(cocos2d::Label* label, const std::string& text)
{
    auto originalLeftSpaceWidth = _leftSpaceWidth + label->getContentSize().width;

    bool startingNewLine = (_textRectWidth == originalLeftSpaceWidth);
    if (!isWrappable(text))
    {
        if (startingNewLine)
            return (int)text.length();
        return 0;
    }

    for (int idx = (int)text.size() - 1; idx >= 0; )
    {
        int newidx = getPrevWord(text, idx);
        if (newidx >= 0)
        {
            idx = newidx;
            auto leftStr = getSubStringOfUTF8String(text, 0, idx);
            label->setString(leftStr);
            if (label->getContentSize().width <= originalLeftSpaceWidth)
                return idx;
        }
        else
        {
            if (startingNewLine)
                return idx;
            return 0;
        }
    }

    // no spaces... return the original label + size
    label->setString(text);
    return (int)text.size();
}

void FUIRichText::handleImageRenderer(FUIRichElement* element)
{
    GLoader* loader = GLoader::create();
    _imageLoaders.pushBack(loader);
    loader->setSize(element->width, element->height);
    loader->setFill(LoaderFillType::SCALE_FREE);
    loader->setURL(element->text);
    loader->displayObject()->setUserData(element);

    _leftSpaceWidth -= (element->width + 4);
    if (_leftSpaceWidth < 0.0f)
    {
        addNewLine();
        _elementRenders.back().push_back(loader->displayObject());
        _leftSpaceWidth -= (element->width + 4);
    }
    else
    {
        _elementRenders.back().push_back(loader->displayObject());
    }
}

void FUIRichText::formarRenderers()
{
    float nextPosY = GUTTER_Y;
    float textWidth = 0;
    float textHeight = 0;
    for (auto& row : _elementRenders)
    {
        if (nextPosY != GUTTER_Y)
            nextPosY += _defaultTextFormat->lineSpacing - 3;

        float nextPosX = GUTTER_X;
        float lineHeight = 0.0f;
        float lineTextHeight = 0.0f;
        for (auto& node : row)
        {
            lineHeight = MAX(node->getContentSize().height, lineHeight);
            if (((FUIRichElement*)node->getUserData())->_type == FUIRichElement::Type::TEXT)
                lineTextHeight = MAX(node->getContentSize().height, lineTextHeight);
        }

        nextPosY += lineHeight;

        for (auto& node : row)
        {
            node->setAnchorPoint(Vec2::ZERO);
            int adjustment = 0;
            if (((FUIRichElement*)node->getUserData())->_type == FUIRichElement::Type::IMAGE)
            {
                nextPosX += 2;
                adjustment = floor((lineHeight - node->getContentSize().height) / 2);
            }
            else //text
            {
                adjustment = floor((lineHeight - lineTextHeight) / 2);
            }
            node->setPosition(nextPosX, _dimensions.height - nextPosY + adjustment);
            this->addChild(node, 1);
            nextPosX += node->getContentSize().width;
            if (((FUIRichElement*)node->getUserData())->_type == FUIRichElement::Type::IMAGE)
                nextPosX += 2;
        }
        nextPosX += GUTTER_X;
        if (nextPosX > textWidth)
            textWidth = nextPosX;

        if (_overflow != Label::Overflow::NONE)
            doHorizontalAlignment(row, nextPosX);
    }
    if (textWidth == GUTTER_X + GUTTER_X)
        textWidth = 0;
    else if (_numLines > 1 || (_defaultTextFormat->align != TextHAlignment::LEFT && _overflow != Label::Overflow::NONE))
        textWidth = MAX(_dimensions.width, textWidth);
    if (nextPosY != GUTTER_Y)
        textHeight = nextPosY + GUTTER_Y;
    else
        textHeight = 0;
    setContentSize(Size(textWidth, textHeight));

    float oldDimensionsHeight = _dimensions.height;
    if (_overflow == Label::Overflow::NONE)
        _dimensions = _contentSize;
    else if (_overflow == Label::Overflow::RESIZE_HEIGHT)
        _dimensions.height = _contentSize.height;
    float delta = _contentSize.height - oldDimensionsHeight;
    if (_defaultTextFormat->verticalAlign == TextVAlignment::CENTER)
        delta -= floor((_dimensions.height - textHeight) * 0.5f);
    else if (_defaultTextFormat->verticalAlign == TextVAlignment::BOTTOM)
        delta -= _dimensions.height - textHeight;
    if (delta != 0)
    {
        Vec2 offset(0, delta);
        for (auto& row : _elementRenders)
        {
            for (auto& node : row)
            {
                node->setPosition(node->getPosition() + offset);
            }
        }
    }

    _elementRenders.clear();
}

void FUIRichText::doHorizontalAlignment(const std::vector<cocos2d::Node*> &row, float rowWidth) {
    if (_defaultTextFormat->align != TextHAlignment::LEFT) {
        const auto diff = stripTrailingWhitespace(row);
        const auto leftOver = _dimensions.width - (rowWidth + diff);
        const float leftPadding = getPaddingAmount(_defaultTextFormat->align, leftOver);
        const Vec2 offset(leftPadding, 0.f);
        for (auto& node : row) {
            node->setPosition(node->getPosition() + offset);
        }
    }
}

NS_FGUI_END
