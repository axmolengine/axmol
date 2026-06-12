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

#pragma once

#include "axmol/ui/Widget.h"
#include "axmol/base/InputDelegate.h"
#include "axmol/2d/DrawNode.h"
#include "axmol/2d/Label.h"
#include "axmol/base/KeyboardEventListener.h"
#include <vector>

namespace ax::ui
{

/**
 * @brief Editable text input widget for UI.
 *
 * InputField provides a modern implementation of editable text input
 * within the UI system. It supports text entry, cursor management,
 * selection handling, and integration with the platform IME (Input Method Editor).
 *
 * This class is the base component for single-line input fields, and can be
 * extended or configured to support multiline input scenarios.
 */
class AX_DLL InputField : public Widget, public InputDelegate
{
    /**
     * @brief Dirty flag pipeline for fine-grained update control.
     * Higher priority updates (lower bit) cascade down to lower priorities.
     */
    enum DirtyFlag
    {
        DIRTY_NONE          = 0,
        DIRTY_CHAR_OFFSETS  = 1 << 0,  // Rebuilds UTF-8 byte offset cache
        DIRTY_LINE_METRICS  = 1 << 1,  // Rebuilds text line wrapping/width metrics
        DIRTY_TEXT_GEOMETRY = 1 << 2,  // Updates render label dimensions and position
        DIRTY_CURSOR        = 1 << 3,  // Updates cursor visual coordinate position
        DIRTY_SELECTION     = 1 << 4,  // Redraws selection highlight geometry
        DIRTY_PREFERRED_X   = 1 << 5,
        DIRTY_TEXT          = 1 << 6,  // Controls label text content, visibility, and style updates
        // Composite Flags for Reusability
        DIRTY_TEXT_CONTENT = DIRTY_CHAR_OFFSETS | DIRTY_TEXT | DIRTY_LINE_METRICS,
        DIRTY_TYPOGRAPHY   = DIRTY_TEXT | DIRTY_LINE_METRICS | DIRTY_CURSOR,

        DIRTY_ALL = DIRTY_CHAR_OFFSETS | DIRTY_LINE_METRICS | DIRTY_TEXT_GEOMETRY | DIRTY_CURSOR | DIRTY_SELECTION |
                    DIRTY_PREFERRED_X | DIRTY_TEXT
    };

    DECLARE_CLASS_GUI_INFO

public:
    static constexpr float DEFAULT_LONG_PRESS_DELAY = 0.5f;

    /**
     * InputField event type.
     */
    enum class EventType
    {
        ATTACH_WITH_IME,
        DETACH_WITH_IME,
        INSERT_TEXT,
        DELETE_BACKWARD,
    };
    using InputFieldCallback = std::function<void(Object*, EventType)>;

    /**
     * @brief Construct a new InputField.
     *
     * The constructor performs minimal initialization; call initWithPlaceHolder
     * or use the static create() helper to construct a fully initialized
     * instance.
     */
    InputField();

    /**
     * @brief Destroy the InputField instance.
     */
    virtual ~InputField();

    /**
     * @brief Create a new InputField instance.
     * @return A new InputField instance or nullptr on failure.
     */
    static InputField* create();

    /**
     * @brief Create and initialize an InputField.
     * @param placeholder Placeholder text shown when the field is empty.
     * @param fontName Font path or system font name.
     * @param fontSize Font size in points.
     * @param cursorWidth Cursor visual width in pixels.
     * @param color Cursor color.
     * @return A new InputField instance or nullptr on failure.
     */
    static InputField* create(std::string_view placeholder,
                              std::string_view fontName,
                              float fontSize,
                              float cursorWidth    = 2,
                              const Color32& color = Color32::WHITE);

    /**
     * @brief Initialize the InputField with placeholder and font settings.
     * @return True if initialization succeeded.
     */
    bool initWithPlaceholder(std::string_view placeholder,
                             std::string_view fontName,
                             float fontSize,
                             float cursorWidth    = 2,
                             const Color32& color = Color32::WHITE);

