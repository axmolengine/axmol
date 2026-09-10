/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include "axmol/ui/Widget.h"
#include "axmol/ui/GUIExport.h"

/**
 * @addtogroup ui
 * @{
 */
namespace ax
{

class Label;
struct AX_DLL ResourceData;

namespace ui
{

/**
 * A widget for displaying BMFont label.
 */
class AX_GUI_DLL TextBMFont : public Widget
{

    DECLARE_CLASS_GUI_INFO

public:
    /**
     * Default constructor
     * @lua new
     */
    TextBMFont();

    /**
     * Default destructor
     * @lua NA
     */
    virtual ~TextBMFont();

    /**
     * Allocates and initializes.
     */
    static TextBMFont* create();

    static TextBMFont* create(std::string_view text, std::string_view filename);

    /** init a bitmap font atlas with an initial string and the FNT file */
    void setFntFile(std::string_view fileName);

    /** set string value for labelbmfont*/
    void setString(std::string_view value);

    /** get string value for labelbmfont*/
    std::string_view getString() const;

    /**
     * Gets the UTF-8 character count of the label.
     * Note: This count will be larger than the raw string length,
     * if you want to get the raw string length, you should call this->getString().size() instead
     *
     * @return  UTF-8 character count.
     */
    ssize_t getCharCount() const;

    Vec2 resolvePreferredSize(const Vec2& /*sizeHint*/) const override;
    Node* getRenderNode() override;
    /**
     * Returns the "class name" of widget.
     */
    std::string getDescription() const override;

    ResourceData getRenderFile();

    /**
     * reset TextBMFont inner label
     */
    void resetRender();

protected:
    void initRenderNode() override;
    void onSizeChanged() override;

    void labelBMFontScaleChangedWithSize();
    Widget* createCloneInstance() override;
    void copySpecialProperties(Widget* model) override;
    void updateLayout() override;

protected:
    Label* _labelBMFontRenderer;
    std::string _fntFileName;
    std::string _stringValue;
    bool _labelBMFontRendererAdaptDirty;
};

}  // namespace ui
}  // namespace ax
// end of ui group
/// @}
