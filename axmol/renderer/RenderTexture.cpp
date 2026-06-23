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
#include "axmol/renderer/RenderTexture.h"

#include "axmol/base/Utils.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/base/Director.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/rhi/DriverContext.h"
#include "axmol/rhi/Texture.h"
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
#    include "axmol/base/CustomEventListener.h"
#    include "axmol/base/EventType.h"
#    include "axmol/renderer/TextureCache.h"
#endif

namespace ax
{

RenderTexture::RenderTexture()
{
    _director = Director::getInstance();

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    _backgroundListener = CustomEventListener::create(EVENT_COME_TO_BACKGROUND, [this](CustomEvent*) {
        if (!_cachedTextureDirty)
            return;
        _cachedTextureDirty = false;
        newImage([this](RefPtr<Image> image) {
            if (image)
                _textureImage = image;
        });
    });
    _director->getEventDispatcher()->addEventListenerWithFixedPriority(_backgroundListener, -1);

    _foregroundListener =
        CustomEventListener::create(EVENT_COME_TO_FOREGROUND, [this](CustomEvent*) { setAntiAliasTexParameters(); });
    _director->getEventDispatcher()->addEventListenerWithFixedPriority(_foregroundListener, -1);
#endif
}

RenderTexture::~RenderTexture()
{
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    if (_rendererRecreatedListener)
        _director->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
    if (_backgroundListener)
        _director->getEventDispatcher()->removeEventListener(_backgroundListener);
    if (_foregroundListener)
        _director->getEventDispatcher()->removeEventListener(_foregroundListener);
#endif
    AX_SAFE_RELEASE(_renderTarget);
    AX_SAFE_RELEASE(_depthStencilTexture);
}

RenderTexture* RenderTexture::create(const Vec2& size, rhi::PixelFormat format, rhi::PixelFormat depthStencilFormat)
{
    return RenderTexture::create(static_cast<int>(size.width), static_cast<int>(size.height), format,
                                 depthStencilFormat);
}

RenderTexture* RenderTexture::create(int w, int h, rhi::PixelFormat format, rhi::PixelFormat depthStencilFormat)
{
    RenderTexture* ret = new RenderTexture();
    if (ret->initWithWidthAndHeight(w, h, format, depthStencilFormat))
    {
        ret->autorelease();
        return ret;
    }
    AX_SAFE_DELETE(ret);
    return nullptr;
}

RenderTexture* RenderTexture::createForCanvas(const Vec2& contentSize,
                                              rhi::PixelFormat format,
                                              rhi::PixelFormat depthStencilFormat)
{
    return RenderTexture::create(Director::getInstance()->canvasToPixels(contentSize), format, depthStencilFormat);
}

bool RenderTexture::initWithWidthAndHeight(int w, int h, rhi::PixelFormat format, rhi::PixelFormat depthStencilFormat)
{
    return initWithWidthAndHeightInternal(w, h, format, depthStencilFormat, std::nullopt);
}

bool RenderTexture::initWithWidthAndHeight(int w,
                                           int h,
                                           rhi::PixelFormat format,
                                           rhi::PixelFormat depthStencilFormat,
                                           const Color& clearColorHint)
{
    return initWithWidthAndHeightInternal(w, h, format, depthStencilFormat, clearColorHint);
}

bool RenderTexture::initWithWidthAndHeightInternal(int w,
                                                   int h,
                                                   rhi::PixelFormat format,
                                                   rhi::PixelFormat depthStencilFormat,
                                                   std::optional<Color> clearColorHint)
{
    AXASSERT(format == rhi::PixelFormat::RGBA8 || format == PixelFormat::RGB8 || format == PixelFormat::RGBA4,
             "only RGB and RGBA formats are valid for a render texture");

    _depthStencilFormat = depthStencilFormat;

    rhi::TextureDesc desc;
    desc.width        = w;
    desc.height       = h;
    desc.textureUsage = TextureUsage::RENDER_TARGET;
    desc.pixelFormat  = format;

    const Color initialClearColor = clearColorHint.value_or(Color(0, 0, 0, 0));
    if (!initWithSpec(desc, Texture2D::DEFAULT_SLICE_DATA, rhi::PixelFormat::NONE, !!AX_ENABLE_PREMULTIPLIED_ALPHA,
                      initialClearColor))
        return false;

    _width  = w;
    _height = h;
    _maxS   = 1;
    _maxT   = 1;

    // Create depth/stencil texture if requested
    if (depthStencilFormat != rhi::PixelFormat::NONE)
    {
        desc.pixelFormat     = depthStencilFormat;
        _depthStencilTexture = new Texture2D();
        if (!_depthStencilTexture->initWithSpec(desc, Texture2D::DEFAULT_SLICE_DATA))
        {
            AX_SAFE_RELEASE(_depthStencilTexture);
            return false;
        }
    }

    // Create render target
    _renderTarget =
        axdrv->createRenderTarget(_rhiTexture, _depthStencilTexture ? _depthStencilTexture->getRHITexture() : nullptr);
    _renderTarget->setColorTexture(_rhiTexture);

    if (_depthStencilTexture)
        _renderTarget->setDepthStencilTexture(_depthStencilTexture->getRHITexture());

    setAntiAliasTexParameters();

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    _cachedTextureDirty = true;

    if (!_rendererRecreatedListener)
    {
        _cachedDesc               = desc;
        _cachedDSDesc             = desc;
        _cachedDSDesc.pixelFormat = depthStencilFormat;
        _cachedDSFormat           = depthStencilFormat;

        _rendererRecreatedListener = CustomEventListener::create(
            EVENT_RENDERER_RECREATED, [this, w, h, format, depthStencilFormat, clearColorHint](CustomEvent*) {
            AX_SAFE_RELEASE(_renderTarget);
            AX_SAFE_RELEASE(_depthStencilTexture);
            AX_SAFE_RELEASE(_rhiTexture);
            initWithWidthAndHeightInternal(w, h, format, depthStencilFormat, clearColorHint);
#    if AX_ENABLE_CONTEXT_LOSS_RECOVERY
            if (_textureImage)
            {
                auto pixelSize = getPixelSize();
                VolatileTextureMgr::addDataTexture(this, _textureImage->getData(),
                                                   static_cast<int>(pixelSize.x * pixelSize.y * 4),
                                                   rhi::PixelFormat::RGBA8, pixelSize);
            }
#    endif
        });
        _director->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener, -2);
    }
#endif