    /**
     * @brief Returns the internal `Label` used for rendering text.
     */
    Label* getRenderLabel();

    /**
     * @brief Get the text character count (in UTF-8 characters/graphemes).
     * @return Character count.
     */
    inline int getCharCount() const { return static_cast<int>(_charCount); };

    /**
     * @brief Set placeholder text color.
     */
    virtual void setPlaceholderColor(const Color32& color);

    /**
     * @brief Get placeholder text color.
     */
    virtual const Color32& getPlaceholderColor() const;

    /**
     * @brief Set the color used to render input text.
     */
    virtual void setTextColor(const Color32& textColor);

    /**
     * @brief Get the color used for input text.
     */
    virtual const Color32& getTextColor(void) const;

    /**
     * @brief Set cursor color.
     */
    void setCursorColor(const Color32& color);

    /**
     * @brief Get current cursor color.
     */
    const Color32& getCursorColor(void) const;

    /**
     * @brief Set the input text. This updates rendering and cursor state.
     * @param text New UTF-8 text for the field.
     */
    virtual void setString(std::string_view text);

    /**
     * @brief Get the current input text (UTF-8 string view).
     */
    virtual std::string_view getString() const;

    FontType getFontType() const { return _fontType; }

    /**
     * @brief Set the continuous-touch delay.
     *
     * Timeout in seconds used to detect a continuous touch on mobile (iOS).
     * Default is 0.6s. When a continuous-touch callback is set, that callback
     * fires after this delay; otherwise the system edit menu is shown.
     *
     * @param delay Non-negative seconds before the continuous-touch action occurs.
     */
    void setContinuousTouchDelayTime(float delay) { _continuousTouchDelayTime = delay; }

    /**
     * @brief Get the continuous touch delay time.
     */
    float getContinuousTouchDelayTime() const { return _continuousTouchDelayTime; }

    /**
     * @brief Set a callback invoked after continuous touch delay elapses.
     *
     * When a continuous touch is detected on mobile platforms (currently iOS only),
     * the engine will, by default, present the system copy/paste edit menu automatically
     * after the touch ends. If a continuous touch callback is provided via this API,
     * the automatic system menu presentation will be suppressed and the supplied
     * callback will be invoked instead.
     *
     * The default continuous touch delay is **0.6 seconds**. Use
     * `setContinuousTouchDelayTime(float seconds)` to change the delay.
     *
     * @param callback A function that will be called with the touch location in
     *                 world coordinates (`Point& worldPoint`) when the continuous
     *                 touch timeout elapses. Passing an empty/null callback will
     *                 restore the default behavior (i.e., automatic system menu).
     */
    void setContinuousTouchCallback(std::function<void(const Point& worldPoint)> callback)
    {
        _continuousTouchCallback = std::move(callback);
    }

    /**
     * @brief Set the single visible mask character used when password mode is enabled.
     * @param ch UTF-8 string view for the visible mask character (uses first grapheme).
     */
    void setPasswordChar(std::string_view ch);

    /**
     * @brief Get the current password mask character.
     */
    std::string_view getPasswordChar() const { return _passwordChar; }

    /**
     * @brief Set placeholder text string.
     */
    virtual void setPlaceholderText(std::string_view text);

    /**
     * @brief Get placeholder text string.
     */
    virtual std::string_view getPlaceholderText(void) const;

    /**
     * @brief Enable or disable password (secure) entry mode.
     */
    virtual void setPasswordEnabled(bool value);

    /**
     * @brief Return whether password entry is enabled.
     */
    virtual bool isPasswordEnabled() const;

    /**
     * Add an event listener to InputField.
     */
    void addEventListener(const InputFieldCallback& callback);

    /**
     * @brief Check whether the field contains no visible characters.
     */
    bool isEmpty(void) const { return _charCount == 0 || _inputText.empty(); }

