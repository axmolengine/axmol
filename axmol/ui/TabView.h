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

#pragma once

#include "axmol/ui/AbstractCheckButton.h"
#include "axmol/ui/Widget.h"

/**
 * @addtogroup ui
 * @{
 */
namespace ax
{

class Label;

namespace ui
{

class LayoutGroup;
class TabView;
/**
 * the header button in TabView
 */
class AX_GUI_DLL TabHeader : public AbstractCheckButton
{
    friend class TabView;

public:
    enum class EventType
    {
        SELECTED,
        UNSELECTED
    };

    /**
     * Create and return a empty TabHeader instance pointer.
     */
    static TabHeader* create();

    /**
     * factory method to create a TabHeader instance.
     * This method uses less resource to create a TabHeader.
     * @param titleStr The text on the TabHeader
     * @param backGround The background image name in `std::string`.
     * @param cross The cross image name in `std::string`.
     * @param texType  The texture's resource type in `Widget::TextureResType`.
     * @return A TabHeader instance pointer
     */
    static TabHeader* create(std::string_view titleStr,
                             std::string_view backGround,
                             std::string_view cross,
                             TextureResType texType = TextureResType::LOCAL);

    /**
     * Create a TabHeader with various images.
     * @param titleStr The text on the TabHeader
     * @param backGround    backGround texture.
     * @param backGroundSelected    backGround selected state texture.
     * @param cross    cross texture.
     * @param backGroundDisabled    backGround disabled state texture.
     * @param frontCrossDisabled    cross dark state texture.
     * @param texType    @see `Widget::TextureResType`
     *
     * @return A TabHeader instance pointer.
     */
    static TabHeader* create(std::string_view titleStr,
                             std::string_view backGround,
                             std::string_view backGroundSelected,
                             std::string_view cross,
                             std::string_view backGroundDisabled,
                             std::string_view frontCrossDisabled,
                             TextureResType texType = TextureResType::LOCAL);
    /**
     * Return the inner Label renderer of TabHeader.
     * @return The TabHeader Label.
     */
    Label* getTitleRenderer() const;

    /**
     * Change the content of Header's text.
     *@param text The Header's text.
     */
    void setTitleText(std::string_view text);

    /**
     * get the TabHeader text
     *@return he TabHeader text
     */
    std::string_view getTitleText() const;

    /**
     * Change the color of he TabHeader text
     *@param color The he TabHeader text's color in Color32.
     */
    void setTitleColor(const Color32& color);

    /**
     * get the TabHeader text color.
     *@return Color32 of TabHeader text.
     */
    const Color32& getTitleColor() const;

    /**
     * Change the font size of TabHeader text
     *@param size TabHeader text's font size in float.
     */
    void setTitleFontSize(float size);

    /**
     * get the font size of TabHeader text
     *@return TabHeader text's font size in float.
     */
    float getTitleFontSize() const;

    /**
     * Change the font name of TabHeader text
     *@param fontName a font name string.
     */
    void setTitleFontName(std::string_view fontName);

    /**
     * get the font name of TabHeader text
     *@return font name in std::string
     */
    std::string_view getTitleFontName() const;

    /**
     * get the index this header in the TabView
     * @return -1 means not in any TabView
     */
    int getIndexInTabControl() const;

protected:
    TabHeader();
    ~TabHeader();

    void initRenderNode() override;
    void onSizeChanged() override;

    void updateContentSize() override;

    void releaseUpEvent() override;
    void dispatchSelectChangedEvent(bool select) override;

    void copySpecialProperties(Widget* model) override;

private:
    Label* _tabLabelRender;
    float _tabLabelFontSize;
    TabView* _tabView;

    using TabCallback = std::function<void(int, TabHeader::EventType)>;
    TabCallback _tabSelectedEvent;

    FontType _fontType;
};

/**
 * TabView, use header button switch container
 */
class AX_GUI_DLL TabView : public Widget
{
public:
    enum class Dock
    {
        TOP,
        LEFT,
        BOTTOM,
        RIGHT
    };

    enum class EventType
    {
        SELECT_CHANGED,
    };
    using TabViewCallback = std::function<void(int, EventType)>;

    static TabView* create();

    /// @{
    /// @name behaviours

    /**
     * remove the tab from this TabView
     * @param index The index of tab
     */
    void removeTab(int index);

