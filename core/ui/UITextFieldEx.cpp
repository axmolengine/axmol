/****************************************************************************
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

#include "UITextFieldEx.h"
#include "base/Director.h"
#include "base/IMEDispatcher.h"
#include <algorithm>

namespace ax
{

#if defined(WINAPI_FAMILY) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
#    define axbeep(t) MessageBeep(t)
#else
#    define axbeep(t)
#endif

static Label* _createLabel(std::string_view text,
                          std::string_view font,
                          float fontSize,
                          const Vec2& dimensions    = Vec2::ZERO,
                          TextHAlignment hAlignment = TextHAlignment::LEFT,
                          TextVAlignment vAlignment = TextVAlignment::TOP)
{
    if (FileUtils::getInstance()->isFileExist(font))
    {
        return Label::createWithTTF(text, font, fontSize, dimensions, hAlignment, vAlignment);
    }
    else
    {
        return Label::createWithSystemFont(text, font, fontSize, dimensions, hAlignment, vAlignment);
    }
}

static bool _checkVisibility(Node* theNode)
{
    // AX_ASSERT(theNode != NULL);
    bool visible = false;
    for (Node* ptr = theNode; (ptr != nullptr && (visible = ptr->isVisible())); ptr = ptr->getParent())
        ;
    return visible;
}

static bool _containsTouchPoint(ax::Node* target, ax::Touch* touch)
{
    assert(target != nullptr);

    ax::Point pt = target->convertTouchToNodeSpace(touch);

    const Vec2& size = target->getContentSize();

    ax::Rect rc(0, 0, size.width, size.height);

    bool contains = (rc.containsPoint(pt));

    // AXLOGD("check {:#x} coordinate:({}, {}), contains:{}", target, pt.x, pt.y, contains);
    return contains;
}

static bool engine_inj_containsPoint(ax::Node* target, const ax::Vec2& worldPoint)
{
    ax::Point pt = target->convertToNodeSpace(worldPoint);

    const Vec2& size = target->getContentSize();

    ax::Rect rc(0, 0, size.width, size.height);

    bool contains = (rc.containsPoint(pt));

    // AXLOGD("check {:#x} coordinate:({}, {}), contains:{}", target, pt.x, pt.y, contains);
    return contains;
}

static uint32_t engine_inj_c4b2dw(const Color4B& value)
{
    auto rvalue = (uint32_t)value.a << 24 | (uint32_t)value.b << 16 | (uint32_t)value.g << 8 | (uint32_t)value.r;
    return rvalue;
}

static Sprite* engine_inj_create_lump(const Color4B& color, int height, int width)
{
    unsigned int* pixels((unsigned int*)malloc(height * width * sizeof(unsigned int)));

    // Fill Pixels
    uint32_t* ptr           = pixels;
    const Color4B fillColor = Color4B::WHITE;
    for (int i = 0; i < height * width; ++i)
    {
        ptr[i] = engine_inj_c4b2dw(fillColor);  // 0xffffffff;
    }

    // create cursor by pixels
    Texture2D* texture = new Texture2D();

    texture->initWithData(pixels, height * width * sizeof(unsigned int), backend::PixelFormat::RGBA8, width, height);

    auto cursor = Sprite::createWithTexture(texture);

    cursor->setColor(Color3B(color));
    cursor->setOpacity(color.a);

    texture->release();

    free(pixels);

    return cursor;
}

namespace ui
{

// Input adapters supply valid UTF-8. Returned offsets are character boundaries.
static size_t inputNextByte(std::string_view text, size_t position)
{
    if (position >= text.size())
    {
        return text.size();
    }

    ++position;
    while (position < text.size() &&
           (static_cast<unsigned char>(text[position]) & 0xC0) == 0x80)
    {
        ++position;
    }
    return position;
}

static size_t inputPrefixBytes(std::string_view text, size_t codePoints)
{
    size_t position = 0;
    while (position < text.size() && codePoints > 0)
    {
        position = inputNextByte(text, position);
        --codePoints;
    }
    return position;
}

static size_t inputByteFromUtf16(std::string_view text, size_t offset)
{
    size_t position = 0;
    size_t units = 0;
    while (position < text.size())
    {
        const size_t next = inputNextByte(text, position);
        const size_t width = next - position == 4 ? 2 : 1;
        if (units + width > offset)
        {
            break;
        }
        units += width;
        position = next;
    }
    return position;
}

static size_t inputUtf16FromByte(std::string_view text, size_t offset)
{
    size_t position = 0;
    size_t units = 0;
    while (position < std::min(offset, text.size()))
    {
        const size_t next = inputNextByte(text, position);
        units += next - position == 4 ? 2 : 1;
        position = next;
    }
    return units;
}

static void internalSetLableFont(Label* l, std::string_view fontName, float fontSize)
{
    if (FileUtils::getInstance()->isFileExist(fontName))
    {
        TTFConfig config    = l->getTTFConfig();
        config.fontFilePath = fontName;
        config.fontSize     = fontSize;
        l->setTTFConfig(config);
    }
    else
    {
        l->setSystemFontName(fontName);
        l->requestSystemFontRefresh();
        l->setSystemFontSize(fontSize);
    }
}

static float internalCalcStringWidth(std::string_view s, std::string_view fontName, float fontSize)
{
    auto label = _createLabel(std::string{s}, fontName, fontSize);
    return label->getContentSize().width;
}

//////////////////////////////////////////////////////////////////////////
// constructor and destructor
//////////////////////////////////////////////////////////////////////////
bool TextFieldEx::s_keyboardVisible = false;
TextFieldEx::TextFieldEx()
    : _editable(true)
    , _renderLabel(nullptr)
    , _charCount(0)
    , _inputText("")
    , _placeHolder("")
    , _colorText(Color4B::WHITE)
    , _colorSpaceHolder(Color4B::GRAY)
    , _secureTextEntry(false)
    , _cursor(nullptr)
    , _touchListener(nullptr)
    , _kbdListener(nullptr)
    , onTextModify(nullptr)
    , onOpenIME(nullptr)
    , onCloseIME(nullptr)
    , _charLimit(std::numeric_limits<int>::max())
    , _systemFontUsed(false)
    , _fontSize(24)
    , _insertPosUtf8(0)
    , _insertPos(0)
    , _cursorPos(0)
    , _touchCursorControlEnabled(true)
    , _cursorVisible(false)
    , _continuousTouchDelayTimerID(nullptr)
    , _continuousTouchDelayTime(0.6)
{}

TextFieldEx::~TextFieldEx()
{
    if (_kbdListener != nullptr)
        _eventDispatcher->removeEventListener(_kbdListener);
    if (_touchListener != nullptr)
        _eventDispatcher->removeEventListener(_touchListener);
}

//////////////////////////////////////////////////////////////////////////
// static constructor
//////////////////////////////////////////////////////////////////////////
TextFieldEx* TextFieldEx::create(std::string_view placeholder,
                                 std::string_view fontName,
                                 float fontSize,
                                 float cursorWidth,
                                 const Color4B& cursorColor)
{
    TextFieldEx* ret = new TextFieldEx();
    if (ret && ret->initWithPlaceHolder("", fontName, fontSize, cursorWidth, cursorColor))
    {
        ret->autorelease();
        if (placeholder.size() > 0)
        {
            ret->setPlaceholderText(placeholder);
        }
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// initialize
//////////////////////////////////////////////////////////////////////////
bool TextFieldEx::initWithPlaceHolder(std::string_view placeholder,
                                      std::string_view fontName,
                                      float fontSize,
                                      float cursorWidth,
                                      const Color4B& cursorColor)
{
    _placeHolder = placeholder;

    _renderLabel =
        _createLabel(placeholder, fontName, fontSize, Vec2::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
    _renderLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
    this->addChild(_renderLabel);

    _director->getScheduler()->runOnAxmolThread([this] { _renderLabel->setPosition(Point(0, this->getContentSize().height / 2)); });

    __initCursor(fontSize, cursorWidth, cursorColor);

    _fontName       = fontName;
    _fontSize       = fontSize;
    _systemFontUsed = !FileUtils::getInstance()->isFileExist(fontName);

    return true;
}

std::string_view TextFieldEx::getTextFontName() const
{
    return _fontName;
}

void TextFieldEx::setTextFontName(std::string_view fontName)
{
    if (FileUtils::getInstance()->isFileExist(fontName))
    {
        TTFConfig config    = _renderLabel->getTTFConfig();
        config.fontFilePath = fontName;
        config.fontSize     = _fontSize;
        _renderLabel->setTTFConfig(config);
        _systemFontUsed = false;
        _fontType      = 1;
    }
    else
    {
        _renderLabel->setSystemFontName(fontName);
        if (!_systemFontUsed)
        {
            _renderLabel->requestSystemFontRefresh();
        }
        _renderLabel->setSystemFontSize(_fontSize);
        _systemFontUsed = true;
        _fontType      = 0;
    }
    _fontName = fontName;

    using namespace std::string_view_literals;
    _asteriskWidth = internalCalcStringWidth("*"sv, _fontName, _fontSize);
}

void TextFieldEx::setTextFontSize(float size)
{
    if (_systemFontUsed)
    {
        _renderLabel->setSystemFontSize(size);
    }
    else
    {
        TTFConfig config = _renderLabel->getTTFConfig();
        config.fontSize  = size;
        _renderLabel->setTTFConfig(config);
    }

    _fontSize = size;

    using namespace std::string_view_literals;
    _asteriskWidth = internalCalcStringWidth("*"sv, _fontName, _fontSize);
}

float TextFieldEx::getTextFontSize() const
{
    return _fontSize;
}

void TextFieldEx::enableIME(Node* control)
{
    if (_touchListener != nullptr)
    {
        return;
    }
    _touchListener = EventListenerTouchOneByOne::create();

    // this->getContentSize() == Vec2::ZERO, so we need to use the size of _renderLabel;
    if(control == nullptr)
        control = this->_renderLabel;

    _touchListener->onTouchBegan = [control,this](Touch* touch, Event*) {
        bool focus = (_checkVisibility(this) && _editable && this->_enabled &&
                      _containsTouchPoint(control, touch));

        if (this->_continuousTouchDelayTimerID != nullptr)
        {
            stimer::kill(this->_continuousTouchDelayTimerID);
            this->_continuousTouchDelayTimerID = nullptr;
        }

        if (focus && _cursorVisible)
        {
            auto worldPoint = touch->getLocation();
            if (this->_continuousTouchCallback)
            {
                this->_continuousTouchDelayTimerID = stimer::delay(
                    this->_continuousTouchDelayTime, [=, this]() { this->_continuousTouchCallback(worldPoint); });
            }
        }
        return true;
    };
    _touchListener->onTouchEnded = [control, this](Touch* touch, Event* e) {
        if (this->_continuousTouchDelayTimerID != nullptr)
        {
            stimer::kill(this->_continuousTouchDelayTimerID);
            this->_continuousTouchDelayTimerID = nullptr;
        }

        bool focus = (_checkVisibility(this) && _editable && this->_enabled &&
                      _containsTouchPoint(control, touch));

        if (focus)
        {
            if (!s_keyboardVisible || !_cursorVisible)
                openIME();
            if (_touchCursorControlEnabled)
            {
                auto renderLabelPoint = _renderLabel->convertToNodeSpace(touch->getLocation());
                __moveCursorTo(renderLabelPoint.x);
            }
        }
        else
        {
            closeIME();
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

    /// enable use keyboard <- -> to move cursor.
    _kbdListener               = EventListenerKeyboard::create();
    _kbdListener->onKeyPressed = [this](EventKeyboard::KeyCode code, Event*) {
        if (_cursorVisible)
        {
            switch (code)
            {
            case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            case EventKeyboard::KeyCode::KEY_DPAD_LEFT:
                this->__moveCursor(-1);
                break;
            case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            case EventKeyboard::KeyCode::KEY_DPAD_RIGHT:
                this->__moveCursor(1);
                break;
            case EventKeyboard::KeyCode::KEY_DELETE:
            case EventKeyboard::KeyCode::KEY_KP_DELETE:
                this->handleDeleteKeyEvent();
                break;
            default:;
            }
        }
    };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(_kbdListener, this);
}

void TextFieldEx::disableIME(void)
{
    _eventDispatcher->removeEventListener(_kbdListener);
    _eventDispatcher->removeEventListener(_touchListener);

    _kbdListener   = nullptr;
    _touchListener = nullptr;
    closeIME();
}

Label* TextFieldEx::getRenderLabel()
{
    return _renderLabel;
}

//////////////////////////////////////////////////////////////////////////
// IMEDelegate
//////////////////////////////////////////////////////////////////////////

bool TextFieldEx::attachWithIME()
{
    bool ret = IMEDelegate::attachWithIME();
    if (ret)
    {
        // open keyboard
        RenderView* renderView = _director->getRenderView();
        if (renderView)
            renderView->setIMEKeyboardState(true);
    }
    return ret;
}

bool TextFieldEx::detachWithIME()
{
    bool ret = IMEDelegate::detachWithIME();
    if (ret)
    {
        // close keyboard
        RenderView* renderView = _director->getRenderView();
        if (renderView)
            renderView->setIMEKeyboardState(false);
    }
    return ret;
}

void TextFieldEx::keyboardDidShow(IMEKeyboardNotificationInfo& /*info*/)
{
    s_keyboardVisible = true;
}

