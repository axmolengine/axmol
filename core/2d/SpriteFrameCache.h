/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Jason Booth
Copyright (c) 2009      Robert J Payne
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmolengine.github.io/

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

#ifndef __SPRITE_CCSPRITE_FRAME_CACHE_H__
#define __SPRITE_CCSPRITE_FRAME_CACHE_H__

#include <set>
#include <unordered_map>
#include <string>
#include "2d/SpriteFrame.h"
#include "2d/SpriteSheetLoader.h"
#include "base/Object.h"
#include "base/Value.h"
#include "base/Map.h"
#include "base/Data.h"

NS_AX_BEGIN

class Sprite;
class Texture2D;
class PolygonInfo;

/**
 * @addtogroup _2d
 * @{
 */

/** @class SpriteFrameCache
 * @brief Singleton that handles the loading of the sprite frames.

 The SpriteFrameCache loads SpriteFrames from a .plist file.
 A SpriteFrame contains information about how to use a sprite
 located in a sprite sheet.

 The .plist file contains the following elements:

 - `frames`:
   Dictionary of sprites. Key is the sprite's name, value a dict containing the sprite frame data.
   A sprite frame consists of the following values:
    - `spriteOffset`:     difference vector between the original sprite's center and the center of the trimmed sprite
    - `spriteSize`:       size of the trimmed sprite
    - `spriteSourceSize`: size of the original sprite
    - `textureRect`:      the position of the sprite in the sprite sheet
    - `textureRotated`:   true if the sprite is rotated clockwise
    - `anchor`:           anchor point in normalized coordinates (optional)
   Optional values when using polygon outlines
    - `triangles`:        3 indices per triangle, pointing to vertices and verticesUV coordinates
    - `vertices`:         vertices in sprite coordinates, each vertex consists of a pair of x and y coordinates
    - `verticesUV`:       vertices in the sprite sheet, each vertex consists of a pair of x and y coordinates

 - `metadata`:
   Dictionary containing additional information about the sprite sheet:
     - `format`:          plist file format, currently 3
     - `size`:            size of the texture (optional)
     - `textureFileName`: name of the texture's image file

 Use one of the following tools to create the .plist file and sprite sheet:
 - [TexturePacker](https://www.codeandweb.com/texturepacker/cocos2d)
 - [Zwoptex](https://zwopple.com/zwoptex/)

 @since v0.9
 @js cc.spriteFrameCache
 */
class AX_DLL SpriteFrameCache
{
public:
    /** Returns the shared instance of the Sprite Frame cache.
     *
     * @return The instance of the Sprite Frame Cache.
     * @js NA
     */
    static SpriteFrameCache* getInstance();

    /** Destroys the cache. It releases all the Sprite Frames and the retained instance.
     * @js NA
     */
    static void destroyInstance();

    /** Destructor.
     * @js NA
     * @lua NA
     */
    virtual ~SpriteFrameCache();

    /** Initialize method.
     *
     * @return if success return true.
     */
    bool init();

    /** Adds multiple Sprite Frames from a plist file.
     * A texture will be loaded automatically. The texture name will composed by replacing the .plist suffix with .png.
     * If you want to use another texture, you should use the addSpriteFramesWithFile(std::string_view plist, const
     * std::string& textureFileName) method.
     * @js addSpriteFrames
     * @lua addSpriteFrames
     *
     * @param spriteSheetFileName file name.
     * @param spriteSheetFormat
     */
    void addSpriteFramesWithFile(std::string_view spriteSheetFileName,
                                 uint32_t spriteSheetFormat = SpriteSheetFormat::PLIST);

    /** Adds multiple Sprite Frames from a plist file. The texture will be associated with the created sprite frames.
     @since v0.99.5
     * @js addSpriteFrames
     * @lua addSpriteFrames
     *
     * @param spriteSheetFileName Plist file name.
     * @param textureFileName Texture file name.
     * @param spriteSheetFormat
     */
    void addSpriteFramesWithFile(std::string_view spriteSheetFileName,
                                 std::string_view textureFileName,
                                 uint32_t spriteSheetFormat = SpriteSheetFormat::PLIST);

    /** Adds multiple Sprite Frames from a plist file. The texture will be associated with the created sprite frames.
     * @js addSpriteFrames
     * @lua addSpriteFrames
     *
     * @param spriteSheetFileName file name.
     * @param texture Texture pointer.
     * @param spriteSheetFormat
     */
    void addSpriteFramesWithFile(std::string_view spriteSheetFileName,
                                 Texture2D* texture,
                                 uint32_t spriteSheetFormat = SpriteSheetFormat::PLIST);

    /** Adds multiple Sprite Frames from a plist file content. The texture will be associated with the created sprite
     * frames.
     * @js NA
     * @lua addSpriteFrames
     *
     * @param content file content string.
     * @param texture Texture pointer.
     * @param spriteSheetFormat
     */
    void addSpriteFramesWithFileContent(const Data& content,
                                        Texture2D* texture,
                                        uint32_t spriteSheetFormat = SpriteSheetFormat::PLIST);

    /** Adds an sprite frame with a given name.
     If the name already exists, then the contents of the old name will be replaced with the new one.
     *
     * @param frame A certain sprite frame.
     * @param frameName The name of the sprite frame.
     */
    void addSpriteFrame(SpriteFrame* frame, std::string_view frameName);