    /**
     * set tab selected, switch the current selected tab and visible container
     * @param index The index of tab
     */
    void setSelectTab(int index);

    /**
     * set tab selected, switch the current selected tab and visible container
     * @param tabHeader The tab instance
     */
    void setSelectTab(TabHeader* tabHeader);

    /**
     * get TabHeader
     * @param index The index of tab
     */
    TabHeader* getTabHeader(int index) const;

    /**
     * get Container
     * @param index The index of tab
     */
    LayoutGroup* getTabContainer(int index) const;

    /**
     * insert tab, and init the position of header and container
     * @param index The index tab should be
     * @param header The header Button, will be a protected child in TabView
     * @param container The container, will be a protected child in TabView
     */
    void insertTab(int index, TabHeader* header, LayoutGroup* container);

    /**
     * get the count of tabs in this TabView
     * @return the count of tabs
     */
    size_t getTabCount() const;

    /**
     * get current selected tab's index
     * @return the current selected tab index
     */
    int getSelectedTabIndex() const;

    /**
     * get the index of tabCell in TabView, return -1 if not exists in.
     * @return the index of tabCell in TabView, `-1` means not exists in.
     */
    int indexOfTabHeader(const TabHeader* tabCell) const;

    /**
     * Add a callback function which would be called when selected tab changes.
     */
    void setTabChangedEventListener(const TabViewCallback& callback);

    /// @}

    /// @{
    /// @ properties

    /**
     * set header width, affect all tab
     * @param headerWidth each tab header's width
     */
    void setHeaderWidth(float headerWidth);

    /**
     * get tab header's width
     * @return header's width
     */
    float getHeaderWidth() const { return (float)_headerWidth; }

    /**
     * set header height, affect all tab
     * @param headerHeight each tab header's height
     */
    void setHeaderHeight(float headerHeight);

    /**
     * get tab header's height
     * @return header's height
     */
    int getHeaderHeight() const { return _headerHeight; }

    /**
     * ignore the textures' size in header, scale them with _headerWidth and _headerHeight
     * @param ignore is `true`, the header's texture scale with _headerWidth and _headerHeight
     *        ignore is `false`, use the texture's size, do not scale them
     */
    void ignoreHeadersTextureSize(bool ignore);

    /**
     * get whether ignore the textures' size in header, scale them with _headerWidth and _headerHeight
     * @return whether ignore the textures' size in header
     */
    bool isIgnoreHeadersTextureSize() const { return _ignoreHeaderTextureSize; };

    /**
     * set the delta zoom of selected tab
     * @param zoom The delta zoom
     */
    void setHeaderSelectedZoom(float zoom);

    /**
     * get the delta zoom of selected tab
     * @return zoom, the delta zoom
     */
    float getHeaderSelectedZoom() const { return _currentHeaderZoom; }

    /**
     * the header dock place of header in TabView
     * @param dockPlace The strip place
     */
    void setHeaderDockPlace(TabView::Dock dockPlace);
    TabView::Dock getHeaderDockPlace() const { return _headerDockPlace; }

    /// @}
protected:
    TabView();
    ~TabView();

    void onSizeChanged() override;
    void initTabHeadersPos(int startIndex);
    void initContainers();
    void copySpecialProperties(Widget* model) override;

    // dispatch selected changed
    void dispatchSelectedTabChanged(int tabIndex, TabHeader::EventType eventType);

private:
    typedef struct CellContainer
    {
        TabHeader* header;
        LayoutGroup* container;

        CellContainer(TabHeader* headerCell, LayoutGroup* layout)
        {
            header    = headerCell;
            container = layout;
        }
    } TabItem;

    // format tab header and container after insert
    void initAfterInsert(int index);
    void activeTabItem(TabItem* item);
    void deactiveTabItem(TabItem* item);
    Vec2 getHeaderAnchorWithDock() const;

    TabItem* _selectedItem;

    int _headerHeight;
    int _headerWidth;
    Dock _headerDockPlace;
    Vec2 _containerPosition;
    Vec2 _containerSize;
    float _currentHeaderZoom;
    bool _ignoreHeaderTextureSize;
    TabViewCallback _tabChangedCallback;

    // for index the cells and containers
    std::vector<TabItem*> _tabItems;
};

using TabControl = TabView;

}  // namespace ui
// end group
/// @}
}  // namespace ax