void TextFieldEx::keyboardDidHide(IMEKeyboardNotificationInfo& /*info*/)
{
    s_keyboardVisible = false;
}

void TextFieldEx::openIME(void)
{
    AXLOGD("TextFieldEx:: openIME");
    this->attachWithIME();
    __updateCursorPosition();
    __showCursor();

    if (this->onOpenIME)
        this->onOpenIME();
}

void TextFieldEx::closeIME(void)
{
    AXLOGD("TextFieldEx:: closeIME");
    __hideCursor();
    this->detachWithIME();

    if (this->onCloseIME)
        this->onCloseIME();
}

bool TextFieldEx::canAttachWithIME()
{
    return true;  //(_delegate) ? (! _delegate->onTextFieldAttachWithIME(this)) : true;
}

bool TextFieldEx::canDetachWithIME()
{
    return true;  //(_delegate) ? (! _delegate->onTextFieldDetachWithIME(this)) : true;
}

bool TextFieldEx::supportsTextInputRanges() const
{
    return true;
}

int TextFieldEx::getTextSelection() const
{
    return static_cast<int>(inputUtf16FromByte(_inputText, static_cast<size_t>(_insertPos)));
}

void TextFieldEx::__setCursorBytePosition(size_t position)
{
    position = std::min(position, _inputText.size());
    while (position > 0 && position < _inputText.size() &&
           (static_cast<unsigned char>(_inputText[position]) & 0xC0) == 0x80)
    {
        --position;
    }

    _insertPos = static_cast<int>(position);
    _insertPosUtf8 = static_cast<int>(
        StringUtils::countUTF8Chars(std::string_view(_inputText.data(), position)));

    const std::string_view display = _inputText.empty() ? std::string_view{} :
        (_secureTextEntry ? _renderLabel->getString() : std::string_view(_inputText));
    _cursorPos = static_cast<int>(_secureTextEntry ?
        inputPrefixBytes(display, static_cast<size_t>(_insertPosUtf8)) : position);

    if (_cursor)
    {
        const float width = _cursorPos == 0 ? 0.0f :
            internalCalcStringWidth(display.substr(0, static_cast<size_t>(_cursorPos)), _fontName, _fontSize);
        _cursor->setPosition(Point(width, this->getContentSize().height / 2));
    }
}

