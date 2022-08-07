/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismOffscreenSurface_Cocos2dx.hpp"

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

CubismOffscreenFrame_Cocos2dx::CubismOffscreenFrame_Cocos2dx()
    : _renderTexture(NULL)
    , _colorBuffer(NULL)
    , _isInheritedRenderTexture(false)
    , _previousColorBuffer(NULL)
    , _bufferWidth(0)
    , _bufferHeight(0)
{
}


void CubismOffscreenFrame_Cocos2dx::BeginDraw(CubismCommandBuffer_Cocos2dx* commandBuffer,
                                              backend::TextureBackend* colorBufferOnFinishDrawing)
{
    if (!IsValid())
    {
        return;
    }

    // バックバッファのサーフェイスを記憶しておく
    if (colorBufferOnFinishDrawing == NULL)
    {
        _previousColorBuffer = commandBuffer->GetColorBuffer();
        if (_previousColorBuffer == NULL)
        {
            auto rt = GetCocos2dRenderer()->getRenderTarget();
            _previousColorBuffer = !rt->isDefaultRenderTarget() ? rt->_color[0].texture : nullptr; // GetCocos2dRenderer()->getColorAttachment();
        }
    }
    else
    {
        _previousColorBuffer = colorBufferOnFinishDrawing;
    }

    // マスク用RenderTextureをactiveにセット
    commandBuffer->SetColorBuffer(_renderTexture->getSprite()->getTexture()->getBackendTexture());
}

void CubismOffscreenFrame_Cocos2dx::EndDraw(CubismCommandBuffer_Cocos2dx* commandBuffer)
{
    if (!IsValid())
    {
        return;
    }

    // 描画対象を戻す
    commandBuffer->SetColorBuffer(_previousColorBuffer);
}

void CubismOffscreenFrame_Cocos2dx::Clear(CubismCommandBuffer_Cocos2dx* commandBuffer, float r, float g, float b, float a)
{
    // マスクをクリアする
    commandBuffer->Clear(r, g, b, a);
}

csmBool CubismOffscreenFrame_Cocos2dx::CreateOffscreenFrame(csmUint32 displayBufferWidth, csmUint32 displayBufferHeight, ax::RenderTexture* renderTexture)
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


            _renderTexture = ax::RenderTexture::create(displayBufferWidth, displayBufferHeight);

            if (!_renderTexture)
            {
                break;
            }

            _renderTexture->retain();


            _renderTexture->getSprite()->getTexture()->setTexParameters(
                ax::Texture2D::TexParams(
                    ax::backend::SamplerFilter::LINEAR,                    // MagFilter
                    ax::backend::SamplerFilter::LINEAR,                    // MinFilter
                    ax::backend::SamplerAddressMode::CLAMP_TO_EDGE,      // AddressingMode S
                    ax::backend::SamplerAddressMode::CLAMP_TO_EDGE       // AddressingMode T
                )
            );

            texture2d                 = _renderTexture->getSprite()->getTexture();
            _colorBuffer              = texture2d->getBackendTexture();
            _isInheritedRenderTexture = false;
        }
        else
        {
            // Use the inherited RenderTexture.
            _renderTexture = renderTexture;

            texture2d    = _renderTexture->getSprite()->getTexture();
            _colorBuffer = texture2d->getBackendTexture();


            _isInheritedRenderTexture = true;
        }

        if (_colorBuffer)
        {
            _viewPortSize = csmRectF(0.0f, 0.0f, texture2d->getContentSizeInPixels().width,
                                     texture2d->getContentSizeInPixels().height);
        }
        else
        {
            _viewPortSize = csmRectF(0.0f, 0.0f, _bufferWidth, _bufferHeight);
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

void CubismOffscreenFrame_Cocos2dx::DestroyOffscreenFrame()
{
    if ((_renderTexture != NULL) && !_isInheritedRenderTexture)
    {
        AX_SAFE_RELEASE_NULL(_renderTexture);
        _colorBuffer = NULL;
    }
}

ax::Texture2D* CubismOffscreenFrame_Cocos2dx::GetColorBuffer() const
{
    return _renderTexture->getSprite()->getTexture();
}

csmUint32 CubismOffscreenFrame_Cocos2dx::GetBufferWidth() const
{
    return _bufferWidth;
}

csmUint32 CubismOffscreenFrame_Cocos2dx::GetBufferHeight() const
{
    return _bufferHeight;
}

csmRectF CubismOffscreenFrame_Cocos2dx::GetViewPortSize() const
{
    return _viewPortSize;
}

csmBool CubismOffscreenFrame_Cocos2dx::IsValid() const
{
    return _renderTexture != NULL;
}

}}}}

//------------ LIVE2D NAMESPACE ------------