    /**
     * @brief Enable or disable the widget.
     */
    void setEnabled(bool bEnabled) override;

    /**
     * @brief Mark the field editable or read-only.
     */
    void setReadOnly(bool bReadOnly) { _readOnly = bReadOnly; }

    /**
     * @brief Query whether the field is read-only.
     */
    bool isReadOnly(void) const { return _readOnly; }

    /**
     * @brief Set maximum allowed character length (UTF-8 characters).
     */
    void setMaxLength(int maxLength) { setCharLimit(maxLength); }

    /**
     * @brief Query whether there is an active selection.
     */
    bool hasSelection() const;

    /**
     * @brief Select all text.
     */
    void selectAll();

    /**
     * @brief Clear any text selection.
     */
    void clearSelection();

    /**
     * @brief Return the selected substring (UTF-8).
     */
    std::string getSelectedText() const;

    /**
     * @brief Copy current selection to clipboard. Returns false if nothing copied.
     */
    bool copySelectionToClipboard() const;

    /**
     * @brief Cut current selection to clipboard.
     */
    bool cutSelectionToClipboard();

    /**
     * @brief Paste text from clipboard at the cursor position.
     */
    bool pasteFromClipboard();

    /**
     * @brief Set selection (highlight) color.
     */
    void setSelectionColor(const Color& color);

    /**
     * @brief Get current selection color.
     */
    const Color& getSelectionColor() const;

    /// fonts
    /**
     * @brief Set font size used for rendering the text.
     */
    void setFontSize(float size);

    /**
     * @brief Get current font size.
     */
    float getFontSize() const;

    /**
     * @brief Set the font used for rendering (path or system name).
     */
    void setFontName(std::string_view fontName);

    /**
     * @brief Get the configured font name.
     */
    std::string_view getFontName() const;

    /// text alignment
    /**
     * @brief Set horizontal alignment of rendered text.
     */
    void setTextHorizontalAlignment(TextHAlignment alignment);

    /**
     * @brief Get horizontal alignment.
     */
    TextHAlignment getTextHorizontalAlignment() const;

    /**
     * @brief Set vertical alignment of rendered text.
     */
    void setTextVerticalAlignment(TextVAlignment alignment);

    /**
     * @brief Get vertical alignment.
     */
    TextVAlignment getTextVerticalAlignment() const;

    /// touch area
    /**
     * @brief Set the custom touch area size for hit testing.
     */
    void setTouchAreaSize(const Vec2& size);

    /**
     * @brief Get the touch area size.
     */
    Vec2 getTouchAreaSize() const;

    /**
     * @brief Enable or disable the custom touch area hit test.
     */
    void setTouchAreaEnabled(bool enable);

    bool onPointerHitTest(PointerEvent* event, const Camera* camera, Vec3* outHitPoint) override;

    /**
     * @brief Test whether a point hits the field (respecting touch area if enabled).
     */
    bool hitTestSelf(const Vec2& pt, const Camera* camera, Vec3* p) const override;

    /**
     * @brief Set the maximum allowed character count for this input field.
     *
     * The limit is interpreted as a maximum number of UTF-8 characters (user
     * level characters/grapheme clusters as counted by the control). Calling
     * this affects subsequent input/insert operations which will be clipped to
     * this limit. Use `0` to indicate no limit.
     *
     * @param limit Maximum characters allowed (0 = unlimited).
     */
    void setCharLimit(uint32_t limit);

    /**
     * @brief Get the configured character limit.
     * @return The maximum number of characters allowed (0 means unlimited).
     */
    uint32_t getCharLimit() const { return _charLimit; }

    /**
     * @brief Enable or disable multiline mode.
     * In multiline mode the control supports line breaks and the cursor/selection
     * logic accounts for multiple text lines. The layout is recalculated
     * automatically when the text, font, or content size changes.
     */
    void setMultilineEnabled(bool enabled);