void TextFieldEx::setTextSelection(int start, int end)
{
    if (start < 0 || end < 0)
    {
        return;
    }
    // This widget draws one caret; the input adapter retains the selection range.
    __setCursorBytePosition(inputByteFromUtf16(_inputText, static_cast<size_t>(end)));
}

void TextFieldEx::replaceTextRange(int start, int end, std::string_view replacement)
{
    if (!_editable || !this->_enabled || start < 0 || end < start)
    {
        return;
    }

    const size_t utf16Length = inputUtf16FromByte(_inputText, _inputText.size());
    if (static_cast<size_t>(end) > utf16Length)
    {
        return;
    }

    const size_t first = inputByteFromUtf16(_inputText, static_cast<size_t>(start));
    const size_t last = inputByteFromUtf16(_inputText, static_cast<size_t>(end));
    const size_t removed = StringUtils::countUTF8Chars(
        std::string_view(_inputText.data() + first, last - first));
    const size_t retained = _charCount - removed;

    const auto newline = replacement.find('\n');
    const bool finish = newline != std::string_view::npos;
    if (finish)
    {
        replacement = replacement.substr(0, newline);
    }

    const size_t allowed = _charLimit == 0 ? replacement.size() :
        (_charLimit > retained ? _charLimit - retained : 0);
    const size_t bytes = inputPrefixBytes(replacement, allowed);
    std::string updated = _inputText;
    updated.replace(first, last - first, bytes == 0 ? "" : replacement.data(), bytes);
    const bool changed = updated != _inputText;
    setString(updated);
    __setCursorBytePosition(first + bytes);

    if (changed && onTextModify)
    {
        onTextModify();
    }
    if (finish)
    {
        closeIME();
    }
}

