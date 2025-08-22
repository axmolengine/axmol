/****************************************************************************
Copyright (c) 2008      Apple Inc. All Rights Reserved.
Copyright (c) 2010-2012 cocos2d-x.org
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
#include <unordered_map>
#include <span>

#include "axmol/base/Object.h"
#include "axmol/math/Math.h"
#include "axmol/base/Types.h"
#include "axmol/renderer/CustomCommand.h"

namespace ax
{

class Image;
class NinePatchInfo;
class SpriteFrame;

template<typename _PointerType>
struct TextureSliceDataBase
{
    _PointerType data   = nullptr;
    uint32_t dataSize   = 0;
    uint16_t layerIndex = 0;
    uint16_t mipLevel   = 0;
};

using MipmapInfo = TextureSliceDataBase<void*>;
using TextureSliceData = TextureSliceDataBase<const void*>;

namespace ui
{
class Scale9Sprite;
}

namespace rhi
{
class Texture;
class Texture;
class ProgramState;
}  // namespace rhi

/**
 * @addtogroup _2d
 * @{
 */

// CONSTANTS:

// CLASS INTERFACES:

/**
 * @brief Texture2D class. This class allows to easily create OpenGL 2D textures from images, text or raw data.
 * The created Texture2D object will always have power-of-two dimensions.
 * Depending on how you create the Texture2D object, the actual image area of the texture might be smaller than the
 * texture dimensions i.e. "contentSize" != (pixelsWide, pixelsHigh) and (maxS, maxT) != (1.0, 1.0). Be aware that the
 * content of the generated textures will be upside-down!
 */
class AX_DLL Texture2D : public Object
{
public:
    struct PixelFormatInfo
    {

        PixelFormatInfo(int aBpp, bool aCompressed, bool anAlpha) : bpp(aBpp), compressed(aCompressed), alpha(anAlpha)
        {}

        int bpp;
        bool compressed;
        bool alpha;
    };

    /**
     * Extension to set the Min / Mag filter
     */
    using TexParams = rhi::SamplerDesc;

public:

    static void chooseSamplerDesc(bool antialiasEnabled, bool mipEnabled, rhi::SamplerDesc& desc);

    /**
     */
    Texture2D();
    /**
     * @lua NA
     */
    virtual ~Texture2D();

    /** Initializes a texture from a Image object.

    We will use the format you passed to the function to convert the image format to the texture format.
    If you pass PixelFormat::NONE, we will auto detect the image render type and use that type for texture to render.
    @param image An Image object.
    @param format Texture pixel formats.
    */
    bool initWithImage(Image* image, PixelFormat renderFormat = PixelFormat::NONE, bool autoGenMipmaps = false);

    /** Initializes a texture from a string with dimensions, alignment, font name and font size.

     @param text A null terminated string.
     @param fontName The font name.
     @param fontSize The font size.
     @param dimensions The font dimension.
     @param hAlignment The font horizontal text alignment type.
     @param vAlignment The font vertical text alignment type.
     @param enableWrap Whether enable text wrap or not.
     @param overflow Whether shrink font size when content larger than the dimensions.
     */
    bool initWithString(std::string_view text,
                        std::string_view fontName,
                        float fontSize,
                        const Vec2& dimensions    = Vec2(0, 0),
                        TextHAlignment hAlignment = TextHAlignment::CENTER,
                        TextVAlignment vAlignment = TextVAlignment::TOP,
                        bool enableWrap           = true,
                        int overflow              = 0);

    /** Initializes a texture from a string using a text definition.

     @param text A null terminated string.
     @param textDefinition A FontDefinition object contains font attributes.
     */
    bool initWithString(std::string_view text, const FontDefinition& textDefinition);


    /** Initializes with a texture2d with data.

     @param data Specifies a pointer to the image data in memory.
     @param dataSize The image data length.
     @param pixelFormat The image pixelFormat.
     @param pixelsWide The image width.
     @param pixelsHigh The image height.
     @param contentSize The image content size.
     @param preMultipliedAlpha The texture has premultiplied alpha
     * @lua NA
     */
    bool initWithData(const void* data,
                      ssize_t dataSize,
                      rhi::PixelFormat pixelFormat,
                      int pixelsWide,
                      int pixelsHigh,
                      bool preMultipliedAlpha = false,
                      bool autoGenMipmaps     = false)
    {
        return initWithData(data, dataSize, pixelFormat, pixelFormat, pixelsWide, pixelsHigh, preMultipliedAlpha,
                            autoGenMipmaps);
    }

