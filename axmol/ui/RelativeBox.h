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
 *@brief RelativeBox is just a convenient wrapper class for relative layout type.
 * RelativeBox lays out its children relative to a widget or a position.
 */
class AX_GUI_DLL RelativeBox : public Layout
{

public:
    /**
     * Default constructor.
     * @lua new
     */
    RelativeBox();

    /**
     * Default destructor.
     * @lua NA
     */
    virtual ~RelativeBox();

    /**
     * Create an empty RelativeBox instance.
     * @return A RelativeBox instance.
     */
    static RelativeBox* create();

    /**
     * @brief Create a RelativeBox with a fixed size.
     *
     * @param size The size in `Vec2`.
     * @return A RelativeBox instance.
     */
    static RelativeBox* create(const Vec2& size);

    // initializes state of widget.
    bool init() override;
    virtual bool initWithSize(const Vec2& size);
};

}  // namespace ui

// end of ui group
/// @}
}  // namespace ax