void TextFieldEx::insertText(const char* text, size_t len)
{
    if (!text || len == 0)
    {
        return;
    }
    const int position = getTextSelection();
    replaceTextRange(position, position, std::string_view(text, len));
}

void TextFieldEx::deleteBackward(size_t numChars)
{
    if (!_editable || !this->_enabled || numChars == 0)
    {
        return;
    }

    size_t first = std::min(static_cast<size_t>(std::max(_insertPos, 0)), _inputText.size());
    const size_t last = first;
    while (first > 0 && numChars > 0)
    {
        --first;
        while (first > 0 &&
               (static_cast<unsigned char>(_inputText[first]) & 0xC0) == 0x80)
        {
            --first;
        }
        --numChars;
    }

    replaceTextRange(static_cast<int>(inputUtf16FromByte(_inputText, first)),
                     static_cast<int>(inputUtf16FromByte(_inputText, last)), {});
}

void TextFieldEx::handleDeleteKeyEvent()
{
    const size_t first = std::min(static_cast<size_t>(std::max(_insertPos, 0)), _inputText.size());
    const size_t last = inputNextByte(_inputText, first);
    replaceTextRange(static_cast<int>(inputUtf16FromByte(_inputText, first)),
                     static_cast<int>(inputUtf16FromByte(_inputText, last)), {});
}

