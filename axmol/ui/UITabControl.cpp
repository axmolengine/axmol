/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#include "platform/FileUtils.h"
#include "2d/Sprite.h"
#include "2d/Label.h"
#include "ui/UILayout.h"
#include "ui/UITabControl.h"

namespace ax
{

namespace ui
{

TabControl::TabControl()
    : _selectedItem(nullptr)
    , _headerHeight(20)
    , _headerWidth(50)
    , _headerDockPlace(Dock::TOP)
    , _containerPosition(Vec2::ZERO)
    , _containerSize(Vec2::ZERO)
    , _currentHeaderZoom(0.1f)
    , _ignoreHeaderTextureSize(true)
{
    this->_anchorPoint = Vec2(0.f, 0.f);
    setContentSize(Vec2(200, 200));
}

TabControl::~TabControl()
{
    for (auto&& item : _tabItems)
    {
        if (item)
            AX_SAFE_DELETE(item);
    }
    _tabItems.clear();
}

void TabControl::insertTab(int index, TabHeader* header, Layout* container)
{
    int cellSize = (int)_tabItems.size();
    if (index > cellSize)
    {
        AXLOGW("{}", "insert index error");
        return;
    }

    addProtectedChild(container, -3, -1);
    addProtectedChild(header, -2, -1);

    _tabItems.insert(_tabItems.begin() + index, new TabItem(header, container));
    header->_tabView = this;
    header->_tabSelectedEvent =
        AX_CALLBACK_2(TabControl::dispatchSelectedTabChanged, this);  // binding tab selected event

    initAfterInsert(index);
}

void TabControl::initAfterInsert(int index)
{
    auto cellSize   = _tabItems.size();
    auto tabItem    = _tabItems.at(index);
    auto headerCell = tabItem->header;
    auto container  = tabItem->container;

    if (cellSize == 1)
    {
        setSelectTab(0);
    }
    else
    {
        deactiveTabItem(tabItem);
    }

    headerCell->setContentSize(Vec2(_headerWidth, _headerHeight));
    headerCell->setAnchorPoint(getHeaderAnchorWithDock());
    if (headerCell->isIgnoreContentAdaptWithSize() == _ignoreHeaderTextureSize)
    {
        headerCell->ignoreContentAdaptWithSize(!_ignoreHeaderTextureSize);
        if (_ignoreHeaderTextureSize)
            headerCell->setContentSize(Vec2(_headerWidth, _headerHeight));
        headerCell->backGroundDisabledTextureScaleChangedWithSize();
        headerCell->backGroundSelectedTextureScaleChangedWithSize();
        headerCell->backGroundDisabledTextureScaleChangedWithSize();
        headerCell->frontCrossTextureScaleChangedWithSize();
        headerCell->frontCrossDisabledTextureScaleChangedWithSize();
    }

    initTabHeadersPos(index);
    if (_containerSize.equals(Vec2::ZERO))
        initContainers();
    else
    {
        container->setPosition(_containerPosition);
        container->setContentSize(_containerSize);
    }
}

void TabControl::removeTab(int index)
{
    int cellSize = (int)_tabItems.size();
    if (cellSize == 0 || index >= cellSize)
    {
        AXLOGW("{}", "no tab or remove index error");
        return;
    }

    auto tabItem = _tabItems.at(index);
    if (tabItem == _selectedItem)
        _selectedItem = nullptr;

    auto header    = tabItem->header;
    auto container = tabItem->container;
    if (tabItem)
        AX_SAFE_DELETE(tabItem);
    _tabItems.erase(_tabItems.begin() + index);

    if (header != nullptr)
    {
        header->_tabSelectedEvent = nullptr;
        header->_tabView          = nullptr;
        removeProtectedChild(header);
        removeProtectedChild(container);
    }
    initTabHeadersPos(index);
}

size_t TabControl::getTabCount() const
{
    return _tabItems.size();
}

void TabControl::setHeaderWidth(float headerWidth)
{
    _headerWidth = headerWidth;
    if (_headerDockPlace == Dock::TOP || _headerDockPlace == Dock::BOTTOM)
        initTabHeadersPos(0);
    if (_headerDockPlace == Dock::LEFT || _headerDockPlace == Dock::RIGHT)
        initContainers();
}

void TabControl::setHeaderHeight(float headerHeight)
{
    _headerHeight = headerHeight;
    if (_headerDockPlace == Dock::LEFT || _headerDockPlace == Dock::RIGHT)
        initTabHeadersPos(0);
    if (_headerDockPlace == Dock::TOP || _headerDockPlace == Dock::BOTTOM)
        initContainers();
}

void TabControl::setHeaderDockPlace(TabControl::Dock dockPlace)
{
    if (_headerDockPlace != dockPlace)
    {
        _headerDockPlace = dockPlace;
        initTabHeadersPos(0);
        initContainers();

        auto anpoint = getHeaderAnchorWithDock();
        for (auto&& item : _tabItems)
        {
            item->header->setAnchorPoint(anpoint);
        }
    }
}

ax::Vec2 TabControl::getHeaderAnchorWithDock() const
{
    Vec2 anpoint(.5f, .0f);
    switch (_headerDockPlace)
    {
    case Dock::TOP:
        break;
    case Dock::LEFT:
        anpoint.x = 1.f;
        anpoint.y = .5f;
        break;
    case Dock::BOTTOM:
        anpoint.x = .5f;
        anpoint.y = 1.f;
        break;
    case Dock::RIGHT:
        anpoint.x = 0.f;
        anpoint.y = .5f;
        break;
    default:
        break;
    }
    return anpoint;
}

void TabControl::onSizeChanged()
{
    initTabHeadersPos(0);
    initContainers();
}

void TabControl::initTabHeadersPos(int startIndex)
{
    int cellSize = (int)_tabItems.size();
    if (startIndex >= cellSize)
        return;

    float originX = _headerWidth * .5f;
    float originY = _contentSize.height - _headerHeight;
    Vec2 deltaPos(0.f, 0.f);
    switch (_headerDockPlace)
    {
    case Dock::TOP:
        deltaPos.x = _headerWidth;
        break;
    case Dock::LEFT:
        originX    = _headerWidth;
        originY    = _contentSize.height - _headerHeight * .5f;
        deltaPos.y = 0 - _headerHeight;
        break;
    case Dock::BOTTOM:
        originY    = _headerHeight;
        deltaPos.x = _headerWidth;
        break;
    case Dock::RIGHT:
        originX    = _contentSize.width - _headerWidth;
        originY    = _contentSize.height - _headerHeight * .5f;
        deltaPos.y = 0 - _headerHeight;
        break;
    default:
        break;
    }

    for (int cellI = startIndex; cellI < cellSize; cellI++)
    {
        auto headerCell = _tabItems.at(cellI)->header;
        headerCell->setPosition(Vec2(originX + cellI * deltaPos.x, originY + cellI * deltaPos.y));
    }
}

void TabControl::initContainers()
{
    switch (_headerDockPlace)
    {
    case Dock::TOP:
        _containerPosition = Vec2(0, 0);
        _containerSize     = Vec2(_contentSize.width, _contentSize.height - _headerHeight);
        break;
    case Dock::LEFT:
        _containerPosition = Vec2(_headerWidth, 0);
        _containerSize     = Vec2(_contentSize.width - _headerWidth, _contentSize.height);
        break;
    case Dock::BOTTOM:
        _containerPosition = Vec2(0, _headerHeight);
        _containerSize     = Vec2(_contentSize.width, _contentSize.height - _headerHeight);
        break;
    case Dock::RIGHT:
        _containerPosition = Vec2(0, 0);
        _containerSize     = Vec2(_contentSize.width - _headerWidth, _contentSize.height);
        break;
    default:
        break;
    }

    for (auto&& tabItem : _tabItems)
    {
        Layout* container = tabItem->container;
        container->setPosition(_containerPosition);
        container->setContentSize(_containerSize);
    }
}

TabHeader* TabControl::getTabHeader(int index) const
{
    if (index >= (int)getTabCount())
        return nullptr;

    return _tabItems.at(index)->header;
}

Layout* TabControl::getTabContainer(int index) const
{
    if (index >= (int)getTabCount())
        return nullptr;
    return _tabItems.at(index)->container;
}

void TabControl::dispatchSelectedTabChanged(int tabIndex, TabHeader::EventType eventType)
{
    if (eventType == TabHeader::EventType::SELECTED)
    {
        if (tabIndex <= -1 || tabIndex >= (int)_tabItems.size())
        {
            deactiveTabItem(_selectedItem);
            _selectedItem = nullptr;
        }
        else
        {
            deactiveTabItem(_selectedItem);
            auto tabItem = _tabItems.at(tabIndex);
            activeTabItem(tabItem);
            _selectedItem = tabItem;
        }
    }
    else if (eventType == TabHeader::EventType::UNSELECTED)
    {
        if (tabIndex >= 0 && tabIndex < (int)_tabItems.size())
        {
            auto tabItem = _tabItems.at(tabIndex);
            if (tabItem == _selectedItem)
            {
                deactiveTabItem(_selectedItem);
                _selectedItem = nullptr;
            }
        }
    }

    if (_tabChangedCallback != nullptr)
    {
        int currentIndex = getSelectedTabIndex();
        _tabChangedCallback(currentIndex, EventType::SELECT_CHANGED);
    }
}

void TabControl::setTabChangedEventListener(const ccTabControlCallback& callback)
{
    _tabChangedCallback = callback;
}

int TabControl::indexOfTabHeader(const TabHeader* tabCell) const
{
    int n = (int)_tabItems.size();
    for (auto i = 0; i < n; i++)
    {
        if (tabCell == _tabItems.at(i)->header)
        {
            return i;
        }
    }
    return -1;
}

TabControl* TabControl::create()
{
    TabControl* tabview = new TabControl();
    if (tabview->init())
    {
        tabview->autorelease();
        return tabview;
    }
    AX_SAFE_DELETE(tabview);
    return nullptr;
}

void TabControl::setSelectTab(int index)
{
    dispatchSelectedTabChanged(index, TabHeader::EventType::SELECTED);
}

void TabControl::setSelectTab(TabHeader* tabHeader)
{
    if (_selectedItem != nullptr && tabHeader == _selectedItem->header)
        return;

    setSelectTab(indexOfTabHeader(tabHeader));
}

void TabControl::setHeaderSelectedZoom(float zoom)
{
    if (_currentHeaderZoom != zoom)
    {
        _currentHeaderZoom = zoom;
        if (_selectedItem != nullptr)
        {
            auto currentHeader = _selectedItem->header;
            currentHeader->setScale(1.0f + _currentHeaderZoom);
        }
    }
}

void TabControl::activeTabItem(TabItem* item)
{
    if (item != nullptr)
    {
        item->header->setLocalZOrder(-1);
        item->header->setScale(1.0f + _currentHeaderZoom);
        item->header->setSelected(true);
        item->container->setVisible(true);
        _reorderProtectedChildDirty = true;
    }
}

void TabControl::deactiveTabItem(TabItem* item)
{
    if (item != nullptr)
    {
        item->header->setLocalZOrder(-2);
        item->header->setScale(1.0f);
        item->header->setSelected(false);
        item->container->setVisible(false);
        _reorderProtectedChildDirty = true;
    }
}

void TabControl::copySpecialProperties(Widget* model)
{
    auto srcTab = dynamic_cast<TabControl*>(model);
    if (srcTab != nullptr)
    {
        Widget::copySpecialProperties(srcTab);
        _headerWidth        = srcTab->_headerWidth;
        _headerHeight       = srcTab->_headerHeight;
        _headerDockPlace    = srcTab->_headerDockPlace;
        _currentHeaderZoom  = srcTab->_currentHeaderZoom;
        _tabChangedCallback = srcTab->_tabChangedCallback;
    }
}

void TabControl::ignoreHeadersTextureSize(bool ignore)
{
    if (_ignoreHeaderTextureSize == ignore)
        return;

    _ignoreHeaderTextureSize = ignore;
    for (auto&& item : _tabItems)
    {
        item->header->ignoreContentAdaptWithSize(!ignore);
        if (ignore)
            item->header->setContentSize(Vec2(_headerWidth, _headerHeight));
        item->header->backGroundDisabledTextureScaleChangedWithSize();
        item->header->backGroundSelectedTextureScaleChangedWithSize();
        item->header->backGroundDisabledTextureScaleChangedWithSize();
        item->header->frontCrossTextureScaleChangedWithSize();
        item->header->frontCrossDisabledTextureScaleChangedWithSize();
    }
}

int TabControl::getSelectedTabIndex() const
{
    return _selectedItem == nullptr ? -1 : indexOfTabHeader(_selectedItem->header);
}

TabHeader::TabHeader()
    : _tabLabelRender(nullptr)
    , _tabLabelFontSize(12)
    , _tabView(nullptr)
    , _tabSelectedEvent(nullptr)
    , _fontType(FontType::SYSTEM)
{}

TabHeader::~TabHeader()
{
    _tabLabelRender   = nullptr;
    _tabView          = nullptr;
    _tabSelectedEvent = nullptr;
}

TabHeader* TabHeader::create()
{
    TabHeader* tabcell = new TabHeader();
    if (tabcell->init())
    {
        tabcell->_frontCrossRenderer->setVisible(false);  // _isSelected == false
        tabcell->_anchorPoint = Vec2(.5f, 0.0f);
        tabcell->autorelease();
        return tabcell;
    }
    AX_SAFE_DELETE(tabcell);
    return nullptr;
}

TabHeader* TabHeader::create(std::string_view titleStr,
                             std::string_view backGround,
                             std::string_view cross,
                             TextureResType texType)
{
    TabHeader* tabcell = new TabHeader;
    if (tabcell->init(backGround, "", cross, "", "", texType))
    {
        tabcell->_frontCrossRenderer->setVisible(false);
        tabcell->_tabLabelRender->setString(titleStr);
        tabcell->_anchorPoint = Vec2(.5f, 0.0f);
        tabcell->autorelease();
        return tabcell;
    }
    AX_SAFE_DELETE(tabcell);
    return nullptr;
}

TabHeader* TabHeader::create(std::string_view titleStr,
                             std::string_view backGround,
                             std::string_view backGroundSelected,
                             std::string_view cross,
                             std::string_view backGroundDisabled,
                             std::string_view frontCrossDisabled,
                             TextureResType texType /*= TextureResType::LOCAL*/)
{
    TabHeader* tabcell = new TabHeader;
    if (tabcell->init(backGround, backGroundSelected, cross, backGroundDisabled, frontCrossDisabled, texType))
    {
        tabcell->_frontCrossRenderer->setVisible(false);
        tabcell->_tabLabelRender->setString(titleStr);
        tabcell->_anchorPoint = Vec2(.5f, 0.0f);
        tabcell->autorelease();
        return tabcell;
    }
    AX_SAFE_DELETE(tabcell);
    return nullptr;
}

void TabHeader::initRenderer()
{
    _backGroundBoxRenderer         = Sprite::create();
    _backGroundSelectedBoxRenderer = Sprite::create();
    _frontCrossRenderer            = Sprite::create();
    _backGroundBoxDisabledRenderer = Sprite::create();
    _frontCrossDisabledRenderer    = Sprite::create();
    _tabLabelRender                = Label::create();

    addProtectedChild(_backGroundBoxRenderer, -2, -1);
    addProtectedChild(_backGroundSelectedBoxRenderer, -2, -1);
    addProtectedChild(_frontCrossRenderer, -2, -1);
    addProtectedChild(_backGroundBoxDisabledRenderer, -2, -1);
    addProtectedChild(_frontCrossDisabledRenderer, -2, -1);
    addProtectedChild(_tabLabelRender, -1, -1);
}

void TabHeader::setTitleText(std::string_view text)
{
    if (text == getTitleText())
    {
        return;
    }

    _tabLabelRender->setString(text);
    updateContentSize();
    _tabLabelRender->setPosition(_contentSize * 0.5f);
}

std::string_view TabHeader::getTitleText() const
{
    if (nullptr == _tabLabelRender)
    {
        return ""sv;
    }
    return _tabLabelRender->getString();
}

void TabHeader::setTitleColor(const Color32& color)
{
    _tabLabelRender->setTextColor(color);
}

const Color32& TabHeader::getTitleColor() const
{
    if (nullptr == _tabLabelRender)
    {
        return Color32::WHITE;
    }
    return _tabLabelRender->getTextColor();
}

void TabHeader::setTitleFontSize(float size)
{
    _tabLabelFontSize = size;
    if (_fontType == FontType::SYSTEM)
    {
        _tabLabelRender->setSystemFontSize(_tabLabelFontSize);
    }
    else if (_fontType == FontType::TTF)
    {
        TTFConfig config = _tabLabelRender->getTTFConfig();
        config.fontSize  = _tabLabelFontSize;
        _tabLabelRender->setTTFConfig(config);
    }
    // we can't change font size of BMFont.
    if (FontType::BMFONT != _fontType)
    {
        updateContentSize();
    }
}

float TabHeader::getTitleFontSize() const
{
    return _tabLabelFontSize;
}

void TabHeader::updateContentSize()
{
    ProtectedNode::setContentSize(_customSize);
    onSizeChanged();
}

void TabHeader::setTitleFontName(std::string_view fontName)
{
    if (FileUtils::getInstance()->isFileExist(fontName))
    {
        std::string lowerCasedFontName{fontName};
        std::transform(lowerCasedFontName.begin(), lowerCasedFontName.end(), lowerCasedFontName.begin(), ::tolower);
        if (lowerCasedFontName.find(".fnt") != std::string::npos)
        {
            _tabLabelRender->setBMFontFilePath(fontName);
            _fontType = FontType::BMFONT;
        }
        else
        {
            TTFConfig config    = _tabLabelRender->getTTFConfig();
            config.fontFilePath = fontName;
            config.fontSize     = _tabLabelFontSize;
            _tabLabelRender->setTTFConfig(config);
            _fontType = FontType::TTF;
        }
    }
    else
    {
        _tabLabelRender->setSystemFontName(fontName);
        if (_fontType == FontType::TTF)
        {
            _tabLabelRender->requestSystemFontRefresh();
        }
        _tabLabelRender->setSystemFontSize(_tabLabelFontSize);
        _fontType = FontType::SYSTEM;
    }
    this->updateContentSize();
}

Label* TabHeader::getTitleRenderer() const
{
    return _tabLabelRender;
}

std::string_view TabHeader::getTitleFontName() const
{
    if (this->_fontType == FontType::SYSTEM)
    {
        return _tabLabelRender->getSystemFontName();
    }
    else if (this->_fontType == FontType::TTF)
    {
        return _tabLabelRender->getTTFConfig().fontFilePath;
    }
    else
    {
        return _tabLabelRender->getBMFontFilePath();
    }
    return ""sv;
}

void TabHeader::onSizeChanged()
{
    AbstractCheckButton::onSizeChanged();
    _tabLabelRender->setPosition(_contentSize * 0.5f);
}

void TabHeader::releaseUpEvent()
{
    Widget::releaseUpEvent();

    if (!_isSelected)
    {
        setSelected(true);
        dispatchSelectChangedEvent(true);
    }
}

void TabHeader::dispatchSelectChangedEvent(bool select)
{
    if (_tabView == nullptr)
        return;

    EventType eventType = (select ? EventType::SELECTED : EventType::UNSELECTED);

    if (_tabSelectedEvent != nullptr)
    {
        int index = _tabView->indexOfTabHeader(this);
        if (index != -1)
            _tabSelectedEvent(index, eventType);
    }

    if (_ccEventCallback != nullptr)
    {
        _ccEventCallback(this, static_cast<int>(eventType));
    }
}

int TabHeader::getIndexInTabControl() const
{
    if (_tabView == nullptr)
        return -1;
    return _tabView->indexOfTabHeader(this);
}

void TabHeader::copySpecialProperties(Widget* model)
{
    auto header = dynamic_cast<TabHeader*>(model);
    if (header != nullptr)
    {
        AbstractCheckButton::copySpecialProperties(model);
        _fontType         = header->_fontType;
        _tabLabelFontSize = header->_tabLabelFontSize;
        _tabSelectedEvent = header->_tabSelectedEvent;
    }
}

}  // namespace ui
}