    /**
     * @brief Return whether multiline mode is active.
     */
    bool isMultilineEnabled() const { return _multilineEnabled; }

    /**
     * @brief Get the internal renderer node.
     * @return Pointer to the renderer node.
     */
    Node* getRenderNode() override;

    /**
     * @brief Get preferred size based on rendered text.
     * @return Preferred size.
     */
    Vec2 resolvePreferredSize(const Vec2& /*sizeHint*/) const override;

protected:
    void dispatchEvent(EventType eventType);
    void updatePresentation();

    void updateLineHeight();

    //////////////////////////////////////////////////////////////////////////

    void onEnter() override;
    void onExit() override;

    bool canAttachWithIME() const override;
    bool canDetachWithIME() const override;

    // InputDelegate interface
    void insertText(std::string_view text) override;
    void deleteBackward(unsigned int numChars) override;

    void handleDeleteKeyEvent();

    /**
    @brief    Open keyboard and receive input text.
    */
    bool attachWithIME() override;

    /**
    @brief    End text input and close keyboard.
    */
    bool detachWithIME() override;

    void keyboardWillShow(IMEKeyboardNotificationInfo& info) override;
    void keyboardWillHide(IMEKeyboardNotificationInfo& info) override;

    bool hitTestWithIME(const Vec2& location) override;
    void keyboardDidShow(IMEKeyboardNotificationInfo& /*info*/) override;
    void keyboardDidHide(IMEKeyboardNotificationInfo& /*info*/) override;

    void updateContentSize(void) override;

    void showCursor(void);
    void hideCursor(void);
    void updateCursorTransform(void);

    void moveCursor(int direction);
    void moveCursor(int direction, bool keepSelection);

    void moveCursorTo(const Vec2& point);
    void moveCursorTo(const Vec2& point, bool keepSelection);
    void setCursorOffset(int cursorOffset, bool keepSelection, bool markPrefxDirty = true);

    void moveCursorVertically(int direction);

    /**
     * @brief Calculate the coordinate for a given cursor index.
     * @param cursorOffset UTF-8 character index (0 = before first char)
     * @return X coordinate in local node space, accounting for text alignment
     */
    Vec2 cursorPositionFromOffset(int cursorOffset) const;

    /**
     * @brief Find the cursor index at a given coordinate.
     * @param x X coordinate in local node space
     * @return UTF-8 character index (inverse of cursorPositionFromIndex)
     */
    int cursorOffsetFromPosition(const Vec2& position) const;

    int getByteOffset(int cursorOffset) const;

    float getRenderLabelTextBottomY() const;

    void setSelection(int start, int end);
    void updateSelectionLayer(void);
    bool deleteSelection(bool notify);

    bool isCursorVisible() const;

    void updateLayout() override;
    void onSizeChanged() override;

    // Override Widget's touch event handlers
    bool onPointerDown(PointerEvent* pointerEvent) override;
    void onPointerMove(PointerEvent* pointerEvent) override;
    void onPointerUp(PointerEvent* pointerEvent) override;
    void onPointerCancel(PointerEvent* pointerEvent) override;

    void resetTouchState();

    // Override InputDelegate's Editor action
    void performEditAction(EditAction action) override;

    bool canPerformEditAction(EditAction action) const;

    /**
     * @brief Check if a specific dirty flag is set.
     * @param flag The dirty flag to check.
     * @return True if the flag is set.
     */
    inline bool isDirty(int flag) const { return (_dirtyFlags & flag) != 0; }

    /**
     * @brief Clear specific dirty flag(s).
     * @param flag The dirty flag(s) to clear.
     */
    inline void clearDirty(int flag) { _dirtyFlags &= ~flag; }

    /**
     * @brief Mark specific dirty flag(s) to trigger deferred layout/render updates.
     */
    inline void markDirty(uint32_t flag) { _dirtyFlags |= flag; }

