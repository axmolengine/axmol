/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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
#ifndef __CCLABEL_ATLAS_H__
#define __CCLABEL_ATLAS_H__

#include "2d/AtlasNode.h"
#if AX_LABELATLAS_DEBUG_DRAW
#    include "renderer/CustomCommand.h"
#    include "2d/DrawNode.h"
#endif
namespace ax
{

/**
 * @addtogroup _2d
 * @{
 */

/**
 * @class LabelAtlas
 * @brief LabelAtlas is a subclass of AtlasNode.
 *
 * It can be as a replacement of Label since it is MUCH faster.
 *
 * LabelAtlas versus Label:
 * - LabelAtlas is MUCH faster than Label.
 * - LabelAtlas "characters" have a fixed height and width.
 * - LabelAtlas "characters" can be anything you want since they are taken from an image file.
 *
 * A more flexible class is LabelBMFont. It supports variable width characters and it also has a nice editor.
 */
class AX_DLL LabelAtlas : public AtlasNode, public LabelProtocol
{
public:
    /** Creates the LabelAtlas with a string, a char map file(the atlas), the width and height of each element and the
     * starting char of the atlas. */
    static LabelAtlas* create(std::string_view string,
                              std::string_view charMapFile,
                              int itemWidth,
                              int itemHeight,
                              int startCharMap);

    /**
     * Creates the LabelAtlas with a string and a configuration file.
     * @since v2.0
     */
    static LabelAtlas* create(std::string_view string, std::string_view fntFile);

    /**
     * Creates the LabelAtlas with a string, a texture, the width and height of each element and the starting char of
     * the atlas.
     */
    static LabelAtlas* create(std::string_view string,
                              Texture2D* texture,
                              int itemWidth,
                              int itemHeight,
                              int startCharMap);

    /** Initializes the LabelAtlas with a string, a char map file(the atlas), the width and height of each element and
     * the starting char of the atlas. */
    bool initWithString(std::string_view string,
                        std::string_view charMapFile,
                        int itemWidth,
                        int itemHeight,
                        int startCharMap);

    /**
     * Initializes the LabelAtlas with a string and a configuration file.
     * @since v2.0
     */
    bool initWithString(std::string_view string, std::string_view fntFile);

    /** Initializes the LabelAtlas with a string, a texture, the width and height in points of each element and the
     * starting char of the atlas */
    bool initWithString(std::string_view string, Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);

    virtual void setString(std::string_view label) override;
    virtual std::string_view getString() const override;

    virtual void updateAtlasValues() override;
    /**
     * @js NA
     */
    virtual std::string getDescription() const override;

#if AX_LABELATLAS_DEBUG_DRAW
    virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;
#endif

    LabelAtlas()
    {
#if AX_LABELATLAS_DEBUG_DRAW
        _debugDrawNode = DrawNode::create();
        addChild(_debugDrawNode);
#endif
    }

    virtual ~LabelAtlas() { _string.clear(); }

protected:
    virtual void updateColor() override;

#if AX_LABELATLAS_DEBUG_DRAW
    DrawNode* _debugDrawNode;
#endif

    // string to render
    std::string _string;
    // the first char in the char map
    int _mapStartChar;
};

// end group
/// @}

}

#endif  //__CCLABEL_ATLAS_H__
