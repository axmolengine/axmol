/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
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

#include "2d/MenuItem.h"
#include "2d/ActionInterval.h"
#include "2d/Sprite.h"
#include "2d/LabelAtlas.h"
#include "2d/Label.h"
#include "base/UTF8.h"
#include <stdarg.h>

namespace ax
{

static int _globalFontSize         = kItemSize;
static std::string _globalFontName = "Marker Felt";
static bool _globalFontNameRelease = false;

const unsigned int kZoomActionTag = 0xc0c05002;

//
// MenuItem
//

MenuItem* MenuItem::create()
{
    return MenuItem::create((const ccMenuCallback&)nullptr);
}

MenuItem* MenuItem::create(const ccMenuCallback& callback)
{
    MenuItem* ret = new MenuItem();
    ret->initWithCallback(callback);
    ret->autorelease();
    return ret;
}

bool MenuItem::initWithCallback(const ccMenuCallback& callback)
{
    setAnchorPoint(Vec2(0.5f, 0.5f));
    _callback = callback;
    _enabled  = true;
    _selected = false;
    return true;
}

MenuItem::~MenuItem() {}

void MenuItem::selected()
{
    _selected = true;
}

void MenuItem::unselected()
{
    _selected = false;
}

void MenuItem::activate()
{
    if (_enabled)
    {
        if (_callback)
        {
            _callback(this);
        }
#if AX_ENABLE_SCRIPT_BINDING
        BasicScriptData data(this);
        ScriptEvent scriptEvent(kMenuClickedEvent, &data);
        ScriptEngineManager::sendEventToLua(scriptEvent);
#endif
    }
}

void MenuItem::setEnabled(bool enabled)
{
    _enabled = enabled;
}

bool MenuItem::isEnabled() const
{
    return _enabled;
}

Rect MenuItem::rect() const
{
    return Rect(_position.x - _contentSize.width * _anchorPoint.x, _position.y - _contentSize.height * _anchorPoint.y,
                _contentSize.width, _contentSize.height);
}

bool MenuItem::isSelected() const
{
    return _selected;
}

void MenuItem::setCallback(const ccMenuCallback& callback)
{
    _callback = callback;
}

std::string MenuItem::getDescription() const
{
    return fmt::format("<MenuItem | tag = {}>", _tag);
}

//
// CCMenuItemLabel
//

void MenuItemLabel::setLabel(Node* var)
{
    if (var)
    {
        var->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
        setContentSize(var->getContentSize());
        addChild(var);
    }

    if (_label)
    {
        removeChild(_label, true);
    }

    _label = var;
}

MenuItemLabel* MenuItemLabel::create(Node* label, const ccMenuCallback& callback)
{
    MenuItemLabel* ret = new MenuItemLabel();
    ret->initWithLabel(label, callback);
    ret->autorelease();
    return ret;
}

MenuItemLabel* MenuItemLabel::create(Node* label)
{
    MenuItemLabel* ret = new MenuItemLabel();
    ret->initWithLabel(label, (const ccMenuCallback&)nullptr);
    ret->autorelease();
    return ret;
}

bool MenuItemLabel::initWithLabel(Node* label, const ccMenuCallback& callback)
{
    MenuItem::initWithCallback(callback);
    _originalScale = 1.0f;
    _colorBackup   = Color3B::WHITE;
    setDisabledColor(Color3B(126, 126, 126));
    this->setLabel(label);

    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    return true;
}

MenuItemLabel::~MenuItemLabel() {}

void MenuItemLabel::setString(std::string_view label)
{
    dynamic_cast<LabelProtocol*>(_label)->setString(label);
    this->setContentSize(_label->getContentSize());
}

std::string_view MenuItemLabel::getString() const
{
    auto label = dynamic_cast<LabelProtocol*>(_label);
    return label->getString();
}

void MenuItemLabel::activate()
{
    if (_enabled)
    {
        this->stopAllActions();
        this->setScale(_originalScale);
        MenuItem::activate();
    }
}

void MenuItemLabel::selected()
{
    // subclass to change the default action
    if (_enabled)
    {
        MenuItem::selected();

        Action* action = getActionByTag(kZoomActionTag);
        if (action)
        {
            this->stopAction(action);
        }
        else
        {
            _originalScale = this->getScale();
        }

        Action* zoomAction = ScaleTo::create(0.1f, _originalScale * 1.2f);
        zoomAction->setTag(kZoomActionTag);
        this->runAction(zoomAction);
    }
}

void MenuItemLabel::unselected()
{
    // subclass to change the default action
    if (_enabled)
    {
        MenuItem::unselected();
        this->stopActionByTag(kZoomActionTag);
        Action* zoomAction = ScaleTo::create(0.1f, _originalScale);
        zoomAction->setTag(kZoomActionTag);
        this->runAction(zoomAction);
    }
}

void MenuItemLabel::setEnabled(bool enabled)
{
    if (_enabled != enabled)
    {
        if (enabled == false)
        {
            _colorBackup = this->getColor();
            this->setColor(_disabledColor);
        }
        else
        {
            this->setColor(_colorBackup);
        }
    }
    MenuItem::setEnabled(enabled);
}

//
// CCMenuItemAtlasFont
//

MenuItemAtlasFont* MenuItemAtlasFont::create(std::string_view value,
                                             std::string_view charMapFile,
                                             int itemWidth,
                                             int itemHeight,
                                             char startCharMap)
{
    return MenuItemAtlasFont::create(value, charMapFile, itemWidth, itemHeight, startCharMap,
                                     (const ccMenuCallback&)nullptr);
}

MenuItemAtlasFont* MenuItemAtlasFont::create(std::string_view value,
                                             std::string_view charMapFile,
                                             int itemWidth,
                                             int itemHeight,
                                             char startCharMap,
                                             const ccMenuCallback& callback)
{
    MenuItemAtlasFont* ret = new MenuItemAtlasFont();
    ret->initWithString(value, charMapFile, itemWidth, itemHeight, startCharMap, callback);
    ret->autorelease();
    return ret;
}

bool MenuItemAtlasFont::initWithString(std::string_view value,
                                       std::string_view charMapFile,
                                       int itemWidth,
                                       int itemHeight,
                                       char startCharMap,
                                       const ccMenuCallback& callback)
{
    AXASSERT(value.size() != 0, "value length must be greater than 0");
    LabelAtlas* label = LabelAtlas::create(value, charMapFile, itemWidth, itemHeight, startCharMap);
    if (MenuItemLabel::initWithLabel(label, callback))
    {
        // do something ?
    }
    return true;
}

//
// CCMenuItemFont
//

void MenuItemFont::setFontSize(int s)
{
    _globalFontSize = s;
}

int MenuItemFont::getFontSize()
{
    return _globalFontSize;
}

void MenuItemFont::setFontName(std::string_view name)
{
    if (_globalFontNameRelease)
    {
        _globalFontName.clear();
    }
    _globalFontName        = name;
    _globalFontNameRelease = true;
}

std::string_view MenuItemFont::getFontName()
{
    return _globalFontName;
}

MenuItemFont* MenuItemFont::create(std::string_view value, const ccMenuCallback& callback)
{
    MenuItemFont* ret = new MenuItemFont();
    ret->initWithString(value, callback);
    ret->autorelease();
    return ret;
}

MenuItemFont* MenuItemFont::create(std::string_view value)
{
    MenuItemFont* ret = new MenuItemFont();
    ret->initWithString(value, (const ccMenuCallback&)nullptr);
    ret->autorelease();
    return ret;
}

MenuItemFont::MenuItemFont() : _fontSize(0), _fontName("") {}

MenuItemFont::~MenuItemFont()
{
    AXLOGV("In the destructor of MenuItemFont ({}).", fmt::ptr(this));
}

bool MenuItemFont::initWithString(std::string_view value, const ccMenuCallback& callback)
{
    AXASSERT(!value.empty(), "Value length must be greater than 0");

    _fontName = _globalFontName;
    _fontSize = _globalFontSize;

    Label* label = Label::createWithSystemFont(value, _fontName, _fontSize);
    if (MenuItemLabel::initWithLabel(label, callback))
    {
        // do something ?
    }
    return true;
}

void MenuItemFont::setFontSizeObj(int s)
{
    _fontSize = s;
    dynamic_cast<Label*>(_label)->setSystemFontSize(_fontSize);
    this->setContentSize(dynamic_cast<Label*>(_label)->getContentSize());
}

int MenuItemFont::getFontSizeObj() const
{
    return _fontSize;
}

void MenuItemFont::setFontNameObj(std::string_view name)
{
    _fontName = name;
    dynamic_cast<Label*>(_label)->setSystemFontName(_fontName);
    this->setContentSize(dynamic_cast<Label*>(_label)->getContentSize());
}

std::string_view MenuItemFont::getFontNameObj() const
{
    return _fontName;
}

//
// CCMenuItemSprite
//

void MenuItemSprite::setNormalImage(Node* image)
{
    if (image != _normalImage)
    {
        if (image)
        {
            addChild(image);
            image->setAnchorPoint(Vec2(0, 0));
        }

        if (_normalImage)
        {
            removeChild(_normalImage, true);
        }

        _normalImage = image;
        if (_normalImage != nullptr)
        {
            this->setContentSize(_normalImage->getContentSize());
        }
        this->updateImagesVisibility();
    }
}

void MenuItemSprite::setSelectedImage(Node* image)
{
    if (image != _normalImage)
    {
        if (image)
        {
            addChild(image);
            image->setAnchorPoint(Vec2(0, 0));
        }

        if (_selectedImage)
        {
            removeChild(_selectedImage, true);
        }

        _selectedImage = image;
        this->updateImagesVisibility();
    }
}

void MenuItemSprite::setDisabledImage(Node* image)
{
    if (image != _normalImage)
    {
        if (image)
        {
            addChild(image);
            image->setAnchorPoint(Vec2(0, 0));
        }

        if (_disabledImage)
        {
            removeChild(_disabledImage, true);
        }

        _disabledImage = image;
        this->updateImagesVisibility();
    }
}

//
// CCMenuItemSprite
//

MenuItemSprite* MenuItemSprite::create(Node* normalSprite, Node* selectedSprite, Node* disabledSprite)
{
    return MenuItemSprite::create(normalSprite, selectedSprite, disabledSprite, (const ccMenuCallback&)nullptr);
}

MenuItemSprite* MenuItemSprite::create(Node* normalSprite, Node* selectedSprite, const ccMenuCallback& callback)
{
    return MenuItemSprite::create(normalSprite, selectedSprite, nullptr, callback);
}

MenuItemSprite* MenuItemSprite::create(Node* normalSprite,
                                       Node* selectedSprite,
                                       Node* disabledSprite,
                                       const ccMenuCallback& callback)
{
    MenuItemSprite* ret = new MenuItemSprite();
    ret->initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, callback);
    ret->autorelease();
    return ret;
}

