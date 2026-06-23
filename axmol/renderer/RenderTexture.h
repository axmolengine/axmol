/****************************************************************************
Copyright (c) 2009      Jason Booth
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

#include <optional>
#include <memory>

#include "axmol/base/RefPtr.h"
#include "axmol/renderer/Texture2D.h"
#include "axmol/platform/Image.h"
#include "axmol/rhi/RenderTarget.h"

namespace ax
{

class Camera;
class CustomEventListener;

/**
 * @brief RenderTexture is a render target resource, inheriting from Texture2D.
 *
 * It holds the GPU resources (color texture, optional depth/stencil texture,
 * render target binding) needed for offscreen rendering.
 *
 * To render into a RenderTexture, use RenderTexturePass:
 * @code
 * auto rt = RenderTexture::create(w, h);
 * auto pass = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(rt), tlx::adopt_object);
 * pass->begin();
 * // ... render content ...
 * pass->end();
 * @endcode
 *
 * @section Y-axis consistency across graphics backends
 *
 * On OpenGL, offscreen FBO textures have a bottom-left origin, while Direct3D,
 * Metal, and Vulkan use a top-left origin.  By default (`_autoFlipY == true`),
 * RenderTexturePass automatically flips the visiting camera's projection matrix
 * on OpenGL so that the rendered texture has a consistent top-left origin
 * across all platforms.
 *
 * If you access `getRenderTarget()` directly and manage the rendering pipeline
 * yourself (e.g., Live2D CubismRenderer), you must check
 * `rhi::DriverContext::isOpenGL()` and flip the Y-axis manually.
 *
 * If you render through RenderTexturePass, call
 * `RenderTexturePass::setAutoFlipY(false)` to preserve the raw backend-native
 * orientation. This is useful when the texture will be consumed by a system
 * that expects the native coordinate system, such as VR/OpenXR compositors.
 */
class AX_DLL RenderTexture : public Texture2D
{
public:
    /**
     * @brief Callback invoked after an asynchronous save-to-file request completes.
     */
    using SaveFileCallbackType = std::function<void(RenderTexture*, std::string_view)>;

    /**
     * @brief Creates a render texture with an optional depth/stencil attachment.
     *
     * @param w Width in pixels.
     * @param h Height in pixels.
     * @param format Color texture pixel format.
     * @param depthStencilFormat Depth/stencil texture format, or NONE if not needed.
     * @return A new autoreleased RenderTexture instance, or nullptr on failure.
     */
    static RenderTexture* create(int w,
                                 int h,
                                 rhi::PixelFormat format             = rhi::PixelFormat::RGBA8,
                                 rhi::PixelFormat depthStencilFormat = rhi::PixelFormat::NONE);
    static RenderTexture* create(const Vec2& size,
                                 rhi::PixelFormat format             = rhi::PixelFormat::RGBA8,
                                 rhi::PixelFormat depthStencilFormat = rhi::PixelFormat::NONE);

    /**
     * @brief Creates a render texture for content measured in canvas coordinates.
     *
     * The input size is interpreted in canvas coordinates and converted to physical
     * texture pixels using Director::canvasToPixels(). Use create() when the size is
     * already in texture pixels.
     */
    static RenderTexture* createForCanvas(const Vec2& contentSize,
                                          rhi::PixelFormat format             = rhi::PixelFormat::RGBA8,
                                          rhi::PixelFormat depthStencilFormat = rhi::PixelFormat::NONE);

    /**
     * @brief Returns the underlying render target used for offscreen rendering.
     */
    rhi::RenderTarget* getRenderTarget() const { return _renderTarget; }

    /**
     * @brief Returns the optional depth/stencil texture attached to this render texture.
     */
    Texture2D* getDepthStencilTexture() const { return _depthStencilTexture; }

    /**
     * @brief Returns the depth/stencil attachment format.
     */
    rhi::PixelFormat getDepthStencilFormat() const { return _depthStencilFormat; }

    /**
     * @brief Returns the render target size in pixels.
     *
     * This is a render-target semantic alias of Texture2D::getPixelSize().
     */
    Vec2 getRenderSize() const { return getPixelSize(); }

    /**
     * @brief Reads the render texture pixels into an Image asynchronously.
     *
     * @param imageCallback Callback receiving the captured image.
     */
    void newImage(std::function<void(RefPtr<Image>)> imageCallback);

    /**
     * @brief Saves the render texture to a file using the filename extension to infer the format.
     */
    bool saveToFile(std::string_view filename, bool isRGBA = true, SaveFileCallbackType callback = nullptr);

    /**
     * @brief Saves the render texture to a file using an explicit image format.
     */
    bool saveToFile(std::string_view fileName,
                    Image::Format format,
                    bool isRGBA                   = true,
                    SaveFileCallbackType callback = nullptr);

    /**
     * @brief Saves the render texture to a file after converting pixels from premultiplied alpha.
     */
    bool saveToFileAsNonPMA(std::string_view filename, bool isRGBA = true, SaveFileCallbackType callback = nullptr);

    /**
     * @brief Saves the render texture to a file using an explicit format after converting pixels from premultiplied
     * alpha.
     */
    bool saveToFileAsNonPMA(std::string_view fileName,
                            Image::Format format,
                            bool isRGBA,
                            SaveFileCallbackType callback);

public:
    RenderTexture();
    ~RenderTexture();

    bool initWithWidthAndHeight(int w,
                                int h,
                                rhi::PixelFormat format,
                                rhi::PixelFormat depthStencilFormat = rhi::PixelFormat::NONE);
    bool initWithWidthAndHeight(int w,
                                int h,
                                rhi::PixelFormat format,
                                rhi::PixelFormat depthStencilFormat,
                                const Color& clearColorHint);

private:
    bool initWithWidthAndHeightInternal(int w,
                                        int h,
                                        rhi::PixelFormat format,
                                        rhi::PixelFormat depthStencilFormat,
                                        std::optional<Color> clearColorHint);
    void onSaveToFile(std::string fileName, bool isRGBA, bool forceNonPMA);

    Director* _director                  = nullptr;
    rhi::RenderTarget* _renderTarget     = nullptr;
    Texture2D* _depthStencilTexture      = nullptr;
    rhi::PixelFormat _depthStencilFormat = rhi::PixelFormat::NONE;

    std::function<void(RenderTexture*, std::string_view)> _saveFileCallback = nullptr;

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    rhi::TextureDesc _cachedDesc;
    rhi::TextureDesc _cachedDSDesc;
    rhi::PixelFormat _cachedDSFormat = rhi::PixelFormat::NONE;
    RefPtr<Image> _textureImage;
    bool _cachedTextureDirty                  = false;
    EventListener* _backgroundListener        = nullptr;
    EventListener* _foregroundListener        = nullptr;
    EventListener* _rendererRecreatedListener = nullptr;
#endif

    AX_DISALLOW_COPY_AND_ASSIGN(RenderTexture);
};

}  // namespace ax
