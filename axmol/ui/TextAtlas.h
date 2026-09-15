/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/ui/Widget.h"
#include "axmol/ui/GUIExport.h"

namespace ax
{

/**
 * @addtogroup ui
 * @{
 */

class Label;
struct AX_DLL ResourceData;

namespace ui
{

/**
 * @brief UI TextAtlas widget.
 */
class AX_GUI_DLL TextAtlas : public Widget
{

    DECLARE_CLASS_GUI_INFO

public:
    /**
     * Default constructor.
     *
     * @lua new
     */
    TextAtlas();

    /**
     * Default destructor.
     *
     * @lua NA
     */
    virtual ~TextAtlas();

    /**
     * Create a TexAtlas object.
     *
     * @return An autoreleased TextAtlas object.
     */
    static TextAtlas* create();

    /**
     * Create a LabelAtlas from a char map file.
     *
     * @param stringValue A given string needs to be displayed.
     * @param charMapFile A given char map file name.
     * @param itemWidth The element width.
     * @param itemHeight The element height.
     * @param startCharMap The starting char of the atlas.
     * @return An autoreleased TextAtlas object.
     */
    static TextAtlas* create(std::string_view stringValue,
                             std::string_view charMapFile,
                             int itemWidth,
                             int itemHeight,
                             std::string_view startCharMap);

    /** Initializes the LabelAtlas with a string, a char map file(the atlas), the width and height of each element and
     * the starting char of the atlas.
     *
     * @param stringValue A given string needs to be displayed.
     * @param charMapFile A given char map file name.
     * @param itemWidth The element width.
     * @param itemHeight The element height.
     * @param startCharMap The starting char of the atlas.
     */

    void setProperty(std::string_view stringValue,
                     std::string_view charMapFile,
                     int itemWidth,
                     int itemHeight,
                     std::string_view startCharMap);

    /**Set string value for labelatlas.
     *
     * @param value A given string needs to be displayed.
     */
    void setString(std::string_view value);

    /**Get string value for labelatlas.
     *
     * @return The string value of TextAtlas.
     */
    std::string_view getString() const;

    /**
     * Gets the UTF-8 character count of the label.
     * Note: This count will be larger than the raw string length,
     * if you want to get the raw string length, you should call this->getString().size() instead
     *
     * @return  UTF-8 character count.
     */
    ssize_t getCharCount() const;

    // override "resolvePreferredSize" method of widget.
    Vec2 resolvePreferredSize(const Vec2& /*sizeHint*/) const override;

    // override "getRenderNode" method of widget.
    Node* getRenderNode() override;

    /**
     * Returns the "class name" of widget.
     */
    std::string getDescription() const override;

    /**
     */
    void updateLayout() override;

    ResourceData getRenderFile();

protected:
    void initRenderNode() override;
    void onSizeChanged() override;

    void labelAtlasScaleChangedWithSize();
    Widget* createCloneInstance() override;
    void copySpecialProperties(Widget* model) override;

protected:
    Label* _labelAtlasRenderer;
    std::string _stringValue;
    std::string _charMapFileName;
    int _itemWidth;
    int _itemHeight;
    std::string _startCharMap;
    bool _labelAtlasRendererAdaptDirty;
};

}  // namespace ui

// end of ui group
/// @}

}  // namespace ax
