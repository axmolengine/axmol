//
// Copyright (c) 2014-2020 @HALX99 - All Rights Reserved
//
#ifndef _UITEXTFIELD_CPP_H_
#define _UITEXTFIELD_CPP_H_
#include "UITextFieldEx.h"

#include "base/CCDirector.h"
// #include "CCGLView.h"
//#include "NXMacroDefs.h"
//#include "purelib/utils/iconvw.h"

/// cocos2d singleton objects
#define CCDIRECTOR  cocos2d::Director::getInstance()
#define CCRUNONGL   CCDIRECTOR->getScheduler()->performFunctionInCocosThread
#define CCEVENTMGR  CCDIRECTOR->getEventDispatcher()
#define CCSCHTASKS  CCDIRECTOR->getScheduler()
#define CCACTIONMGR CCDIRECTOR->getActionManager()
#define CCFILEUTILS cocos2d::FileUtils::getInstance()
#define CCAUDIO     cocos2d::SimpleAudioEngine::getInstance()
#define CCAPP       cocos2d::CCApplication::getInstance()

NS_CC_BEGIN

#ifdef _WIN32
#define nxbeep(t) MessageBeep(t)
#else
#define nxbeep(t)
#endif

static Label* createLabel(const std::string& text, const std::string& font, float fontSize, const Size& dimensions  = Size::ZERO , TextHAlignment hAlignment  = TextHAlignment::LEFT , TextVAlignment vAlignment  = TextVAlignment::TOP )
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

static bool engine_inj_checkVisibility(Node* theNode)
{
    // CC_ASSERT(theNode != NULL);
    bool visible = false;
    for (Node* ptr = theNode; (ptr != nullptr && (visible = ptr->isVisible())); ptr = ptr->getParent())
        ;
    return visible;
}

static bool engine_inj_containsTouchPoint(cocos2d::Node* target, cocos2d::Touch* touch)
{
    assert(target != nullptr);

    cocos2d::Point pt = target->convertTouchToNodeSpace(touch);

    const cocos2d::Size& size = target->getContentSize();

    cocos2d::Rect rc(0, 0, size.width, size.height);

    bool contains = (rc.containsPoint(pt));

    // CCLOG("check %#x coordinate:(%f, %f), contains:%d", target, pt.x, pt.y, contains);
    return contains;
}

static bool  engine_inj_containsPoint(cocos2d::Node* target, const cocos2d::Vec2& worldPoint)
{
    cocos2d::Point pt = target->convertToNodeSpace(worldPoint);

    const cocos2d::Size& size = target->getContentSize();

    cocos2d::Rect rc(0, 0, size.width, size.height);

    bool contains = (rc.containsPoint(pt));

    // CCLOG("check %#x coordinate:(%f, %f), contains:%d", target, pt.x, pt.y, contains);
    return contains;
}

static uint32_t engine_inj_c4b2dw(const Color4B& value)
{
    auto rvalue = (uint32_t)value.a << 24 |
        (uint32_t)value.b << 16 |
        (uint32_t)value.g << 8 |
        (uint32_t)value.r;
    return rvalue;
}

static Sprite* engine_inj_create_lump(const Color4B& color, int height, int width)
{
    unsigned int* pixels((unsigned int*)malloc(height * width * sizeof(unsigned int)));

    // Fill Pixels
    uint32_t* ptr = pixels;
    const Color4B fillColor = Color4B::WHITE;
    for (int i = 0; i < height * width; ++i)
    {
        ptr[i] = engine_inj_c4b2dw(fillColor);// 0xffffffff;
    }

    // create cursor by pixels
    Texture2D* texture = new Texture2D();

    texture->initWithData(pixels, height * width * sizeof(unsigned int), backend::PixelFormat::RGBA8, width, height, Size(width, height));

    auto cursor = Sprite::createWithTexture(texture);

    cursor->setColor(Color3B(color));
    cursor->setOpacity(color.a);

    texture->release();

    free(pixels);

    return cursor;
}

namespace ui {