bool MenuItemSprite::initWithNormalSprite(Node* normalSprite,
                                          Node* selectedSprite,
                                          Node* disabledSprite,
                                          const ccMenuCallback& callback)
{
    MenuItem::initWithCallback(callback);
    setNormalImage(normalSprite);
    setSelectedImage(selectedSprite);
    setDisabledImage(disabledSprite);

    if (_normalImage)
    {
        this->setContentSize(_normalImage->getContentSize());
    }

    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    return true;
}

/**
 @since v0.99.5
 */
void MenuItemSprite::selected()
{
    MenuItem::selected();

    if (_normalImage)
    {
        if (_disabledImage)
        {
            _disabledImage->setVisible(false);
        }

        if (_selectedImage)
        {
            _normalImage->setVisible(false);
            _selectedImage->setVisible(true);
        }
        else
        {
            _normalImage->setVisible(true);
        }
    }
}

void MenuItemSprite::unselected()
{
    MenuItem::unselected();
    this->updateImagesVisibility();
}

void MenuItemSprite::setEnabled(bool bEnabled)
{
    if (_enabled != bEnabled)
    {
        MenuItem::setEnabled(bEnabled);
        this->updateImagesVisibility();
    }
}

// Helper
void MenuItemSprite::updateImagesVisibility()
{
    if (_enabled)
    {
        if (_normalImage)
            _normalImage->setVisible(true);
        if (_selectedImage)
            _selectedImage->setVisible(false);
        if (_disabledImage)
            _disabledImage->setVisible(false);
    }
    else
    {
        if (_disabledImage)
        {
            if (_normalImage)
                _normalImage->setVisible(false);
            if (_selectedImage)
                _selectedImage->setVisible(false);
            if (_disabledImage)
                _disabledImage->setVisible(true);
        }
        else
        {
            if (_normalImage)
                _normalImage->setVisible(true);
            if (_selectedImage)
                _selectedImage->setVisible(false);
            if (_disabledImage)
                _disabledImage->setVisible(false);
        }
    }
}

