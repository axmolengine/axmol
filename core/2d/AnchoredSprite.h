/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
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
#pragma once

#include <string>
#include "2d/Sprite.h"

NS_AX_BEGIN

/**
 * @addtogroup _2d
 * @{
 */

/**
 * Anchored Sprite
 *
 * This node follows the same rules as the normal Sprite.
 * but instead of anchoring the node with its children,
 * it instead moves the vertices and anchors them without
 * altering the node's children positions
 *
 * The default anchorPoint of Anchored Sprite is (0.5, 0.5).
 */
class AX_DLL AnchoredSprite : public Sprite
{
public:
    /// @name Creators
    /// @{

    /**
     * Creates an empty anchored sprite without texture. You can call setTexture method subsequently.
     *
     * @memberof Sprite
     * @return An autoreleased sprite object.
     */
    static AnchoredSprite* create();

    /**
     * Creates an anchored sprite with an image filename.
     *
     * After creation, the rect of sprite will be the size of the image,
     * and the offset will be (0,0).
     *
     * @param   filename A path to image file, e.g., "scene1/monster.png".
     * @return  An autoreleased sprite object.
     */
    static AnchoredSprite* create(std::string_view filename);
    static AnchoredSprite* create(std::string_view filename, PixelFormat format);

    /**
     * Creates an anchored polygon sprite with a polygon info.
     *
     * After creation, the rect of sprite will be the size of the image,
     * and the offset will be (0,0).
     *
     * @param   polygonInfo A path to image file, e.g., "scene1/monster.png".
     * @return  An autoreleased sprite object.
     */
    static AnchoredSprite* create(const PolygonInfo& info);

    /**
     * Creates an anchored sprite with an image filename and a rect.
     *
     * @param   filename A path to image file, e.g., "scene1/monster.png".
     * @param   rect     A subrect of the image file.
     * @return  An autoreleased sprite object.
     */
    static AnchoredSprite* create(std::string_view filename, const Rect& rect);

    /**
     * Creates an anchored sprite with a Texture2D object.
     *
     * After creation, the rect will be the size of the texture, and the offset will be (0,0).
     *
     * @param   texture    A pointer to a Texture2D object.
     * @return  An autoreleased sprite object.
     */
    static AnchoredSprite* createWithTexture(Texture2D* texture);

    /**
     * Creates an anchored sprite with a texture and a rect.
     *
     * After creation, the offset will be (0,0).
     *
     * @param   texture     A pointer to an existing Texture2D object.
     *                      You can use a Texture2D object for many sprites.
     * @param   rect        Only the contents inside the rect of this texture will be applied for this sprite.
     * @param   rotated     Whether or not the rect is rotated.
     * @return  An autoreleased sprite object.
     */
    static AnchoredSprite* createWithTexture(Texture2D* texture, const Rect& rect, bool rotated = false);

    /**
     * Creates an anchored sprite with an sprite frame.
     *
     * @param   spriteFrame    A sprite frame which involves a texture and a rect.
     * @return  An autoreleased sprite object.
     */
    static AnchoredSprite* createWithSpriteFrame(SpriteFrame* spriteFrame);

    /**
     * Creates an anchored sprite with an sprite frame name.
     *
     * A SpriteFrame will be fetched from the SpriteFrameCache by spriteFrameName param.
     * If the SpriteFrame doesn't exist it will raise an exception.
     *
     * @param   spriteFrameName The name of sprite frame.
     * @return  An autoreleased sprite object.
     */
    static AnchoredSprite* createWithSpriteFrameName(std::string_view spriteFrameName);

    /* This function will reposition the sprite's vertices itself instead of the node */
    virtual void setAnchorPoint(const Vec2& anchor) override;
    /* Gets the hit area of the anchored sprite, this requires special calculations for shifted vertices and should be used with touch event listeners */
    virtual Rect getTouchRect();

protected:
    virtual void setVertexCoords(const Rect& rect, V3F_C4B_T2F_Quad* outQuad) override;
    Vec2 _spriteVertexAnchor = Vec2::ANCHOR_MIDDLE;
};

// end of sprite_nodes group
/// @}

NS_AX_END