    /// calculate the UTF-8 string's char count.
    static int _calcCharCount(const char * text)
    {
        int n = 0;
        char ch = 0;
        while ((ch = *text) != 0x0)
        {
            CC_BREAK_IF(!ch);

            if (0x80 != (0xC0 & ch))
            {
                ++n;
            }
            ++text;
        }
        return n;
    }

    /// calculate the UTF-8 string's char count.
    static int _truncateUTF8String(const char * text, int limit, int& nb)
    {
        int n = 0;
        char ch = 0;
        nb = 0;
        while ((ch = *text) != 0x0)
        {
            CC_BREAK_IF(!ch || n > limit);

            if (0x80 != (0xC0 & ch))
            {
                ++n;
            }
            ++nb;
            ++text;
        }
        return n;
    }

    static void internalSetLableFont(Label* l, const std::string& fontName, float fontSize)
    {
        if (FileUtils::getInstance()->isFileExist(fontName))
        {
            TTFConfig config = l->getTTFConfig();
            config.fontFilePath = fontName;
            config.fontSize = fontSize;
            l->setTTFConfig(config);
        }
        else
        {
            l->setSystemFontName(fontName);
            l->requestSystemFontRefresh();
            l->setSystemFontSize(fontSize);
        }
    }

    static float internalCalcStringWidth(const std::string& s, const std::string& fontName, float fontSize)
    {
        auto label = createLabel(s, fontName, fontSize);
        return label->getContentSize().width;
    }

    static std::string internalUTF8MoveLeft(const std::string& utf8Text, int length/* default utf8Text.length() */)
    {
        if (!utf8Text.empty() && length > 0) {

            // get the delete byte number
            size_t deleteLen = 1;    // default, erase 1 byte

            while ((length - deleteLen >= 0) && 0x80 == (0xC0 & utf8Text.at(length - deleteLen)))
            {
                ++deleteLen;
            }

            std::string temp = utf8Text;
            temp.resize(length - deleteLen);

            return temp;
        }
        else {
            return utf8Text;
        }
    }