    /** Initializes with a texture2d with data.

     @param data Specifies a pointer to the image data in memory.
     @param dataSize The image data length.
     @param pixelFormat The image pixelFormat.
     @param renderFormat The format converted to.
     @param pixelsWide The image width.
     @param pixelsHigh The image height.
     @param contentSize The image content size.
     @param preMultipliedAlpha The texture has premultiplied alpha
     * @lua NA
     */
    bool initWithData(const void* data,
                      ssize_t dataSize,
                      rhi::PixelFormat pixelFormat,
                      rhi::PixelFormat renderFormat,
                      int pixelsWide,
                      int pixelsHigh,
                      bool preMultipliedAlpha = false,
                      bool autoGenMipmaps     = false);

    /** Initializes with mipmaps.

     @param mipmaps Specifies a pointer to the image data in memory.
     @param mipmapsNum The mipmaps number.
     @param pixelFormat The image pixelFormat.
     @param pixelsWide The image width.
     @param pixelsHigh The image height.
     @param preMultipliedAlpha The texture has premultiplied alpha
     */
    bool initWithMipmaps(std::span<MipmapInfo> mipmaps,
                         rhi::PixelFormat pixelFormat,
                         rhi::PixelFormat renderFormat,
                         int pixelsWide,
                         int pixelsHigh,
                         bool preMultipliedAlpha = false);

    bool initWithSpec(rhi::TextureDesc desc,
                      std::span<TextureSliceData> subDatas,
                      PixelFormat renderFormat = PixelFormat::NONE,
                      bool preMultipliedAlpha = false);

    /*
     * @brief update data by text
    */
    bool updateData(std::string_view text, const FontDefinition& textDefinition);

    /**
     * @brief update data by image
     *
     * @param image
     */
    bool updateData(Image* image);

    /**
     * @brief update data by subDatas
     *
     * @param subDatas
     */
    bool updateData(std::span<TextureSliceData> subDatas);

    /**
     * Update the texture with a new image data.
     * @param data Specifies a pointer to the image data in memory.
     * @param width Specifies the width of the texture.
     * @param height Specifies the height of the texture.
     * @param level Specifies the mipmap level to update. Default is 0.
     * @param layerIndex Specifies the layer index to update. Default is 0.
    */
    bool updateData(const void* data, int width, int height, int level = 0, int layerIndex = 0);

    /** Update texture sub data.

     @param data Specifies a pointer to the image data in memory.
     @param offsetX Specifies a texel offset in the x direction within the texture array.
     @param offsetY Specifies a texel offset in the y direction within the texture array.
     @param width Specifies the width of the texture subimage.
     @param height Specifies the height of the texture subimage.
     */
    bool updateSubData(const void* data, int offsetX, int offsetY, int width, int height, int level = 0, int layerIndex = 0);

    /*
    * Invalidate the texture for we can re-create RHI GPU texture when app context loss recovery
    * For example:
    *   texture2d->invalidate();
    *   texture2d->updateData(...); // re-create the texture
    */
    void invalidate();

    /**
    Drawing extensions to make it easy to draw basic quads using a Texture2D object.
    These functions require GL_TEXTURE_2D and both GL_VERTEX_ARRAY and GL_TEXTURE_COORD_ARRAY client states to be
    enabled.
    */
    /** Draws a texture at a given point. */
    void drawAtPoint(const Vec2& point, float globalZOrder);
    /** Draws a texture inside a rect.*/
    void drawInRect(const Rect& rect, float globalZOrder);

    bool isRenderTarget() const;

    void setTexParameters(const TexParams& params);

    /** Sets antialias texture parameters:
     - GL_TEXTURE_MIN_FILTER = GL_LINEAR
     - GL_TEXTURE_MAG_FILTER = GL_LINEAR

     @warning Calling this method could allocate additional texture memory.

     @since v0.8
     */
    void setAntiAliasTexParameters();