    return true;
}

void RenderTexture::newImage(std::function<void(RefPtr<Image>)> imageCallback)
{
    AXASSERT(_originalPF == rhi::PixelFormat::RGBA8, "only RGBA8888 can be saved as image");

    if (!_renderTarget)
        return;

    bool hasPremultipliedAlpha = Texture2D::hasPremultipliedAlpha();

    auto callback = [hasPremultipliedAlpha, imageCallback](const rhi::PixelBufferDesc& pbd) {
        if (pbd)
        {
            auto image = utils::makeInstance<Image>(&Image::initWithRawData, pbd._data.getBytes(), pbd._data.getSize(),
                                                    pbd._width, pbd._height, 8, hasPremultipliedAlpha);
            imageCallback(image);
        }
        else
            imageCallback(nullptr);
    };
    _director->getRenderer()->readPixels(_renderTarget, callback);
}

bool RenderTexture::saveToFileAsNonPMA(std::string_view filename, bool isRGBA, SaveFileCallbackType callback)
{
    std::string basename(filename);
    std::transform(basename.begin(), basename.end(), basename.begin(), ::tolower);

    if (basename.find(".png") != std::string::npos)
        return saveToFileAsNonPMA(filename, Image::Format::PNG, isRGBA, std::move(callback));
    else if (basename.find(".jpg") != std::string::npos)
    {
        if (isRGBA)
            AXLOGD("RGBA is not supported for JPG format.");
        return saveToFileAsNonPMA(filename, Image::Format::JPG, false, std::move(callback));
    }
    else
        AXLOGD("Only PNG and JPG format are supported now!");

    return saveToFileAsNonPMA(filename, Image::Format::JPG, false, std::move(callback));
}