    static std::string internalUTF8MoveRight(const std::string& utf8Text, int length/* default utf8Text.length() */)
    {
        if (!utf8Text.empty() && length >= 0) {

            // get the delete byte number
            size_t addLen = 1;    // default, erase 1 byte

            while ((length + addLen < utf8Text.size()) && 0x80 == (0xC0 & utf8Text.at(length + addLen)))
            {
                ++addLen;
            }

            std::string temp = utf8Text;
            temp.resize(length + addLen);

            return temp;
        }
        else {
            return utf8Text;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // constructor and destructor
    //////////////////////////////////////////////////////////////////////////
    bool TextFieldEx::s_keyboardVisible = false;
    TextFieldEx::TextFieldEx()
        : editable(true)
        , renderLabel(nullptr)
        , charCount(0)
        , inputText("")
        , placeHolder("")
        , colorText(Color4B::WHITE)
        , colorSpaceHolder(Color4B::GRAY)
        , secureTextEntry(false)
        , cursor(nullptr)
        , enabled(true)
        , touchListener(nullptr)
        , kbdListener(nullptr)
        , onTextModify(nullptr)
        , onOpenIME(nullptr)
        , onCloseIME(nullptr)
        , charLimit(std::numeric_limits<int>::max())
        , systemFontUsed(false)
        , fontSize(24)
        , insertPosUtf8(0)
        , insertPos(0)
        , cursorPos(0)
        , touchCursorControlEnabled(true)
        , cursorVisible(false)
        , _continuousTouchDelayTimerID(nullptr)
        , _continuousTouchDelayTime(0.6)
    {
    }

    TextFieldEx::~TextFieldEx()
    {
        if(this->kbdListener != nullptr) 
            CCEVENTMGR->removeEventListener(this->kbdListener);
        if (this->touchListener != nullptr)
            CCEVENTMGR->removeEventListener(this->touchListener);
    }

    //////////////////////////////////////////////////////////////////////////
    // static constructor
    //////////////////////////////////////////////////////////////////////////
    TextFieldEx * TextFieldEx::create(const std::string& placeholder, const std::string& fontName, float fontSize, float cursorWidth, const Color4B& cursorColor)
    {
        TextFieldEx *ret = new TextFieldEx();
        if (ret && ret->initWithPlaceHolder("", fontName, fontSize, cursorWidth, cursorColor))
        {
            ret->autorelease();
            if (placeholder.size() > 0)
            {
                ret->setPlaceholderText(placeholder);
            }
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    //////////////////////////////////////////////////////////////////////////
    // initialize
    //////////////////////////////////////////////////////////////////////////
    bool TextFieldEx::initWithPlaceHolder(const std::string& placeholder, const std::string& fontName, float fontSize, float cursorWidth, const Color4B& cursorColor)
    {
        this->placeHolder = placeholder;

        this->renderLabel = createLabel(placeholder, fontName, fontSize, Size::ZERO, TextHAlignment::CENTER, TextVAlignment::CENTER);
        this->renderLabel->setAnchorPoint(Point::ANCHOR_MIDDLE_LEFT);
        this->addChild(this->renderLabel);

        CCRUNONGL([this]{
            renderLabel->setPosition(Point(0, this->getContentSize().height / 2));
        });

        __initCursor(fontSize, cursorWidth, cursorColor);

        this->fontName = fontName;
        this->fontSize = fontSize;
        this->systemFontUsed = !FileUtils::getInstance()->isFileExist(fontName);
        
        return true;
    }

    const std::string& TextFieldEx::getTextFontName() const
    {
        return this->fontName;
    }

    void TextFieldEx::setTextFontName(const std::string& fontName)
    {
        if (FileUtils::getInstance()->isFileExist(fontName))
        {
            TTFConfig config = renderLabel->getTTFConfig();
            config.fontFilePath = fontName;
            config.fontSize = this->fontSize;
            renderLabel->setTTFConfig(config);
            systemFontUsed = false;
            _fontType = 1;
        }
        else
        {
            renderLabel->setSystemFontName(fontName);
            if (!systemFontUsed)
            {
                renderLabel->requestSystemFontRefresh();
            }
            renderLabel->setSystemFontSize(this->fontSize);
            systemFontUsed = true;
            _fontType = 0;
        }
        this->fontName = fontName;

        this->asteriskWidth = internalCalcStringWidth("*", this->fontName, this->fontSize);
    }

    void TextFieldEx::setTextFontSize(float size)
    {
        if (this->systemFontUsed)
        {
            renderLabel->setSystemFontSize(size);
        }
        else
        {
            TTFConfig config = renderLabel->getTTFConfig();
            config.fontSize = size;
            renderLabel->setTTFConfig(config);
        }

        this->fontSize = size;

        this->asteriskWidth = internalCalcStringWidth("*", this->fontName, this->fontSize);
    }

    float TextFieldEx::getTextFontSize() const
    {
        return this->fontSize;
    }

    void TextFieldEx::enableIME(Node* control)
    {
        if (touchListener != nullptr) {
            return;
        }
        touchListener = EventListenerTouchOneByOne::create();

        if (control == nullptr)
            control = this;

        touchListener->onTouchBegan = [=](Touch* touch, Event*){
            bool focus = (engine_inj_checkVisibility(this) && this->editable && this->enabled && engine_inj_containsTouchPoint(control, touch));

            if (this->_continuousTouchDelayTimerID != nullptr) {
                stimer::kill(this->_continuousTouchDelayTimerID);
                this->_continuousTouchDelayTimerID = nullptr;
            }

            if (focus && this->cursorVisible) {
                auto worldPoint = touch->getLocation();
                if (this->_continuousTouchCallback) {
                    this->_continuousTouchDelayTimerID = stimer::delay(this->_continuousTouchDelayTime, [=]() {
                        this->_continuousTouchCallback(worldPoint);
                    });
                }
            }
            return true; 
        };
        touchListener->onTouchEnded = [control, this](Touch* touch, Event* e) {
            if (this->_continuousTouchDelayTimerID != nullptr) {
                stimer::kill(this->_continuousTouchDelayTimerID);
                this->_continuousTouchDelayTimerID = nullptr;
            }

            bool focus = (engine_inj_checkVisibility(this) && this->editable && this->enabled && engine_inj_containsTouchPoint(control, touch));

            if (focus)
            {
                if (!s_keyboardVisible || !this->cursorVisible)
                    openIME();
                if (this->touchCursorControlEnabled) {
                    auto renderLabelPoint = renderLabel->convertToNodeSpace(touch->getLocation());
                    __moveCursorTo(renderLabelPoint.x);
                }
            }
            else {
                closeIME();
            }
        };

        CCEVENTMGR->addEventListenerWithSceneGraphPriority(touchListener, this);

        /// enable use keyboard <- -> to move cursor.
        kbdListener = EventListenerKeyboard::create();
        kbdListener->onKeyPressed = [this](EventKeyboard::KeyCode code, Event*){
            if (this->cursorVisible) {
                switch (code)
                {
                case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
                    cocos2d::log("******** Move Left ********* ");
                    this->__moveCursor(-1);
                    break;
                case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
                    this->__moveCursor(1);
                    break;
                case EventKeyboard::KeyCode::KEY_DELETE:
                case EventKeyboard::KeyCode::KEY_KP_DELETE:
                    this->handleDeleteKeyEvent();
                    break;
                }
            }
        };

        CCEVENTMGR->addEventListenerWithSceneGraphPriority(kbdListener, this);
    }

    void TextFieldEx::disableIME(void)
    {
        CCEVENTMGR->removeEventListener(kbdListener);
        CCEVENTMGR->removeEventListener(touchListener);

        kbdListener = nullptr;
        touchListener = nullptr;
        closeIME();
    }

    Label* TextFieldEx::getRenderLabel()
    {
        return this->renderLabel;
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
            GLView * pGlView = Director::getInstance()->getOpenGLView();
            if (pGlView)
            {
                pGlView->setIMEKeyboardState(true);
            }
        }
        return ret;
    }

    bool TextFieldEx::detachWithIME()
    {
        bool ret = IMEDelegate::detachWithIME();
        if (ret)
        {
            // close keyboard
            GLView * glView = Director::getInstance()->getOpenGLView();
            if (glView)
            {
                glView->setIMEKeyboardState(false);
            }
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
        CCLOG("TextFieldEx:: openIME");
        this->attachWithIME();
        __updateCursorPosition();
        __showCursor();

        if (this->onOpenIME)
            this->onOpenIME();
    }

    void TextFieldEx::closeIME(void)
    {
        CCLOG("TextFieldEx:: closeIME");
        __hideCursor();
        this->detachWithIME();

        if (this->onCloseIME)
            this->onCloseIME();
    }

    bool TextFieldEx::canAttachWithIME()
    {
        return true;//(_delegate) ? (! _delegate->onTextFieldAttachWithIME(this)) : true;
    }

    bool TextFieldEx::canDetachWithIME()
    {
        return true;//(_delegate) ? (! _delegate->onTextFieldDetachWithIME(this)) : true;
    }

    void TextFieldEx::insertText(const char * text, size_t len)
    {
        if (!this->editable || !this->enabled)
        {
            return;
        }

        if (this->charLimit > 0 && this->charCount >= this->charLimit)
        { // regard zero as unlimited
            nxbeep(0);
            return;
        }

        int nb;
        auto n = _truncateUTF8String(text, this->charLimit - this->charCount, nb);

        std::string insert(text, nb);

        // insert \n means input end
        auto pos = insert.find('\n');
        if (insert.npos != pos)
        {
            len = pos;
            insert.erase(pos);
        }

        if (len > 0)
        {
            //if (_delegate && _delegate->onTextFieldInsertText(this, insert.c_str(), len))
            //{
            //    // delegate doesn't want to insert text
            //    return;
            //}

            charCount += n; // _calcCharCount(insert.c_str());
            std::string sText(inputText);
            sText.insert(this->insertPos, insert); // original is: sText.append(insert);

            // bool needUpdatePos
            this->setString(sText);
            while (n-- > 0)
                __moveCursor(1);

            // this->contentDirty = true;
            // __updateCursorPosition();

            if (this->onTextModify)
                this->onTextModify();
        }

        if (insert.npos == pos) {
            return;
        }

        // '\n' inserted, let delegate process first
        /*if (_delegate && _delegate->onTextFieldInsertText(this, "\n", 1))
        {
        return;
        }*/

        // if delegate hasn't processed, detach from IME by default
        this->closeIME();
    }

    void TextFieldEx::deleteBackward()
    {
        if (!this->editable || !this->enabled || 0 == this->charCount) {
            nxbeep(0);
            return;
        }

        size_t len = inputText.length();
        if (0 == len || insertPos == 0)
        {
            nxbeep(0);
            // there is no string
            // __updateCursorPosition();
            return;
        }

        // get the delete byte number
        size_t deleteLen = 1;    // default, erase 1 byte

        while (0x80 == (0xC0 & inputText.at(insertPos - deleteLen)))
        {
            ++deleteLen;
        }

        //if (_delegate && _delegate->onTextFieldDeleteBackward(this, _inputText.c_str() + len - deleteLen, static_cast<int>(deleteLen)))
        //{
        //    // delegate doesn't wan't to delete backwards
        //    return;
        //}

        // if all text deleted, show placeholder string
        if (len <= deleteLen)
        {
            __moveCursor(-1);

            this->inputText.clear();
            this->charCount = 0;
            this->renderLabel->setTextColor(colorSpaceHolder);
            this->renderLabel->setString(placeHolder);

            // __updateCursorPosition();
            
            // this->contentDirty = true;

            if (this->onTextModify)
                this->onTextModify();
            return;
        }

        // set new input text
        std::string text = inputText; // (inputText.c_str(), len - deleteLen);
        text.erase(insertPos - deleteLen, deleteLen);

        __moveCursor(-1);

        this->setString(text);

        //__updateCursorPosition();
        // __moveCursor(-1);

        if (this->onTextModify)
            this->onTextModify();
    }

    void TextFieldEx::handleDeleteKeyEvent()
    {
        if (!this->editable || !this->enabled || 0 == this->charCount) {
            nxbeep(0);
            return;
        }

        size_t len = inputText.length();
        if (0 == len || insertPosUtf8 == this->charCount)
        {
            nxbeep(0);
            // there is no string
            // __updateCursorPosition();
            return;
        }

        // get the delete byte number
        size_t deleteLen = 1;    // default, erase 1 byte

        while ((inputText.length() > insertPos + deleteLen) && 0x80 == (0xC0 & inputText.at(insertPos + deleteLen)))
        {
            ++deleteLen;
        }

        //if (_delegate && _delegate->onTextFieldDeleteBackward(this, _inputText.c_str() + len - deleteLen, static_cast<int>(deleteLen)))
        //{
        //    // delegate doesn't wan't to delete backwards
        //    return;
        //}

        // if all text deleted, show placeholder string
        if (len <= deleteLen)
        {
            this->inputText.clear();
            this->charCount = 0;
            this->renderLabel->setTextColor(colorSpaceHolder);
            this->renderLabel->setString(placeHolder);

            __updateCursorPosition();

            // this->contentDirty = true;

            if (this->onTextModify)
                this->onTextModify();
            return;
        }

        // set new input text
        std::string text = inputText; // (inputText.c_str(), len - deleteLen);
        text.erase(insertPos, deleteLen);

        // __moveCursor(-1);

        this->setString(text);

        if (this->onTextModify)
            this->onTextModify();
    }

    const std::string& TextFieldEx::getContentText()
    {
        return inputText;
    }

    void TextFieldEx::setTextColor(const Color4B &color)
    {
        colorText = color;
        if (!this->inputText.empty())
            this->renderLabel->setTextColor(colorText);
    }

    const Color4B&  TextFieldEx::getTextColor(void) const
    {
        return colorText;
    }

    void TextFieldEx::setCursorColor(const Color3B& color)
    {
        this->cursor->setColor(color);
    }

    const Color3B& TextFieldEx::getCursorColor(void) const
    {
        return this->cursor->getColor();
    }

    const Color4B& TextFieldEx::getPlaceholderColor() const
    {
        return colorSpaceHolder;
    }

    void TextFieldEx::setPlaceholderColor(const Color4B& color)
    {
        colorSpaceHolder = color;
        if (this->inputText.empty())
            this->renderLabel->setTextColor(color);
    }

    //////////////////////////////////////////////////////////////////////////
    // properties
    //////////////////////////////////////////////////////////////////////////

    // input text property
    void TextFieldEx::setString(const std::string &text)
    {
        static char bulletString[] = { (char)0xe2, (char)0x80, (char)0xa2, (char)0x00 };

        this->inputText = text;

        std::string secureText;

        std::string* displayText = &this->inputText;

        if (!this->inputText.empty())
        {
            if (secureTextEntry)
            {
                size_t length = inputText.length();
                displayText = &secureText;

                while (length > 0)
                {
                    displayText->append(bulletString);
                    --length;
                }
            }
        }

        // if there is no input text, display placeholder instead
        if (this->inputText.empty())
        {
            renderLabel->setTextColor(colorSpaceHolder);
            renderLabel->setString(placeHolder);
        }
        else
        {
            renderLabel->setTextColor(colorText);
            renderLabel->setString(*displayText);
        }

        bool bInsertAtEnd = (insertPosUtf8 == charCount);

        charCount = _calcCharCount(inputText.c_str());

        if (bInsertAtEnd)
        {
            insertPosUtf8 = charCount;
            insertPos = inputText.length();
            cursorPos = displayText->length();
        }
    }

    void TextFieldEx::updateContentSize(void)
    {
        this->setContentSize(renderLabel->getContentSize());
    }

    const std::string& TextFieldEx::getString() const
    {
        return inputText;
    }

    // place holder text property
    void TextFieldEx::setPlaceholderText(const std::string& text)
    {
        placeHolder = text;
        if (inputText.empty())
        {
            renderLabel->setTextColor(colorSpaceHolder);
            renderLabel->setString(placeHolder);
        }
    }

    const std::string& TextFieldEx::getPlaceholderText() const
    {
        return placeHolder;
    }

    // secureTextEntry
    void TextFieldEx::setPasswordEnabled(bool value)
    {
        if (secureTextEntry != value)
        {
            secureTextEntry = value;
            this->setString(this->getString());
            __updateCursorPosition();
        }
    }

    bool TextFieldEx::isPasswordEnabled() const
    {
        return secureTextEntry;
    }

    const Size& TextFieldEx::getContentSize() const
    {
        // const_cast<TextFieldEx*>(this)->setContentSize(renderLabel->getContentSize());
        return Node::getContentSize();
    }

    void  TextFieldEx::setEnabled(bool bEnabled)
    {
        if (this->enabled != bEnabled)
        {
            if (!bEnabled) {
                this->closeIME();
            }
            this->enabled = bEnabled;
        }
    }

    bool  TextFieldEx::isEnabled(void) const
    {
        return this->enabled;
    }

    int TextFieldEx::getFontType() const
    { 
        return _fontType;
    }

    void TextFieldEx::__initCursor(int height, int width, const Color4B& color)
    {
        this->cursor = engine_inj_create_lump(Color4B(color), height, width);

        this->addChild(this->cursor);

        this->cursor->setPosition(Point(0, this->getContentSize().height / 2));
        // nodes_layout::setNodeLB(this->cursor, cocos2d::Point::ZERO);

        /*CCAction* blink = CCRepeatForever::create(
            (CCActionInterval *)CCSequence::create(CCFadeOut::create(0.25f),
            CCFadeIn::create(0.25f),
            NULL));*/

        __hideCursor();

        __updateCursorPosition();
    }

    void TextFieldEx::__showCursor(void)
    {
        if (this->cursor) {
            this->cursorVisible = true;
            this->cursor->setVisible(true);
            this->cursor->runAction(RepeatForever::create(Blink::create(1, 1)));
        }
    }

    void TextFieldEx::__hideCursor(void)
    {
        if (this->cursor) {
            this->cursor->setVisible(false);
            this->cursorVisible = false;
            this->cursor->stopAllActions();
        }
    }

    void TextFieldEx::__updateCursorPosition(void)
    {
        if (this->cursor && this->insertPosUtf8 == this->charCount)
        {
            if (0 == this->getCharCount())
            {
                this->cursor->setPosition(Point(0, this->getContentSize().height / 2));
            }
            else
            {
                this->cursor->setPosition(Point(renderLabel->getContentSize().width, this->getContentSize().height / 2));
            }
        }
    }

    void  TextFieldEx::__moveCursor(int direction)
    {
        /*bool checkSupport = this->renderLabel->getLetter(0) != nullptr;
        if (!checkSupport)
        {
            MessageBeep(MB_ICONHAND);
            return;
        }*/

        auto newOffset = this->insertPosUtf8 + direction;

        if (newOffset > 0 && newOffset <= this->charCount) {

            const std::string* displayText = nullptr;
            if (!secureTextEntry)
                displayText = &this->getString();
            else if(!this->inputText.empty())
                displayText = &renderLabel->getString();

            if (direction < 0)
            {
                this->insertPos = internalUTF8MoveLeft(this->inputText, this->insertPos).size();

                auto s = internalUTF8MoveLeft(*displayText, this->cursorPos);
                
                auto width = internalCalcStringWidth(s, this->fontName, this->fontSize);
                this->cursor->setPosition(Point(width, this->getContentSize().height / 2));
                this->cursorPos = s.length();
            }
            else {
                this->insertPos = internalUTF8MoveRight(this->inputText, this->insertPos).size();

                auto s = internalUTF8MoveRight(*displayText, this->cursorPos);
                auto width = internalCalcStringWidth(s, this->fontName, this->fontSize);
                this->cursor->setPosition(Point(width, this->getContentSize().height / 2));
                this->cursorPos = s.length();
            }

            this->insertPosUtf8 = newOffset;
        }
        else if (newOffset == 0) {
            this->cursor->setPosition(Point(0, this->getContentSize().height / 2));
            this->insertPosUtf8 = newOffset;
            this->insertPos = 0;
            this->cursorPos = 0;
        }
        else {
            // MessageBeep(0);
        }
    }

    void  TextFieldEx::__moveCursorTo(float x)
    { // test
        // normalized x
        float normalizedX = 0;

        std::string displayText;
        if (!secureTextEntry)
        {
            displayText = this->inputText;
        }
        else
        {
            if (!this->inputText.empty()) { 
                displayText = renderLabel->getString();
            }
        }
        
        int length = displayText.length();
        int n = this->charCount; // UTF8 char counter

        int insertWhere = 0;
        int insertWhereUtf8 = 0;
        while (length > 0)
        {
            auto checkX = internalCalcStringWidth(displayText, this->fontName, this->fontSize);
            if (x >= checkX)
            {
                insertWhere = length;
                insertWhereUtf8 = n;
                normalizedX = checkX;
                break;
            }

            // clamp backward
            size_t backwardLen = 1;    // default, erase 1 byte
            while (0x80 == (0xC0 & displayText.at(displayText.length() - backwardLen)))
            {
                ++backwardLen;
            }
            
            --n;
            displayText.resize(displayText.length() - backwardLen);
           
            length -= backwardLen;
        }

        this->insertPos = !this->secureTextEntry ? insertWhere : insertWhereUtf8;
        this->cursorPos = insertWhere;
        this->insertPosUtf8 = insertWhereUtf8;
        this->cursor->setPosition(Point(normalizedX, this->getContentSize().height / 2));
    }
};

NS_CC_END

#endif