size_t TextFieldEx::getContentTextMaxLength()
{
    return _charLimit;
}

std::string_view TextFieldEx::getContentText()
{
    return _inputText;
}

void TextFieldEx::setTextColor(const Color4B& color)
{
    _colorText = color;
    if (!_inputText.empty())
        _renderLabel->setTextColor(_colorText);
}

const Color4B& TextFieldEx::getTextColor(void) const
{
    return _colorText;
}

void TextFieldEx::setCursorColor(const Color3B& color)
{
    _cursor->setColor(color);
}

const Color3B& TextFieldEx::getCursorColor(void) const
{
    return _cursor->getColor();
}

const Color4B& TextFieldEx::getPlaceholderColor() const
{
    return _colorSpaceHolder;
}

void TextFieldEx::setPlaceholderColor(const Color4B& color)
{
    _colorSpaceHolder = color;
    if (_inputText.empty())
        _renderLabel->setTextColor(color);
}

//////////////////////////////////////////////////////////////////////////
// properties
//////////////////////////////////////////////////////////////////////////

// input text property
void TextFieldEx::setString(std::string_view text)
{
    static char bulletString[] = {(char)0xe2, (char)0x80, (char)0xa2, (char)0x00};

    _inputText = text;

    std::string secureText;

    std::string* displayText = &_inputText;

    if (!_inputText.empty())
    {
        if (_secureTextEntry)
        {
            size_t length = _inputText.length();
            displayText   = &secureText;

            while (length > 0)
            {
                displayText->append(bulletString);
                --length;
            }
        }
    }

    // if there is no input text, display placeholder instead
    if (_inputText.empty())
    {
        _renderLabel->setTextColor(_colorSpaceHolder);
        _renderLabel->setString(_placeHolder);
    }
    else
    {
        _renderLabel->setTextColor(_colorText);
        _renderLabel->setString(*displayText);
    }

    bool bInsertAtEnd = (_insertPosUtf8 == _charCount);

    _charCount = StringUtils::countUTF8Chars(_inputText);

    if (bInsertAtEnd)
    {
        _insertPosUtf8 = static_cast<int>(_charCount);
        _insertPos     = static_cast<int>(_inputText.length());
        _cursorPos     = static_cast<int>(displayText->length());
    }
}

void TextFieldEx::updateContentSize(void)
{
    this->setContentSize(_renderLabel->getContentSize());
}

