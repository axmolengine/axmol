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

#include "axmol/ui/InputField.h"
#include "axmol/base/Director.h"
#include "axmol/base/text_utils.h"
#include "axmol/platform/Device.h"
#include "axmol/platform/RenderView.h"

#include <algorithm>
#include <cstring>
#include <limits>

namespace ax::ui
{

#if defined(WINAPI_FAMILY) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
#    define axbeep(t) MessageBeep(t)
#else
#    define axbeep(t)
#endif

namespace
{
static constexpr int INPUT_FIELD_RENDERER_Z             = (-1);
static constexpr std::string_view DEFAULT_PASSWORD_CHAR = "\xe2\x80\xa2"sv;
static FontType labelTypeToFontType(Label::LabelType type)
{
    switch (type)
    {
    case Label::LabelType::STRING_TEXTURE:
        return FontType::SYSTEM;
    case Label::LabelType::TTF:
        return FontType::TTF;
    case Label::LabelType::BMFONT:
        return FontType::BMFONT;
    default:
        return FontType::SYSTEM;
    }
}

static Node* findUIRoot(Node* current)
{
    ax::Node* top2DNode = nullptr;

    // Get the target mask bit for the 2D default camera (usually 1)
    uint16_t default2DMask = static_cast<uint16_t>(ax::CameraFlag::DEFAULT);

    // Climb up the scene graph
    while (current != nullptr)
    {
        auto parent = current->getParent();

        // Check if the parent is the Scene (meaning 'current' is a top-level node)
        if (parent && dynamic_cast<ax::Scene*>(parent) != nullptr)
        {
            // Verify if this top-level node is rendered by the 2D default camera
            if (current->getCameraMask() & default2DMask)
            {
                top2DNode = current;
                break;
            }
        }
        current = parent;
    }

    // Fallback to parent if no scene-level top node matched
    return top2DNode ? top2DNode : current->getParent();
}

}  // namespace

//////////////////////////////////////////////////////////////////////////
// constructor and destructor
//////////////////////////////////////////////////////////////////////////
bool InputField::s_keyboardVisible = false;

IMPLEMENT_CLASS_GUI_INFO(InputField)

InputField::InputField()
{
    _passwordChar = DEFAULT_PASSWORD_CHAR;
}

InputField::~InputField()
{
    if (_kbdListener != nullptr)
        _eventDispatcher->removeEventListener(_kbdListener);

    // Release instance-specific measure label
    AX_SAFE_RELEASE_NULL(_measureLabel);
}

//////////////////////////////////////////////////////////////////////////
// static constructor
//////////////////////////////////////////////////////////////////////////

InputField* InputField::create()
{
    InputField* ret = new InputField();
    if (ret && ret->initWithPlaceholder(""sv, "Arial"sv, 24, 2, Color32::BLACK))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}
InputField* InputField::create(std::string_view placeholder,
                               std::string_view fontName,
                               float fontSize,
                               float cursorWidth,
                               const Color32& cursorColor)
{
    InputField* ret = new InputField();
    if (ret && ret->initWithPlaceholder(placeholder, fontName, fontSize, cursorWidth, cursorColor))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

//////////////////////////////////////////////////////////////////////////
// text measurement
//////////////////////////////////////////////////////////////////////////
Vec2 InputField::measureText(std::string_view text) const
{
    // Lazy initialize instance-specific measure label
    if (!_measureLabel)
    {
        _measureLabel = Label::create(""sv, _fontName, _fontSize);
        _measureLabel->retain();  // Manual reference counting
    }

    // Measure text (font is already synced in setFontName/setFontSize)
    _measureLabel->setString(text);
    return _measureLabel->getContentSize();
}

//////////////////////////////////////////////////////////////////////////
// initialize
//////////////////////////////////////////////////////////////////////////
bool InputField::initWithPlaceholder(std::string_view placeholder,
                                     std::string_view fontName,
                                     float fontSize,
                                     float cursorWidth,
                                     const Color32& cursorColor)
{

    ui::Widget::init();

    _fontName        = fontName;
    _fontSize        = fontSize;
    _placeholderText = placeholder;

    /// render label
    // Create render label according to font type: BMFont (.fnt), TTF file, or system font
    _renderLabel = Label::create(placeholder, fontName, fontSize);
    AXASSERT(_renderLabel, "Failed to create render label for InputField");
    _fontType = labelTypeToFontType(_renderLabel->getLabelType());

    if (!placeholder.empty())
        _renderLabel->setTextColor(_colorSpaceHolder);
    this->addProtectedChild(_renderLabel, INPUT_FIELD_RENDERER_Z);

    _renderLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);

    /// selection layer
    _selectionLayer = DrawNode::create();
    this->addProtectedChild(_selectionLayer);

    /// Line Metrics
    updateLineHeight();

    _passwordCharWidth = measureText(_passwordChar).width;

    // Initialize content size based on placeholder text
    updateContentSize();

    /// cursor
    _cursor = Sprite::createWithTexture(_director->getTextureCache()->getWhiteTexture());
    _cursor->setContentSize(Vec2{cursorWidth, _fontSize});
    this->addProtectedChild(_cursor);
    hideCursor();

    markDirty(DIRTY_CHAR_OFFSETS | DIRTY_LINE_METRICS);

    return true;
}

void InputField::onEnter()
{
    Widget::onEnter();

    setPointerEnabled(true);

    // Enable keyboard listener for cursor control and shortcuts
    if (!_kbdListener)
    {
        _kbdListener = KeyboardEventListener::create();
        AX_SAFE_RETAIN(_kbdListener);
        _kbdListener->onKeyPressed = [this](KeyboardEvent* event) {
            auto code = event->getKeyCode();
            switch (code)
            {
            case KeyboardEvent::KeyCode::KEY_CTRL:
            case KeyboardEvent::KeyCode::KEY_RIGHT_CTRL:
            case KeyboardEvent::KeyCode::KEY_HYPER:
                _ctrlKeyPressed = true;
                return;
            case KeyboardEvent::KeyCode::KEY_SHIFT:
            case KeyboardEvent::KeyCode::KEY_RIGHT_SHIFT:
                _shiftKeyPressed = true;
                return;
            default:
                break;
            }

            if (isCursorVisible())
            {
                if (_ctrlKeyPressed)
                {
                    switch (code)
                    {
                    case KeyboardEvent::KeyCode::KEY_A:
                    case KeyboardEvent::KeyCode::KEY_CAPITAL_A:
                        this->selectAll();
                        return;
                    case KeyboardEvent::KeyCode::KEY_C:
                    case KeyboardEvent::KeyCode::KEY_CAPITAL_C:
                        this->copySelectionToClipboard();
                        return;
                    case KeyboardEvent::KeyCode::KEY_X:
                    case KeyboardEvent::KeyCode::KEY_CAPITAL_X:
                        this->cutSelectionToClipboard();
                        return;
#if AX_TARGET_PLATFORM != AX_PLATFORM_WINRT  // Windows UWP TextBox will dispatchInsertText event with clipboard text,
                                             // so no need to handle paste action in keyboard event.
                    case KeyboardEvent::KeyCode::KEY_V:
                    case KeyboardEvent::KeyCode::KEY_CAPITAL_V:
                        this->pasteFromClipboard();
                        return;
#endif
                    default:
                        break;
                    }
                }

                switch (code)
                {
                case KeyboardEvent::KeyCode::KEY_LEFT_ARROW:
                    this->moveCursor(-1, _shiftKeyPressed);
                    break;
                case KeyboardEvent::KeyCode::KEY_RIGHT_ARROW:
                    this->moveCursor(1, _shiftKeyPressed);
                    break;
                case KeyboardEvent::KeyCode::KEY_DELETE:
                case KeyboardEvent::KeyCode::KEY_KP_DELETE:
                    this->handleDeleteKeyEvent();
                    break;
                case KeyboardEvent::KeyCode::KEY_UP_ARROW:
                    this->moveCursorVertically(-1);
                    break;
                case KeyboardEvent::KeyCode::KEY_DOWN_ARROW:
                    this->moveCursorVertically(1);
                    break;
                default:;
                }
            }
        };
        _kbdListener->onKeyReleased = [this](KeyboardEvent* event) {
            switch (event->getKeyCode())
            {
            case KeyboardEvent::KeyCode::KEY_CTRL:
            case KeyboardEvent::KeyCode::KEY_RIGHT_CTRL:
            case KeyboardEvent::KeyCode::KEY_HYPER:
                _ctrlKeyPressed = false;
                break;
            case KeyboardEvent::KeyCode::KEY_SHIFT:
            case KeyboardEvent::KeyCode::KEY_RIGHT_SHIFT:
                _shiftKeyPressed = false;
                break;
            default:
                break;
            }
        };

        _eventDispatcher->addEventListenerWithSceneGraphPriority(_kbdListener, this);
    }
}

void InputField::onExit()
{
    detachWithIME();

    if (_kbdListener)
    {
        _eventDispatcher->removeEventListener(_kbdListener);
        AX_SAFE_RELEASE_NULL(_kbdListener);
    }
    setPointerEnabled(false);
    Widget::onExit();
}

std::string_view InputField::getFontName() const
{
    return _fontName;
}

void InputField::setFontName(std::string_view fontName)
{
    _renderLabel->setFontInfo(fontName, _fontSize);

    _fontName = fontName;
    _fontType = labelTypeToFontType(_renderLabel->getLabelType());

    // Sync measure label font
    if (_measureLabel)
        _measureLabel->setFontInfo(fontName, _fontSize);

    _passwordCharWidth = measureText(_passwordChar).width;
    updateLineHeight();
    _cursor->setContentSize(Vec2{_cursor->getContentSize().width, _fontSize});
    markDirty(DIRTY_LINE_METRICS);
}

void InputField::setFontSize(float size)
{
    if (_fontSize == size)
        return;

    if (_fontType == FontType::BMFONT)
    {
        // BMFont size cannot be changed at runtime; ignore size change
        return;
    }
    if (_fontType == FontType::SYSTEM)
    {
        _renderLabel->setSystemFontSize(size);
    }
    else if (_fontType == FontType::TTF)
    {
        TTFConfig config = _renderLabel->getTTFConfig();
        config.fontSize  = size;
        _renderLabel->setTTFConfig(config);
    }

    _fontSize = size;

    // Sync measure label to the same font type and configuration as the render label.
    if (_measureLabel)
    {
        if (_fontType == FontType::SYSTEM)
            _measureLabel->setSystemFontSize(_fontSize);
        else if (_fontType == FontType::TTF)
            _measureLabel->setTTFConfig(_renderLabel->getTTFConfig());
    }

    _passwordCharWidth = measureText(_passwordChar).width;
    updateLineHeight();
    markDirty(DIRTY_LINE_METRICS);
}

float InputField::getFontSize() const
{
    return _fontSize;
}

Label* InputField::getRenderLabel()
{
    return _renderLabel;
}

//////////////////////////////////////////////////////////////////////////
// InputDelegate
//////////////////////////////////////////////////////////////////////////

bool InputField::attachWithIME()
{
    if (_isAttachWithIME)
        return true;
    bool ret = InputDelegate::attachWithIME();
    if (ret)
    {
        // Initialize touch selection state
        _selectingByTouch    = false;
        _selectionTouchMoved = false;
        _isAttachWithIME     = true;

        // Open keyboard
        auto renderView = _director->getRenderView();
        if (renderView)
            renderView->setIMEKeyboardState(true);

        showCursor();
        dispatchEvent(EventType::ATTACH_WITH_IME);

        markDirty(DIRTY_CURSOR);
    }
    return ret;
}

bool InputField::detachWithIME()
{
    bool ret = InputDelegate::detachWithIME();
    if (ret)
    {
        _isAttachWithIME = false;

        // Hide cursor immediately
        hideCursor();

        // Close keyboard
        auto renderView = _director->getRenderView();
        if (renderView)
            renderView->setIMEKeyboardState(false);

        // Dispatch event before removing listeners
        dispatchEvent(EventType::DETACH_WITH_IME);
    }
    return ret;
}

bool InputField::hitTestWithIME(const Vec2& location)
{
    const Camera* camera = _hittedByCamera ? _hittedByCamera : Camera::getDefaultCamera();
    return hitTestSelf(location, camera, nullptr);
}

void InputField::keyboardDidShow(IMEKeyboardNotificationInfo& /*info*/)
{
    s_keyboardVisible = true;
}

void InputField::keyboardDidHide(IMEKeyboardNotificationInfo& /*info*/)
{
    _director->getRenderView()->hideContextMenu();

    s_keyboardVisible = false;
    bool ret          = InputDelegate::detachWithIME();
    if (ret)
    {
        _isAttachWithIME = false;

        // Hide cursor immediately
        hideCursor();

        dispatchEvent(EventType::DETACH_WITH_IME);
    }
}

bool InputField::isCursorVisible() const
{
    return _cursor && _cursor->isVisible();
}

bool InputField::canAttachWithIME() const
{
    return true;
}

bool InputField::canDetachWithIME() const
{
    return true;
}

void InputField::setCharLimit(uint32_t limit)
{
    _charLimit = limit;

    // If the new limit is non-zero and shorter than current text, truncate
    // the stored input so the control remains in a valid state. Use
    // setString() to ensure rendering, prefix caches and cursor are updated
    // consistently.
    if (_charLimit > 0 && _charCount > _charLimit)
    {
        auto byteOffset = getByteOffset(static_cast<int>(_charLimit));
        std::string_view textView(_inputText);
        setString(textView.substr(0, byteOffset));
    }
}

void InputField::insertText(std::string_view text)
{
    if (_readOnly || !this->_enabled)
        return;

    // In multiline mode we allow newline characters; otherwise they signify commit and we strip them.
    if (!_multilineEnabled)
    {
        auto pos = text.find_first_of("\r\n");
        if (pos != std::string::npos)
        {
            text = text.substr(0, pos);
            detachWithIME();
        }
        if (text.empty())
            return;
    }

    if (hasSelection())
        deleteSelection(false);

    size_t insertionCharCount;

    // Character limit
    if (_charLimit > 0)
    {
        int remaining = static_cast<int>(_charLimit - _charCount);
        if (remaining <= 0)
        {
            axbeep(0);
            return;
        }
        // Truncate insertStr to not exceed limit
        auto result = text_utils::countUTF8WithLimit(text, remaining);
        if (!result)
        {
            axbeep(0);
            return;
        }
        text               = text.substr(0, result.byteCount);
        insertionCharCount = result.charCount;
    }
    else
    {
        insertionCharCount = text_utils::countUTF8Chars(text);
    }

    if (text.empty())
        return;

    markDirty(DIRTY_TEXT_CONTENT);

    auto prevCursorOffset = static_cast<int>(_cursorCharOffset);
    bool bInsertAtEnd     = (prevCursorOffset == static_cast<int>(_charCount));

    // 1. Performance optimization: Modify _inputText in-place to avoid full string allocation or copying
    _inputText.insert(_cursorByteOffset, text.data(), text.size());

    // 2. Incremental optimization: Simply add the new character count to the existing total
    auto newCharCount = _charCount + static_cast<uint32_t>(insertionCharCount);

    updateLogicalLimits(newCharCount);

    // 3. Resolve cursor positioning logic that previously relied on updatePresentation parameters
    if (bInsertAtEnd)  // Equivalent to your original condition when inserting at the string tail
    {
        setCursorOffset(_charCount, false);
    }
    else
    {
        int newCursorOffset = prevCursorOffset + static_cast<int>(insertionCharCount);
        setCursorOffset(newCursorOffset, hasSelection());
    }

    dispatchEvent(EventType::INSERT_TEXT);
}

void InputField::deleteBackward(unsigned int numChars)
{
    if (hasSelection())
    {
        deleteSelection(true);
        return;
    }

    if (_readOnly || !this->_enabled || 0 == _charCount)
    {
        axbeep(0);
        return;
    }

    int len = static_cast<int>(_inputText.length());
    if (0 == len || _cursorByteOffset == 0)
    {
        axbeep(0);
        return;
    }

    auto deleteChars     = (std::min)(static_cast<int>(numChars), _cursorCharOffset);
    auto startCharOffset = _cursorCharOffset - deleteChars;
    auto startByteOffset = getByteOffset(startCharOffset);
    auto totalDeleteLen  = _cursorByteOffset - startByteOffset;

    markDirty(DIRTY_TEXT_CONTENT);

    // If all text is deleted, fast-clear the buffer
    if (len <= totalDeleteLen)
    {
        _inputText.clear();
        updateLogicalLimits(0);
        setCursorOffset(0, false);
    }
    else
    {
        // Performance optimization: Erase characters in-place to avoid copying the whole string
        _inputText.erase(startByteOffset, totalDeleteLen);

        // Incremental optimization: Safely subtract the deleted character count
        uint32_t newCharCount = (_charCount > static_cast<uint32_t>(deleteChars)) ? (_charCount - deleteChars) : 0;

        updateLogicalLimits(newCharCount);

        // Update the final cursor position to the deletion starting point
        setCursorOffset(startCharOffset, false);
    }

    dispatchEvent(EventType::DELETE_BACKWARD);
}

void InputField::handleDeleteKeyEvent()
{
    if (hasSelection())
    {
        deleteSelection(true);
        return;
    }

    if (_readOnly || !this->_enabled || 0 == _charCount)
    {
        axbeep(0);
        return;
    }

    int len = static_cast<int>(_inputText.length());
    if (0 == len || _cursorCharOffset == static_cast<int>(_charCount))
    {
        axbeep(0);
        return;
    }

    auto nextByteOffset = getByteOffset(_cursorCharOffset + 1);
    auto deleteLen      = nextByteOffset - _cursorByteOffset;

    markDirty(DIRTY_TEXT_CONTENT);

    // If all text is deleted, fast-clear the buffer
    if (len <= deleteLen)
    {
        _inputText.clear();
        updateLogicalLimits(0);
        setCursorOffset(0, false);
    }
    else
    {

        // Performance optimization: Erase the forward character in-place without heap allocations
        _inputText.erase(_cursorByteOffset, deleteLen);

        // Incremental optimization: Simply decrement the total character count by 1
        auto newCharCount = (_charCount > 0) ? (_charCount - 1) : 0;
        updateLogicalLimits(newCharCount);

        // Maintain the cursor position at the current cursor offset
        setCursorOffset(_cursorCharOffset, false);
    }

    dispatchEvent(EventType::DELETE_BACKWARD);
}

void InputField::setTextColor(const Color32& color)
{
    _colorText = color;
    if (!_inputText.empty())
        _renderLabel->setTextColor(_colorText);
}

const Color32& InputField::getTextColor(void) const
{
    return _colorText;
}

void InputField::setCursorColor(const Color32& color)
{
    _cursor->setColor(color);
}

const Color32& InputField::getCursorColor(void) const
{
    return _cursor->getColor();
}

const Color32& InputField::getPlaceholderColor() const
{
    return _colorSpaceHolder;
}

void InputField::setPlaceholderColor(const Color32& color)
{
    _colorSpaceHolder = color;
    if (_inputText.empty())
        _renderLabel->setTextColor(color);
}

//////////////////////////////////////////////////////////////////////////
// properties
//////////////////////////////////////////////////////////////////////////

void InputField::setMultilineEnabled(bool enabled)
{
    if (_multilineEnabled != enabled)
    {
        _multilineEnabled = enabled;
        markDirty(DIRTY_TEXT_GEOMETRY | DIRTY_LINE_METRICS);
    }
}

Node* InputField::getRenderNode()
{
    return _renderLabel;
}

Vec2 InputField::resolvePreferredSize(const Vec2& /*sizeHint*/) const
{
    return _renderLabel->getContentSize();
}

// input text property
void InputField::setString(std::string_view text)
{
    _inputText = text;

    auto charCount = static_cast<uint32_t>(text_utils::countUTF8Chars(_inputText));
    updateLogicalLimits(charCount);

    setCursorOffset(static_cast<int>(_charCount), false);

    // Defer text updating and geometry remeasuring to the layout pass
    markDirty(DIRTY_CHAR_OFFSETS | DIRTY_TEXT | DIRTY_LINE_METRICS | DIRTY_CURSOR);
}

void InputField::updateLogicalLimits(uint32_t newCharCount)
{
    _charCount = newCharCount;

    // Safely clamp selection ranges within the new valid logical bounds
    _selectionStart  = (std::min)(_selectionStart, static_cast<int>(_charCount));
    _selectionEnd    = (std::min)(_selectionEnd, static_cast<int>(_charCount));
    _selectionAnchor = (std::min)(_selectionAnchor, static_cast<int>(_charCount));
}

void InputField::updatePresentation()
{
    clearDirty(DIRTY_TEXT);  // Clear flag to support JIT queries safely

    if (_inputText.empty())
    {
        _renderLabel->setString(_placeholderText);
        _renderLabel->setTextColor(_colorSpaceHolder);
    }
    else
    {
        if (_passwordEnabled)
        {
            _renderLabel->setString(makePasswordString());
        }
        else
        {
            _renderLabel->setString(_inputText);
        }
        _renderLabel->setTextColor(_colorText);
    }
}

void InputField::updateLineHeight()
{
    _lineHeight = measureText("M").y;
}

std::string InputField::makePasswordString()
{
    std::string passwordStr;

    size_t length = _charCount;
    // Pre-reserve memory to prevent frequent reallocation during appending
    passwordStr.reserve(length * _passwordChar.size());

    while (length > 0)
    {
        passwordStr += _passwordChar;
        --length;
    }

    return passwordStr;
}

void InputField::updateContentSize()
{
    Widget::updateContentSize();

    if (_autoSize && _multilineEnabled)
    {
        float height        = _lineMetrics.size() * _lineHeight;
        _contentSize.height = height;
    }
    if (_autoSize)
    {
        markDirty(DIRTY_TEXT_GEOMETRY | DIRTY_LINE_METRICS);
    }
}

void InputField::onSizeChanged()
{
    Widget::onSizeChanged();
    markDirty(DIRTY_TEXT_GEOMETRY | DIRTY_LINE_METRICS);
}

std::string_view InputField::getString() const
{
    return _inputText;
}

void InputField::setPasswordChar(std::string_view ch)
{
    if (ch != _passwordChar)
    {
        _passwordChar      = !ch.empty() ? ch : DEFAULT_PASSWORD_CHAR;
        _passwordCharWidth = measureText(_passwordChar).width;

        // Changing password glyph affects texture and typography metrics
        markDirty(DIRTY_TEXT | DIRTY_LINE_METRICS | DIRTY_CURSOR);
    }
}

// place holder text property
void InputField::setPlaceholderText(std::string_view text)
{
    _placeholderText = text;
    markDirty(DIRTY_TEXT);  // Mark text presentation as dirty

    if (_inputText.empty())
        markDirty(DIRTY_LINE_METRICS);

    updateContentSize();
}

std::string_view InputField::getPlaceholderText() const
{
    return _placeholderText;
}

// secureTextEntry
void InputField::setPasswordEnabled(bool value)
{
    if (_passwordEnabled != value)
    {
        _passwordEnabled = value;

        // Changing to password mode requires text reshaping and layout re-evaluation
        markDirty(DIRTY_TYPOGRAPHY);
    }
}

bool InputField::isPasswordEnabled() const
{
    return _passwordEnabled;
}

void InputField::addEventListener(const InputFieldCallback& callback)
{
    _eventCallback = callback;
}

void InputField::dispatchEvent(EventType eventType)
{
    retain();

    if (_eventCallback)
        _eventCallback(this, eventType);

    if (_customEventCallback)
        _customEventCallback(this, static_cast<int>(eventType));

    release();
}

void InputField::setEnabled(bool bEnabled)
{
    if (_enabled != bEnabled)
    {
        _enabled = bEnabled;
        if (!bEnabled)
            this->detachWithIME();
    }
}

bool InputField::hasSelection() const
{
    return _selectionStart != _selectionEnd;
}

void InputField::selectAll()
{
    setSelection(0, _charCount);
    _selectionAnchor = 0;
    setCursorOffset(_charCount, true);
}

void InputField::clearSelection()
{
    setSelection(_cursorCharOffset, _cursorCharOffset);
    _selectionAnchor = _cursorCharOffset;
}

std::string InputField::getSelectedText() const
{
    if (!hasSelection())
        return {};

    auto startByte = getByteOffset(_selectionStart);
    auto endByte   = getByteOffset(_selectionEnd);

    return _inputText.substr(startByte, endByte - startByte);
}

bool InputField::copySelectionToClipboard() const
{
    auto selectedText = getSelectedText();
    if (selectedText.empty() || _passwordEnabled)
        return false;

    Device::setClipboardText(selectedText);
    return true;
}

bool InputField::cutSelectionToClipboard()
{
    if (_readOnly || !this->_enabled || !copySelectionToClipboard())
        return false;

    return deleteSelection(true);
}

bool InputField::pasteFromClipboard()
{
    if (_readOnly || !this->_enabled)
        return false;

    Device::getClipboardText([inputField = ax::RefPtr<InputField>(this)](std::string_view text) {
        if (!text.empty() && inputField->isRunning())
            inputField->insertText(text);
    });
    return true;
}

void InputField::setSelectionColor(const Color& color)
{
    _selectionColor = color;

    markDirty(DIRTY_SELECTION);
}

const Color& InputField::getSelectionColor() const
{
    return _selectionColor;
}

void InputField::showCursor()
{
    if (_cursor)
    {
        _cursor->setVisible(true);
        _cursor->setOpacity(255);  // Ensure full opacity

        // Use opacity-based blinking for better performance
        // Fade out to 0 over 0.5s, then fade in over 0.5s (total 1s cycle)
        auto fadeOut       = FadeOut::create(0.5f);
        auto fadeIn        = FadeIn::create(0.5f);
        auto blinkSequence = Sequence::create(fadeOut, fadeIn, nullptr);
        _cursor->runAction(RepeatForever::create(blinkSequence));
    }
}

void InputField::hideCursor()
{
    if (_cursor)
    {
        _cursor->stopAllActions();
        _cursor->setOpacity(255);  // Reset to full opacity
        _cursor->setVisible(false);
    }
}

void InputField::updateCursorTransform()
{
    Vec2 pos = cursorPositionFromOffset(_cursorCharOffset);
    if (_cursor)
    {
        // adjust cursor height to line height
        Vec2 size = _cursor->getContentSize();
        if (_lineHeight > 0)
            _cursor->setContentSize(Vec2(size.width, _fontSize));

        _cursor->setPosition(pos);
    }
    updateSelectionLayer();

    // Apply deferred preferred cursor X update
    if (isDirty(DIRTY_PREFERRED_X))
    {
        _preferredCursorX = pos.x;
        clearDirty(DIRTY_PREFERRED_X);
    }
}

void InputField::moveCursor(int direction)
{
    moveCursor(direction, false);
}

void InputField::moveCursor(int direction, bool keepSelection)
{
    auto cursorOffset = static_cast<int>(_cursorCharOffset) + direction;
    cursorOffset      = std::clamp(cursorOffset, 0, static_cast<int>(_charCount));
    setCursorOffset(cursorOffset, keepSelection);
}

void InputField::moveCursorTo(const Vec2& point)
{
    moveCursorTo(point, false);
}

void InputField::moveCursorTo(const Vec2& point, bool keepSelection)
{
    setCursorOffset(cursorOffsetFromPosition(point), keepSelection);
}

void InputField::setCursorOffset(int cursorOffset, bool keepSelection, bool markPrefxDirty)
{
    cursorOffset = (std::min)(cursorOffset, static_cast<int>(_charCount));

    if (keepSelection)
    {
        setSelection(_selectionAnchor, cursorOffset);
    }
    else
    {
        _selectionAnchor = cursorOffset;
        setSelection(cursorOffset, cursorOffset);
    }

    _cursorCharOffset = cursorOffset;
    _cursorByteOffset = getByteOffset(cursorOffset);

    if (markPrefxDirty)
        markDirty(DIRTY_PREFERRED_X);

    markDirty(DIRTY_CURSOR);
}

int InputField::getByteOffset(int cursorOffset) const
{
    if (isDirty(DIRTY_CHAR_OFFSETS))
        const_cast<InputField*>(this)->rebuildCharByteOffsets();

    if (cursorOffset >= _charCount)
        return static_cast<int>(_inputText.length());

    return _charByteOffsets[cursorOffset];
}

float InputField::getRenderLabelTextBottomY() const
{
    // Ensure layout metrics are up-to-date before calculating vertical offset
    if (isDirty(DIRTY_LINE_METRICS))
        const_cast<InputField*>(this)->rebuildLineMetrics();

    // If there is no render label, fallback to 0
    if (!_renderLabel)
        return 0.0f;

    // Get label container position and anchor in InputField local coords
    float labelPosY   = _renderLabel->getPositionY();
    float anchorY     = _renderLabel->getAnchorPoint().y;
    float labelHeight = _renderLabel->getContentSize().height;

    // Compute the bottom Y of the label container
    // labelBottomY = labelPosY - anchorY * labelHeight
    float labelBottomY = labelPosY - anchorY * labelHeight;
    float labelTopY    = labelBottomY + labelHeight;

    // Compute the actual text height based on line metrics and line height
    // If line metrics are not available, fallback to a single line height
    float totalTextHeight = static_cast<float>(_lineMetrics.size()) * _lineHeight;
    if (totalTextHeight <= 0.0f)
    {
        // fallback to a measured single-line height
        totalTextHeight = _lineHeight;
    }

    // Determine where the text block sits inside the label container
    // This must respect the vertical alignment used by the label rendering.
    // We use InputField's _textVAlignment which should match the label's valign.
    float textBottomYInLabel = labelBottomY;

    if (labelHeight > totalTextHeight)
    {
        if (_textVAlignment == TextVAlignment::TOP)
        {
            // Text is aligned to the top of the label container.
            // Text top equals labelTopY, so bottom is labelTopY - totalTextHeight.
            textBottomYInLabel = labelTopY - totalTextHeight;
        }
        else if (_textVAlignment == TextVAlignment::CENTER)
        {
            // Text is vertically centered inside the label container.
            // Bottom = labelBottomY + (labelHeight - totalTextHeight) / 2
            textBottomYInLabel = labelBottomY + (labelHeight - totalTextHeight) * 0.5f;
        }
        else  // TextVAlignment::BOTTOM
        {
            // Text is bottom-aligned; bottom equals labelBottomY
            textBottomYInLabel = labelBottomY;
        }
    }
    else
    {
        // If text fills or overflows the label container, clamp to label bottom.
        textBottomYInLabel = labelBottomY;
    }

    // If the Label exposes internal padding or baseline offsets, apply them here.
    // Example (pseudo): textBottomYInLabel += _renderLabel->getInternalPaddingBottom();

    return textBottomYInLabel;
}

void InputField::setSelection(int selStart, int selEnd)
{
    selStart = (std::min)(selStart, static_cast<int>(_charCount));
    selEnd   = (std::min)(selEnd, static_cast<int>(_charCount));
    if (selStart > selEnd)
        std::swap(selStart, selEnd);

    if (selStart != _selectionStart || selEnd != _selectionEnd)
    {
        _selectionStart = selStart;
        _selectionEnd   = selEnd;
    }

    markDirty(DIRTY_SELECTION);
}

void InputField::updateSelectionLayer(void)
{
    if (!_selectionLayer)
        return;
    _selectionLayer->clear();

    if (!hasSelection())
        return;

    int selStart = std::min(_selectionStart, _selectionEnd);
    int selEnd   = std::max(_selectionStart, _selectionEnd);

    if (selStart == selEnd)
        return;

    float labelTextBottomY = getRenderLabelTextBottomY();
    int totalLines         = static_cast<int>(_lineMetrics.size());

    for (size_t lineIdx = 0; lineIdx < _lineMetrics.size(); ++lineIdx)
    {
        const LineMetrics& line = _lineMetrics[lineIdx];

        if (line.endCharIndex <= selStart || line.startCharIndex >= selEnd)
            continue;

        int lineSelStart = std::max(selStart, line.startCharIndex) - line.startCharIndex;
        int lineSelEnd   = std::min(selEnd, line.endCharIndex) - line.startCharIndex;

        if (lineSelEnd <= lineSelStart)
            continue;

        float x1 = line.charXOffsets[lineSelStart];
        float x2 = line.charXOffsets[lineSelEnd];

        // apply horizontal alignment
        float totalTextWidth = line.lineWidth;
        float lineStartX     = 0.0f;

        if (_textHAlignment == TextHAlignment::CENTER)
            lineStartX = (_contentSize.width - totalTextWidth) * 0.5f;
        else if (_textHAlignment == TextHAlignment::RIGHT)
            lineStartX = _contentSize.width - totalTextWidth;

        float selX1 = lineStartX + x1;
        float selX2 = lineStartX + x2;

        // Invert the line index for vertical coordinates:
        // lineIdx = 0 should be rendered at the visual top.
        int invertedLineIdx = totalLines - 1 - static_cast<int>(lineIdx);

        // vertical coordinates using labelTextBottomY
        float selY1 = labelTextBottomY + invertedLineIdx * _lineHeight;
        float selY2 = selY1 + _lineHeight;

        _selectionLayer->drawSolidRect(Vec2(selX1, selY1), Vec2(selX2, selY2), _selectionColor);
    }
}

bool InputField::deleteSelection(bool notify)
{
    if (_readOnly || !this->_enabled || !hasSelection())
        return false;

    auto startByte = getByteOffset(_selectionStart);
    auto endByte   = getByteOffset(_selectionEnd);
    auto newCursor = _selectionStart;

    // Calculate how many characters are being removed before mutating the string
    auto deletedCharCount = static_cast<uint32_t>(_selectionEnd - _selectionStart);

    markDirty(DIRTY_CHAR_OFFSETS | DIRTY_LINE_METRICS);

    // Performance optimization: Erase the selected slice in-place
    _inputText.erase(startByte, endByte - startByte);

    // Incremental optimization: Deduct the exact selection block length
    auto newCharCount = (_charCount > deletedCharCount) ? (_charCount - deletedCharCount) : 0;

    updateLogicalLimits(newCharCount);

    // Reposition the cursor to where the selection started
    setCursorOffset(newCursor, false);

    markDirty(DIRTY_CHAR_OFFSETS | DIRTY_TEXT | DIRTY_LINE_METRICS);

    if (notify)
        dispatchEvent(EventType::DELETE_BACKWARD);

    return true;
}

//////////////////////////////////////////////////////////////////////////
// Touch Area and Hit Test
//////////////////////////////////////////////////////////////////////////
void InputField::setTouchAreaSize(const Vec2& size)
{
    _touchAreaSize = size;
}

Vec2 InputField::getTouchAreaSize() const
{
    return _touchAreaSize;
}

void InputField::setTouchAreaEnabled(bool enable)
{
    _useTouchArea = enable;
}

bool InputField::onPointerHitTest(PointerEvent* event, const Camera* camera, Vec3* outHitPoint)
{
    if (!event || !camera)
        return false;

    // Normal Widget path:
    // If the pointer is inside the input field, let Widget handle and cache _hitted/_hittedByCamera.
    if (Widget::onPointerHitTest(event, camera, outHitPoint))
        return true;

    // Important:
    // If this field currently owns IME focus, it must still receive PointerDown outside
    // itself so onPointerDown() can detachWithIME().
    //
    // Return true only for PointerDown. onPointerDown() will call Widget::onPointerDown(),
    // fail the hit test, detachWithIME(), and return false, so it will not capture or swallow.
    if (event->getPhase() == InputPhase::PointerDown && event->isPrimaryPressed() && _isAttachWithIME)
    {
        _hitted         = false;
        _hittedByCamera = camera;
        return true;
    }

    return false;
}

bool InputField::hitTestSelf(const Vec2& pt, const Camera* camera, Vec3* outHitPoint) const
{
    if (!camera)
        return false;

    if (!_useTouchArea)
    {
        return Widget::hitTestSelf(pt, camera, outHitPoint);
    }

    if (_touchAreaSize.width <= 0.0f || _touchAreaSize.height <= 0.0f)
        return false;

    const auto size = getContentSize();
    const auto anch = getAnchorPoint();

    Rect rect((size.width - _touchAreaSize.width) * anch.x, (size.height - _touchAreaSize.height) * anch.y,
              _touchAreaSize.width, _touchAreaSize.height);

    return camera->isWorldPointInRect(pt, getWorldToNodeTransform(), rect, outHitPoint);
}

//////////////////////////////////////////////////////////////////////////
// Text Alignment
//////////////////////////////////////////////////////////////////////////
void InputField::setTextHorizontalAlignment(TextHAlignment alignment)
{
    if (_textHAlignment != alignment)
    {
        _textHAlignment = alignment;
        _renderLabel->setHorizontalAlignment(alignment);
        markDirty(DIRTY_CURSOR | DIRTY_SELECTION);
    }
}

TextHAlignment InputField::getTextHorizontalAlignment() const
{
    return _textHAlignment;
}

void InputField::setTextVerticalAlignment(TextVAlignment alignment)
{
    if (_textVAlignment != alignment)
    {
        _textVAlignment = alignment;
        _renderLabel->setVerticalAlignment(alignment);
        markDirty(DIRTY_CURSOR | DIRTY_SELECTION);
    }
}

TextVAlignment InputField::getTextVerticalAlignment() const
{
    return _textVAlignment;
}

void InputField::updateLayout()
{
    if (_continuousTouchPending)
        _continuousTouchElapsedTime += _director->getDeltaTime();

    if (isDirty(DIRTY_CHAR_OFFSETS))
        rebuildCharByteOffsets();

    if (isDirty(DIRTY_TEXT))
        updatePresentation();

    if (isDirty(DIRTY_TEXT_GEOMETRY))
    {
        clearDirty(DIRTY_TEXT_GEOMETRY);

        // Important:
        // The render label must keep the same local coordinate space as InputField.
        // Otherwise auto-size labels are centered by their own changing content width,
        // while the cursor is positioned in InputField coordinates.
        _renderLabel->setDimensions(_contentSize.width, _contentSize.height);
        _renderLabel->setPosition(_contentSize.width * 0.5f, _contentSize.height * 0.5f);
    }

    if (isDirty(DIRTY_LINE_METRICS))
    {
        rebuildLineMetrics();
        markDirty(DIRTY_CURSOR | DIRTY_SELECTION);
    }

    if (isDirty(DIRTY_CURSOR))
        updateCursorTransform();

    if (isDirty(DIRTY_SELECTION))
        updateSelectionLayer();

    _dirtyFlags = 0;
}

//////////////////////////////////////////////////////////////////////////
// Touch Event Handlers (Override Widget's methods)
//////////////////////////////////////////////////////////////////////////

bool InputField::onPointerDown(PointerEvent* event)
{
    if (!_enabled)
    {
        detachWithIME();
        return false;
    }

    if (!Widget::onPointerDown(event))
    {
        detachWithIME();
        return false;
    }

    const bool focus     = isCursorVisible();
    _selectingByTouch    = focus;
    _selectionTouchMoved = false;
    _selectionAnchor     = _cursorCharOffset;

    if (focus)
    {
        Vec2 localPoint  = this->convertToNodeSpace(event->getLocation());
        _selectionAnchor = cursorOffsetFromPosition(localPoint);

        _continuousTouchPending     = true;
        _continuousTouchElapsedTime = 0.0f;
    }

    return true;
}

void InputField::onPointerMove(PointerEvent* event)
{
    Widget::onPointerMove(event);

    if (!_selectingByTouch || !_enabled)
        return;

    Vec2 localPoint = this->convertToNodeSpace(event->getLocation());
    setCursorOffset(cursorOffsetFromPosition(localPoint), true);
    _selectionTouchMoved = hasSelection();
}

void InputField::onPointerUp(PointerEvent* event)
{
    bool focus = _hitted && _enabled;
    if (focus)
    {
        if (!s_keyboardVisible || !isCursorVisible())
            attachWithIME();

        Vec2 worldPoint = event->getLocation();
        Vec2 localPoint = this->convertToNodeSpace(worldPoint);
        moveCursorTo(localPoint, _selectionTouchMoved);

        if (_continuousTouchPending && _continuousTouchElapsedTime >= _continuousTouchDelayTime)
        {
            // Delay time reached, execute callback
            if (_continuousTouchCallback)
            {
                _continuousTouchCallback(worldPoint);
            }
            else
            {
                // Default behavior: show system edit menu at touch location
                _director->getRenderView()->showContextMenu(event->getScreenLocation(), _charCount > 0, hasSelection(),
                                                            _readOnly);
            }
        }
        else
        {
            _director->getRenderView()->hideContextMenu();
        }
    }
    else
    {
        detachWithIME();
    }

    resetTouchState();

    Widget::onPointerUp(event);
}

void InputField::onPointerCancel(PointerEvent* event)
{
    _director->getRenderView()->hideContextMenu();

    // Resets touch pending state
    resetTouchState();

    // Call parent class
    Widget::onPointerCancel(event);
}

void InputField::resetTouchState()
{
    _continuousTouchElapsedTime = 0;
    _continuousTouchPending     = false;
    _selectingByTouch           = false;
}

Vec2 InputField::cursorPositionFromOffset(int cursorOffset) const
{
    if (isDirty(DIRTY_LINE_METRICS))
        const_cast<InputField*>(this)->rebuildLineMetrics();

    cursorOffset = std::min(cursorOffset, static_cast<int>(_charCount));
    if (_lineMetrics.empty())
        return Vec2::ZERO;

    // Find the line containing cursorOffset
    auto it     = std::upper_bound(_lineMetrics.begin(), _lineMetrics.end(), cursorOffset,
                                   [](int pos, const LineMetrics& line) { return pos < line.startCharIndex; });
    int lineIdx = (it == _lineMetrics.begin()) ? 0 : int(it - _lineMetrics.begin()) - 1;
    if (lineIdx >= (int)_lineMetrics.size())
        lineIdx = (int)_lineMetrics.size() - 1;

    const LineMetrics& line = _lineMetrics[lineIdx];
    int charInLine          = cursorOffset - line.startCharIndex;
    if (charInLine < 0)
        charInLine = 0;
    if (charInLine > (int)line.charXOffsets.size() - 1)
        charInLine = (int)line.charXOffsets.size() - 1;

    float x = line.charXOffsets[charInLine];  // x offset from line start to cursor
    // Apply horizontal alignment
    float totalTextWidth = line.lineWidth;
    float lineStartX     = 0.0f;
    if (_textHAlignment == TextHAlignment::CENTER)
        lineStartX = (_contentSize.width - totalTextWidth) * 0.5f;
    else if (_textHAlignment == TextHAlignment::RIGHT)
        lineStartX = _contentSize.width - totalTextWidth;

    float finalX = lineStartX + x;

    // Use the label text bottom Y as the vertical base
    auto yOffset        = getRenderLabelTextBottomY();
    int invertedLineIdx = static_cast<int>(_lineMetrics.size()) - 1 - lineIdx;
    float finalY        = yOffset + (invertedLineIdx + 0.5f) * _lineHeight;  // center of line
    return Vec2(finalX, finalY);
}

int InputField::cursorOffsetFromPosition(const Vec2& position) const
{
    if (isDirty(DIRTY_LINE_METRICS))
        const_cast<InputField*>(this)->rebuildLineMetrics();
    if (_inputText.empty() || _lineMetrics.empty())
        return 0;

    float yOffset = getRenderLabelTextBottomY();

    // Calculate the distance from the top of the text block to map Y coordinates correctly
    // Since Axmol's Y-axis points upwards, lineIdx = 0 should be visually at the top.
    float totalTextHeight = static_cast<float>(_lineMetrics.size()) * _lineHeight;
    float topY            = yOffset + totalTextHeight;  // Visual top of the text block
    float distFromTop     = topY - position.y;          // Distance from top to the click position

    // Calculate line index (0 is the top visual line)
    int lineIdx = std::clamp(static_cast<int>(distFromTop / _lineHeight), 0, static_cast<int>(_lineMetrics.size()) - 1);

    // Fallback if user clicked above the entire text block
    if (distFromTop < 0.0f)
        lineIdx = 0;

    const LineMetrics& line = _lineMetrics[lineIdx];

    // Calculate relative X based on horizontal alignment
    float lineStartX = 0.0f;
    if (_textHAlignment == TextHAlignment::CENTER)
        lineStartX = (_contentSize.width - line.lineWidth) * 0.5f;
    else if (_textHAlignment == TextHAlignment::RIGHT)
        lineStartX = _contentSize.width - line.lineWidth;

    float relX = position.x - lineStartX;

    // Handle boundary cases: before first char or after last char
    if (relX <= 0.0f)
        return line.startCharIndex;
    if (relX >= line.lineWidth)
        return line.endCharIndex;

    // Binary search using std::lower_bound to find first offset greater than relX
    auto& offsets = line.charXOffsets;
    auto it       = std::lower_bound(offsets.begin(), offsets.end(), relX);
    int idx = (int)(it - offsets.begin());  // idx is the first offset > relX, so the character interval is [idx-1, idx]

    if (idx == 0)
        return line.startCharIndex;  // clicked before first char

    float left  = offsets[idx - 1];
    float right = offsets[idx];
    float mid   = (left + right) * 0.5f;

    // Decide cursor position based on left/right half of the character
    if (relX < mid)
        return line.startCharIndex + idx - 1;  // left half ==> before char
    else
        return line.startCharIndex + idx;  // right half ==> after char
}

void InputField::rebuildLineMetrics()
{
    // Ensure string mapping dependency is resolved before text layout
    if (isDirty(DIRTY_CHAR_OFFSETS))
        rebuildCharByteOffsets();

    // Ensure Label string is updated before measuring its geometry
    if (isDirty(DIRTY_TEXT))
        updatePresentation();

    clearDirty(DIRTY_LINE_METRICS);
    _lineMetrics.clear();
    if (_inputText.empty())
    {
        // At least one empty line placeholder
        _lineMetrics.push_back({0, 0, 0.0f, {0.0f}});
        return;
    }

    float availableWidth = _multilineEnabled ? _contentSize.width : std::numeric_limits<float>::max();
    if (availableWidth <= 0.0f && _multilineEnabled)
        availableWidth = std::numeric_limits<float>::max();  // Treat as unlimited

    const int totalChars = static_cast<int>(_charByteOffsets.size() - 1);
    int currentCharIdx   = 0;

    // Pre-allocate core container memory to prevent frequent reallocations
    _lineMetrics.reserve(totalChars / 40 + 1);

    // Initialize the tracking metrics for the first line
    LineMetrics currentLine;
    currentLine.startCharIndex = 0;
    currentLine.charXOffsets.push_back(0.0f);

    while (currentCharIdx < totalChars)
    {
        size_t charByteOffset = _charByteOffsets[currentCharIdx];

        // 1. Handle explicit line breaks (\n)
        if (_multilineEnabled && _inputText.at(charByteOffset) == '\n')
        {
            currentLine.endCharIndex = currentCharIdx;
            currentLine.lineWidth    = currentLine.charXOffsets.back();
            _lineMetrics.push_back(std::move(currentLine));

            // Setup a new line starting right after the '\n' character
            currentLine.startCharIndex = currentCharIdx + 1;
            currentLine.charXOffsets.clear();
            currentLine.charXOffsets.push_back(0.0f);

            ++currentCharIdx;
            continue;
        }

        // 2. Obtain current character width
        float charWidth = _passwordCharWidth;
        if (!_passwordEnabled)
        {
            size_t nextByteOffset = _charByteOffsets[currentCharIdx + 1];
            std::string_view charView(_inputText.data() + charByteOffset, nextByteOffset - charByteOffset);
            charWidth = measureText(charView).width;
        }

        // 3. Auto-wrapping threshold evaluation
        if (_multilineEnabled && currentCharIdx > currentLine.startCharIndex)
        {
            // If adding this character exceeds available width, commit current line before tracking it
            if (currentLine.charXOffsets.back() + charWidth > availableWidth)
            {
                currentLine.endCharIndex = currentCharIdx;
                currentLine.lineWidth    = currentLine.charXOffsets.back();
                _lineMetrics.push_back(std::move(currentLine));

                // Start a new line with the current character as its first element
                currentLine.startCharIndex = currentCharIdx;
                currentLine.charXOffsets.clear();
                currentLine.charXOffsets.push_back(0.0f);

                // Re-evaluate the current character in the next iteration as the line-start
                continue;
            }
        }

        // 4. Record the glyph placement offsets and advance
        currentLine.charXOffsets.push_back(currentLine.charXOffsets.back() + charWidth);
        ++currentCharIdx;
    }

    // Wrap up: Record the final remaining line segment after loop termination
    currentLine.endCharIndex = totalChars;
    currentLine.lineWidth    = currentLine.charXOffsets.back();
    _lineMetrics.push_back(std::move(currentLine));
}

void InputField::rebuildCharByteOffsets()
{
    clearDirty(DIRTY_CHAR_OFFSETS);
    _charByteOffsets.clear();
    _charByteOffsets.reserve(_inputText.length() + 1);
    for (size_t i = 0; i < _inputText.length(); ++i)
    {
        if ((static_cast<uint8_t>(_inputText[i]) & 0xC0) != 0x80)
        {
            _charByteOffsets.push_back(static_cast<int>(i));
        }
    }
    _charByteOffsets.push_back(static_cast<int>(_inputText.length()));
}

void InputField::moveCursorVertically(int direction)
{
    if (isDirty(DIRTY_LINE_METRICS))
        rebuildLineMetrics();

    if (_lineMetrics.empty())
        return;

    // 1. Locate current line index via standard binary search
    auto currentLineIt =
        std::lower_bound(_lineMetrics.begin(), _lineMetrics.end(), _cursorCharOffset,
                         [](const LineMetrics& lm, int cursorIdx) { return lm.endCharIndex < cursorIdx; });

    int curLine = (currentLineIt == _lineMetrics.end())
                      ? static_cast<int>(_lineMetrics.size()) - 1
                      : static_cast<int>(std::distance(_lineMetrics.begin(), currentLineIt));

    // 2. Compute and clamp the target line index
    int targetLine = std::clamp(curLine + direction, 0, static_cast<int>(_lineMetrics.size()) - 1);
    if (targetLine == curLine)
        return;

    const auto& targetMetrics = _lineMetrics[targetLine];
    const auto& offsets       = targetMetrics.charXOffsets;

    // 3. Convert Content-Space preferred X to Line-Relative X based on alignment
    float lineStartX = 0.0f;
    if (_textHAlignment == TextHAlignment::CENTER)
        lineStartX = (_contentSize.width - targetMetrics.lineWidth) * 0.5f;
    else if (_textHAlignment == TextHAlignment::RIGHT)
        lineStartX = _contentSize.width - targetMetrics.lineWidth;

    float lineRelX = std::clamp(_preferredCursorX - lineStartX, 0.0f, targetMetrics.lineWidth);

    // 4. Use standard upper_bound to find the closest matching character offset
    auto offsetIt  = std::upper_bound(offsets.begin(), offsets.end(), lineRelX);
    int charInLine = static_cast<int>(std::distance(offsets.begin(), offsetIt)) - 1;

    // Nearest-neighbor correction: snap to the closer edge if it sits between two characters
    if (charInLine >= 0 && charInLine < static_cast<int>(offsets.size()) - 1)
    {
        float leftDist  = lineRelX - offsets[charInLine];
        float rightDist = offsets[charInLine + 1] - lineRelX;
        if (rightDist < leftDist)
            ++charInLine;
    }
    charInLine = std::clamp(charInLine, 0, static_cast<int>(offsets.size()) - 1);

    // 5. Apply the final cursor position updates
    int newCursorPos = targetMetrics.startCharIndex + charInLine;

    // Note: Do NOT overwrite _preferredCursorX here to preserve vertical movement memory
    // when skipping through short lines back into long lines.
    setCursorOffset(newCursorPos, false, false);
}

void InputField::performEditAction(EditAction action)
{
    if (!canPerformEditAction(action))
        return;

    switch (action)
    {
    case EditAction::Copy:
        copySelectionToClipboard();
        break;
    case EditAction::Cut:
        cutSelectionToClipboard();
        break;
    case EditAction::Paste:
        pasteFromClipboard();
        break;
    case EditAction::SelectAll:
        selectAll();
        break;
    default:
        break;
    }
}

bool InputField::canPerformEditAction(EditAction action) const
{
    if (_readOnly || !_enabled)
        return false;

    switch (action)
    {
    case EditAction::Copy:
        return hasSelection() && !_passwordEnabled;
    case EditAction::Cut:
        return hasSelection() && !_passwordEnabled;
    case EditAction::SelectAll:
        return _charCount > 0;
    case EditAction::Paste:
        return true;  // Paste can be allowed even in password fields, as it doesn't reveal content
    default:
        return false;
    }
}

void InputField::keyboardWillShow(IMEKeyboardNotificationInfo& info)
{
    if (_readOnly || !_enabled || !_focused)
        return;

    // Find the top-level 2D UI root container
    auto uiRoot = findUIRoot(this);
    if (!uiRoot)
        return;

    // Record the true original Y position before any movement
    if (std::isnan(_uiRootOriginY))
    {
        _uiRootOriginY = uiRoot->getPositionY();
    }

    // Get the world bounding box of the current input field
    Rect rectTracked = getWorldBoundingBox();
    rectTracked.origin.y -= uiRoot->getPositionY() - _uiRootOriginY;
    rectTracked.origin.y -= 4;  // 4-pixel safety margin

    // AXLOGI("##### keyboardWillShow: keyboardFrame: origin: ({}, {}) size: ({},{})", info.keyboardFrame.origin.x,
    //        info.keyboardFrame.origin.y, info.keyboardFrame.size.x, info.keyboardFrame.size.y);

    // If the keyboard doesn't cover the input field, do nothing
    if (!rectTracked.intersectsRect(info.keyboardFrame))
    {
        return;
    }

    // Calculate the vertical pixel offset needed to slide the UI up
    _adjustHeight = info.keyboardFrame.getMaxY() - rectTracked.getMinY();

    if (_adjustHeight > 0.0f)
    {
        // Stop any ongoing actions to prevent animation conflicts
        uiRoot->stopAllActions();

        // Slide the entire UI root container upwards based on its original Y
        auto moveUI = ax::MoveTo::create(info.duration, Vec2(uiRoot->getPositionX(), _uiRootOriginY + _adjustHeight));
        uiRoot->runAction(moveUI);
    }
}

void InputField::keyboardWillHide(IMEKeyboardNotificationInfo& info)
{
    // Find the top-level 2D UI root container and reset it
    auto uiRoot = findUIRoot(this);
    if (uiRoot && _adjustHeight > 0.0f)
    {
        uiRoot->stopAllActions();

        // Use the recorded origin Y to reset, fallback to 0.0f only if NaN
        float targetY = std::isnan(_uiRootOriginY) ? 0.0f : _uiRootOriginY;

        // Smoothly reset the UI container's Y position back to its original position
        auto resetUI = ax::MoveTo::create(info.duration, Vec2(uiRoot->getPositionX(), targetY));
        uiRoot->runAction(resetUI);
    }

    // Reset the adjustment height and the origin Y state
    _adjustHeight  = 0.0f;
    _uiRootOriginY = std::numeric_limits<float>::quiet_NaN();
}

}  // namespace ax::ui