    /** Sets alias texture parameters:
    - GL_TEXTURE_MIN_FILTER = GL_NEAREST
    - GL_TEXTURE_MAG_FILTER = GL_NEAREST

    @warning Calling this method could allocate additional texture memory.

    @since v0.8
    */
    void setAliasTexParameters();

    /** Returns the pixel format.
     @since v2.0
     */
    const char* getStringForFormat() const;

    /** Returns the bits-per-pixel of the in-memory OpenGL texture
    @since v1.0
    */
    unsigned int getBitsPerPixelForFormat() const;

    /** Helper functions that returns bits per pixels for a given format.
     @since v2.0
     */
    unsigned int getBitsPerPixelForFormat(rhi::PixelFormat format) const;

    /** Get content size. */
    const Vec2& getContentSizeInPixels();

    /** Whether or not the texture has their Alpha premultiplied. */
    bool hasPremultipliedAlpha() const;
    void setPremultipliedAlpha(bool premultipliedAlpha);

    /** Whether or not the texture has mip maps.*/
    bool hasMipmaps() const;

    /** Gets the pixel format of the texture. */
    rhi::PixelFormat getPixelFormat() const;

    int getSamplerFlags() const { return _samplerFlags; }

    /** Gets the width of the texture in pixels. */
    int getPixelsWide() const;

    /** Gets the height of the texture in pixels. */
    int getPixelsHigh() const;

    rhi::Texture* getRHITexture() const;

    /** Gets max S. */
    float getMaxS() const;
    /** Sets max S. */
    void setMaxS(float maxS);

    /** Gets max T. */
    float getMaxT() const;
    /** Sets max T. */
    void setMaxT(float maxT);

    /** Get the texture content size.*/
    Vec2 getContentSize() const;

    std::string getPath() const { return _filePath; }

private:
    /**
     * A struct for storing 9-patch image capInsets.
     */

    class NinePatchInfo
    {
    public:
        Rect capInsetSize;
        std::unordered_map<SpriteFrame*, Rect> capInsetMap;
    };

    /**
     * Whether the texture contains a 9-patch capInset info or not.
     *
     * @return True is Texture contains a 9-patch info, false otherwise.
     */
    bool isContain9PatchInfo() const;

    /**
     * Get spriteFrame capInset, If spriteFrame can't be found in 9-patch info map,
     * then single 9-patch texture capInset will be returned.
     * If the arg is nullptr, the capInset of single 9-patch texture will be returned.
     *
     * @param spriteFrame A SpriteFrame object pointer.
     *
     * @return The capInset of the SpriteFrame object.
     */
    const Rect& getSpriteFrameCapInset(SpriteFrame* spriteFrame) const;
    /**
     * Remove the spriteFrame capInset info when the spriteFrame is removed.
     *
     * @param spriteFrame A SpriteFrame object pointer.
     */
    void removeSpriteFrameCapInset(SpriteFrame* spriteFrame);
    /**
     * Add capInset for sprite atlas.
     * When handling single texture, pass nullptr in the first arg.
     *
     * @param spritframe The sprite frame object.
     * @param capInsets The parsed capInset from a .9 patch image.
     */
    void addSpriteFrameCapInset(SpriteFrame* spritframe, const Rect& capInsets);

    void initProgram();

protected:

    void updateData(std::span<TextureSliceData> subDatas, PixelFormat renderFormat, bool compressed);

    /** width in pixels */
    int _pixelsWide;

    /** height in pixels */
    int _pixelsHigh;

    /** texture name */
    rhi::Texture* _rhiTexture;

    /** texture max S */
    float _maxS;

    /** texture max T */
    float _maxT;

    /** content size */
    Vec2 _contentSize;

    uint8_t _flags;
    uint8_t _samplerFlags;

    /** pixel format of the texture */
    rhi::PixelFormat _originalPF;

    NinePatchInfo* _ninePatchInfo;
    friend class SpriteFrameCache;
    friend class TextureCache;
    friend class ui::Scale9Sprite;

    std::string _filePath;

    rhi::ProgramState* _programState = nullptr;
    rhi::UniformLocation _mvpMatrixLocation;
    rhi::UniformLocation _textureLocation;
    CustomCommand _customCommand;
};

// end of textures group
/// @}

}