std::string_view TextFieldEx::getString() const
{
    return _inputText;
}

// place holder text property
void TextFieldEx::setPlaceholderText(std::string_view text)
{
    _placeHolder = text;
    if (_inputText.empty())
    {
        _renderLabel->setTextColor(_colorSpaceHolder);
        _renderLabel->setString(_placeHolder);
    }
}

std::string_view TextFieldEx::getPlaceholderText() const
{
    return _placeHolder;
}

// secureTextEntry
void TextFieldEx::setPasswordEnabled(bool value)
{
    if (_secureTextEntry != value)
    {
        _secureTextEntry = value;
        this->setString(this->getString());
        __updateCursorPosition();
    }
}

bool TextFieldEx::isPasswordEnabled() const
{
    return _secureTextEntry;
}

void TextFieldEx::setEnabled(bool bEnabled)
{
    if (this->_enabled != bEnabled)
    {
        if (!bEnabled)
        {
            this->closeIME();
        }
        this->_enabled = bEnabled;
    }
}

int TextFieldEx::getFontType() const
{
    return _fontType;
}

void TextFieldEx::__initCursor(int height, int width, const Color4B& color)
{
    _cursor = engine_inj_create_lump(Color4B(color), height, width);

    this->addChild(_cursor);

    _cursor->setPosition(Point(0, this->getContentSize().height / 2));
    // nodes_layout::setNodeLB(_cursor, ax::Point::ZERO);

    __hideCursor();

    __updateCursorPosition();
}

void TextFieldEx::__showCursor(void)
{
    if (_cursor)
    {
        _cursorVisible = true;
        _cursor->setVisible(true);
        _cursor->runAction(RepeatForever::create(Blink::create(1, 1)));
    }
}

void TextFieldEx::__hideCursor(void)
{
    if (_cursor)
    {
        _cursor->setVisible(false);
        _cursorVisible = false;
        _cursor->stopAllActions();
    }
}

void TextFieldEx::__updateCursorPosition(void)
{
    if (_cursor && _insertPosUtf8 == _charCount)
    {
        if (0 == this->getCharCount())
        {
            _cursor->setPosition(Point(0, this->getContentSize().height / 2));
        }
        else
        {
            _cursor->setPosition(Point(_renderLabel->getContentSize().width, this->getContentSize().height / 2));
        }
    }
}

void TextFieldEx::__notifySelectionChanged()
{
    IMEDispatcher::sharedDispatcher()->notifyTextSelectionChanged(this);
}

void TextFieldEx::__moveCursor(int direction)
{
    const int next = std::max(0, std::min(_insertPosUtf8 + direction, static_cast<int>(_charCount)));
    __setCursorBytePosition(inputPrefixBytes(_inputText, static_cast<size_t>(next)));
    __notifySelectionChanged();
}

void TextFieldEx::__moveCursorTo(float x)
{  // test
    // normalized x
    float normalizedX = 0;

    std::string_view displayText;
    if (!_secureTextEntry)
    {
        displayText = _inputText;
    }
    else
    {
        if (!_inputText.empty())
        {
            displayText = _renderLabel->getString();
        }
    }

    int length = static_cast<int>(displayText.length());
    int n      = static_cast<int>(_charCount);  // UTF8 char counter

    int insertWhere     = 0;
    int insertWhereUtf8 = 0;
    while (length > 0)
    {
        auto checkX = internalCalcStringWidth(displayText, _fontName, _fontSize);
        if (x >= checkX)
        {
            insertWhere     = length;
            insertWhereUtf8 = n;
            normalizedX     = checkX;
            break;
        }

        // clamp backward
        size_t backwardLen = 1;  // default, erase 1 byte
        while (0x80 == (0xC0 & displayText.at(displayText.length() - backwardLen)))
        {
            ++backwardLen;
        }

        --n;
        displayText.remove_suffix(backwardLen);

        length -= backwardLen;
    }

    _insertPos     = static_cast<int>(inputPrefixBytes(_inputText, static_cast<size_t>(insertWhereUtf8)));
    _cursorPos     = insertWhere;
    _insertPosUtf8 = insertWhereUtf8;
    _cursor->setPosition(Point(normalizedX, this->getContentSize().height / 2));
    __notifySelectionChanged();
}
};  // namespace ui

}

