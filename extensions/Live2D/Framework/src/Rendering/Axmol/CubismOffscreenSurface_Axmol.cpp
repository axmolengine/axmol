/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismOffscreenSurface_Axmol.hpp"

using namespace ax;

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

CubismOffscreenFrame_Axmol::CubismOffscreenFrame_Axmol()
    : _renderTexture(NULL)
    , _colorBuffer(NULL)
    , _isInheritedRenderTexture(false)
    , _previousColorBuffer(NULL)
    , _bufferWidth(0)
    , _bufferHeight(0)
{
}


void CubismOffscreenFrame_Axmol::BeginDraw(CubismCommandBuffer_Axmol* commandBuffer,
                                              rhi::Texture* colorBufferOnFinishDrawing)
{
    if (!IsValid())
    {
        return;
    }

    // Save the render target to restore after mask rendering.
    //
    // nullptr means "restore default render target".
    // This is important because the default render target color texture may be
    // a swapchain image, which must not be treated as an ordinary offscreen texture.
    if (colorBufferOnFinishDrawing)
    {
        // Caller explicitly provided a non-default color buffer.
        _previousColorBuffer = colorBufferOnFinishDrawing;
    }
    else
    {
        _previousColorBuffer = commandBuffer->GetColorBuffer();

        if (_previousColorBuffer == nullptr)
        {
            auto rt = GetCocos2dRenderer()->getRenderTarget();

            if (rt && !rt->isDefaultRenderTarget() && !rt->_color.empty())
            {
                _previousColorBuffer = rt->_color[0].texture;
            }
            else
            {
                // Default render target.
                // Restore through SetColorBuffer(nullptr), not through swapchain texture.
                _previousColorBuffer = nullptr;
            }
        }
    }

    // Set mask RenderTexture as current render target.
    commandBuffer->SetColorBuffer(_renderTexture->getSprite()->getTexture()->getRHITexture());
}

void CubismOffscreenFrame_Axmol::EndDraw(CubismCommandBuffer_Axmol* commandBuffer)
{
    if (!IsValid())
    {
        return;
    }

    // nullptr restores the default render target.
    // Non-null restores an offscreen color buffer.
    commandBuffer->SetColorBuffer(_previousColorBuffer);

    _previousColorBuffer = nullptr;
}

void CubismOffscreenFrame_Axmol::Clear(CubismCommandBuffer_Axmol* commandBuffer, float r, float g, float b, float a)
{
    // マスクをクリアする
    commandBuffer->Clear(r, g, b, a);
}

csmBool CubismOffscreenFrame_Axmol::CreateOffscreenFrame(csmUint32 displayBufferWidth,
                                                         csmUint32 displayBufferHeight,
                                                         ax::RenderTexture* renderTexture,
                                                         const ax::Color& clearColorHint)
{
    // 一旦削除
    DestroyOffscreenFrame();

    do
    {
        Texture2D* texture2d;
        if (!renderTexture)
        {
            // The inherited RenderTexture is nothing.
            // Make a RenderTexture.


            csmBool initResult = false;


            _renderTexture = ax::RenderTexture::create(displayBufferWidth,
                                                       displayBufferHeight,
                                                       ax::rhi::PixelFormat::RGBA8,
                                                       false,
                                                       clearColorHint);

            if (!_renderTexture)
            {
                break;
            }

            _renderTexture->retain();


            _renderTexture->getSprite()->getTexture()->setTexParameters(ax::Texture2D::TexParams{});

            texture2d                 = _renderTexture->getSprite()->getTexture();
            _colorBuffer              = texture2d->getRHITexture();
            _isInheritedRenderTexture = false;
        }
        else
        {
            // Use the inherited RenderTexture.
            _renderTexture = renderTexture;

            texture2d    = _renderTexture->getSprite()->getTexture();
            _colorBuffer = texture2d->getRHITexture();


            _isInheritedRenderTexture = true;
        }

        if (_colorBuffer)
        {
            _viewportSize = csmRectF(0.0f, 0.0f, texture2d->getContentSizeInPixels().width,
                                     texture2d->getContentSizeInPixels().height);
        }
        else
        {
            _viewportSize = csmRectF(0.0f, 0.0f, _bufferWidth, _bufferHeight);
        }

        _bufferWidth = displayBufferWidth;
        _bufferHeight = displayBufferHeight;


        // 成功
        return true;

    } while (0);

    // 失敗したので削除
    DestroyOffscreenFrame();

    return false;
}

void CubismOffscreenFrame_Axmol::DestroyOffscreenFrame()
{
    if ((_renderTexture != NULL) && !_isInheritedRenderTexture)
    {
        AX_SAFE_RELEASE_NULL(_renderTexture);
        _colorBuffer = NULL;
    }
}

ax::Texture2D* CubismOffscreenFrame_Axmol::GetColorBuffer() const
{
    if (!_renderTexture)
        return nullptr;
    return _renderTexture->getSprite()->getTexture();
}

csmUint32 CubismOffscreenFrame_Axmol::GetBufferWidth() const
{
    return _bufferWidth;
}

csmUint32 CubismOffscreenFrame_Axmol::GetBufferHeight() const
{
    return _bufferHeight;
}

csmRectF CubismOffscreenFrame_Axmol::GetViewPortSize() const
{
    return _viewportSize;
}

csmBool CubismOffscreenFrame_Axmol::IsValid() const
{
    return _renderTexture != NULL;
}

}}}}

//------------ LIVE2D NAMESPACE ------------
