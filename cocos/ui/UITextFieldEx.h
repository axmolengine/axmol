//
// Copyright (c) 2014~2017 purelib - All Rights Reserved
//
#ifndef _UITEXTFIELDEX_H_
#define _UITEXTFIELDEX_H_

#include "cocos2d.h"
#include "base/ccMacros.h"
#include "base/SimpleTimer.h"

NS_CC_BEGIN

namespace ui {

    /**
    @brief  A extension implementation of ui::TextField
    */
    class CC_DLL TextFieldEx : public cocos2d::Node, public IMEDelegate
    {
    public:
        /**
         * @js ctor
         */
        TextFieldEx();
        /**
         * @js NA
         * @lua NA
         */
        virtual ~TextFieldEx();

        static TextFieldEx* create(const std::string& placeholder, const std::string& fontName, float fontSize, float cursorWidth = 2, const Color4B& color = Color4B::WHITE);

        bool initWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize, float cursorWidth = 2, const Color4B& color = Color4B::WHITE);

        void enableIME(Node* control);
        void disableIME(void);

        Label* getRenderLabel();

        inline int                getCharCount() const { return charCount; };

        virtual void              setPlaceholderColor(const Color4B& color);
        virtual const Color4B& getPlaceholderColor() const;

        virtual void              setTextColor(const Color4B& textColor);
        virtual const Color4B& getTextColor(void) const;

        void                      setCursorColor(const Color3B& color);
        const Color3B& getCursorColor(void) const;

        // input text property
        virtual void               setString(const std::string& text);
        virtual const std::string& getString() const;

        // Continuous touch event trigger support.
        void setContinuousTouchDelayTime(float delay) { _continuousTouchDelayTime = delay; }
        float getContinuousTouchDelayTime() const { return _continuousTouchDelayTime; }
        void setContinuousTouchCallback(std::function<void(const Point& worldPoint)> callback) { _continuousTouchCallback = std::move(callback); }

        // place holder text property
        // place holder text displayed when there is no text in the text field.
        virtual void               setPlaceholderText(const std::string& text);
        virtual const std::string& getPlaceholderText(void) const;

        virtual void         setPasswordEnabled(bool value);
        virtual bool         isPasswordEnabled() const;

        /*virtual void         visit(Renderer *renderer, const kmMat4 &parentTransform, bool parentTransformUpdated) override;*/

        bool                 empty(void) const { return this->charCount == 0 || this->inputText.empty(); }

        virtual const Size& getContentSize() const override;

        virtual void         setEnabled(bool bEnabled);
        virtual bool         isEnabled(void) const;

        void                 setEditable(bool bEditable) { editable = bEditable; }
        bool                 isEditable(void) const { return editable; }

        void                 setMaxLength(int maxLength) { setCharLimit(maxLength); }

        int getFontType() const;

        /// fonts
        void setTextFontSize(float size);
        float getTextFontSize() const;
        void setTextFontName(const std::string& fontName);
        const std::string& getTextFontName() const;

        CC_SYNTHESIZE(size_t, charLimit, CharLimit);

        bool isSystemFont(void) const { return systemFontUsed; }

    public:
        std::function<void(void)>   onTextModify;
        std::function<void(void)>   onOpenIME;
        std::function<void(void)>   onCloseIME;
        // IMEDelegate interface
        //////////////////////////////////////////////////////////////////////////
        void openIME(void);
        void closeIME(void);

        virtual void insertText(const char* text, size_t len) override;
    protected:
        //////////////////////////////////////////////////////////////////////////


        virtual bool canAttachWithIME() override;
        virtual bool canDetachWithIME() override;

        virtual void deleteBackward() override;
        virtual const std::string& getContentText() override;

        void handleDeleteKeyEvent();

        /**
        @brief    Open keyboard and receive input text.
        */
        virtual bool attachWithIME();

        /**
        @brief    End text input and close keyboard.
        */
        virtual bool detachWithIME();

        void keyboardDidShow(IMEKeyboardNotificationInfo& /*info*/) override;
        void keyboardDidHide(IMEKeyboardNotificationInfo& /*info*/) override;

        void         updateContentSize(void);

        void         __initCursor(int height, int width = 6, const Color4B& color = Color4B::WHITE);
        void         __showCursor(void);
        void         __hideCursor(void);
        void         __updateCursorPosition(void);

        void         __moveCursor(int direction);

        void         __moveCursorTo(float x);


    protected:

        bool                        systemFontUsed;
        std::string                 fontName;
        float                       fontSize;

        bool                        editable;

        Label* renderLabel;

        size_t                      charCount;
        std::string                 inputText;

        std::string                 placeHolder;
        Color4B                     colorSpaceHolder;
        Color4B                     colorText;

        bool                        secureTextEntry;

        Sprite* cursor;
        bool                        cursorVisible;

        int                         insertPosUtf8;
        int                         insertPos; // The actual input content insertPos, step: bytes
        int                         cursorPos; // The cursor normalzed pos, 

        bool                        enabled;

        EventListenerTouchOneByOne* touchListener;
        EventListenerKeyboard* kbdListener;

        bool                        touchCursorControlEnabled;
        float                       asteriskWidth;

        int                         _fontType;

        cocos2d::stimer::TIMER_ID   _continuousTouchDelayTimerID;
        float                       _continuousTouchDelayTime;
        std::function<void(const Point& worldPoint)>   _continuousTouchCallback;

        static bool                  s_keyboardVisible;
    };

    // end of input group
    /// @}

};

NS_CC_END

#ifdef _UITEXTFIELDEX_INLINE_
#include "UITextFieldEx.cpp"
#endif

#endif
