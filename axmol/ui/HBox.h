/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

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
 * HBox is just a convenient wrapper class for horizontal layout type.
 * HBox lays out its children in a single horizontal row.
 */
class AX_GUI_DLL HBox : public LayoutGroup
{
public:
    /**
     * Default constructor
     * @lua new
     */
    HBox();

    /**
     * Default destructor
     * @lua NA
     */
    virtual ~HBox();

    /**
     * Create a empty HBox instance.
     * @return A HBox instance pointer.
     */
    static HBox* create();

    /**
     *Create a HBox with a certain size.
     *@param size The content size of the layout.
     *@return A HBox instance pointer.
     */
    static HBox* create(const Vec2& size);

    // initializes state of widget.
    bool init() override;
    virtual bool initWithSize(const Vec2& size);
};

}  // namespace ui

// end of ui group
/// @}
}  // namespace ax
