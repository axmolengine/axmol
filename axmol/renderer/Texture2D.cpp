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

/*
 * Support for RGBA_4_4_4_4 and RGBA_5_5_5_1 was copied from:
 * https://devforums.apple.com/message/37855#37855 by a1studmuffin
 */

#include "axmol/renderer/Texture2D.h"
#include "axmol/platform/Image.h"
#include "axmol/platform/GL.h"
#include "axmol/base/Utils.h"
#include "axmol/platform/Device.h"
#include "axmol/base/Config.h"
#include "axmol/base/Macros.h"
#include "axmol/base/text_utils.h"
#include "axmol/base/Configuration.h"
#include "axmol/platform/PlatformMacros.h"
#include "axmol/base/Director.h"
#include "axmol/base/NinePatchImageParser.h"
#include "axmol/rhi/DriverBase.h"
#include "axmol/rhi/ProgramState.h"
#include "axmol/renderer/Shaders.h"
#include "axmol/rhi/RHIUtils.h"
#include "axmol/renderer/Renderer.h"

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
#    include "axmol/renderer/TextureCache.h"
#endif

namespace ax
{
static bool createStringTextureData(std::string_view text,
                                    const FontDefinition& textDefinition,
                                    Data& outData,
                                    int& imageWidth,
                                    int& imageHeight,
                                    bool& premultipliedAlpha)
{
    Device::TextAlign align;

    if (TextVAlignment::TOP == textDefinition._vertAlignment)
    {
        align = (TextHAlignment::CENTER == textDefinition._alignment) ? Device::TextAlign::TOP
                : (TextHAlignment::LEFT == textDefinition._alignment) ? Device::TextAlign::TOP_LEFT
                                                                        : Device::TextAlign::TOP_RIGHT;
    }
    else if (TextVAlignment::CENTER == textDefinition._vertAlignment)
    {
        align = (TextHAlignment::CENTER == textDefinition._alignment) ? Device::TextAlign::CENTER
                : (TextHAlignment::LEFT == textDefinition._alignment) ? Device::TextAlign::LEFT
                                                                        : Device::TextAlign::RIGHT;
    }
    else if (TextVAlignment::BOTTOM == textDefinition._vertAlignment)
    {
        align = (TextHAlignment::CENTER == textDefinition._alignment) ? Device::TextAlign::BOTTOM
                : (TextHAlignment::LEFT == textDefinition._alignment) ? Device::TextAlign::BOTTOM_LEFT
                                                                        : Device::TextAlign::BOTTOM_RIGHT;
    }
    else
    {
        AXASSERT(false, "Not supported alignment format!");
        return false;
    }

#if (AX_TARGET_PLATFORM != AX_PLATFORM_ANDROID) && (AX_TARGET_PLATFORM != AX_PLATFORM_IOS)
    AXASSERT(textDefinition._stroke._strokeEnabled == false, "Currently stroke only supported on iOS and Android!");
#endif

    auto textDef            = textDefinition;
    auto contentScaleFactor = AX_CONTENT_SCALE_FACTOR();
    textDef._fontSize *= contentScaleFactor;
    textDef._dimensions.width *= contentScaleFactor;
    textDef._dimensions.height *= contentScaleFactor;
    textDef._stroke._strokeSize *= contentScaleFactor;
    textDef._shadow._shadowEnabled = false;

    outData = Device::getTextureDataForText(text, textDef, align, imageWidth, imageHeight, premultipliedAlpha);
    if (outData.isNull())
        return false;

    const auto maxTextureSize = axdrv->getMaxTextureSize();
    if (imageWidth > maxTextureSize || imageHeight > maxTextureSize)
    {
        AXLOGW("Texture2D::initWithString fail, the texture size:{}x{} too large, max texture size:{}", imageWidth,
                imageHeight, maxTextureSize);
        return false;
    }

    return true;
}

void Texture2D::chooseSamplerDesc(bool antialiasEnabled, bool mipEnabled, rhi::SamplerDesc& desc)
{
    if (antialiasEnabled)
    {
        desc.minFilter = rhi::SamplerFilter::MIN_LINEAR;
        desc.magFilter = rhi::SamplerFilter::MAG_LINEAR;
    }
    else
    {
        desc.minFilter = rhi::SamplerFilter::MIN_NEAREST;
        desc.magFilter = rhi::SamplerFilter::MAG_NEAREST;
    }

    if (mipEnabled)
    {
        if(desc.mipFilter == rhi::SamplerFilter::MIP_DEFAULT)
            desc.mipFilter = antialiasEnabled ? rhi::SamplerFilter::MIP_LINEAR : rhi::SamplerFilter::MIP_NEAREST;
    }
    else
        desc.mipFilter = rhi::SamplerFilter::MIP_DEFAULT;
}

Texture2D::Texture2D()
    : _originalPF(rhi::PixelFormat::NONE)
    , _pixelsWide(0)
    , _pixelsHigh(0)
    , _rhiTexture(nullptr)
    , _maxS(0.0)
    , _maxT(0.0)
    , _flags(TextureFlag::ANTIALIAS_ENABLED)
    , _samplerFlags(TextureSamplerFlag::DEFAULT)
    , _ninePatchInfo(nullptr)
{
}

Texture2D::~Texture2D()
{
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    VolatileTextureMgr::removeTexture(this);
#endif

    AX_SAFE_DELETE(_ninePatchInfo);

    AX_SAFE_RELEASE(_rhiTexture);
    _customCommand.releasePSVL();
}

rhi::PixelFormat Texture2D::getPixelFormat() const
{
    return _originalPF;
}

int Texture2D::getPixelsWide() const
{
    return _pixelsWide;
}

int Texture2D::getPixelsHigh() const
{
    return _pixelsHigh;
}

rhi::Texture* Texture2D::getRHITexture() const
{
    return _rhiTexture;
}

Vec2 Texture2D::getContentSize() const
{
    Vec2 ret;
    ret.width  = _contentSize.width / AX_CONTENT_SCALE_FACTOR();
    ret.height = _contentSize.height / AX_CONTENT_SCALE_FACTOR();

    return ret;
}

const Vec2& Texture2D::getContentSizeInPixels()
{
    return _contentSize;
}

float Texture2D::getMaxS() const
{
    return _maxS;
}

void Texture2D::setMaxS(float maxS)
{
    _maxS = maxS;
}

float Texture2D::getMaxT() const
{
    return _maxT;
}

void Texture2D::setMaxT(float maxT)
{
    _maxT = maxT;
}

bool Texture2D::hasPremultipliedAlpha() const
{
    return _flags & TextureFlag::PREMULTIPLIEDALPHA;
}

void Texture2D::setPremultipliedAlpha(bool premultipliedAlpha)
{
    if (premultipliedAlpha)
        _flags |= TextureFlag::PREMULTIPLIEDALPHA;
    else
        _flags &= ~TextureFlag::PREMULTIPLIEDALPHA;
}

bool Texture2D::initWithImage(Image* image, rhi::PixelFormat renderFormat, bool autoGenMipmaps)
{
    if (image == nullptr)
    {
        AXLOGW("axmol: Texture2D. Can't create Texture. UIImage is nil");
        return false;
    }

    if (this->_filePath.empty())
        this->_filePath = image->getFilePath();

    auto imagePixelFormat = image->getPixelFormat();
    int imageWidth        = image->getWidth();
    int imageHeight       = image->getHeight();
    auto imageData        = image->getData();
    auto imageDataSize    = image->getDataSize();

    if (image->getNumberOfMipmaps() > 1)
    {
        // pixel format of data is not converted, renderFormat can be different from pixelFormat
        // it will be done later
        initWithMipmaps(image->getMipmaps(),
                        imagePixelFormat, renderFormat, imageHeight,
                        imageWidth, image->hasPremultipliedAlpha());
    }
    else if (image->isCompressed())
    {  // !Only hardware support texture will be compression PixelFormat, otherwise, will convert to RGBA8 duraing image
       // load
        initWithData(imageData, imageDataSize, imagePixelFormat, imagePixelFormat, imageWidth, imageHeight,
                     image->hasPremultipliedAlpha(), autoGenMipmaps);
    }
    else
    {
        // after conversion, renderFormat == pixelFormat of data
        initWithData(imageData, imageDataSize, imagePixelFormat, renderFormat, imageWidth, imageHeight,
                     image->hasPremultipliedAlpha(), autoGenMipmaps);
    }

    return true;
}

bool Texture2D::initWithData(const void* data,
                             ssize_t dataSize,
                             rhi::PixelFormat pixelFormat,
                             rhi::PixelFormat renderFormat,
                             int pixelsWide,
                             int pixelsHigh,
                             bool preMultipliedAlpha,
                             bool autoGenMipmaps)
{
    AXASSERT(dataSize > 0 && pixelsWide > 0 && pixelsHigh > 0, "Invalid size");

    rhi::TextureDesc desc;

    desc.width       = pixelsWide;
    desc.height      = pixelsHigh;
    desc.pixelFormat = pixelFormat;

    if (autoGenMipmaps)
        desc.mipLevels = 0;  // generate mipmaps by GPU

    TextureSliceData subDatas[] = {data, (uint32_t)dataSize, 0, 0};  // layerIndex = 0, mipLevel = 0
    return initWithSpec(desc, subDatas, renderFormat, preMultipliedAlpha);
}

bool Texture2D::initWithMipmaps(std::span<MipmapInfo> mipmaps,
                                  rhi::PixelFormat pixelFormat,
                                  rhi::PixelFormat renderFormat,
                                  int pixelsWide,
                                  int pixelsHigh,
                                  bool preMultipliedAlpha)
{

    rhi::TextureDesc desc;

    desc.width = pixelsWide;
    desc.height = pixelsHigh;
    desc.pixelFormat = pixelFormat;
    desc.mipLevels   = mipmaps.size();
    return initWithSpec(desc, reinterpret_cast<std::span<TextureSliceData>&>(mipmaps), renderFormat, preMultipliedAlpha);
}

bool Texture2D::initWithSpec(rhi::TextureDesc desc,
                             std::span<const TextureSliceData> subDatas,
                             PixelFormat renderFormat,
                             bool preMultipliedAlpha)
{
    if (renderFormat == rhi::PixelFormat::NONE)
        renderFormat = desc.pixelFormat;

    AXASSERT(desc.pixelFormat != rhi::PixelFormat::NONE && renderFormat != rhi::PixelFormat::NONE,
             "PixelFormat should not be Auto");
    AXASSERT(desc.width > 0 && desc.height > 0, "Invalid size");

    if (_rhiTexture)
        return true;

    auto& pfd = rhi::RHIUtils::getFormatDesc(desc.pixelFormat);
    if (!pfd.bpp)
    {
        AXLOGW("WARNING: unsupported pixelformat: {:x}", (uint32_t)desc.pixelFormat);
#if AX_RENDER_API == AX_RENDER_API_MTL
        AXASSERT(false, "pixeformat not found in _pixelFormatInfoTables, register required!");
#endif
        return false;
    }

    bool compressed = rhi::RHIUtils::isCompressed(desc.pixelFormat);

    if (compressed && !Configuration::getInstance()->supportsPVRTC() &&
        !Configuration::getInstance()->supportsETC2() && !Configuration::getInstance()->supportsS3TC() &&
        !Configuration::getInstance()->supportsASTC() && !Configuration::getInstance()->supportsATITC())
    {
        AXLOGW("WARNING: PVRTC/ETC images are not supported");
        return false;
    }

    // !override renderFormat since some render format by RHI
    #if AX_RENDER_API == AX_RENDER_API_MTL
    switch (renderFormat)
    {
#    if (AX_TARGET_PLATFORM != AX_PLATFORM_IOS || TARGET_OS_SIMULATOR)
    // packed 16 bits pixels only available on iOS
    case PixelFormat::RGB565:
    case PixelFormat::RGB5A1:
    case PixelFormat::RGBA4:
#    endif
    case PixelFormat::RGB8:
        // Note: conversion to RGBA8 will happends
        renderFormat = PixelFormat::RGBA8;
        break;
    default:
        break;
    }
#elif AX_RENDER_API == AX_RENDER_API_D3D
    switch (renderFormat)
    {
    case PixelFormat::RGB8:
        // Note: conversion to RGBA8 will happends
        renderFormat = PixelFormat::RGBA8;
        break;
    default:
        break;
    }
#endif

    _originalPF      = desc.pixelFormat;
    _contentSize = Vec2((float)desc.width, (float)desc.height);
    _pixelsWide  = desc.width;
    _pixelsHigh  = desc.height;
    _maxS        = 1;
    _maxT        = 1;

    desc.pixelFormat = renderFormat;

    chooseSamplerDesc(true, desc.mipLevels != 1, desc.samplerDesc);
    _rhiTexture = static_cast<rhi::Texture*>(axdrv->createTexture(desc));

    updateData(subDatas, renderFormat, compressed);

    if (desc.arraySize == 2)
    {
        setPremultipliedAlpha(Image::isCompressedImageHavePMA(Image::CompressedImagePMAFlag::DUAL_SAMPLER));
        _samplerFlags |= TextureSamplerFlag::DUAL_SAMPLER;
    }
    else
    {
        setPremultipliedAlpha(preMultipliedAlpha);
    }

    return true;
}

// implementation Texture2D (Text)
bool Texture2D::initWithString(std::string_view text,
                               std::string_view fontName,
                               float fontSize,
                               const Vec2& dimensions /* = Vec2(0, 0)*/,
                               TextHAlignment hAlignment /* =  TextHAlignment::CENTER */,
                               TextVAlignment vAlignment /* =  TextVAlignment::TOP */,
                               bool enableWrap /* = false */,
                               int overflow /* = 0 */)
{
    FontDefinition tempDef;

    tempDef._shadow._shadowEnabled = false;
    tempDef._stroke._strokeEnabled = false;

    tempDef._fontName      = fontName;
    tempDef._fontSize      = fontSize;
    tempDef._dimensions    = dimensions;
    tempDef._alignment     = hAlignment;
    tempDef._vertAlignment = vAlignment;
    tempDef._fontFillColor = Color32::WHITE;
    tempDef._enableWrap    = enableWrap;
    tempDef._overflow      = overflow;

    return initWithString(text, tempDef);
}

bool Texture2D::initWithString(std::string_view text, const FontDefinition& textDefinition)
{
    if (text.empty())
        return false;

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    // cache the texture data
    VolatileTextureMgr::addStringTexture(this, text, textDefinition);
#endif

    Data outData;
    int imageWidth, imageHeight;
    bool premultipliedAlpha{false};
    if (!createStringTextureData(text, textDefinition, outData, imageWidth, imageHeight, premultipliedAlpha))
        return false;

    const PixelFormat pixelFormat = PixelFormat::RGBA8;

    return initWithData(outData.getBytes(), imageWidth * imageHeight * 4, pixelFormat, imageWidth, imageHeight,
                        premultipliedAlpha);
}

bool Texture2D::updateData(std::string_view text, const FontDefinition& textDefinition) {
    if (text.empty())
        return false;

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    // cache the texture data
    VolatileTextureMgr::addStringTexture(this, text, textDefinition);
#endif

   Data outData;
    int imageWidth, imageHeight;
    bool premultipliedAlpha{false};
    if (!createStringTextureData(text, textDefinition, outData, imageWidth, imageHeight, premultipliedAlpha))
        return false;

    TextureSliceData subDatas[] = {
        {outData.getBytes(), (uint32_t)(imageWidth * imageHeight * 4), 0, 0}  // layerIndex = 0, mipLevel = 0
    };

    return updateData(subDatas);
}

bool Texture2D::updateData(Image* image)
{
    if (!_rhiTexture) [[unlikely]]
        return false;

    if (image->getNumberOfMipmaps() > 1)
    {
        auto mipmaps = image->getMipmaps();
        return updateData(reinterpret_cast<std::span<TextureSliceData>&>(mipmaps));
    }
    else
    {
        TextureSliceData subDatas[] = {{image->getData(), (uint32_t)image->getDataSize(), 0, 0}};
        return updateData(subDatas);
    }
}

bool Texture2D::updateData(std::span<const TextureSliceData> subDatas) {
    if (!_rhiTexture)[[unlikely]]
        return false;

    const auto renderFormat = _rhiTexture->getPixelFormat();
    updateData(subDatas, _rhiTexture->getPixelFormat(), rhi::RHIUtils::isCompressed(renderFormat));
    return true;
}

void Texture2D::updateData(std::span<const TextureSliceData> subDatas, PixelFormat renderFormat, bool compressed)
{
    for (auto& subres : subDatas)
    {
        auto width  = (std::max)(1, _pixelsWide >> subres.mipLevel);
        auto height = (std::max)(1, _pixelsHigh >> subres.mipLevel);

        if (!subres.data) [[unlikely]]
        {
            // uncompressed pixel data can be nullptr for initializing texture storage purpose
            if (!compressed)
                _rhiTexture->updateData(nullptr, width, height, subres.mipLevel, subres.layerIndex);
            continue;
        }

        uint8_t* convertedData{nullptr};
        size_t convertedDataLen{0};

        const void* pixelData{nullptr};
        size_t pixelDataSize{0};
        if (_originalPF != renderFormat && !compressed)  // need conversion
        {
            auto convertedFormat =
                rhi::RHIUtils::convertDataToFormat((const uint8_t*)subres.data, subres.dataSize, _originalPF,
                                                   renderFormat, &convertedData, &convertedDataLen);
            if (convertedFormat != renderFormat || !convertedData)
            {
                AXLOGE("PixelFormat convert fail");
                continue;
            }
            pixelData     = convertedData;
            pixelDataSize = convertedDataLen;
        }
        else
        {
            pixelData     = subres.data;
            pixelDataSize = subres.dataSize;
        }

        if (compressed)
            _rhiTexture->updateCompressedData(pixelData, width, height, pixelDataSize, subres.mipLevel,
                                              subres.layerIndex);
        else
            _rhiTexture->updateData(pixelData, width, height, subres.mipLevel, subres.layerIndex);

        if (convertedData)
            free(convertedData);
    }
}

bool Texture2D::updateData(const void* data, int width, int height, int level, int layerIndex)
{
    if (_rhiTexture && width > 0 && height > 0)
    {
        // updateData must be called with the same pixel format as the original pixel format
        assert(_rhiTexture->getPixelFormat() == _originalPF);
        _rhiTexture->updateData(data, width, height, level, layerIndex);
        return true;
    }
    return false;
}

bool Texture2D::updateSubData(const void* data, int offsetX, int offsetY, int width, int height, int level, int layerIndex)
{
    if (_rhiTexture && width > 0 && height > 0)
    {
        // updateSubData must be called with the same pixel format as the original pixel format
        assert(_rhiTexture->getPixelFormat() == _originalPF);
        _rhiTexture->updateSubData(offsetX, offsetY, width, height, level, data, layerIndex);
        return true;
    }
    return false;
}

void Texture2D::invalidate()
{
    if(_rhiTexture)
        _rhiTexture->invalidate();
}

bool Texture2D::isRenderTarget() const
{
    return _rhiTexture && _rhiTexture->getTextureUsage() == rhi::TextureUsage::RENDER_TARGET;
}

bool Texture2D::hasMipmaps() const
{
    return _rhiTexture->hasMipmaps();
}

void Texture2D::setAliasTexParameters()
{
    if ((_flags & TextureFlag::ANTIALIAS_ENABLED) == 0)
    {
        return;
    }

    _flags &= ~TextureFlag::ANTIALIAS_ENABLED;

    rhi::SamplerDesc desc;
    chooseSamplerDesc(false, _rhiTexture->hasMipmaps(), desc);
    _rhiTexture->updateSamplerDesc(desc);
}

void Texture2D::setAntiAliasTexParameters()
{
    if (_flags & TextureFlag::ANTIALIAS_ENABLED)
    {
        return;
    }
    _flags |= TextureFlag::ANTIALIAS_ENABLED;

    rhi::SamplerDesc desc;
    chooseSamplerDesc(true, _rhiTexture->hasMipmaps(), desc);
    _rhiTexture->updateSamplerDesc(desc);
}

const char* Texture2D::getStringForFormat() const
{
    return rhi::RHIUtils::getFormatDesc(_originalPF).name;
}

unsigned int Texture2D::getBitsPerPixelForFormat(rhi::PixelFormat format) const
{
    return rhi::RHIUtils::getFormatDesc(format).bpp;
}

unsigned int Texture2D::getBitsPerPixelForFormat() const
{
    return getBitsPerPixelForFormat(_originalPF);
}

void Texture2D::addSpriteFrameCapInset(SpriteFrame* spritframe, const Rect& capInsets)
{
    if (nullptr == _ninePatchInfo)
    {
        _ninePatchInfo = new NinePatchInfo;
    }
    if (nullptr == spritframe)
    {
        _ninePatchInfo->capInsetSize = capInsets;
    }
    else
    {
        _ninePatchInfo->capInsetMap[spritframe] = capInsets;
    }
}

bool Texture2D::isContain9PatchInfo() const
{
    return nullptr != _ninePatchInfo;
}

const Rect& Texture2D::getSpriteFrameCapInset(ax::SpriteFrame* spriteFrame) const
{
    AXASSERT(_ninePatchInfo != nullptr,
             "Can't get the sprite frame capInset when the texture contains no 9-patch info.");
    if (nullptr == spriteFrame)
    {
        return this->_ninePatchInfo->capInsetSize;
    }
    else
    {
        auto& capInsetMap = this->_ninePatchInfo->capInsetMap;
        auto it           = capInsetMap.find(spriteFrame);
        if (it != capInsetMap.end())
        {
            return it->second;
        }
        else
        {
            return this->_ninePatchInfo->capInsetSize;
        }
    }
}

void Texture2D::removeSpriteFrameCapInset(SpriteFrame* spriteFrame)
{
    if (nullptr != this->_ninePatchInfo)
    {
        auto capInsetMap = this->_ninePatchInfo->capInsetMap;
        auto it          = capInsetMap.find(spriteFrame);
        if (it != capInsetMap.end())
        {
            capInsetMap.erase(it);
        }
    }
}

void Texture2D::setTexParameters(const Texture2D::TexParams& desc)
{
    _rhiTexture->updateSamplerDesc(desc);
}

void Texture2D::initProgram()
{
    if (_customCommand.unsafePS())
        return;

    // create program state
    auto* program      = axpm->getBuiltinProgram(rhi::ProgramType::POSITION_TEXTURE);
    auto programState      = new ax::rhi::ProgramState(program);
    _mvpMatrixLocation = programState->getUniformLocation("u_MVPMatrix");
    _textureLocation       = programState->getUniformLocation("u_tex0");

    _customCommand.setOwnPSVL(programState, program->getVertexLayout(), RenderCommand::ADOPT_FLAG_PS);

    // create vertex buffer
    _customCommand.setDrawType(CustomCommand::DrawType::ARRAY);
    _customCommand.setPrimitiveType(CustomCommand::PrimitiveType::TRIANGLE_STRIP);
    _customCommand.createVertexBuffer(sizeof(Vec3) + sizeof(Vec2), 4, CustomCommand::BufferUsage::DYNAMIC);

    // setup blend state
    BlendFunc blendFunc;
    if (hasPremultipliedAlpha())
    {
        blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    }
    else
    {
        blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
    }

    auto& blendDesc                = _customCommand.blendDesc();
    blendDesc.blendEnabled         = true;
    blendDesc.sourceRGBBlendFactor = blendDesc.sourceAlphaBlendFactor = blendFunc.src;
    blendDesc.destinationRGBBlendFactor = blendDesc.destinationAlphaBlendFactor = blendFunc.dst;

    programState->setTexture(_textureLocation, 0, _rhiTexture);
}

void Texture2D::drawAtPoint(const Vec2& point, float globalZOrder)
{
    float width  = (float)_pixelsWide * _maxS;
    float height = (float)_pixelsHigh * _maxT;
    Rect rect    = {point.x, point.y, width, height};
    drawInRect(rect, globalZOrder);
}

void Texture2D::drawInRect(const Rect& rect, float globalZOrder)
{
    initProgram();
    _customCommand.init(globalZOrder);
    auto director          = Director::getInstance();
    const auto& modelView  = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    const auto& projection = director->getMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_PROJECTION);

    Mat4 matrixMVP = projection * modelView;

    // vertex layout is: V3F_T2F
    float vertexData[] = {rect.origin.x,
                          rect.origin.y,
                          0.0f,
                          0.0f,
                          _maxT,
                          rect.size.width + rect.origin.x,
                          rect.origin.y,
                          0.0f,
                          _maxS,
                          _maxT,
                          rect.origin.x,
                          rect.size.height + rect.origin.y,
                          0.0f,
                          0.0f,
                          0.0f,
                          rect.size.width + rect.origin.x,
                          rect.size.height + rect.origin.y,
                          0.0f,
                          _maxS,
                          0.0f};

    _customCommand.unsafePS()->setUniform(_mvpMatrixLocation, matrixMVP.m, sizeof(matrixMVP.m));
    _customCommand.updateVertexBuffer(vertexData, sizeof(vertexData));
    Director::getInstance()->getRenderer()->addCommand(&_customCommand);
}

}