///
/// MenuItemImage
///

MenuItemImage* MenuItemImage::create()
{
    MenuItemImage* ret = new MenuItemImage();
    if (ret->init())
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool MenuItemImage::init()
{
    return initWithNormalImage("", "", "", (const ccMenuCallback&)nullptr);
}

MenuItemImage* MenuItemImage::create(std::string_view normalImage, std::string_view selectedImage)
{
    return MenuItemImage::create(normalImage, selectedImage, "", (const ccMenuCallback&)nullptr);
}

MenuItemImage* MenuItemImage::create(std::string_view normalImage,
                                     std::string_view selectedImage,
                                     const ccMenuCallback& callback)
{
    return MenuItemImage::create(normalImage, selectedImage, "", callback);
}

MenuItemImage* MenuItemImage::create(std::string_view normalImage,
                                     std::string_view selectedImage,
                                     std::string_view disabledImage,
                                     const ccMenuCallback& callback)
{
    MenuItemImage* ret = new MenuItemImage();
    if (ret->initWithNormalImage(normalImage, selectedImage, disabledImage, callback))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

MenuItemImage* MenuItemImage::create(std::string_view normalImage,
                                     std::string_view selectedImage,
                                     std::string_view disabledImage)
{
    MenuItemImage* ret = new MenuItemImage();
    if (ret->initWithNormalImage(normalImage, selectedImage, disabledImage, (const ccMenuCallback&)nullptr))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

bool MenuItemImage::initWithNormalImage(std::string_view normalImage,
                                        std::string_view selectedImage,
                                        std::string_view disabledImage,
                                        const ccMenuCallback& callback)
{
    Node* normalSprite   = nullptr;
    Node* selectedSprite = nullptr;
    Node* disabledSprite = nullptr;

    if (normalImage.size() > 0)
    {
        normalSprite = Sprite::create(normalImage);
    }

    if (selectedImage.size() > 0)
    {
        selectedSprite = Sprite::create(selectedImage);
    }

    if (disabledImage.size() > 0)
    {
        disabledSprite = Sprite::create(disabledImage);
    }
    return initWithNormalSprite(normalSprite, selectedSprite, disabledSprite, callback);
}

//
// Setter of sprite frames
//
void MenuItemImage::setNormalSpriteFrame(SpriteFrame* frame)
{
    setNormalImage(Sprite::createWithSpriteFrame(frame));
}

void MenuItemImage::setSelectedSpriteFrame(SpriteFrame* frame)
{
    setSelectedImage(Sprite::createWithSpriteFrame(frame));
}

void MenuItemImage::setDisabledSpriteFrame(SpriteFrame* frame)
{
    setDisabledImage(Sprite::createWithSpriteFrame(frame));
}

//
// MenuItemToggle
//

MenuItemToggle* MenuItemToggle::createWithCallback(const ccMenuCallback& callback, const Vector<MenuItem*>& menuItems)
{
    MenuItemToggle* ret = new MenuItemToggle();
    ret->MenuItem::initWithCallback(callback);
    ret->autorelease();
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        for (const auto& item : menuItems)
        {
            if (item)
            {
                sEngine->retainScriptObject(ret, item);
            }
        }
    }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    ret->_subItems      = menuItems;
    ret->_selectedIndex = UINT_MAX;
    ret->setSelectedIndex(0);
    return ret;
}

MenuItemToggle* MenuItemToggle::createWithCallback(const ccMenuCallback& callback, MenuItem* item, ...)
{
    va_list args;
    va_start(args, item);
    MenuItemToggle* ret = new MenuItemToggle();
    ret->initWithCallback(callback, item, args);
    ret->autorelease();
    va_end(args);
    return ret;
}

MenuItemToggle* MenuItemToggle::create()
{
    MenuItemToggle* ret = new MenuItemToggle();
    ret->initWithItem(nullptr);
    ret->autorelease();
    return ret;
}

bool MenuItemToggle::initWithCallback(const ccMenuCallback& callback, MenuItem* item, va_list args)
{
    MenuItem::initWithCallback(callback);

    int z       = 0;
    MenuItem* i = item;

#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS

    while (i)
    {
        z++;
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
        if (sEngine)
        {
            sEngine->retainScriptObject(this, i);
        }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
        _subItems.pushBack(i);
        i = va_arg(args, MenuItem*);
    }
    _selectedIndex = UINT_MAX;
    this->setSelectedIndex(0);
    return true;
}

MenuItemToggle* MenuItemToggle::create(MenuItem* item)
{
    MenuItemToggle* ret = new MenuItemToggle();
    ret->initWithItem(item);
    ret->autorelease();
    return ret;
}

bool MenuItemToggle::initWithItem(MenuItem* item)
{
    MenuItem::initWithCallback((const ccMenuCallback&)nullptr);

    if (item)
    {
        addSubItem(item);
    }
    _selectedIndex = UINT_MAX;
    this->setSelectedIndex(0);

    setCascadeColorEnabled(true);
    setCascadeOpacityEnabled(true);

    return true;
}

void MenuItemToggle::addSubItem(MenuItem* item)
{
#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        sEngine->retainScriptObject(this, item);
    }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    _subItems.pushBack(item);
}

void MenuItemToggle::cleanup()
{
    for (const auto& item : _subItems)
    {
#if defined(AX_NATIVE_CONTROL_SCRIPT) && !AX_NATIVE_CONTROL_SCRIPT
        ScriptEngineManager::getInstance()->getScriptEngine()->releaseScriptObject(this, item);
#endif
        item->cleanup();
    }
    MenuItem::cleanup();
}

void MenuItemToggle::setSelectedIndex(unsigned int index)
{
    if (index != _selectedIndex && !_subItems.empty())
    {
        _selectedIndex = index;
        if (_selectedItem)
        {
            _selectedItem->removeFromParentAndCleanup(false);
        }

        _selectedItem = _subItems.at(_selectedIndex);
        this->addChild(_selectedItem);
        Vec2 s = _selectedItem->getContentSize();
        this->setContentSize(s);
        _selectedItem->setPosition(s.width / 2, s.height / 2);
    }
}

void MenuItemToggle::selected()
{
    MenuItem::selected();
    _subItems.at(_selectedIndex)->selected();
}

void MenuItemToggle::unselected()
{
    MenuItem::unselected();
    _subItems.at(_selectedIndex)->unselected();
}

void MenuItemToggle::activate()
{
    // update index
    if (_enabled)
    {
        unsigned int newIndex = (_selectedIndex + 1) % _subItems.size();
        this->setSelectedIndex(newIndex);
    }
    MenuItem::activate();
}
void MenuItemToggle::setEnabled(bool enabled)
{
    if (_enabled != enabled)
    {
        MenuItem::setEnabled(enabled);

        for (const auto& item : _subItems)
        {
            item->setEnabled(enabled);
        }
    }
}

MenuItem* MenuItemToggle::getSelectedItem()
{
    return _subItems.at(_selectedIndex);
}

}
