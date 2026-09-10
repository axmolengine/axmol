/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/ui/LayoutGroup.h"
#include "axmol/ui/GUIExport.h"

namespace ax
{
/**
 * @addtogroup ui
 * @{
 */

namespace ui
{

/**
 * VBox is just a convenient wrapper class for vertical layout type.
 * VBox lays out its children in a single vertical column.
 */
class AX_GUI_DLL VBox : public LayoutGroup
{
public:
    /**
     * Default constructor
     * @lua new
     */
    VBox();

    /**
     * Default destructor
     * @lua NA
     */
    virtual ~VBox();

    /**
     * Create a empty VBox instance.
     * @return A VBox instance pointer.
     */
    static VBox* create();

    /**
     * Create a VBox with a certain size.
     *@param size The content size of the layout.
     *@return A VBox instance pointer.
     */
    static VBox* create(const Vec2& size);

    // initializes state of widget.
    bool init() override;
    virtual bool initWithSize(const Vec2& size);
};

}  // namespace ui

// end of ui group
/// @}
}  // namespace ax