bool RenderTexture::saveToFile(std::string_view filename, bool isRGBA, SaveFileCallbackType callback)
{
    std::string basename(filename);
    std::transform(basename.begin(), basename.end(), basename.begin(), ::tolower);

    if (basename.find(".png") != std::string::npos)
        return saveToFile(filename, Image::Format::PNG, isRGBA, std::move(callback));
    else if (basename.find(".jpg") != std::string::npos)
    {
        if (isRGBA)
            AXLOGD("RGBA is not supported for JPG format.");
        return saveToFile(filename, Image::Format::JPG, false, std::move(callback));
    }
    else
        AXLOGD("Only PNG and JPG format are supported now!");

    return saveToFile(filename, Image::Format::JPG, false, std::move(callback));
}

bool RenderTexture::saveToFileAsNonPMA(std::string_view fileName,
                                       Image::Format format,
                                       bool isRGBA,
                                       SaveFileCallbackType callback)
{
    AXASSERT(format == Image::Format::JPG || format == Image::Format::PNG,
             "the image can only be saved as JPG or PNG format");
    if (isRGBA && format == Image::Format::JPG)
        AXLOGD("RGBA is not supported for JPG format");

    _saveFileCallback = std::move(callback);

    std::string fullpath = FileUtils::getInstance()->getWritablePath().append(fileName);

    auto renderer          = _director->getRenderer();
    auto saveToFileCommand = renderer->nextCallbackCommand();
    saveToFileCommand->init(0);
    saveToFileCommand->func = AX_CALLBACK_0(RenderTexture::onSaveToFile, this, std::move(fullpath), isRGBA, true);

    renderer->addCommand(saveToFileCommand);
    return true;
}

bool RenderTexture::saveToFile(std::string_view fileName,
                               Image::Format format,
                               bool isRGBA,
                               SaveFileCallbackType callback)
{
    AXASSERT(format == Image::Format::JPG || format == Image::Format::PNG,
             "the image can only be saved as JPG or PNG format");
    if (isRGBA && format == Image::Format::JPG)
        AXLOGD("RGBA is not supported for JPG format");

    _saveFileCallback = std::move(callback);

    std::string fullpath = FileUtils::getInstance()->getWritablePath().append(fileName);

    auto renderer          = _director->getRenderer();
    auto saveToFileCommand = renderer->nextCallbackCommand();
    saveToFileCommand->init(0);
    saveToFileCommand->func = AX_CALLBACK_0(RenderTexture::onSaveToFile, this, std::move(fullpath), isRGBA, false);

    renderer->addCommand(saveToFileCommand);
    return true;
}

void RenderTexture::onSaveToFile(std::string filename, bool isRGBA, bool forceNonPMA)
{
    auto callbackFunc = [this, self = RefPtr<RenderTexture>(this), filename = std::move(filename), isRGBA,
                         forceNonPMA](RefPtr<Image> image) {
        if (image)
        {
            if (forceNonPMA && image->hasPremultipliedAlpha())
            {
                _director->runAsync([image]() { image->reversePremultipliedAlpha(); },
                                    [self, this, image, _filename = std::move(filename), isRGBA, forceNonPMA]() {
                    image->saveToFile(_filename, !isRGBA);
                    if (self->_saveFileCallback)
                        self->_saveFileCallback(self, _filename);
                });
            }
            else
            {
                image->saveToFile(filename, !isRGBA);
                if (_saveFileCallback)
                    _saveFileCallback(this, filename);
            }
        }
        else
        {
            if (_saveFileCallback)
                _saveFileCallback(this, filename);
        }
    };
    newImage(callbackFunc);
}

}  // namespace ax
