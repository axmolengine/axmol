/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef __UIHBox_H__
#define __UIHBox_H__

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
 * HBox is just a convenient wrapper class for horizontal layout type.
 * HBox lays out its children in a single horizontal row.
 */
class AX_GUI_DLL HBox : public Layout
{
public:
    /**
     * Default constructor
     * @js ctor
     * @lua new
     */
    HBox();

    /**
     * Default destructor
     * @js NA
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
    virtual bool init() override;
    virtual bool initWithSize(const Vec2& size);
};

}  // namespace ui

// end of ui group
/// @}
NS_AX_END

#endif /* defined(__UIHBox__) */
