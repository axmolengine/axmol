/****************************************************************************
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

#ifndef __UIRelativeBox_H__
#define __UIRelativeBox_H__

#include "ui/UILayout.h"
#include "ui/GUIExport.h"

NS_AX_BEGIN
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
     * @js ctor
     * @lua new
     */
    RelativeBox();

    /**
     * Default destructor.
     * @js NA
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
    virtual bool init() override;
    virtual bool initWithSize(const Vec2& size);
};

}  // namespace ui

// end of ui group
/// @}
NS_AX_END

#endif /* defined(__UIRelativeBox__) */
