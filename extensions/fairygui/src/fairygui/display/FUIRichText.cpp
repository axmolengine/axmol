#include "FUIRichText.h"
#include "utils/html/HtmlElement.h"
#include "utils/html/HtmlObject.h"

#include <sstream>
#include <vector>
#include <locale>
#include <algorithm>

#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
using namespace ax;

using namespace std;

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32) || (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)
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
        AXASSERT(false, "invalid horizontal alignment!");
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

static float stripTrailingWhitespace(const std::vector<Node*>& row) {
    if (!row.empty()) {
        if (auto label = dynamic_cast<Label*>(row.back())) {
            const auto width = label->getContentSize().width;
            std::string str{label->getString()};
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
        AXLOGE("Can't convert string to UTF-32: {}", str);
        return "";
    }
    if (utf32.size() < start) {
        AXLOGE("'start' is out of range: {}, {}", static_cast<long>(start), str);
        return "";
    }
    std::string result;
    if (!StringUtils::UTF32ToUTF8(utf32.substr(start, length), result)) {
        AXLOGE("Can't convert internal UTF-32 string to UTF-8: {}", str);
        return "";
    }
    return result;
}

HtmlObject* createHtmlObject(HtmlElement* element)
{
    if (element->type == HtmlElement::Type::IMAGE
        || element->type == HtmlElement::Type::INPUT
        || element->type == HtmlElement::Type::SELECT
        || element->type == HtmlElement::Type::OBJECT)
        return new HtmlObject();
    else
        return nullptr;
}

FUIRichText::FUIRichText() :
    _formatTextDirty(true),
    _textChanged(false),
    _leftSpaceWidth(0.0f),
    _textRectWidth(0.0f),
    _numLines(0),
    _overflow(Label::Overflow::NONE),
    _objectFactory(&createHtmlObject)
{
}

FUIRichText::~FUIRichText()
{
    for (auto &it : _elements)
        delete it;

    for (auto &it : _controls)
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

void FUIRichText::setOverflow(ax::Label::Overflow overflow)
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

bool FUIRichText::isAnchorTextUnderline()
{
    return _parseOptions.linkUnderline;
}

void FUIRichText::setAnchorTextUnderline(bool enable)
{
    if (_parseOptions.linkUnderline != enable)
    {
        _parseOptions.linkUnderline = enable;
        _formatTextDirty = true;
    }
}

const ax::Color3B& FUIRichText::getAnchorFontColor()
{
    return _parseOptions.linkColor;
}

void FUIRichText::setAnchorFontColor(const ax::Color3B & color)
{
    _parseOptions.linkColor = color;
    _formatTextDirty = true;
}

HtmlObject* FUIRichText::getControl(const std::string& name) const
{
    if (_formatTextDirty)
        const_cast<FUIRichText*>(this)->formatText();

    for (auto &obj : _controls)
    {
        if (obj->getElement()->getString("name") == name)
            return obj;
    }

    return nullptr;
}

const char* FUIRichText::hitTestLink(const ax::Vec2 & worldPoint)
{
    Rect rect;
    for (auto &child : _children)
    {
        HtmlElement* element = (HtmlElement*)child->getUserData();
        if (!element || !element->link)
            continue;

        rect.size = child->getContentSize();
        if (rect.containsPoint(child->convertToNodeSpace(worldPoint)))
            return element->link->text.c_str();
    }
    return nullptr;
}

void FUIRichText::visit(ax::Renderer * renderer, const ax::Mat4 & parentTransform, uint32_t parentFlags)
{
    if (_visible)
        formatText();

    Node::visit(renderer, parentTransform, parentFlags);
}

void FUIRichText::formatText()
{
    if (!_formatTextDirty)
        return;

    _formatTextDirty = false;

    if (_textChanged)
    {
        _textChanged = false;
        for (auto &it : _elements)
            delete it;
        _elements.clear();
        for (auto &it : _controls)
            delete it;
        _controls.clear();
        _numLines = 0;

        if (!_text.empty())
            HtmlParser::defaultParser.parse(_text, _textFormat, _elements, _parseOptions);
    }

    removeAllChildrenWithCleanup(true);
    _renderers.clear();

    if (_overflow == Label::Overflow::NONE)
        _textRectWidth = FLT_MAX;
    else
        _textRectWidth = _dimensions.width - GUTTER_X * 2;

    int size = (int)_elements.size();
    if (size == 0)
    {
        formarRenderers();
        return;
    }

    addNewLine();
    for (int i = 0; i < size; ++i)
    {
        HtmlElement* element = static_cast<HtmlElement*>(_elements.at(i));
        switch (element->type)
        {
        case HtmlElement::Type::TEXT:
        {
            FastSplitter fs;
            fs.start(element->text.c_str(), (int)element->text.size(), '\n');
            bool first = true;
            while (fs.next())
            {
                if (!first)
                    addNewLine();
                if (fs.getTextLength() > 0)
                    handleTextRenderer(element, string(fs.getText(), fs.getTextLength()));
                first = false;
            }
            break;
        }
        default:
            element->space = floor(_leftSpaceWidth);
            handleRichRenderer(element);
            break;
        }
    }

    formarRenderers();
}

void FUIRichText::addNewLine()
{
    _leftSpaceWidth = _textRectWidth;
    _renderers.emplace_back();
    _numLines++;
}

void FUIRichText::handleTextRenderer(HtmlElement* element, const std::string& text)
{
    FUILabel* textRenderer = FUILabel::create();
    textRenderer->setAnchorPoint(Vec2::ZERO);
    textRenderer->setCascadeOpacityEnabled(true);
    textRenderer->getTextFormat()->setFormat(element->format);
    textRenderer->applyTextFormat();
    textRenderer->setUnderlineColor(_parseOptions.linkColor);
    textRenderer->setString(text);
    textRenderer->setUserData(element);

    float textRendererWidth = textRenderer->getContentSize().width;
    _leftSpaceWidth -= textRendererWidth;
    if (_leftSpaceWidth >= 0)
    {
        _renderers.back().push_back(textRenderer);
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
        leftRenderer->setAnchorPoint(Vec2::ZERO);
        leftRenderer->setCascadeOpacityEnabled(true);
        leftRenderer->getTextFormat()->setFormat(element->format);
        leftRenderer->applyTextFormat();
        leftRenderer->setUnderlineColor(_parseOptions.linkColor);
        leftRenderer->setString(getSubStringOfUTF8String(leftWords, 0, leftLength));
        leftRenderer->setUserData(element);

        _renderers.back().push_back(leftRenderer);
    }

    if (cutWords.length() > 0)
    {
        addNewLine();
        handleTextRenderer(element, cutWords);
    }
}

int FUIRichText::findSplitPositionForWord(ax::Label* label, const std::string& text)
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

void FUIRichText::handleRichRenderer(HtmlElement* element)
{
    HtmlObject* obj = _objectFactory(element);
    if (obj == nullptr)
        return;

    element->obj = obj;
    _controls.push_back(obj);
    obj->create(this, element);

    if (obj->isHidden())
        return;

    obj->getUI()->displayObject()->setUserData(element);

    float width = obj->getUI()->getWidth();

    _leftSpaceWidth -= (width + 4);
    if (_leftSpaceWidth < 0.0f)
    {
        addNewLine();
        _leftSpaceWidth -= (width + 4);
    }

    _renderers.back().push_back(obj->getUI()->displayObject());
}

void FUIRichText::formarRenderers()
{
    float nextPosY = GUTTER_Y;
    float textWidth = 0;
    float textHeight = 0;
    for (auto& row : _renderers)
    {
        if (nextPosY != GUTTER_Y)
            nextPosY += _textFormat.lineSpacing - 3;

        float nextPosX = GUTTER_X;
        float lineHeight = 0.0f;
        float lineTextHeight = 0.0f;
        for (auto& node : row)
        {
            HtmlElement* element = (HtmlElement*)node->getUserData();
            lineHeight = MAX(node->getContentSize().height, lineHeight);
            if (element->obj == nullptr)
                lineTextHeight = MAX(node->getContentSize().height, lineTextHeight);
        }

        nextPosY += lineHeight;

        for (auto& node : row)
        {
            HtmlElement* element = (HtmlElement*)node->getUserData();
            if (element->obj != nullptr)
            {
                nextPosX += 2;
                Size sz = node->getContentSize();
                int adjustment = floor((lineHeight - sz.height) / 2);
                node->setPosition(nextPosX + node->getAnchorPoint().x *sz.width,
                    _dimensions.height - nextPosY + adjustment + node->getAnchorPoint().y * sz.height);
                nextPosX += sz.width + 2;
            }
            else //text
            {
                int adjustment = floor((lineHeight - lineTextHeight) / 2);
                node->setPosition(nextPosX, _dimensions.height - nextPosY + adjustment);
                nextPosX += node->getContentSize().width;
            }
            this->addChild(node, 1);
        }
        nextPosX += GUTTER_X;
        if (nextPosX > textWidth)
            textWidth = nextPosX;

        if (_overflow != Label::Overflow::NONE)
            doHorizontalAlignment(row, nextPosX);
    }
    if (textWidth == GUTTER_X + GUTTER_X)
        textWidth = 0;
    else if (_numLines > 1 || (_textFormat.align != TextHAlignment::LEFT && _overflow != Label::Overflow::NONE))
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
    if (_textFormat.verticalAlign == TextVAlignment::CENTER)
        delta -= floor((_dimensions.height - textHeight) * 0.5f);
    else if (_textFormat.verticalAlign == TextVAlignment::BOTTOM)
        delta -= _dimensions.height - textHeight;
    if (delta != 0)
    {
        Vec2 offset(0, delta);
        for (auto& row : _renderers)
        {
            for (auto& node : row)
            {
                node->setPosition(node->getPosition() + offset);
            }
        }
    }

    _renderers.clear();
}

void FUIRichText::doHorizontalAlignment(const std::vector<ax::Node*>& row, float rowWidth) {
    if (_textFormat.align != TextHAlignment::LEFT) {
        const auto diff = stripTrailingWhitespace(row);
        const auto leftOver = _dimensions.width - (rowWidth + diff);
        const float leftPadding = getPaddingAmount(_textFormat.align, leftOver);
        const Vec2 offset(leftPadding, 0.f);
        for (auto& node : row) {
            node->setPosition(node->getPosition() + offset);
        }
    }
}

NS_FGUI_END