    /**
     * @brief Measure text extent using instance-specific label (optimal performance).
     * @param text The text to measure.
     * @return The content size of the text.
     */
    Vec2 measureText(std::string_view text) const;

    void rebuildLineMetrics();
    void rebuildCharByteOffsets();

    std::string makePasswordString();

    /**
     * @brief Updates the logical text length and clamps selection indices within valid ranges.
     * @note This manages logical index boundaries, not physical/geometric rendering sizes.
     */
    void updateLogicalLimits(uint32_t newCharCount);

    // Per-line metrics used for cursor/selection mapping in both single and multi-line modes.
    struct LineMetrics
    {
        int startCharIndex = 0;     // inclusive UTF-8 character index of the first character in this line
        int endCharIndex   = 0;     // exclusive UTF-8 character index (points past the last visible character)
        float lineWidth    = 0.0f;  // pure text width of this line (before horizontal alignment)
        std::vector<float>
            charXOffsets;  // length = (endCharIndex - startCharIndex + 1)
                           // charXOffsets[i] = x-offset from line start to the *end* of the i-th character
    };

    std::vector<LineMetrics> _lineMetrics;  // single-line mode: size() == 1
    float _lineHeight       = 0.0f;         // uniform line height computed from font metrics
    float _preferredCursorX = 0.0f;         // used when moving cursor vertically to keep desired column

    std::string _fontName;
    float _fontSize{24.0f};

    bool _readOnly{false};
    uint32_t _dirtyFlags{DIRTY_ALL};
    bool _passwordEnabled{false};
    bool _cursorVisible{false};
    bool _selectingByTouch{false};
    bool _selectionTouchMoved{false};
    bool _ctrlKeyPressed{false};
    bool _shiftKeyPressed{false};
    bool _useTouchArea{false};
    bool _multilineEnabled{false};

    bool _isAttachWithIME{false};

    // Continuous touch delay state (manual timing, no external timer dependency)
    bool _continuousTouchPending{false};
    float _continuousTouchElapsedTime{0.0f};
    float _continuousTouchDelayTime{DEFAULT_LONG_PRESS_DELAY};

    float _adjustHeight{0.0f};
    float _uiRootOriginY{std::numeric_limits<float>::quiet_NaN()};

    Label* _renderLabel{nullptr};

    std::string _inputText;
    std::string _placeholderText;
    std::string _passwordChar;
    std::vector<int> _charByteOffsets;  // Maps UTF-8 character index to byte offset in _inputText

    Color32 _colorSpaceHolder{Color32::GRAY};
    Color32 _colorText{Color32::WHITE};

    Sprite* _cursor{nullptr};

    DrawNode* _selectionLayer{nullptr};
    Color _selectionColor{0.26f, 0.52f, 1.0f, 0.35f};

    FontType _fontType{FontType::SYSTEM};

    uint32_t _charLimit{0};
    uint32_t _charCount{0};

    int _selectionStart{0};   // Start character index of current selection (UTF-8)
    int _selectionEnd{0};     // End character index of current selection (UTF-8)
    int _selectionAnchor{0};  // Anchor character index used for extending selection (UTF-8)

    int _cursorCharOffset{0};  // Cursor position as character offset (UTF-8)
    int _cursorByteOffset{0};  // Cursor position as byte offset in string

    float _passwordCharWidth{0.0f};

    KeyboardEventListener* _kbdListener{nullptr};

    /// Touch area
    Vec2 _touchAreaSize;

    /// Text alignment
    TextHAlignment _textHAlignment{TextHAlignment::LEFT};
    TextVAlignment _textVAlignment{TextVAlignment::TOP};

    std::function<void(const Vec2& worldPoint)> _continuousTouchCallback;
    InputFieldCallback _eventCallback;

    // Instance-specific measure label (lazy initialized, no sharing overhead)
    mutable Label* _measureLabel{nullptr};

    static bool s_keyboardVisible;
};

// end of input group
/// @}

}  // namespace ax::ui