    /** Check if multiple Sprite Frames from a plist file have been loaded.
     * @js NA
     * @lua NA
     *
     * @param plist Plist file name.
     * @return True if the file is loaded.
     */
    bool isSpriteFramesWithFileLoaded(std::string_view plist) const;

    /** Purges the dictionary of loaded sprite frames.
     * Call this method if you receive the "Memory Warning".
     * In the short term: it will free some resources preventing your app from being killed.
     * In the medium term: it will allocate more resources.
     * In the long term: it will be the same.
     */
    void removeSpriteFrames();

    /** Removes unused sprite frames.
     * Sprite Frames that have a retain count of 1 will be deleted.
     * It is convenient to call this method after when starting a new Scene.
     * @js NA
     */
    void removeUnusedSpriteFrames();

    void removeUnusedSpriteSheets();

    /** Deletes an sprite frame from the sprite frame cache.
     *
     * @param name The name of the sprite frame that needs to removed.
     */
    void removeSpriteFrameByName(std::string_view name);

    /** Removes multiple Sprite Frames from a plist file.
     * Sprite Frames stored in this file will be removed.
     * It is convenient to call this method when a specific texture needs to be removed.
     * @since v0.99.5
     *
     * @param plist The name of the plist that needs to removed.
     */
    void removeSpriteFramesFromFile(std::string_view plist);

    /** Removes multiple Sprite Frames from a plist file content.
     * Sprite Frames stored in this file will be removed.
     * It is convenient to call this method when a specific texture needs to be removed.
     *
     * @param plist_content The string of the plist content that needs to removed.
     * @js NA
     */
    void removeSpriteFramesFromFileContent(std::string_view plist_content);

    /** Removes all Sprite Frames associated with the specified textures.
     * It is convenient to call this method when a specific texture needs to be removed.
     * @since v0.995.
     *
     * @param texture The texture that needs to removed.
     */
    void removeSpriteFramesFromTexture(Texture2D* texture);

    /** Returns an Sprite Frame that was previously added.
     If the name is not found it will return nil.
     You should retain the returned copy if you are going to use it.
     * @js getSpriteFrame
     * @lua getSpriteFrame
     *
     * @param name A certain sprite frame name.
     * @return The sprite frame.
     */
    SpriteFrame* getSpriteFrameByName(std::string_view name);

    bool reloadTexture(std::string_view spriteSheetFileName);

    SpriteFrame* findFrame(std::string_view frame);

    std::string_view getSpriteFrameName(SpriteFrame* frame);

    /**  Record SpriteFrame with plist and frame name, add frame name
     *    and plist to index
     */
    void insertFrame(const std::shared_ptr<SpriteSheet>& spriteSheet,
                     std::string_view frameName,
                     SpriteFrame* frameObj);

    /** Delete frame from cache, rebuild index
     */
    bool eraseFrame(std::string_view frameName);

    void addSpriteFrameCapInset(SpriteFrame* spriteFrame, const Rect& capInsets, Texture2D* texture);

    void registerSpriteSheetLoader(std::shared_ptr<ISpriteSheetLoader> loader);
    void deregisterSpriteSheetLoader(uint32_t spriteSheetFormat);

    ISpriteSheetLoader* getSpriteSheetLoader(uint32_t spriteSheetFormat);

protected:
    // MARMALADE: Made this protected not private, as deriving from this class is pretty useful
    SpriteFrameCache() {}

    /** Removes multiple Sprite Frames from Dictionary.
     * @since v0.99.5
     */
    void removeSpriteFramesFromDictionary(ValueMap& dictionary);

    /** Delete a list of frames from cache, rebuild index
     */
    bool eraseFrames(const std::vector<std::string_view>& frame);
    /** Delete frame from index and SpriteFrame is kept.
     */
    bool removeSpriteSheet(std::string_view spriteSheetFileName);
    /** Clear index and all SpriteFrames.
     */
    void clear();

    inline bool hasFrame(std::string_view frame) const;
    inline bool isSpriteSheetInUse(std::string_view spriteSheetFileName) const;

    inline StringMap<SpriteFrame*>& getSpriteFrames();

    void markPlistFull(std::string_view spriteSheetFileName, bool full)
    {
        // _spriteSheets[spriteSheetFileName]->full = full;
        auto it = _spriteSheets.find(spriteSheetFileName);
        if (it != _spriteSheets.end())
        {
            it.value()->full = full;
        }
    }
    bool isPlistFull(std::string_view spriteSheetFileName) const
    {
        auto it = _spriteSheets.find(spriteSheetFileName);
        return it == _spriteSheets.end() ? false : it->second->full;
    }

private:
    StringMap<SpriteFrame*> _spriteFrames;
    hlookup::string_map<std::shared_ptr<SpriteSheet>> _spriteSheets;
    hlookup::string_map<std::shared_ptr<SpriteSheet>> _spriteFrameToSpriteSheetMap;

    std::map<uint32_t, std::shared_ptr<ISpriteSheetLoader>> _spriteSheetLoaders;
};

// end of _2d group
/// @}

NS_AX_END

#endif  // __SPRITE_CCSPRITE_FRAME_CACHE_H__
