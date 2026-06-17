/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismRenderer_Axmol.hpp"
#include "Math/CubismMatrix44.hpp"
#include "Type/csmVector.hpp"
#include "Model/CubismModel.hpp"
#include <float.h>
#include "axmol/rhi/DriverContext.h"
#include "axmol/platform/RenderView.h"
#include <algorithm>
#include <cmath>

using namespace ax;

#ifdef CSM_TARGET_WIN_GL
#include <Windows.h>
#endif

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

/*********************************************************************************************************************
*                                      CubismOffscreenRenderTarget_Axmol
********************************************************************************************************************/
CubismOffscreenRenderTarget_Axmol::CubismOffscreenRenderTarget_Axmol()
    : _offscreenIndex(CubismModel::CubismNoIndex_Offscreen)
    , _renderTarget(CSM_NEW CubismOffscreenFrame_Axmol())
    , _parentPartOffscreen(NULL)
    , _oldOffscreen(NULL)
{
}

CubismOffscreenRenderTarget_Axmol::~CubismOffscreenRenderTarget_Axmol()
{
    CSM_DELETE(_renderTarget);
}

void CubismOffscreenRenderTarget_Axmol::SetOffscreenIndex(csmInt32 offscreenIndex)
{
    _offscreenIndex = offscreenIndex;
}

csmInt32 CubismOffscreenRenderTarget_Axmol::GetOffscreenIndex() const
{
    return _offscreenIndex;
}

CubismOffscreenFrame_Axmol* CubismOffscreenRenderTarget_Axmol::GetRenderTarget()
{
    return _renderTarget;
}

const CubismOffscreenFrame_Axmol* CubismOffscreenRenderTarget_Axmol::GetRenderTarget() const
{
    return _renderTarget;
}

void CubismOffscreenRenderTarget_Axmol::SetParentPartOffscreen(CubismOffscreenRenderTarget_Axmol* parent)
{
    _parentPartOffscreen = parent;
}

CubismOffscreenRenderTarget_Axmol* CubismOffscreenRenderTarget_Axmol::GetParentPartOffscreen() const
{
    return _parentPartOffscreen;
}

void CubismOffscreenRenderTarget_Axmol::SetOldOffscreen(CubismOffscreenRenderTarget_Axmol* oldOffscreen)
{
    _oldOffscreen = oldOffscreen;
}

CubismOffscreenRenderTarget_Axmol* CubismOffscreenRenderTarget_Axmol::GetOldOffscreen() const
{
    return _oldOffscreen;
}

/*********************************************************************************************************************
*                                      CubismClippingManager_Axmol
********************************************************************************************************************/
///< ファイルスコープの変数宣言
namespace {
const csmInt32 ColorChannelCount = 4;   ///< 実験時に1チャンネルの場合は1、RGBだけの場合は3、アルファも含める場合は4

CubismRenderer::CubismBlendMode ToCubismBlendMode(const csmBlendMode& blendMode)
{
    switch (blendMode.GetColorBlendType())
    {
    case Core::csmColorBlendType_AddCompatible:
    case Core::csmColorBlendType_Add:
    case Core::csmColorBlendType_AddGlow:
        return CubismRenderer::CubismBlendMode_Additive;
    case Core::csmColorBlendType_MultiplyCompatible:
    case Core::csmColorBlendType_Multiply:
        return CubismRenderer::CubismBlendMode_Multiplicative;
    case Core::csmColorBlendType_Normal:
    default:
        return CubismRenderer::CubismBlendMode_Normal;
    }
}

bool IsAdvancedBlendMode(const csmBlendMode& blendMode)
{
    switch (blendMode.GetColorBlendType())
    {
    case Core::csmColorBlendType_Normal:
        return blendMode.GetAlphaBlendType() != Core::csmAlphaBlendType_Over;
    case Core::csmColorBlendType_AddCompatible:
    case Core::csmColorBlendType_MultiplyCompatible:
        return false;
    default:
        return true;
    }
}

csmInt32 ToColorBlendModeIndex(const csmBlendMode& blendMode)
{
    switch (blendMode.GetColorBlendType())
    {
    case Core::csmColorBlendType_Add:
        return 1;
    case Core::csmColorBlendType_AddGlow:
        return 2;
    case Core::csmColorBlendType_Darken:
        return 3;
    case Core::csmColorBlendType_Multiply:
        return 4;
    case Core::csmColorBlendType_ColorBurn:
        return 5;
    case Core::csmColorBlendType_LinearBurn:
        return 6;
    case Core::csmColorBlendType_Lighten:
        return 7;
    case Core::csmColorBlendType_Screen:
        return 8;
    case Core::csmColorBlendType_ColorDodge:
        return 9;
    case Core::csmColorBlendType_Overlay:
        return 10;
    case Core::csmColorBlendType_SoftLight:
        return 11;
    case Core::csmColorBlendType_HardLight:
        return 12;
    case Core::csmColorBlendType_LinearLight:
        return 13;
    case Core::csmColorBlendType_Hue:
        return 14;
    case Core::csmColorBlendType_Color:
        return 15;
    case Core::csmColorBlendType_Normal:
    default:
        return 0;
    }
}

csmInt32 ToAlphaBlendModeIndex(const csmBlendMode& blendMode)
{
    switch (blendMode.GetAlphaBlendType())
    {
    case Core::csmAlphaBlendType_Atop:
        return 1;
    case Core::csmAlphaBlendType_Out:
        return 2;
    case Core::csmAlphaBlendType_ConjointOver:
        return 3;
    case Core::csmAlphaBlendType_DisjointOver:
        return 4;
    case Core::csmAlphaBlendType_Over:
    default:
        return 0;
    }
}

CubismRenderer::CubismTextureColor ToCubismTextureColor(const Core::csmVector4& color)
{
    return CubismRenderer::CubismTextureColor(color.X, color.Y, color.Z, color.W);
}

CubismRenderer::CubismTextureColor GetNeutralMultiplyColor()
{
    return CubismRenderer::CubismTextureColor(1.0f, 1.0f, 1.0f, 1.0f);
}

CubismRenderer::CubismTextureColor GetNeutralScreenColor()
{
    return CubismRenderer::CubismTextureColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void SetTextureByBindingSlot(ax::rhi::ProgramState* programState,
                             const ax::rhi::UniformLocation& uniformLocation,
                             ax::rhi::Texture* texture)
{
    programState->setTexture(uniformLocation, uniformLocation.location, texture);
}
}

CubismClippingManager_Axmol::CubismClippingManager_Axmol() :
                                                                   _currentFrameNo(0)
                                                                   , _clippingMaskBufferSize(256, 256)
{
    CubismRenderer::CubismTextureColor* tmp;
    tmp = CSM_NEW CubismRenderer::CubismTextureColor();
    tmp->R = 1.0f;
    tmp->G = 0.0f;
    tmp->B = 0.0f;
    tmp->A = 0.0f;
    _channelColors.PushBack(tmp);
    tmp = CSM_NEW CubismRenderer::CubismTextureColor();
    tmp->R = 0.0f;
    tmp->G = 1.0f;
    tmp->B = 0.0f;
    tmp->A = 0.0f;
    _channelColors.PushBack(tmp);
    tmp = CSM_NEW CubismRenderer::CubismTextureColor();
    tmp->R = 0.0f;
    tmp->G = 0.0f;
    tmp->B = 1.0f;
    tmp->A = 0.0f;
    _channelColors.PushBack(tmp);
    tmp = CSM_NEW CubismRenderer::CubismTextureColor();
    tmp->R = 0.0f;
    tmp->G = 0.0f;
    tmp->B = 0.0f;
    tmp->A = 1.0f;
    _channelColors.PushBack(tmp);

}

CubismClippingManager_Axmol::~CubismClippingManager_Axmol()
{
    for (csmUint32 i = 0; i < _clippingContextListForMask.GetSize(); i++)
    {
        if (_clippingContextListForMask[i]) CSM_DELETE_SELF(CubismClippingContext_Axmol, _clippingContextListForMask[i]);
        _clippingContextListForMask[i] = NULL;
    }

    // _clippingContextListForDrawは_clippingContextListForMaskにあるインスタンスを指している。上記の処理により要素ごとのDELETEは不要。
    for (csmUint32 i = 0; i < _clippingContextListForDraw.GetSize(); i++)
    {
        _clippingContextListForDraw[i] = NULL;
    }

    for (csmUint32 i = 0; i < _clippingContextListForOffscreen.GetSize(); i++)
    {
        _clippingContextListForOffscreen[i] = NULL;
    }

    for (csmUint32 i = 0; i < _channelColors.GetSize(); i++)
    {
        if (_channelColors[i]) CSM_DELETE(_channelColors[i]);
        _channelColors[i] = NULL;
    }
}

void CubismClippingManager_Axmol::Initialize(CubismModel& model, csmInt32 drawableCount, const csmInt32** drawableMasks, const csmInt32* drawableMaskCounts)
{
    //クリッピングマスクを使う描画オブジェクトを全て登録する
    //クリッピングマスクは、通常数個程度に限定して使うものとする
    for (csmInt32 i = 0; i < drawableCount; i++)
    {
        if (drawableMaskCounts[i] <= 0)
        {
            //クリッピングマスクが使用されていないアートメッシュ（多くの場合使用しない）
            _clippingContextListForDraw.PushBack(NULL);
            continue;
        }

        // 既にあるClipContextと同じかチェックする
        CubismClippingContext_Axmol* cc = FindSameClip(drawableMasks[i], drawableMaskCounts[i]);
        if (cc == NULL)
        {
            // 同一のマスクが存在していない場合は生成する
            cc = CSM_NEW CubismClippingContext_Axmol(this, model, drawableMasks[i], drawableMaskCounts[i]);
            _clippingContextListForMask.PushBack(cc);
        }

        cc->AddClippedDrawable(i);

        _clippingContextListForDraw.PushBack(cc);

    }
}

void CubismClippingManager_Axmol::Initialize(CubismModel& model, csmInt32 maskBufferCount, CubismRenderer::DrawableObjectType drawableObjectType)
{
    csmInt32 objectCount = 0;
    const csmInt32* objectMaskCounts = NULL;
    const csmInt32** objectMasks = NULL;
    csmVector<CubismClippingContext_Axmol*>* objectClippingContextList = NULL;

    switch (drawableObjectType)
    {
    case CubismRenderer::DrawableObjectType_Offscreen:
        objectCount = model.GetOffscreenCount();
        objectMaskCounts = model.GetOffscreenMaskCounts();
        objectMasks = model.GetOffscreenMasks();
        objectClippingContextList = &_clippingContextListForOffscreen;
        break;
    case CubismRenderer::DrawableObjectType_Drawable:
    default:
        objectCount = model.GetDrawableCount();
        objectMaskCounts = model.GetDrawableMaskCounts();
        objectMasks = model.GetDrawableMasks();
        objectClippingContextList = &_clippingContextListForDraw;
        break;
    }

    for (csmInt32 i = 0; i < objectCount; ++i)
    {
        if (objectMaskCounts[i] <= 0)
        {
            objectClippingContextList->PushBack(NULL);
            continue;
        }

        CubismClippingContext_Axmol* cc = FindSameClip(objectMasks[i], objectMaskCounts[i]);
        if (cc == NULL)
        {
            cc = CSM_NEW CubismClippingContext_Axmol(this, model, objectMasks[i], objectMaskCounts[i]);
            _clippingContextListForMask.PushBack(cc);
        }

        if (drawableObjectType == CubismRenderer::DrawableObjectType_Offscreen)
        {
            cc->AddClippedOffscreen(i);
        }
        else
        {
            cc->AddClippedDrawable(i);
        }

        objectClippingContextList->PushBack(cc);
    }
}

CubismClippingContext_Axmol* CubismClippingManager_Axmol::FindSameClip(const csmInt32* drawableMasks, csmInt32 drawableMaskCounts) const
{
    // 作成済みClippingContextと一致するか確認
    for (csmUint32 i = 0; i < _clippingContextListForMask.GetSize(); i++)
    {
        CubismClippingContext_Axmol* cc = _clippingContextListForMask[i];
        const csmInt32 count = cc->_clippingIdCount;
        if (count != drawableMaskCounts) continue; //個数が違う場合は別物
        csmInt32 samecount = 0;

        // 同じIDを持つか確認。配列の数が同じなので、一致した個数が同じなら同じ物を持つとする。
        for (csmInt32 j = 0; j < count; j++)
        {
            const csmInt32 clipId = cc->_clippingIdList[j];
            for (csmInt32 k = 0; k < count; k++)
            {
                if (drawableMasks[k] == clipId)
                {
                    samecount++;
                    break;
                }
            }
        }
        if (samecount == count)
        {
            return cc;
        }
    }
    return NULL; //見つからなかった
}

void CubismClippingManager_Axmol::SetupClippingContext(CubismModel& model,
                                                       CubismRenderer_Axmol* renderer,
                                                       rhi::Texture* lastColorBuffer,
                                                       csmRectF lastViewport,
                                                       CubismOffscreenFrame_Axmol* maskBuffer,
                                                       CubismRenderer::DrawableObjectType drawableObjectType,
                                                       csmBool isRightHanded,
                                                       const CubismMatrix44& mvp)
{
    _currentFrameNo++;

    // 全てのクリッピングを用意する
    // 同じクリップ（複数の場合はまとめて１つのクリップ）を使う場合は１度だけ設定する
    csmInt32 usingClipCount = 0;
    for (csmUint32 clipIndex = 0; clipIndex < _clippingContextListForMask.GetSize(); clipIndex++)
    {
        // １つのクリッピングマスクに関して
        CubismClippingContext_Axmol* cc = _clippingContextListForMask[clipIndex];

        // このクリップを利用する描画オブジェクト群全体を囲む矩形を計算
        CalcClippedDrawTotalBounds(model, cc, drawableObjectType);

        if (cc->_isUsing)
        {
            usingClipCount++; //使用中としてカウント
        }
    }

    // マスク作成処理
    if (usingClipCount > 0)
    {
        if (!renderer->IsUsingHighPrecisionMask())
        {
            // 生成したFrameBufferと同じサイズでビューポートを設定
            renderer->GetCommandBuffer()->Viewport(0, 0, maskBuffer->GetViewPortSize().Width, maskBuffer->GetViewPortSize().Height);

            // モデル描画時にDrawMeshNowに渡される変換（モデルtoワールド座標変換）
            CubismMatrix44 modelToWorldF = renderer->GetMvpMatrix();

            renderer->PreDraw(); // バッファをクリアする

            // _offscreenFrameBufferへ切り替え
            maskBuffer->BeginDraw(renderer->GetCommandBuffer(), lastColorBuffer);

            // マスクをクリアする
            // 1が無効（描かれない）領域、0が有効（描かれる）領域。（シェーダで Cd*Csで0に近い値をかけてマスクを作る。1をかけると何も起こらない）
            maskBuffer->Clear(renderer->GetCommandBuffer(), 1.0f, 1.0f, 1.0f, 1.0f);
        }

        // 各マスクのレイアウトを決定していく
        SetupLayoutBounds(renderer->IsUsingHighPrecisionMask() ? 0 : usingClipCount);

        // 実際にマスクを生成する
        // 全てのマスクをどの様にレイアウトして描くかを決定し、ClipContext , ClippedDrawContext に記憶する
        for (csmUint32 clipIndex = 0; clipIndex < _clippingContextListForMask.GetSize(); clipIndex++)
        {
            // --- 実際に１つのマスクを描く ---
            CubismClippingContext_Axmol* clipContext = _clippingContextListForMask[clipIndex];
            csmRectF* allClippedDrawRect = clipContext->_allClippedDrawRect; //このマスクを使う、全ての描画オブジェクトの論理座標上の囲み矩形
            csmRectF* layoutBoundsOnTex01 = clipContext->_layoutBounds; //この中にマスクを収める
            const csmFloat32 MARGIN = 0.05f;
            csmFloat32 scaleX = 0.0f;
            csmFloat32 scaleY = 0.0f;

            if (renderer->IsUsingHighPrecisionMask())
            {
                const csmFloat32 ppu = model.GetPixelsPerUnit();
                const csmFloat32 maskPixelWidth = clipContext->_owner->_clippingMaskBufferSize.X;
                const csmFloat32 maskPixelHeight = clipContext->_owner->_clippingMaskBufferSize.Y;
                const csmFloat32 physicalMaskWidth = layoutBoundsOnTex01->Width * maskPixelWidth;
                const csmFloat32 physicalMaskHeight = layoutBoundsOnTex01->Height * maskPixelHeight;


                _tmpBoundsOnModel.SetRect(allClippedDrawRect);

                if (_tmpBoundsOnModel.Width * ppu > physicalMaskWidth)
                {
                    _tmpBoundsOnModel.Expand(allClippedDrawRect->Width * MARGIN, 0.0f);
                    scaleX = layoutBoundsOnTex01->Width / _tmpBoundsOnModel.Width;
                }
                else
                {
                    scaleX = ppu / physicalMaskWidth;
                }

                if (_tmpBoundsOnModel.Height * ppu > physicalMaskHeight)
                {
                    _tmpBoundsOnModel.Expand(0.0f, allClippedDrawRect->Height * MARGIN);
                    scaleY = layoutBoundsOnTex01->Height / _tmpBoundsOnModel.Height;
                }
                else
                {
                    scaleY = ppu / physicalMaskHeight;
                }
            }
            else
            {
                // モデル座標上の矩形を、適宜マージンを付けて使う
                _tmpBoundsOnModel.SetRect(allClippedDrawRect);
                _tmpBoundsOnModel.Expand(allClippedDrawRect->Width * MARGIN, allClippedDrawRect->Height * MARGIN);
                //########## 本来は割り当てられた領域の全体を使わず必要最低限のサイズがよい
                // シェーダ用の計算式を求める。回転を考慮しない場合は以下のとおり
                // movePeriod' = movePeriod * scaleX + offX     [[ movePeriod' = (movePeriod - tmpBoundsOnModel.movePeriod)*scale + layoutBoundsOnTex01.movePeriod ]]
                scaleX = layoutBoundsOnTex01->Width / _tmpBoundsOnModel.Width;
                scaleY = layoutBoundsOnTex01->Height / _tmpBoundsOnModel.Height;
            }

            // マスク生成時に使う行列を求める
            {
                // シェーダに渡す行列を求める <<<<<<<<<<<<<<<<<<<<<<<< 要最適化（逆順に計算すればシンプルにできる）
                _tmpMatrix.LoadIdentity();
                {
                    // Layout0..1 を -1..1に変換
                    _tmpMatrix.TranslateRelative(-1.0f, -1.0f);
                    _tmpMatrix.ScaleRelative(2.0f, 2.0f);
                }
                {
                    // view to Layout0..1
                    _tmpMatrix.TranslateRelative(layoutBoundsOnTex01->X, layoutBoundsOnTex01->Y * (isRightHanded ? -1.0f : 1.0f)); //new = [translate]
                    _tmpMatrix.ScaleRelative(scaleX, scaleY * (isRightHanded ? -1.0f : 1.0f)); //new = [translate][scale]
                    _tmpMatrix.TranslateRelative(-_tmpBoundsOnModel.X, -_tmpBoundsOnModel.Y);
                    //new = [translate][scale][translate]
                }
                // tmpMatrixForMask が計算結果
                _tmpMatrixForMask.SetMatrix(_tmpMatrix.GetArray());
            }

            //--------- draw時の mask 参照用行列を計算
            {
                // シェーダに渡す行列を求める <<<<<<<<<<<<<<<<<<<<<<<< 要最適化（逆順に計算すればシンプルにできる）
                _tmpMatrix.LoadIdentity();
                {
                    _tmpMatrix.TranslateRelative(layoutBoundsOnTex01->X, layoutBoundsOnTex01->Y); //new = [translate]
                    _tmpMatrix.ScaleRelative(scaleX, scaleY); //new = [translate][scale]
                    _tmpMatrix.TranslateRelative(-_tmpBoundsOnModel.X, -_tmpBoundsOnModel.Y);
                    //new = [translate][scale][translate]
                }

                _tmpMatrixForDraw.SetMatrix(_tmpMatrix.GetArray());
            }

            clipContext->_matrixForMask.SetMatrix(_tmpMatrixForMask.GetArray());

            clipContext->_matrixForDraw.SetMatrix(_tmpMatrixForDraw.GetArray());
            if (drawableObjectType == CubismRenderer::DrawableObjectType_Offscreen)
            {
                CubismMatrix44 invertMvp = mvp.GetInvert();
                clipContext->_matrixForDraw.MultiplyByMatrix(&invertMvp);
            }

            if (!renderer->IsUsingHighPrecisionMask())
            {
                const csmInt32 clipDrawCount = clipContext->_clippingIdCount;
                for (csmInt32 i = 0; i < clipDrawCount; i++)
                {
                    const csmInt32 clipDrawIndex = clipContext->_clippingIdList[i];
                    CubismCommandBuffer_Axmol::DrawCommandBuffer* drawCommandBufferData = clipContext->_clippingCommandBufferList->At(i);// [i];


                    // 頂点情報が更新されておらず、信頼性がない場合は描画をパスする
                    if (!model.GetDrawableDynamicFlagVertexPositionsDidChange(clipDrawIndex))
                    {
                        continue;
                    }

                    // Update Vertex / Index buffer.
                    {
                        csmFloat32* vertices = const_cast<csmFloat32*>(model.GetDrawableVertices(clipDrawIndex));
                        Core::csmVector2* uvs = const_cast<Core::csmVector2*>(model.GetDrawableVertexUvs(clipDrawIndex));
                        csmUint16* vertexIndices = const_cast<csmUint16*>(model.GetDrawableVertexIndices(clipDrawIndex));
                        const csmUint32 vertexCount = model.GetDrawableVertexCount(clipDrawIndex);
                        const csmUint32 vertexIndexCount = model.GetDrawableVertexIndexCount(clipDrawIndex);

                        drawCommandBufferData->UpdateVertexBuffer(vertices, uvs, vertexCount);
                        drawCommandBufferData->CommitVertexBuffer();
                        if (vertexIndexCount > 0)
                        {
                            drawCommandBufferData->UpdateIndexBuffer(vertexIndices, vertexIndexCount);
                        }

                        if (vertexCount <= 0)
                        {
                            continue;
                        }

                    }

                    renderer->IsCulling(model.GetDrawableCulling(clipDrawIndex) != 0);

                    // 今回専用の変換を適用して描く
                    // チャンネルも切り替える必要がある(A,R,G,B)
                    renderer->SetClippingContextBufferForMask(clipContext);

                    renderer->DrawMeshCocos2d(
                        drawCommandBufferData->GetCommandDraw(),
                        model.GetDrawableTextureIndex(clipDrawIndex),
                        model.GetDrawableVertexIndexCount(clipDrawIndex),
                        model.GetDrawableVertexCount(clipDrawIndex),
                        const_cast<csmUint16*>(model.GetDrawableVertexIndices(clipDrawIndex)),
                        const_cast<csmFloat32*>(model.GetDrawableVertices(clipDrawIndex)),
                        reinterpret_cast<csmFloat32*>(const_cast<Core::csmVector2*>(model.GetDrawableVertexUvs(clipDrawIndex))),
                        model.GetDrawableOpacity(clipDrawIndex),
                        CubismRenderer::CubismBlendMode_Normal,   //クリッピングは通常描画を強制
                        GetNeutralMultiplyColor(),
                        GetNeutralScreenColor(),
                        csmBlendMode(),
                        NULL,
                        false   // マスク生成時はクリッピングの反転使用は全く関係がない
                    );
                }
            }
        }

        if (!renderer->IsUsingHighPrecisionMask())
        {
            // --- 後処理 ---
            maskBuffer->EndDraw(renderer->GetCommandBuffer()); // 描画対象を戻す
            renderer->SetClippingContextBufferForMask(NULL);
            renderer->GetCommandBuffer()->Viewport(lastViewport.X, lastViewport.Y, lastViewport.Width, lastViewport.Height);
        }
    }
}

void CubismClippingManager_Axmol::CalcClippedDrawTotalBounds(CubismModel& model, CubismClippingContext_Axmol* clippingContext)
{
    CalcClippedDrawTotalBounds(model, clippingContext, CubismRenderer::DrawableObjectType_Drawable);
}

void CubismClippingManager_Axmol::CalcClippedDrawTotalBounds(CubismModel& model, CubismClippingContext_Axmol* clippingContext, CubismRenderer::DrawableObjectType drawableObjectType)
{
    // 被クリッピングマスク（マスクされる描画オブジェクト）の全体の矩形
    csmFloat32 clippedDrawTotalMinX = FLT_MAX, clippedDrawTotalMinY = FLT_MAX;
    csmFloat32 clippedDrawTotalMaxX = -FLT_MAX, clippedDrawTotalMaxY = -FLT_MAX;

    // このマスクが実際に必要か判定する
    // このクリッピングを利用する「描画オブジェクト」がひとつでも使用可能であればマスクを生成する必要がある

    csmInt32 clippedDrawCount = 0;
    csmVector<csmInt32> clippedOffscreenChildDrawableIndexList;
    if (drawableObjectType == CubismRenderer::DrawableObjectType_Offscreen)
    {
        const csmInt32 clippedOffscreenCount = clippingContext->_clippedOffscreenIndexList->GetSize();
        for (csmInt32 clippedOffscreenIndex = 0; clippedOffscreenIndex < clippedOffscreenCount; ++clippedOffscreenIndex)
        {
            CollectOffscreenChildDrawableIndexList(model,
                                                   (*clippingContext->_clippedOffscreenIndexList)[clippedOffscreenIndex],
                                                   clippedOffscreenChildDrawableIndexList);
        }
        clippedDrawCount = clippedOffscreenChildDrawableIndexList.GetSize();
    }
    else
    {
        clippedDrawCount = clippingContext->_clippedDrawableIndexList->GetSize();
    }

    for (csmInt32 clippedDrawableIndex = 0; clippedDrawableIndex < clippedDrawCount; clippedDrawableIndex++)
    {
        // マスクを使用する描画オブジェクトの描画される矩形を求める
        const csmInt32 drawableIndex = drawableObjectType == CubismRenderer::DrawableObjectType_Offscreen
            ? clippedOffscreenChildDrawableIndexList[clippedDrawableIndex]
            : (*clippingContext->_clippedDrawableIndexList)[clippedDrawableIndex];

        const csmInt32 drawableVertexCount = model.GetDrawableVertexCount(drawableIndex);
        csmFloat32* drawableVertexes = const_cast<csmFloat32*>(model.GetDrawableVertices(drawableIndex));

        csmFloat32 minX = FLT_MAX, minY = FLT_MAX;
        csmFloat32 maxX = -FLT_MAX, maxY = -FLT_MAX;

        csmInt32 loop = drawableVertexCount * Constant::VertexStep;
        for (csmInt32 pi = Constant::VertexOffset; pi < loop; pi += Constant::VertexStep)
        {
            csmFloat32 x = drawableVertexes[pi];
            csmFloat32 y = drawableVertexes[pi + 1];
            if (x < minX) minX = x;
            if (x > maxX) maxX = x;
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
        }

        //
        if (minX == FLT_MAX) continue; //有効な点がひとつも取れなかったのでスキップする

        // 全体の矩形に反映
        if (minX < clippedDrawTotalMinX) clippedDrawTotalMinX = minX;
        if (minY < clippedDrawTotalMinY) clippedDrawTotalMinY = minY;
        if (maxX > clippedDrawTotalMaxX) clippedDrawTotalMaxX = maxX;
        if (maxY > clippedDrawTotalMaxY) clippedDrawTotalMaxY = maxY;
    }
    if (clippedDrawTotalMinX == FLT_MAX)
    {
        clippingContext->_allClippedDrawRect->X = 0.0f;
        clippingContext->_allClippedDrawRect->Y = 0.0f;
        clippingContext->_allClippedDrawRect->Width = 0.0f;
        clippingContext->_allClippedDrawRect->Height = 0.0f;
        clippingContext->_isUsing = false;
    }
    else
    {
        clippingContext->_isUsing = true;
        csmFloat32 w = clippedDrawTotalMaxX - clippedDrawTotalMinX;
        csmFloat32 h = clippedDrawTotalMaxY - clippedDrawTotalMinY;
        clippingContext->_allClippedDrawRect->X = clippedDrawTotalMinX;
        clippingContext->_allClippedDrawRect->Y = clippedDrawTotalMinY;
        clippingContext->_allClippedDrawRect->Width = w;
        clippingContext->_allClippedDrawRect->Height = h;
    }
}

void CubismClippingManager_Axmol::CollectOffscreenChildDrawableIndexList(CubismModel& model, csmInt32 offscreenIndex, csmVector<csmInt32>& childDrawableIndexList)
{
    const csmInt32 ownerIndex = model.GetOffscreenOwnerIndices()[offscreenIndex];
    CollectPartChildDrawableIndexList(model, ownerIndex, childDrawableIndexList);
}

void CubismClippingManager_Axmol::CollectPartChildDrawableIndexList(CubismModel& model, csmInt32 partIndex, csmVector<csmInt32>& childDrawableIndexList)
{
    PartChildDrawObjects childDrawObjects = model.GetPartsHierarchy()[partIndex].ChildDrawObjects;
    for (csmInt32 i = 0; i < childDrawObjects.DrawableIndices.GetSize(); ++i)
    {
        childDrawableIndexList.PushBack(childDrawObjects.DrawableIndices[i]);
    }
    for (csmInt32 i = 0; i < childDrawObjects.OffscreenIndices.GetSize(); ++i)
    {
        CollectOffscreenChildDrawableIndexList(model, childDrawObjects.OffscreenIndices[i], childDrawableIndexList);
    }
}

void CubismClippingManager_Axmol::SetupLayoutBounds(csmInt32 usingClipCount) const
{
    if (usingClipCount <= 0)
    {// この場合は一つのマスクターゲットを毎回クリアして使用する
        for (csmUint32 index = 0; index < _clippingContextListForMask.GetSize(); index++)
        {
            CubismClippingContext_Axmol* cc = _clippingContextListForMask[index];
            cc->_layoutChannelNo = 0; // どうせ毎回消すので固定で良い
            cc->_layoutBounds->X = 0.0f;
            cc->_layoutBounds->Y = 0.0f;
            cc->_layoutBounds->Width = 1.0f;
            cc->_layoutBounds->Height = 1.0f;
        }
        return;
    }

    // ひとつのRenderTextureを極力いっぱいに使ってマスクをレイアウトする
    // マスクグループの数が4以下ならRGBA各チャンネルに１つずつマスクを配置し、5以上6以下ならRGBAを2,2,1,1と配置する

    // RGBAを順番に使っていく。
    const csmInt32 div = usingClipCount / ColorChannelCount; //１チャンネルに配置する基本のマスク個数
    const csmInt32 mod = usingClipCount % ColorChannelCount; //余り、この番号のチャンネルまでに１つずつ配分する

    // RGBAそれぞれのチャンネルを用意していく(0:R , 1:G , 2:B, 3:A, )
    csmInt32 curClipIndex = 0; //順番に設定していく

    for (csmInt32 channelNo = 0; channelNo < ColorChannelCount; channelNo++)
    {
        // このチャンネルにレイアウトする数
        const csmInt32 layoutCount = div + (channelNo < mod ? 1 : 0);

        // 分割方法を決定する
        if (layoutCount == 0)
        {
            // 何もしない
        }
        else if (layoutCount == 1)
        {
            //全てをそのまま使う
            CubismClippingContext_Axmol* cc = _clippingContextListForMask[curClipIndex++];
            cc->_layoutChannelNo = channelNo;
            cc->_layoutBounds->X = 0.0f;
            cc->_layoutBounds->Y = 0.0f;
            cc->_layoutBounds->Width = 1.0f;
            cc->_layoutBounds->Height = 1.0f;
        }
        else if (layoutCount == 2)
        {
            for (csmInt32 i = 0; i < layoutCount; i++)
            {
                const csmInt32 xpos = i % 2;

                CubismClippingContext_Axmol* cc = _clippingContextListForMask[curClipIndex++];
                cc->_layoutChannelNo = channelNo;

                cc->_layoutBounds->X = xpos * 0.5f;
                cc->_layoutBounds->Y = 0.0f;
                cc->_layoutBounds->Width = 0.5f;
                cc->_layoutBounds->Height = 1.0f;
                //UVを2つに分解して使う
            }
        }
        else if (layoutCount <= 4)
        {
            //4分割して使う
            for (csmInt32 i = 0; i < layoutCount; i++)
            {
                const csmInt32 xpos = i % 2;
                const csmInt32 ypos = i / 2;

                CubismClippingContext_Axmol* cc = _clippingContextListForMask[curClipIndex++];
                cc->_layoutChannelNo = channelNo;

                cc->_layoutBounds->X = xpos * 0.5f;
                cc->_layoutBounds->Y = ypos * 0.5f;
                cc->_layoutBounds->Width = 0.5f;
                cc->_layoutBounds->Height = 0.5f;
            }
        }
        else if (layoutCount <= 9)
        {
            //9分割して使う
            for (csmInt32 i = 0; i < layoutCount; i++)
            {
                const csmInt32 xpos = i % 3;
                const csmInt32 ypos = i / 3;

                CubismClippingContext_Axmol* cc = _clippingContextListForMask[curClipIndex++];
                cc->_layoutChannelNo = channelNo;

                cc->_layoutBounds->X = xpos / 3.0f;
                cc->_layoutBounds->Y = ypos / 3.0f;
                cc->_layoutBounds->Width = 1.0f / 3.0f;
                cc->_layoutBounds->Height = 1.0f / 3.0f;
            }
        }
        else
        {
            CubismLogError("not supported mask count : %d", layoutCount);

            // 開発モードの場合は停止させる
            CSM_ASSERT(0);

            // 引き続き実行する場合、 SetupShaderProgramでオーバーアクセスが発生するので仕方なく適当に入れておく
            // もちろん描画結果はろくなことにならない
            for (csmInt32 i = 0; i < layoutCount; i++)
            {
                CubismClippingContext_Axmol* cc = _clippingContextListForMask[curClipIndex++];
                cc->_layoutChannelNo = 0;
                cc->_layoutBounds->X = 0.0f;
                cc->_layoutBounds->Y = 0.0f;
                cc->_layoutBounds->Width = 1.0f;
                cc->_layoutBounds->Height = 1.0f;
            }
        }
    }
}

CubismRenderer::CubismTextureColor* CubismClippingManager_Axmol::GetChannelFlagAsColor(csmInt32 channelNo)
{
    return _channelColors[channelNo];
}

csmVector<CubismClippingContext_Axmol*>* CubismClippingManager_Axmol::GetClippingContextListForDraw()
{
    return &_clippingContextListForDraw;
}

csmVector<CubismClippingContext_Axmol*>* CubismClippingManager_Axmol::GetClippingContextListForOffscreen()
{
    return &_clippingContextListForOffscreen;
}

void CubismClippingManager_Axmol::SetClippingMaskBufferSize(csmFloat32 width, csmFloat32 height)
{
    _clippingMaskBufferSize = CubismVector2(width, height);
}

CubismVector2 CubismClippingManager_Axmol::GetClippingMaskBufferSize() const
{
    return _clippingMaskBufferSize;
}

/*********************************************************************************************************************
*                                      CubismClippingContext_Axmol
********************************************************************************************************************/
CubismClippingContext_Axmol::CubismClippingContext_Axmol(CubismClippingManager_Axmol* manager, CubismModel& model, const csmInt32* clippingDrawableIndices, csmInt32 clipCount)
{
    _owner = manager;

    // クリップしている（＝マスク用の）Drawableのインデックスリスト
    _clippingIdList = clippingDrawableIndices;

    // マスクの数
    _clippingIdCount = clipCount;

    _layoutChannelNo = 0;
    _bufferIndex = 0;

    _allClippedDrawRect = CSM_NEW csmRectF();
    _layoutBounds = CSM_NEW csmRectF();

    _clippedDrawableIndexList = CSM_NEW csmVector<csmInt32>();
    _clippedOffscreenIndexList = CSM_NEW csmVector<csmInt32>();
    _clippingCommandBufferList = CSM_NEW csmVector<CubismCommandBuffer_Axmol::DrawCommandBuffer*>;


    for (csmUint32 i = 0; i < _clippingIdCount; ++i)
    {
        const csmInt32 clippingId = _clippingIdList[i];
        CubismCommandBuffer_Axmol::DrawCommandBuffer* drawCommandBuffer = NULL;
        const csmInt32 drawableVertexCount = model.GetDrawableVertexCount(clippingId);
        const csmInt32 drawableVertexIndexCount = model.GetDrawableVertexIndexCount(clippingId);
        const csmSizeInt vertexSize = sizeof(csmFloat32) * 4;


        drawCommandBuffer = CSM_NEW CubismCommandBuffer_Axmol::DrawCommandBuffer();
        drawCommandBuffer->GetCommandDraw()->GetCommand()->setDrawType(ax::CustomCommand::DrawType::ELEMENT);
        drawCommandBuffer->GetCommandDraw()->GetCommand()->setPrimitiveType(ax::rhi::PrimitiveType::TRIANGLE);
        drawCommandBuffer->CreateVertexBuffer(vertexSize, drawableVertexCount);      // Vertices + UVs
        drawCommandBuffer->CreateIndexBuffer(drawableVertexIndexCount);


        _clippingCommandBufferList->PushBack(drawCommandBuffer);
    }
}

CubismClippingContext_Axmol::~CubismClippingContext_Axmol()
{
    if (_layoutBounds != NULL)
    {
        CSM_DELETE(_layoutBounds);
        _layoutBounds = NULL;
    }

    if (_allClippedDrawRect != NULL)
    {
        CSM_DELETE(_allClippedDrawRect);
        _allClippedDrawRect = NULL;
    }

    if (_clippedDrawableIndexList != NULL)
    {
        CSM_DELETE(_clippedDrawableIndexList);
        _clippedDrawableIndexList = NULL;
    }

    if (_clippedOffscreenIndexList != NULL)
    {
        CSM_DELETE(_clippedOffscreenIndexList);
        _clippedOffscreenIndexList = NULL;
    }

    if (_clippingCommandBufferList != NULL)
    {
        for (csmUint32 i = 0; i < _clippingCommandBufferList->GetSize(); ++i)
        {
            CSM_DELETE(_clippingCommandBufferList->At(i));
            _clippingCommandBufferList->At(i) = NULL;
        }

        CSM_DELETE(_clippingCommandBufferList);
        _clippingCommandBufferList = NULL;
    }
}

void CubismClippingContext_Axmol::AddClippedDrawable(csmInt32 drawableIndex)
{

    _clippedDrawableIndexList->PushBack(drawableIndex);
}

void CubismClippingContext_Axmol::AddClippedOffscreen(csmInt32 offscreenIndex)
{
    _clippedOffscreenIndexList->PushBack(offscreenIndex);
}

CubismClippingManager_Axmol* CubismClippingContext_Axmol::GetClippingManager()
{
    return _owner;
}



/*********************************************************************************************************************
*                                      CubismDrawProfile_OpenGL
********************************************************************************************************************/
void CubismRendererProfile_Axmol::Save()
{
    //-- push state --
    _lastScissorTest = GetCocos2dRenderer()->getScissorTest();
    _lastStencilTest = GetCocos2dRenderer()->getStencilTest();
    _lastDepthTest   = GetCocos2dRenderer()->getDepthTest();
    _lastCullFace    = GetCocos2dRenderer()->getCullMode();
    _lastWinding     = GetCocos2dRenderer()->getWinding();

    // Save render target and viewport.
    _lastRenderTarget = GetCocos2dRenderer()->getRenderTarget();

    // Important:
    // For the default render target, _color[0].texture may be a swapchain image.
    // Swapchain images must not be treated as ordinary sampled/offscreen textures.
    //
    // Therefore:
    // - default render target  -> save nullptr color/depth/stencil buffers
    // - offscreen render target -> save actual attachments
    if (_lastRenderTarget && !_lastRenderTarget->isDefaultRenderTarget())
    {
        _lastColorBuffer = _lastRenderTarget->_color.size() > 0 ? _lastRenderTarget->_color[0].texture : nullptr;

        _lastDepthBuffer   = _lastRenderTarget->_depthStencil.texture;
        _lastStencilBuffer = _lastRenderTarget->_depthStencil.texture;
    }
    else
    {
        _lastColorBuffer   = nullptr;
        _lastDepthBuffer   = nullptr;
        _lastStencilBuffer = nullptr;
    }

    const auto& viewport = GetCocos2dRenderer()->getViewport();
    _lastViewport        = csmRectF(viewport.x, viewport.y, viewport.w, viewport.h);
}

void CubismRendererProfile_Axmol::Restore()
{
    GetCocos2dRenderer()->setScissorTest(_lastScissorTest);
    GetCocos2dRenderer()->setStencilTest(_lastStencilTest);
    GetCocos2dRenderer()->setDepthTest(_lastDepthTest);
    GetCocos2dRenderer()->setCullMode(_lastCullFace);
    GetCocos2dRenderer()->setWinding(_lastWinding);

    if (_lastRenderTarget)
    {
        // Do not overwrite attachments of the default render target.
        // Its color attachment is the current swapchain image and must be managed
        // by the renderer / backend.
        if (_lastRenderTarget->isDefaultRenderTarget())
        {
            GetCocos2dRenderer()->setRenderTarget(_lastRenderTarget);
        }
        else
        {
            _lastRenderTarget->setColorTexture(_lastColorBuffer);
            _lastRenderTarget->setDepthStencilTexture(_lastDepthBuffer);

            GetCocos2dRenderer()->setRenderTarget(_lastRenderTarget);
        }
    }
    else
    {
        GetCocos2dRenderer()->setRenderTarget(GetCocos2dRenderer()->getDefaultRenderTarget());
    }

    GetCocos2dRenderer()->setViewport(_lastViewport.X, _lastViewport.Y, _lastViewport.Width, _lastViewport.Height);
}


/*********************************************************************************************************************
*                                       CubismShader_Axmol
********************************************************************************************************************/
namespace {
    const csmInt32 ShaderCount = 26; ///< 互換描画用 + コピー用 + 5.5ブレンドモード用
    CubismShader_Axmol* s_instance;
}

enum ShaderNames
{
    // SetupMask
    ShaderNames_SetupMask,

    //Normal
    ShaderNames_Normal,
    ShaderNames_NormalMasked,
    ShaderNames_NormalMaskedInverted,
    ShaderNames_NormalPremultipliedAlpha,
    ShaderNames_NormalMaskedPremultipliedAlpha,
    ShaderNames_NormalMaskedInvertedPremultipliedAlpha,

    //Add
    ShaderNames_Add,
    ShaderNames_AddMasked,
    ShaderNames_AddMaskedInverted,
    ShaderNames_AddPremultipliedAlpha,
    ShaderNames_AddMaskedPremultipliedAlpha,
    ShaderNames_AddMaskedPremultipliedAlphaInverted,

    //Mult
    ShaderNames_Mult,
    ShaderNames_MultMasked,
    ShaderNames_MultMaskedInverted,
    ShaderNames_MultPremultipliedAlpha,
    ShaderNames_MultMaskedPremultipliedAlpha,
    ShaderNames_MultMaskedPremultipliedAlphaInverted,

    ShaderNames_Copy,
    ShaderNames_Blend,
    ShaderNames_BlendMasked,
    ShaderNames_BlendMaskedInverted,
    ShaderNames_BlendPremultipliedAlpha,
    ShaderNames_BlendMaskedPremultipliedAlpha,
    ShaderNames_BlendMaskedInvertedPremultipliedAlpha,
};

void CubismShader_Axmol::ReleaseShaderProgram()
{
    for (csmUint32 i = 0; i < _shaderSets.GetSize(); i++)
    {
        if (_shaderSets[i]->ShaderProgram)
        {
            CSM_DELETE(_shaderSets[i]);
        }
    }
}

// SetupMask
static const csmChar* VertShaderSrcSetupMask = "custom/live2d_setup_mask_vs";
static const csmChar* FragShaderSrcSetupMask = "custom/live2d_setup_mask_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcSetupMaskTegra = "custom/live2d_setup_mask_tegra_fs";
#endif


//----- バーテックスシェーダプログラム -----
// Normal & Add & Mult 共通
static const csmChar* VertShaderSrc = "custom/live2d_vs";

// Normal & Add & Mult 共通（クリッピングされたものの描画用）
static const csmChar* VertShaderSrcMasked = "custom/live2d_masked_vs";
static const csmChar* VertShaderSrcBlend = "custom/live2d_blend_vs";
static const csmChar* VertShaderSrcMaskedBlend = "custom/live2d_masked_blend_vs";

//----- フラグメントシェーダプログラム -----
// Normal & Add & Mult 共通
static const csmChar* FragShaderSrc = "custom/live2d_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcTegra = "custom/live2d_tegra_fs";
#endif

// Normal & Add & Mult 共通 （PremultipliedAlpha）
static const csmChar* FragShaderSrcPremultipliedAlpha = "custom/live2d_pma_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcPremultipliedAlphaTegra = "custom/live2d_pma_tegra_fs";
#endif

// Normal & Add & Mult 共通（クリッピングされたものの描画用）
static const csmChar* FragShaderSrcMask = "custom/live2d_mask_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcMaskTegra = "custom/live2d_mask_tegra_fs";
#endif

// Normal & Add & Mult 共通（クリッピングされて反転使用の描画用）
static const csmChar* FragShaderSrcMaskInverted = "custom/live2d_mask_inv_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcMaskInvertedTegra = "custom/live2d_mask_inv_tegra_fs";
#endif

// Normal & Add & Mult 共通（クリッピングされたものの描画用、PremultipliedAlphaの場合）
static const csmChar* FragShaderSrcMaskPremultipliedAlpha = "custom/live2d_mask_pma_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcMaskPremultipliedAlphaTegra = "custom/live2d_mask_pma_tegra_fs";
#endif

// Normal & Add & Mult 共通（クリッピングされて反転使用の描画用、PremultipliedAlphaの場合）
static const csmChar* FragShaderSrcMaskInvertedPremultipliedAlpha =
    "custom/live2d_mask_inv_pma_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcMaskInvertedPremultipliedAlphaTegra = "custom/live2d_mask_inv_pma_tegra_fs";
#endif

static const csmChar* FragShaderSrcCopy = "custom/live2d_copy_fs";
static const csmChar* FragShaderSrcBlend = "custom/live2d_blend_fs";
static const csmChar* FragShaderSrcPremultipliedAlphaBlend = "custom/live2d_pma_blend_fs";
static const csmChar* FragShaderSrcMaskBlend = "custom/live2d_mask_blend_fs";
static const csmChar* FragShaderSrcMaskInvertedBlend = "custom/live2d_mask_inv_blend_fs";
static const csmChar* FragShaderSrcMaskPremultipliedAlphaBlend = "custom/live2d_mask_pma_blend_fs";
static const csmChar* FragShaderSrcMaskInvertedPremultipliedAlphaBlend =
    "custom/live2d_mask_inv_pma_blend_fs";

CubismShader_Axmol::CubismShader_Axmol()
{ }

CubismShader_Axmol::~CubismShader_Axmol()
{
    ReleaseShaderProgram();
}

CubismShader_Axmol* CubismShader_Axmol::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = CSM_NEW CubismShader_Axmol();
    }
    return s_instance;
}

void CubismShader_Axmol::DeleteInstance()
{
    if (s_instance)
    {
        CSM_DELETE_SELF(CubismShader_Axmol, s_instance);
        s_instance = NULL;
    }
}

void CubismShader_Axmol::GenerateShaders()
{
    for (csmInt32 i = 0; i < ShaderCount; i++)
    {
        _shaderSets.PushBack(CSM_NEW CubismShaderSet());
    }

    _shaderSets[0]->ShaderProgram = LoadShaderProgram(VertShaderSrcSetupMask, FragShaderSrcSetupMask);

    _shaderSets[1]->ShaderProgram = LoadShaderProgram(VertShaderSrc, FragShaderSrc);
    _shaderSets[2]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMask);
    _shaderSets[3]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMaskInverted);
    _shaderSets[4]->ShaderProgram = LoadShaderProgram(VertShaderSrc, FragShaderSrcPremultipliedAlpha);
    _shaderSets[5]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMaskPremultipliedAlpha);
    _shaderSets[6]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMaskInvertedPremultipliedAlpha);



    // 加算も通常と同じシェーダーを利用する
    _shaderSets[7]->ShaderProgram = _shaderSets[1]->ShaderProgram;
    _shaderSets[8]->ShaderProgram = _shaderSets[2]->ShaderProgram;
    _shaderSets[9]->ShaderProgram = _shaderSets[3]->ShaderProgram;
    _shaderSets[10]->ShaderProgram = _shaderSets[4]->ShaderProgram;
    _shaderSets[11]->ShaderProgram = _shaderSets[5]->ShaderProgram;
    _shaderSets[12]->ShaderProgram = _shaderSets[6]->ShaderProgram;


    // 乗算も通常と同じシェーダーを利用する
    _shaderSets[13]->ShaderProgram = _shaderSets[1]->ShaderProgram;
    _shaderSets[14]->ShaderProgram = _shaderSets[2]->ShaderProgram;
    _shaderSets[15]->ShaderProgram = _shaderSets[3]->ShaderProgram;
    _shaderSets[16]->ShaderProgram = _shaderSets[4]->ShaderProgram;
    _shaderSets[17]->ShaderProgram = _shaderSets[5]->ShaderProgram;
    _shaderSets[18]->ShaderProgram = _shaderSets[6]->ShaderProgram;

    _shaderSets[ShaderNames_Copy]->ShaderProgram = LoadShaderProgram(VertShaderSrc, FragShaderSrcCopy);
    _shaderSets[ShaderNames_Blend]->ShaderProgram = LoadShaderProgram(VertShaderSrcBlend, FragShaderSrcBlend);
    _shaderSets[ShaderNames_BlendMasked]->ShaderProgram = LoadShaderProgram(VertShaderSrcMaskedBlend, FragShaderSrcMaskBlend);
    _shaderSets[ShaderNames_BlendMaskedInverted]->ShaderProgram = LoadShaderProgram(VertShaderSrcMaskedBlend, FragShaderSrcMaskInvertedBlend);
    _shaderSets[ShaderNames_BlendPremultipliedAlpha]->ShaderProgram = LoadShaderProgram(VertShaderSrcBlend, FragShaderSrcPremultipliedAlphaBlend);
    _shaderSets[ShaderNames_BlendMaskedPremultipliedAlpha]->ShaderProgram = LoadShaderProgram(VertShaderSrcMaskedBlend, FragShaderSrcMaskPremultipliedAlphaBlend);
    _shaderSets[ShaderNames_BlendMaskedInvertedPremultipliedAlpha]->ShaderProgram =
        LoadShaderProgram(VertShaderSrcMaskedBlend, FragShaderSrcMaskInvertedPremultipliedAlphaBlend);

    // SetupMask
    _shaderSets[0]->AttributePositionLocation = _shaderSets[0]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[0]->AttributeTexCoordLocation = _shaderSets[0]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[0]->SamplerTexture0Location = _shaderSets[0]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[0]->UniformClipMatrixLocation = _shaderSets[0]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[0]->UnifromChannelFlagLocation = _shaderSets[0]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[0]->UniformBaseColorLocation = _shaderSets[0]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常
    _shaderSets[1]->AttributePositionLocation = _shaderSets[1]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[1]->AttributeTexCoordLocation = _shaderSets[1]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[1]->SamplerTexture0Location = _shaderSets[1]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[1]->UniformMatrixLocation = _shaderSets[1]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[1]->UniformBaseColorLocation = _shaderSets[1]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常（クリッピング）
    _shaderSets[2]->AttributePositionLocation = _shaderSets[2]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[2]->AttributeTexCoordLocation = _shaderSets[2]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[2]->SamplerTexture0Location = _shaderSets[2]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[2]->SamplerTexture1Location = _shaderSets[2]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[2]->UniformMatrixLocation = _shaderSets[2]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[2]->UniformClipMatrixLocation = _shaderSets[2]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[2]->UnifromChannelFlagLocation = _shaderSets[2]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[2]->UniformBaseColorLocation = _shaderSets[2]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常（クリッピング・反転）
    _shaderSets[3]->AttributePositionLocation = _shaderSets[3]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[3]->AttributeTexCoordLocation = _shaderSets[3]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[3]->SamplerTexture0Location = _shaderSets[3]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[3]->SamplerTexture1Location = _shaderSets[3]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[3]->UniformMatrixLocation = _shaderSets[3]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[3]->UniformClipMatrixLocation = _shaderSets[3]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[3]->UnifromChannelFlagLocation = _shaderSets[3]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[3]->UniformBaseColorLocation = _shaderSets[3]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常（PremultipliedAlpha）
    _shaderSets[4]->AttributePositionLocation = _shaderSets[4]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[4]->AttributeTexCoordLocation = _shaderSets[4]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[4]->SamplerTexture0Location = _shaderSets[4]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[4]->UniformMatrixLocation = _shaderSets[4]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[4]->UniformBaseColorLocation = _shaderSets[4]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常（クリッピング、PremultipliedAlpha）
    _shaderSets[5]->AttributePositionLocation = _shaderSets[5]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[5]->AttributeTexCoordLocation = _shaderSets[5]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[5]->SamplerTexture0Location = _shaderSets[5]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[5]->SamplerTexture1Location = _shaderSets[5]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[5]->UniformMatrixLocation = _shaderSets[5]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[5]->UniformClipMatrixLocation = _shaderSets[5]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[5]->UnifromChannelFlagLocation = _shaderSets[5]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[5]->UniformBaseColorLocation = _shaderSets[5]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常（クリッピング・反転、PremultipliedAlpha）
    _shaderSets[6]->AttributePositionLocation = _shaderSets[6]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[6]->AttributeTexCoordLocation = _shaderSets[6]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[6]->SamplerTexture0Location = _shaderSets[6]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[6]->SamplerTexture1Location = _shaderSets[6]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[6]->UniformMatrixLocation = _shaderSets[6]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[6]->UniformClipMatrixLocation = _shaderSets[6]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[6]->UnifromChannelFlagLocation = _shaderSets[6]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[6]->UniformBaseColorLocation = _shaderSets[6]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算
    _shaderSets[7]->AttributePositionLocation = _shaderSets[7]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[7]->AttributeTexCoordLocation = _shaderSets[7]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[7]->SamplerTexture0Location = _shaderSets[7]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[7]->UniformMatrixLocation = _shaderSets[7]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[7]->UniformBaseColorLocation = _shaderSets[7]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算（クリッピング）
    _shaderSets[8]->AttributePositionLocation = _shaderSets[8]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[8]->AttributeTexCoordLocation = _shaderSets[8]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[8]->SamplerTexture0Location = _shaderSets[8]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[8]->SamplerTexture1Location = _shaderSets[8]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[8]->UniformMatrixLocation = _shaderSets[8]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[8]->UniformClipMatrixLocation = _shaderSets[8]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[8]->UnifromChannelFlagLocation = _shaderSets[8]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[8]->UniformBaseColorLocation = _shaderSets[8]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算（クリッピング・反転）
    _shaderSets[9]->AttributePositionLocation = _shaderSets[9]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[9]->AttributeTexCoordLocation = _shaderSets[9]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[9]->SamplerTexture0Location = _shaderSets[9]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[9]->SamplerTexture1Location = _shaderSets[9]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[9]->UniformMatrixLocation = _shaderSets[9]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[9]->UniformClipMatrixLocation = _shaderSets[9]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[9]->UnifromChannelFlagLocation = _shaderSets[9]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[9]->UniformBaseColorLocation = _shaderSets[9]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算（PremultipliedAlpha）
    _shaderSets[10]->AttributePositionLocation = _shaderSets[10]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[10]->AttributeTexCoordLocation = _shaderSets[10]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[10]->SamplerTexture0Location = _shaderSets[10]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[10]->UniformMatrixLocation = _shaderSets[10]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[10]->UniformBaseColorLocation = _shaderSets[10]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算（クリッピング、PremultipliedAlpha）
    _shaderSets[11]->AttributePositionLocation = _shaderSets[11]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[11]->AttributeTexCoordLocation = _shaderSets[11]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[11]->SamplerTexture0Location = _shaderSets[11]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[11]->SamplerTexture1Location = _shaderSets[11]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[11]->UniformMatrixLocation = _shaderSets[11]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[11]->UniformClipMatrixLocation = _shaderSets[11]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[11]->UnifromChannelFlagLocation = _shaderSets[11]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[11]->UniformBaseColorLocation = _shaderSets[11]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算（クリッピング・反転、PremultipliedAlpha）
    _shaderSets[12]->AttributePositionLocation = _shaderSets[12]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[12]->AttributeTexCoordLocation = _shaderSets[12]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[12]->SamplerTexture0Location = _shaderSets[12]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[12]->SamplerTexture1Location = _shaderSets[12]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[12]->UniformMatrixLocation = _shaderSets[12]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[12]->UniformClipMatrixLocation = _shaderSets[12]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[12]->UnifromChannelFlagLocation = _shaderSets[12]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[12]->UniformBaseColorLocation = _shaderSets[12]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算
    _shaderSets[13]->AttributePositionLocation = _shaderSets[13]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[13]->AttributeTexCoordLocation = _shaderSets[13]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[13]->SamplerTexture0Location = _shaderSets[13]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[13]->UniformMatrixLocation = _shaderSets[13]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[13]->UniformBaseColorLocation = _shaderSets[13]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算（クリッピング）
    _shaderSets[14]->AttributePositionLocation = _shaderSets[14]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[14]->AttributeTexCoordLocation = _shaderSets[14]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[14]->SamplerTexture0Location = _shaderSets[14]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[14]->SamplerTexture1Location = _shaderSets[14]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[14]->UniformMatrixLocation = _shaderSets[14]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[14]->UniformClipMatrixLocation = _shaderSets[14]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[14]->UnifromChannelFlagLocation = _shaderSets[14]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[14]->UniformBaseColorLocation = _shaderSets[14]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算（クリッピング・反転）
    _shaderSets[15]->AttributePositionLocation = _shaderSets[15]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[15]->AttributeTexCoordLocation = _shaderSets[15]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[15]->SamplerTexture0Location = _shaderSets[15]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[15]->SamplerTexture1Location = _shaderSets[15]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[15]->UniformMatrixLocation = _shaderSets[15]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[15]->UniformClipMatrixLocation = _shaderSets[15]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[15]->UnifromChannelFlagLocation = _shaderSets[15]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[15]->UniformBaseColorLocation = _shaderSets[15]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算（PremultipliedAlpha）
    _shaderSets[16]->AttributePositionLocation = _shaderSets[16]->ShaderProgram->getVertexInputDesc( "a_position");
    _shaderSets[16]->AttributeTexCoordLocation = _shaderSets[16]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[16]->SamplerTexture0Location = _shaderSets[16]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[16]->UniformMatrixLocation = _shaderSets[16]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[16]->UniformBaseColorLocation = _shaderSets[16]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算（クリッピング、PremultipliedAlpha）
    _shaderSets[17]->AttributePositionLocation = _shaderSets[17]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[17]->AttributeTexCoordLocation = _shaderSets[17]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[17]->SamplerTexture0Location = _shaderSets[17]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[17]->SamplerTexture1Location = _shaderSets[17]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[17]->UniformMatrixLocation = _shaderSets[17]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[17]->UniformClipMatrixLocation = _shaderSets[17]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[17]->UnifromChannelFlagLocation = _shaderSets[17]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[17]->UniformBaseColorLocation = _shaderSets[17]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算（クリッピング・反転、PremultipliedAlpha）
    _shaderSets[18]->AttributePositionLocation = _shaderSets[18]->ShaderProgram->getVertexInputDesc("a_position");
    _shaderSets[18]->AttributeTexCoordLocation = _shaderSets[18]->ShaderProgram->getVertexInputDesc("a_texCoord");
    _shaderSets[18]->SamplerTexture0Location = _shaderSets[18]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[18]->SamplerTexture1Location = _shaderSets[18]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[18]->UniformMatrixLocation = _shaderSets[18]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[18]->UniformClipMatrixLocation = _shaderSets[18]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[18]->UnifromChannelFlagLocation = _shaderSets[18]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[18]->UniformBaseColorLocation = _shaderSets[18]->ShaderProgram->getUniformLocation("u_baseColor");

    for (csmInt32 i = ShaderNames_Normal; i < ShaderCount; ++i)
    {
        _shaderSets[i]->AttributePositionLocation = _shaderSets[i]->ShaderProgram->getVertexInputDesc("a_position");
        _shaderSets[i]->AttributeTexCoordLocation = _shaderSets[i]->ShaderProgram->getVertexInputDesc("a_texCoord");
        _shaderSets[i]->SamplerTexture0Location = _shaderSets[i]->ShaderProgram->getUniformLocation("u_tex0");
        _shaderSets[i]->SamplerTexture1Location = _shaderSets[i]->ShaderProgram->getUniformLocation("u_tex1");
        _shaderSets[i]->SamplerBlendTextureLocation = _shaderSets[i]->ShaderProgram->getUniformLocation("u_blendTexture");
        _shaderSets[i]->UniformMatrixLocation = _shaderSets[i]->ShaderProgram->getUniformLocation("u_matrix");
        _shaderSets[i]->UniformClipMatrixLocation = _shaderSets[i]->ShaderProgram->getUniformLocation("u_clipMatrix");
        _shaderSets[i]->UnifromChannelFlagLocation = _shaderSets[i]->ShaderProgram->getUniformLocation("u_channelFlag");
        _shaderSets[i]->UniformBaseColorLocation = _shaderSets[i]->ShaderProgram->getUniformLocation("u_baseColor");
        _shaderSets[i]->UniformMultiplyColorLocation = _shaderSets[i]->ShaderProgram->getUniformLocation("u_multiplyColor");
        _shaderSets[i]->UniformScreenColorLocation = _shaderSets[i]->ShaderProgram->getUniformLocation("u_screenColor");
        _shaderSets[i]->UniformBlendModeLocation = _shaderSets[i]->ShaderProgram->getUniformLocation("u_blendMode");
    }
}

void CubismShader_Axmol::SetupShaderProgram(CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommand, CubismRenderer_Axmol* renderer, ax::Texture2D* texture
                                                , csmInt32 vertexCount, csmFloat32* vertexArray
                                                , csmFloat32* uvArray, csmFloat32 opacity
                                                , CubismRenderer::CubismBlendMode colorBlendMode
                                                , CubismRenderer::CubismTextureColor baseColor
                                                , CubismRenderer::CubismTextureColor multiplyColor
                                                , CubismRenderer::CubismTextureColor screenColor
                                                , const Live2D::Cubism::Framework::csmBlendMode& sourceBlendMode
                                                , ax::Texture2D* blendTexture
                                                , csmBool isPremultipliedAlpha, CubismMatrix44 matrix4x4
                                                , csmBool invertedMask)
{
    if (_shaderSets.GetSize() == 0)
    {
        GenerateShaders();
    }

    ax::rhi::BlendDesc* blendDescriptor = drawCommand->GetBlendDescriptor();
    auto cmd = drawCommand->GetCommand();

    ax::rhi::ProgramState* programState = cmd->unsafePS();
    VertexLayout* layout                    = nullptr;

    auto needInit = !programState;

    if (renderer->GetClippingContextBufferForMask() != NULL) // マスク生成時
    {
        CubismShaderSet* shaderSet = _shaderSets[ShaderNames_SetupMask];

        if (!programState)
        {
            programState = new ax::rhi::ProgramState(shaderSet->ShaderProgram);
        }

        auto layoutDesc = axvlm->allocateVertexLayoutDesc();

        //テクスチャ設定
        SetTextureByBindingSlot(programState, shaderSet->SamplerTexture0Location, texture->getRHITexture());

        layoutDesc.startLayout(2);

        // 頂点配列の設定
        layoutDesc.addAttrib("a_position", shaderSet->AttributePositionLocation, ax::rhi::VertexFormat::FLOAT2, 0,
                            false);
        // テクスチャ頂点の設定
        layoutDesc.addAttrib("a_texCoord", shaderSet->AttributeTexCoordLocation, ax::rhi::VertexFormat::FLOAT2,
                          sizeof(csmFloat32) * 2, false);
        layoutDesc.endLayout();

        layout = axvlm->getVertexLayout(std::move(layoutDesc));

        // チャンネル
        const csmInt32 channelNo = renderer->GetClippingContextBufferForMask()->_layoutChannelNo;
        CubismRenderer::CubismTextureColor* colorChannel = renderer->GetClippingContextBufferForMask()->GetClippingManager()->GetChannelFlagAsColor(channelNo);
        csmFloat32 colorFlag[4] = { colorChannel->R, colorChannel->G, colorChannel->B, colorChannel->A };
        programState->setUniform(shaderSet->UnifromChannelFlagLocation, colorFlag, sizeof(float) * 4);

        programState->setUniform(shaderSet->UniformClipMatrixLocation,
                                 renderer->GetClippingContextBufferForMask()->_matrixForMask.GetArray(),
                                 sizeof(float) * 16);

        csmRectF* rect = renderer->GetClippingContextBufferForMask()->_layoutBounds;

        csmFloat32 base[4] = { rect->X * 2.0f - 1.0f,
                                    rect->Y * 2.0f - 1.0f,
                                    rect->GetRight() * 2.0f - 1.0f,
                                    rect->GetBottom() * 2.0f - 1.0f };
        programState->setUniform(shaderSet->UniformBaseColorLocation, base, sizeof(float) * 4);

        blendDescriptor->sourceRGBBlendFactor = ax::rhi::BlendFactor::ZERO;
        blendDescriptor->destinationRGBBlendFactor = ax::rhi::BlendFactor::ONE_MINUS_SRC_COLOR;
        blendDescriptor->sourceAlphaBlendFactor = ax::rhi::BlendFactor::ZERO;
        blendDescriptor->destinationAlphaBlendFactor = ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
    }
    else // マスク生成以外の場合
    {
        CubismClippingContext_Axmol* clippingContext = renderer->GetClippingContextBufferForDraw();
        ax::Texture2D* clippingMaskTexture = renderer->_offscreenFrameBuffer.GetColorBuffer();
        if (clippingContext == NULL)
        {
            clippingMaskTexture = renderer->_offscreenMaskFrameBuffer.GetColorBuffer();
            if (clippingMaskTexture)
                clippingContext = renderer->GetClippingContextBufferForOffscreen();
        }

        const csmBool masked = clippingContext != NULL;  // この描画オブジェクトはマスク対象か
        const csmInt32 offset = (masked ? ( invertedMask ? 2 : 1 ) : 0) + (isPremultipliedAlpha ? 3 : 0);
        const csmBool advancedBlend = blendTexture != NULL && IsAdvancedBlendMode(sourceBlendMode);

        CubismShaderSet* shaderSet;
        if (advancedBlend)
        {
            shaderSet = _shaderSets[ShaderNames_Blend + offset];
            blendDescriptor->sourceRGBBlendFactor = ax::rhi::BlendFactor::ONE;
            blendDescriptor->destinationRGBBlendFactor = ax::rhi::BlendFactor::ZERO;
            blendDescriptor->sourceAlphaBlendFactor = ax::rhi::BlendFactor::ONE;
            blendDescriptor->destinationAlphaBlendFactor = ax::rhi::BlendFactor::ZERO;
        }
        else switch (colorBlendMode)
        {
        case CubismRenderer::CubismBlendMode_Normal:
        default:
            shaderSet = _shaderSets[ShaderNames_Normal + offset];
            blendDescriptor->sourceRGBBlendFactor = ax::rhi::BlendFactor::ONE;
            blendDescriptor->destinationRGBBlendFactor = ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
            blendDescriptor->sourceAlphaBlendFactor = ax::rhi::BlendFactor::ONE;
            blendDescriptor->destinationAlphaBlendFactor = ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
            break;

        case CubismRenderer::CubismBlendMode_Additive:
            shaderSet = _shaderSets[ShaderNames_Add + offset];
            blendDescriptor->sourceRGBBlendFactor = ax::rhi::BlendFactor::ONE;
            blendDescriptor->destinationRGBBlendFactor = ax::rhi::BlendFactor::ONE;
            blendDescriptor->sourceAlphaBlendFactor = ax::rhi::BlendFactor::ZERO;
            blendDescriptor->destinationAlphaBlendFactor = ax::rhi::BlendFactor::ONE;
            break;

        case CubismRenderer::CubismBlendMode_Multiplicative:
            shaderSet = _shaderSets[ShaderNames_Mult + offset];
            blendDescriptor->sourceRGBBlendFactor = ax::rhi::BlendFactor::DST_COLOR;
            blendDescriptor->destinationRGBBlendFactor = ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA;
            blendDescriptor->sourceAlphaBlendFactor = ax::rhi::BlendFactor::ZERO;
            blendDescriptor->destinationAlphaBlendFactor = ax::rhi::BlendFactor::ONE;
            break;
        }

        if (!programState)
        {
            programState = new ax::rhi::ProgramState(shaderSet->ShaderProgram);
        }
        auto layoutDesc = axvlm->allocateVertexLayoutDesc();

        layoutDesc.startLayout(2);

        // 頂点配列の設定
        layoutDesc.addAttrib("a_position", shaderSet->AttributePositionLocation, ax::rhi::VertexFormat::FLOAT2, 0,
                          false);
        // テクスチャ頂点の設定
        layoutDesc.addAttrib("a_texCoord", shaderSet->AttributeTexCoordLocation, ax::rhi::VertexFormat::FLOAT2,
                          sizeof(csmFloat32) * 2, false);

        layoutDesc.endLayout();

        layout = axvlm->getVertexLayout(std::move(layoutDesc));

        if (masked)
        {
            // frameBufferに書かれたテクスチャ
            SetTextureByBindingSlot(programState, shaderSet->SamplerTexture1Location, clippingMaskTexture->getRHITexture());

            // View座標をClippingContextの座標に変換するための行列を設定
            programState->setUniform(shaderSet->UniformClipMatrixLocation,
                                     clippingContext->_matrixForDraw.GetArray(),
                                     sizeof(float) * 16);

            // 使用するカラーチャンネルを設定
            const csmInt32 channelNo = clippingContext->_layoutChannelNo;
            CubismRenderer::CubismTextureColor* colorChannel = clippingContext->GetClippingManager()->GetChannelFlagAsColor(channelNo);
            csmFloat32 colorFlag[4] = { colorChannel->R, colorChannel->G, colorChannel->B, colorChannel->A };
            programState->setUniform(shaderSet->UnifromChannelFlagLocation, colorFlag, sizeof(float) * 4);
        }

        //テクスチャ設定
        SetTextureByBindingSlot(programState, shaderSet->SamplerTexture0Location, texture->getRHITexture());

        if (advancedBlend)
        {
            SetTextureByBindingSlot(programState, shaderSet->SamplerBlendTextureLocation, blendTexture->getRHITexture());
            const int blendMode[4] = {
                static_cast<int>(ToColorBlendModeIndex(sourceBlendMode)),
                static_cast<int>(ToAlphaBlendModeIndex(sourceBlendMode)),
                0,
                0
            };
            programState->setUniform(shaderSet->UniformBlendModeLocation, blendMode, sizeof(int) * 4);
        }

        //座標変換
        programState->setUniform(shaderSet->UniformMatrixLocation, matrix4x4.GetArray(), sizeof(float) * 16);

        csmFloat32 base[4] = { baseColor.R, baseColor.G, baseColor.B, baseColor.A };
        programState->setUniform(shaderSet->UniformBaseColorLocation, base, sizeof(float) * 4);
        csmFloat32 multiply[4] = { multiplyColor.R, multiplyColor.G, multiplyColor.B, multiplyColor.A };
        programState->setUniform(shaderSet->UniformMultiplyColorLocation, multiply, sizeof(float) * 4);
        csmFloat32 screen[4] = { screenColor.R, screenColor.G, screenColor.B, screenColor.A };
        programState->setUniform(shaderSet->UniformScreenColorLocation, screen, sizeof(float) * 4);
    }

    blendDescriptor->blendEnabled = true;
    if (needInit)
        cmd->setOwnPSVL(programState, layout, RenderCommand::ADOPT_FLAG_PS);
}

void CubismShader_Axmol::SetupRenderTargetShaderProgram(CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommand,
                                                        ax::Texture2D* texture,
                                                        CubismRenderer::CubismTextureColor baseColor,
                                                        ax::rhi::BlendFactor sourceRGB,
                                                        ax::rhi::BlendFactor destinationRGB,
                                                        ax::rhi::BlendFactor sourceAlpha,
                                                        ax::rhi::BlendFactor destinationAlpha)
{
    if (_shaderSets.GetSize() == 0)
    {
        GenerateShaders();
    }

    CubismShaderSet* shaderSet = _shaderSets[ShaderNames_Copy];
    ax::rhi::BlendDesc* blendDescriptor = drawCommand->GetBlendDescriptor();
    auto cmd = drawCommand->GetCommand();

    ax::rhi::ProgramState* programState = cmd->unsafePS();
    auto needInit = !programState;

    if (!programState)
    {
        programState = new ax::rhi::ProgramState(shaderSet->ShaderProgram);
    }

    auto layoutDesc = axvlm->allocateVertexLayoutDesc();
    layoutDesc.startLayout(2);
    layoutDesc.addAttrib("a_position", shaderSet->AttributePositionLocation, ax::rhi::VertexFormat::FLOAT2, 0, false);
    layoutDesc.addAttrib("a_texCoord", shaderSet->AttributeTexCoordLocation, ax::rhi::VertexFormat::FLOAT2, sizeof(csmFloat32) * 2, false);
    layoutDesc.endLayout();

    VertexLayout* layout = axvlm->getVertexLayout(std::move(layoutDesc));

    CubismMatrix44 identity;
    identity.LoadIdentity();
    programState->setUniform(shaderSet->UniformMatrixLocation, identity.GetArray(), sizeof(float) * 16);

    csmFloat32 base[4] = {baseColor.R, baseColor.G, baseColor.B, baseColor.A};
    programState->setUniform(shaderSet->UniformBaseColorLocation, base, sizeof(float) * 4);
    SetTextureByBindingSlot(programState, shaderSet->SamplerTexture0Location, texture->getRHITexture());

    blendDescriptor->blendEnabled = true;
    blendDescriptor->sourceRGBBlendFactor = sourceRGB;
    blendDescriptor->destinationRGBBlendFactor = destinationRGB;
    blendDescriptor->sourceAlphaBlendFactor = sourceAlpha;
    blendDescriptor->destinationAlphaBlendFactor = destinationAlpha;

    if (needInit)
    {
        cmd->setOwnPSVL(programState, layout, RenderCommand::ADOPT_FLAG_PS);
    }
}

ax::rhi::Program* CubismShader_Axmol::LoadShaderProgram(const csmChar* vertShaderPath,
                                                               const csmChar* fragShaderPath)
{
    // cocos2dx対応
    // Create shader program.
    return ProgramManager::getInstance()->loadProgram(vertShaderPath, fragShaderPath);
}

/*********************************************************************************************************************
 *                                      CubismRenderer_Axmol
 ********************************************************************************************************************/

CubismRenderer* CubismRenderer::Create(csmUint32 width, csmUint32 height)
{
    return CSM_NEW CubismRenderer_Axmol(width, height);
}

void CubismRenderer::StaticRelease()
{
    CubismRenderer_Axmol::DoStaticRelease();
}

namespace
{
    CubismCommandBuffer_Axmol*       _commandBuffer;
    const csmFloat32 ModelRenderTargetVertexArray[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f,  1.0f,
         1.0f,  1.0f,
    };

    const csmFloat32 ModelRenderTargetUvArray[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
    };

    const csmFloat32 ModelRenderTargetReverseUvArray[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
    };

    const csmUint16 ModelRenderTargetIndexArray[] = {
        0, 1, 2,
        1, 3, 2,
    };
}

CubismRenderer_Axmol::CubismRenderer_Axmol(csmUint32 width, csmUint32 height)
                                                   : CubismRenderer(width, height)
                                                     , _clippingManager(NULL)
                                                     , _offscreenClippingManager(NULL)
                                                     , _clippingContextBufferForMask(NULL)
                                                     , _clippingContextBufferForDraw(NULL)
                                                     , _clippingContextBufferForOffscreen(NULL)
                                                     , _currentOffscreen(NULL)
                                                     , _renderTargetDrawCommandBuffer(NULL)
                                                     , _renderTargetCopyDrawCommandBufferIndex(0)
                                                     , _windowResizeListener(NULL)
{
    // テクスチャ対応マップの容量を確保しておく.
    _textures.PrepareCapacity(32, true);

    _windowResizeListener = ax::Director::getInstance()->getEventDispatcher()->addCustomEventListener(
        ax::RenderView::EVENT_WINDOW_RESIZED, [this](ax::CustomEvent*) { OnRenderViewResized(); });
}

CubismRenderer_Axmol::~CubismRenderer_Axmol()
{
    if (_windowResizeListener)
    {
        ax::Director::getInstance()->getEventDispatcher()->removeEventListener(_windowResizeListener);
        _windowResizeListener = NULL;
    }

    CSM_DELETE_SELF(CubismClippingManager_Axmol, _clippingManager);
    CSM_DELETE_SELF(CubismClippingManager_Axmol, _offscreenClippingManager);
    for (csmUint32 i = 0; i < _modelRenderTargets.GetSize(); ++i)
    {
        CSM_DELETE(_modelRenderTargets[i]);
    }
    _modelRenderTargets.Clear();
    for (csmUint32 i = 0; i < _offscreenList.GetSize(); ++i)
    {
        CSM_DELETE(_offscreenList[i]);
    }
    _offscreenList.Clear();
    for (csmUint32 i = 0; i < _drawableDrawCommandBuffer.GetSize(); ++i)
    {
        CSM_DELETE(_drawableDrawCommandBuffer[i]);
    }
    _drawableDrawCommandBuffer.Clear();
    for (csmUint32 i = 0; i < _offscreenDrawCommandBuffer.GetSize(); ++i)
    {
        CSM_DELETE(_offscreenDrawCommandBuffer[i]);
    }
    _offscreenDrawCommandBuffer.Clear();
    for (csmUint32 i = 0; i < _renderTargetCopyDrawCommandBuffers.GetSize(); ++i)
    {
        CSM_DELETE(_renderTargetCopyDrawCommandBuffers[i]);
    }
    _renderTargetCopyDrawCommandBuffers.Clear();
    CSM_DELETE(_renderTargetDrawCommandBuffer);
}

void CubismRenderer_Axmol::DoStaticRelease()
{
#ifdef CSM_TARGET_WINGL
    s_isInitializeGlFunctionsSuccess = false;     ///< 初期化が完了したかどうか。trueなら初期化完了
    s_isFirstInitializeGlFunctions = true;        ///< 最初の初期化実行かどうか。trueなら最初の初期化実行
#endif
    CubismShader_Axmol::DeleteInstance();
}

void CubismRenderer_Axmol::Initialize(CubismModel* model)
{
    Initialize(model, 1);
}

CubismCommandBuffer_Axmol::DrawCommandBuffer* CubismRenderer_Axmol::CreateRenderTargetDrawCommandBuffer()
{
    // TODO: should process in axmol OpenGL RHI backend in the future RHI refactoring
    const bool flipY = DriverContext::isOpenGL();
    CubismCommandBuffer_Axmol::DrawCommandBuffer* commandBuffer = CSM_NEW CubismCommandBuffer_Axmol::DrawCommandBuffer();
    commandBuffer->GetCommandDraw()->GetCommand()->setDrawType(ax::CustomCommand::DrawType::ELEMENT);
    commandBuffer->GetCommandDraw()->GetCommand()->setPrimitiveType(ax::rhi::PrimitiveType::TRIANGLE);
    commandBuffer->CreateVertexBuffer(sizeof(csmFloat32) * 4, 4);
    commandBuffer->CreateIndexBuffer(6);
    commandBuffer->UpdateVertexBuffer(ModelRenderTargetVertexArray, flipY ? ModelRenderTargetReverseUvArray : ModelRenderTargetUvArray, 4);
    commandBuffer->CommitVertexBuffer();
    commandBuffer->UpdateIndexBuffer(ModelRenderTargetIndexArray, 6);

    return commandBuffer;
}

CubismCommandBuffer_Axmol::DrawCommandBuffer* CubismRenderer_Axmol::GetRenderTargetCopyDrawCommandBuffer()
{
    if (_renderTargetCopyDrawCommandBufferIndex >= _renderTargetCopyDrawCommandBuffers.GetSize())
    {
        _renderTargetCopyDrawCommandBuffers.PushBack(CreateRenderTargetDrawCommandBuffer());
    }

    return _renderTargetCopyDrawCommandBuffers[_renderTargetCopyDrawCommandBufferIndex++];
}

void CubismRenderer_Axmol::Initialize(CubismModel* model, csmInt32 maskBufferCount)
{
    if (maskBufferCount < 1)
    {
        maskBufferCount = 1;
        CubismLogWarning("The number of render textures must be an integer greater than or equal to 1. Set the number of render textures to 1.");
    }

    const csmInt32 offscreenCount = model->GetOffscreenCount();
    csmInt32 renderTargetCopyCount = 1;
    if (model->IsBlendModeEnabled())
    {
        renderTargetCopyCount = 0;
        for (csmInt32 i = 0; i < model->GetDrawableCount(); ++i)
        {
            if (IsAdvancedBlendMode(model->GetDrawableBlendModeType(i)))
            {
                ++renderTargetCopyCount;
            }
        }
        for (csmInt32 i = 0; i < offscreenCount; ++i)
        {
            if (IsAdvancedBlendMode(model->GetOffscreenBlendModeType(i)))
            {
                ++renderTargetCopyCount;
            }
        }

        if (renderTargetCopyCount == 0)
        {
            renderTargetCopyCount = 1;
        }
    }

    if (model->IsBlendModeEnabled() || offscreenCount > 0)
    {
        for (csmInt32 i = 0; i < 1 + renderTargetCopyCount; ++i)
        {
            CubismOffscreenFrame_Axmol* renderTarget = CSM_NEW CubismOffscreenFrame_Axmol();
            renderTarget->CreateOffscreenFrame(_modelRenderTargetWidth, _modelRenderTargetHeight);
            _modelRenderTargets.PushBack(renderTarget);
        }
    }

    if (model->IsUsingMasking())
    {
        _clippingManager = CSM_NEW CubismClippingManager_Axmol();  //クリッピングマスク・バッファ前処理方式を初期化
        _clippingManager->Initialize(
            *model,
            maskBufferCount,
            CubismRenderer::DrawableObjectType_Drawable
        );

        _offscreenFrameBuffer.CreateOffscreenFrame(_clippingManager->GetClippingMaskBufferSize().X,
                                                  _clippingManager->GetClippingMaskBufferSize().Y,
                                                  NULL,
                                                  ax::Color::white);
    }

    if (model->IsUsingMaskingForOffscreen())
    {
        _offscreenClippingManager = CSM_NEW CubismClippingManager_Axmol();
        _offscreenClippingManager->Initialize(
            *model,
            maskBufferCount,
            CubismRenderer::DrawableObjectType_Offscreen
        );

        _offscreenMaskFrameBuffer.CreateOffscreenFrame(_offscreenClippingManager->GetClippingMaskBufferSize().X,
                                                       _offscreenClippingManager->GetClippingMaskBufferSize().Y,
                                                       NULL,
                                                       ax::Color::white);
    }

    _sortedDrawableIndexList.Resize(model->GetDrawableCount(), 0);
    _sortedObjectsIndexList.Resize(model->GetDrawableCount() + model->GetOffscreenCount(), 0);
    _sortedObjectsTypeList.Resize(model->GetDrawableCount() + model->GetOffscreenCount(), DrawableObjectType_Drawable);

    for (csmInt32 offscreenIndex = 0; offscreenIndex < offscreenCount; ++offscreenIndex)
    {
        CubismOffscreenRenderTarget_Axmol* offscreen = CSM_NEW CubismOffscreenRenderTarget_Axmol();
        offscreen->SetOffscreenIndex(offscreenIndex);
        _offscreenList.PushBack(offscreen);
        _offscreenDrawCommandBuffer.PushBack(CreateRenderTargetDrawCommandBuffer());
    }

    if (offscreenCount > 0)
    {
        SetupParentOffscreens(model, offscreenCount);
    }

    _drawableDrawCommandBuffer.Resize(model->GetDrawableCount());

    for (csmInt32 i = 0; i < _drawableDrawCommandBuffer.GetSize(); ++i)
    {
        const csmInt32 drawableVertexCount = model->GetDrawableVertexCount(i);
        const csmInt32 drawableVertexIndexCount = model->GetDrawableVertexIndexCount(i);
        const csmSizeInt vertexSize = sizeof(csmFloat32) * 4;

        _drawableDrawCommandBuffer[i] = CSM_NEW CubismCommandBuffer_Axmol::DrawCommandBuffer();
        _drawableDrawCommandBuffer[i]->GetCommandDraw()->GetCommand()->setDrawType(ax::CustomCommand::DrawType::ELEMENT);
        _drawableDrawCommandBuffer[i]->GetCommandDraw()->GetCommand()->setPrimitiveType(ax::rhi::PrimitiveType::TRIANGLE);
        _drawableDrawCommandBuffer[i]->CreateVertexBuffer(vertexSize, drawableVertexCount);      // Vertices + UVs

        if (drawableVertexIndexCount > 0)
        {
            _drawableDrawCommandBuffer[i]->CreateIndexBuffer(drawableVertexIndexCount);
        }
    }

    for (csmInt32 i = 0; i < renderTargetCopyCount; ++i)
    {
        _renderTargetCopyDrawCommandBuffers.PushBack(CreateRenderTargetDrawCommandBuffer());
    }

    _renderTargetDrawCommandBuffer = CreateRenderTargetDrawCommandBuffer();

    CubismRenderer::Initialize(model, maskBufferCount);  //親クラスの処理を呼ぶ
}

void CubismRenderer_Axmol::PreDraw()
{
    _commandBuffer->SetOperationEnable(CubismCommandBuffer_Axmol::OperationType_ScissorTest, false);
    _commandBuffer->SetOperationEnable(CubismCommandBuffer_Axmol::OperationType_StencilTest, false);
    _commandBuffer->SetOperationEnable(CubismCommandBuffer_Axmol::OperationType_DepthTest, false);


    //異方性フィルタリング。プラットフォームのOpenGLによっては未対応の場合があるので、未設定のときは設定しない
    if (GetAnisotropy() > 0.0f)
    {
        // Not supported.
    }
}


void CubismRenderer_Axmol::DoDrawModel()
{
    SyncRenderTargetSizeWithCurrentViewport();

    BeforeDrawModelRenderTarget();

    // axmol process matrix major order with axslcc, so always false
    constexpr csmBool isRightHanded = false;

    //------------ クリッピングマスク・バッファ前処理方式の場合 ------------
    if (_clippingManager != NULL)
    {
        PreDraw();

        // サイズが違う場合はここで作成しなおし
        if (_offscreenFrameBuffer.GetBufferWidth() != static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().X) ||
            _offscreenFrameBuffer.GetBufferHeight() != static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().Y))
        {
            _offscreenFrameBuffer.DestroyOffscreenFrame();
            _offscreenFrameBuffer.CreateOffscreenFrame(
                static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().X),
                static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().Y),
                NULL,
                ax::Color::white);
        }

        rhi::Texture* restoreColorBuffer = _modelRenderTargets.GetSize() > 0
            ? _modelRenderTargets[0]->GetColorBuffer()->getRHITexture()
            : _rendererProfile._lastColorBuffer;
        const csmRectF restoreViewport = GetCurrentDrawViewport();
        _clippingManager->SetupClippingContext(*GetModel(),
                                               this,
                                               restoreColorBuffer,
                                               restoreViewport,
                                               &_offscreenFrameBuffer,
                                               CubismRenderer::DrawableObjectType_Drawable,
                                               isRightHanded);
    }

    if (_offscreenClippingManager != NULL)
    {
        PreDraw();

        if (_offscreenMaskFrameBuffer.GetBufferWidth() != static_cast<csmUint32>(_offscreenClippingManager->GetClippingMaskBufferSize().X) ||
            _offscreenMaskFrameBuffer.GetBufferHeight() != static_cast<csmUint32>(_offscreenClippingManager->GetClippingMaskBufferSize().Y))
        {
            _offscreenMaskFrameBuffer.DestroyOffscreenFrame();
            _offscreenMaskFrameBuffer.CreateOffscreenFrame(
                static_cast<csmUint32>(_offscreenClippingManager->GetClippingMaskBufferSize().X),
                static_cast<csmUint32>(_offscreenClippingManager->GetClippingMaskBufferSize().Y),
                NULL,
                ax::Color::white);
        }

        rhi::Texture* restoreColorBuffer = _modelRenderTargets.GetSize() > 0
            ? _modelRenderTargets[0]->GetColorBuffer()->getRHITexture()
            : _rendererProfile._lastColorBuffer;
        const csmRectF restoreViewport = GetCurrentDrawViewport();
        _offscreenClippingManager->SetupClippingContext(*GetModel(),
                                                        this,
                                                        restoreColorBuffer,
                                                        restoreViewport,
                                                        &_offscreenMaskFrameBuffer,
                                                        CubismRenderer::DrawableObjectType_Offscreen,
                                                        isRightHanded,
                                                        GetMvpMatrix());
    }

    // 上記クリッピング処理内でも一度PreDrawを呼ぶので注意!!
    PreDraw();

    const csmInt32 drawableCount = GetModel()->GetDrawableCount();
    const csmInt32* renderOrder = GetModel()->GetRenderOrders();
    const csmInt32 offscreenCount = GetModel()->GetOffscreenCount();
    const csmInt32 totalCount    = drawableCount + offscreenCount;

    // インデックスを描画順でソート
    for (csmInt32 i = 0; i < drawableCount; ++i)
    {
        const csmInt32 order = renderOrder[i];
        // Axmol spec: CHECK bounds to fix crash
        if (order < drawableCount)
        {
            _sortedDrawableIndexList[order] = i;
        }
    }

    // Update Vertex / Index buffer.
    for (csmInt32 i = 0; i < drawableCount; ++i)
    {
        csmFloat32* vertices = const_cast<csmFloat32*>(GetModel()->GetDrawableVertices(i));
        Core::csmVector2* uvs = const_cast<Core::csmVector2*>(GetModel()->GetDrawableVertexUvs(i));
        csmUint16* vertexIndices = const_cast<csmUint16*>(GetModel()->GetDrawableVertexIndices(i));
        const csmUint32 vertexCount = GetModel()->GetDrawableVertexCount(i);
        const csmUint32 vertexIndexCount = GetModel()->GetDrawableVertexIndexCount(i);

        _drawableDrawCommandBuffer[i]->UpdateVertexBuffer(vertices, uvs, vertexCount);
        _drawableDrawCommandBuffer[i]->CommitVertexBuffer();
        if (vertexIndexCount > 0)
        {
            _drawableDrawCommandBuffer[i]->UpdateIndexBuffer(vertexIndices, vertexIndexCount);
        }

    }

    // 描画
    if (offscreenCount > 0)
    {
        DrawObjectLoop();
        PostDraw();
        AfterDrawModelRenderTarget();
        return;
    }

    for (csmInt32 i = 0; i < drawableCount; ++i)
    {
        const csmInt32 drawableIndex = _sortedDrawableIndexList[i];

        // Drawableが表示状態でなければ処理をパスする
        if (!GetModel()->GetDrawableDynamicFlagIsVisible(drawableIndex))
        {
            continue;
        }

        // クリッピングマスク
        CubismClippingContext_Axmol* clipContext = (_clippingManager != NULL)
            ? (*_clippingManager->GetClippingContextListForDraw())[drawableIndex]
            : NULL;

        if (clipContext != NULL && IsUsingHighPrecisionMask()) // マスクを書く必要がある
        {
            if(clipContext->_isUsing) // 書くことになっていた
            {
                // 生成したFrameBufferと同じサイズでビューポートを設定
                _commandBuffer->Viewport(0, 0, _offscreenFrameBuffer.GetViewPortSize().Width, _offscreenFrameBuffer.GetViewPortSize().Height);

                PreDraw(); // バッファをクリアする

                _offscreenFrameBuffer.BeginDraw(_commandBuffer, _rendererProfile._lastColorBuffer);

                // マスクをクリアする
                // 1が無効（描かれない）領域、0が有効（描かれる）領域。（シェーダで Cd*Csで0に近い値をかけてマスクを作る。1をかけると何も起こらない）
                _offscreenFrameBuffer.Clear(_commandBuffer, 1.0f, 1.0f, 1.0f, 1.0f);
            }

            {
                const csmInt32 clipDrawCount = clipContext->_clippingIdCount;
                for (csmInt32 index = 0; index < clipDrawCount; index++)
                {
                    const csmInt32 clipDrawIndex = clipContext->_clippingIdList[index];
                    CubismCommandBuffer_Axmol::DrawCommandBuffer* drawCommandBufferMask = clipContext->_clippingCommandBufferList->At(index);

                    // 頂点情報が更新されておらず、信頼性がない場合は描画をパスする
                    if (!GetModel()->GetDrawableDynamicFlagVertexPositionsDidChange(clipDrawIndex))
                    {
                        continue;
                    }

                    IsCulling(GetModel()->GetDrawableCulling(clipDrawIndex) != 0);

                    if (GetModel()->GetDrawableVertexIndexCount(clipDrawIndex) <= 0)
                    {
                        continue;
                    }

                    // Update Vertex / Index buffer.
                    {
                        csmFloat32* vertices = const_cast<csmFloat32*>(GetModel()->GetDrawableVertices(clipDrawIndex));
                        Core::csmVector2* uvs = const_cast<Core::csmVector2*>(GetModel()->GetDrawableVertexUvs(clipDrawIndex));
                        csmUint16* vertexIndices = const_cast<csmUint16*>(GetModel()->GetDrawableVertexIndices(clipDrawIndex));
                        const csmUint32 vertexCount = GetModel()->GetDrawableVertexCount(clipDrawIndex);
                        const csmUint32 vertexIndexCount = GetModel()->GetDrawableVertexIndexCount(clipDrawIndex);

                        drawCommandBufferMask->UpdateVertexBuffer(vertices, uvs, vertexCount);
                        drawCommandBufferMask->CommitVertexBuffer();
                        if (vertexIndexCount > 0)
                        {
                            drawCommandBufferMask->UpdateIndexBuffer(vertexIndices, vertexIndexCount);
                        }

                        if (vertexCount <= 0)
                        {
                            continue;
                        }

                    }

                    // 今回専用の変換を適用して描く
                    // チャンネルも切り替える必要がある(A,R,G,B)
                    SetClippingContextBufferForMask(clipContext);
                    DrawMeshCocos2d(
                        drawCommandBufferMask->GetCommandDraw(),
                        GetModel()->GetDrawableTextureIndex(clipDrawIndex),
                        GetModel()->GetDrawableVertexIndexCount(clipDrawIndex),
                        GetModel()->GetDrawableVertexCount(clipDrawIndex),
                        const_cast<csmUint16*>(GetModel()->GetDrawableVertexIndices(clipDrawIndex)),
                        const_cast<csmFloat32*>(GetModel()->GetDrawableVertices(clipDrawIndex)),
                        reinterpret_cast<csmFloat32*>(const_cast<Core::csmVector2*>(GetModel()->GetDrawableVertexUvs(clipDrawIndex))),
                        GetModel()->GetDrawableOpacity(clipDrawIndex),
                        CubismRenderer::CubismBlendMode_Normal,   //クリッピングは通常描画を強制
                        GetNeutralMultiplyColor(),
                        GetNeutralScreenColor(),
                        csmBlendMode(),
                        NULL,
                        false // マスク生成時はクリッピングの反転使用は全く関係がない
                    );
                }
            }

            {
                // --- 後処理 ---
                _offscreenFrameBuffer.EndDraw(_commandBuffer);
                SetClippingContextBufferForMask(NULL);
                const csmRectF drawViewport = GetCurrentDrawViewport();
                _commandBuffer->Viewport(drawViewport.X, drawViewport.Y, drawViewport.Width, drawViewport.Height);

                PreDraw(); // バッファをクリアする
            }
        }

        CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommandDraw = _drawableDrawCommandBuffer[drawableIndex]->GetCommandDraw();

        // クリッピングマスクをセットする
        SetClippingContextBufferForDraw(clipContext);

        IsCulling(GetModel()->GetDrawableCulling(drawableIndex) != 0);

        if (GetModel()->GetDrawableVertexIndexCount(drawableIndex) <= 0)
        {
            continue;
        }

        const csmBlendMode drawableBlendMode = GetModel()->GetDrawableBlendModeType(drawableIndex);
        ax::Texture2D* blendTexture = IsAdvancedBlendMode(drawableBlendMode) ? CopyCurrentModelRenderTarget() : NULL;

        DrawMeshCocos2d(
            drawCommandDraw,
            GetModel()->GetDrawableTextureIndex(drawableIndex),
            GetModel()->GetDrawableVertexIndexCount(drawableIndex),
            GetModel()->GetDrawableVertexCount(drawableIndex),
            const_cast<csmUint16*>(GetModel()->GetDrawableVertexIndices(drawableIndex)),
            const_cast<csmFloat32*>(GetModel()->GetDrawableVertices(drawableIndex)),
            reinterpret_cast<csmFloat32*>(const_cast<Core::csmVector2*>(GetModel()->GetDrawableVertexUvs(drawableIndex))),
            GetModel()->GetDrawableOpacity(drawableIndex),
            ToCubismBlendMode(drawableBlendMode),
            ToCubismTextureColor(GetModel()->GetDrawableMultiplyColor(drawableIndex)),
            ToCubismTextureColor(GetModel()->GetDrawableScreenColor(drawableIndex)),
            drawableBlendMode,
            blendTexture,
            GetModel()->GetDrawableInvertedMask(drawableIndex) // マスクを反転使用するか
        );
    }

    //
    PostDraw();

    AfterDrawModelRenderTarget();
}

void CubismRenderer_Axmol::SetupParentOffscreens(const CubismModel* model, csmInt32 offscreenCount)
{
    for (csmInt32 offscreenIndex = 0; offscreenIndex < offscreenCount; ++offscreenIndex)
    {
        CubismOffscreenRenderTarget_Axmol* parentOffscreen = NULL;
        const csmInt32 ownerIndex = model->GetOffscreenOwnerIndices()[offscreenIndex];
        csmInt32 parentIndex = model->GetPartParentPartIndex(ownerIndex);

        while (parentIndex != CubismModel::CubismNoIndex_Parent)
        {
            for (csmInt32 i = 0; i < offscreenCount; ++i)
            {
                if (model->GetOffscreenOwnerIndices()[_offscreenList.At(i)->GetOffscreenIndex()] != parentIndex)
                {
                    continue;
                }

                parentOffscreen = _offscreenList.At(i);
                break;
            }

            if (parentOffscreen != NULL)
            {
                break;
            }

            parentIndex = model->GetPartParentPartIndex(parentIndex);
        }

        _offscreenList.At(offscreenIndex)->SetParentPartOffscreen(parentOffscreen);
    }
}

void CubismRenderer_Axmol::DrawObjectLoop()
{
    const csmInt32 drawableCount = GetModel()->GetDrawableCount();
    const csmInt32 offscreenCount = GetModel()->GetOffscreenCount();
    const csmInt32 totalCount = drawableCount + offscreenCount;
    const csmInt32* renderOrder = GetModel()->GetRenderOrders();

    _currentOffscreen = NULL;

    // インデックスを描画順でソート
    for (csmInt32 i = 0; i < totalCount; ++i)
    {
        const csmInt32 order = renderOrder[i];
        if (i < drawableCount)
        {
            _sortedObjectsIndexList[order] = i;
            _sortedObjectsTypeList[order] = DrawableObjectType_Drawable;
        }
        else if (i < totalCount)
        { // Axmol spec: sync from live2d runtime 5.5 OpenGL
            _sortedObjectsIndexList[order] = i - drawableCount;
            _sortedObjectsTypeList[order] = DrawableObjectType_Offscreen;
        }
    }

    for (csmInt32 i = 0; i < totalCount; ++i)
    {
        RenderObject(_sortedObjectsIndexList[i], _sortedObjectsTypeList[i]);
    }

    while (_currentOffscreen != NULL)
    {
        SubmitDrawToParentOffscreen(_currentOffscreen->GetOffscreenIndex(), DrawableObjectType_Offscreen);
    }
}

void CubismRenderer_Axmol::RenderObject(csmInt32 objectIndex, DrawableObjectType objectType)
{
    switch (objectType)
    {
    case DrawableObjectType_Drawable:
        DrawDrawable(objectIndex);
        break;
    case DrawableObjectType_Offscreen:
        AddOffscreen(objectIndex);
        break;
    default:
        CubismLogError("Unknown drawable type: %d", objectType);
        break;
    }
}

void CubismRenderer_Axmol::DrawMaskForClippingContext(CubismClippingContext_Axmol* clipContext,
                                                      CubismClippingManager_Axmol* clippingManager,
                                                      CubismOffscreenFrame_Axmol* maskBuffer,
                                                      rhi::Texture* restoreColorBuffer)
{
    if (clipContext == NULL || clippingManager == NULL || maskBuffer == NULL || !clipContext->_isUsing)
    {
        return;
    }

    _commandBuffer->Viewport(0, 0, maskBuffer->GetViewPortSize().Width, maskBuffer->GetViewPortSize().Height);
    PreDraw();
    maskBuffer->BeginDraw(_commandBuffer, restoreColorBuffer);
    maskBuffer->Clear(_commandBuffer, 1.0f, 1.0f, 1.0f, 1.0f);

    const csmInt32 clipDrawCount = clipContext->_clippingIdCount;
    for (csmInt32 index = 0; index < clipDrawCount; ++index)
    {
        const csmInt32 clipDrawIndex = clipContext->_clippingIdList[index];
        CubismCommandBuffer_Axmol::DrawCommandBuffer* drawCommandBufferMask = clipContext->_clippingCommandBufferList->At(index);

        if (!GetModel()->GetDrawableDynamicFlagVertexPositionsDidChange(clipDrawIndex))
        {
            continue;
        }

        if (GetModel()->GetDrawableVertexIndexCount(clipDrawIndex) <= 0)
        {
            continue;
        }

        csmFloat32* vertices = const_cast<csmFloat32*>(GetModel()->GetDrawableVertices(clipDrawIndex));
        Core::csmVector2* uvs = const_cast<Core::csmVector2*>(GetModel()->GetDrawableVertexUvs(clipDrawIndex));
        csmUint16* vertexIndices = const_cast<csmUint16*>(GetModel()->GetDrawableVertexIndices(clipDrawIndex));
        const csmUint32 vertexCount = GetModel()->GetDrawableVertexCount(clipDrawIndex);
        const csmUint32 vertexIndexCount = GetModel()->GetDrawableVertexIndexCount(clipDrawIndex);
        if (vertexCount <= 0)
        {
            continue;
        }

        drawCommandBufferMask->UpdateVertexBuffer(vertices, uvs, vertexCount);
        drawCommandBufferMask->CommitVertexBuffer();
        if (vertexIndexCount > 0)
        {
            drawCommandBufferMask->UpdateIndexBuffer(vertexIndices, vertexIndexCount);
        }

        IsCulling(GetModel()->GetDrawableCulling(clipDrawIndex) != 0);
        SetClippingContextBufferForMask(clipContext);
        DrawMeshCocos2d(
            drawCommandBufferMask->GetCommandDraw(),
            GetModel()->GetDrawableTextureIndex(clipDrawIndex),
            GetModel()->GetDrawableVertexIndexCount(clipDrawIndex),
            GetModel()->GetDrawableVertexCount(clipDrawIndex),
            const_cast<csmUint16*>(GetModel()->GetDrawableVertexIndices(clipDrawIndex)),
            const_cast<csmFloat32*>(GetModel()->GetDrawableVertices(clipDrawIndex)),
            reinterpret_cast<csmFloat32*>(const_cast<Core::csmVector2*>(GetModel()->GetDrawableVertexUvs(clipDrawIndex))),
            GetModel()->GetDrawableOpacity(clipDrawIndex),
            CubismRenderer::CubismBlendMode_Normal,
            GetNeutralMultiplyColor(),
            GetNeutralScreenColor(),
            csmBlendMode(),
            NULL,
            false);
    }

    maskBuffer->EndDraw(_commandBuffer);
    SetClippingContextBufferForMask(NULL);
    const csmRectF drawViewport = GetCurrentDrawViewport();
    _commandBuffer->Viewport(drawViewport.X, drawViewport.Y, drawViewport.Width, drawViewport.Height);
    PreDraw();
}

void CubismRenderer_Axmol::DrawDrawable(csmInt32 drawableIndex)
{
    if (!GetModel()->GetDrawableDynamicFlagIsVisible(drawableIndex))
    {
        return;
    }

    SubmitDrawToParentOffscreen(drawableIndex, DrawableObjectType_Drawable);

    CubismClippingContext_Axmol* clipContext = (_clippingManager != NULL)
        ? (*_clippingManager->GetClippingContextListForDraw())[drawableIndex]
        : NULL;

    if (clipContext != NULL && IsUsingHighPrecisionMask())
    {
        rhi::Texture* restoreColorBuffer = _currentOffscreen != NULL
            ? _currentOffscreen->GetRenderTarget()->GetColorBuffer()->getRHITexture()
            : (_modelRenderTargets.GetSize() > 0 ? _modelRenderTargets[0]->GetColorBuffer()->getRHITexture() : _rendererProfile._lastColorBuffer);
        DrawMaskForClippingContext(clipContext, _clippingManager, &_offscreenFrameBuffer, restoreColorBuffer);
    }

    SetClippingContextBufferForDraw(clipContext);
    IsCulling(GetModel()->GetDrawableCulling(drawableIndex) != 0);

    if (GetModel()->GetDrawableVertexIndexCount(drawableIndex) <= 0)
    {
        return;
    }

    const csmBlendMode drawableBlendMode = GetModel()->GetDrawableBlendModeType(drawableIndex);
    ax::Texture2D* blendTexture = IsAdvancedBlendMode(drawableBlendMode)
        ? CopyCurrentRenderTarget(_currentOffscreen != NULL ? _currentOffscreen->GetRenderTarget() : _modelRenderTargets[0])
        : NULL;

    DrawMeshCocos2d(
        _drawableDrawCommandBuffer[drawableIndex]->GetCommandDraw(),
        GetModel()->GetDrawableTextureIndex(drawableIndex),
        GetModel()->GetDrawableVertexIndexCount(drawableIndex),
        GetModel()->GetDrawableVertexCount(drawableIndex),
        const_cast<csmUint16*>(GetModel()->GetDrawableVertexIndices(drawableIndex)),
        const_cast<csmFloat32*>(GetModel()->GetDrawableVertices(drawableIndex)),
        reinterpret_cast<csmFloat32*>(const_cast<Core::csmVector2*>(GetModel()->GetDrawableVertexUvs(drawableIndex))),
        GetModel()->GetDrawableOpacity(drawableIndex),
        ToCubismBlendMode(drawableBlendMode),
        ToCubismTextureColor(GetModel()->GetDrawableMultiplyColor(drawableIndex)),
        ToCubismTextureColor(GetModel()->GetDrawableScreenColor(drawableIndex)),
        drawableBlendMode,
        blendTexture,
        GetModel()->GetDrawableInvertedMask(drawableIndex));
}

void CubismRenderer_Axmol::SubmitDrawToParentOffscreen(csmInt32 objectIndex, DrawableObjectType objectType)
{
    if (_currentOffscreen == NULL || objectIndex == CubismModel::CubismNoIndex_Offscreen)
    {
        return;
    }

    csmInt32 currentOwnerIndex = GetModel()->GetOffscreenOwnerIndices()[_currentOffscreen->GetOffscreenIndex()];
    if (currentOwnerIndex == CubismModel::CubismNoIndex_Offscreen)
    {
        return;
    }

    csmInt32 targetParentIndex = CubismModel::CubismNoIndex_Parent;
    switch (objectType)
    {
    case DrawableObjectType_Drawable:
        targetParentIndex = GetModel()->GetDrawableParentPartIndex(objectIndex);
        break;
    case DrawableObjectType_Offscreen:
        targetParentIndex = GetModel()->GetPartParentPartIndex(GetModel()->GetOffscreenOwnerIndices()[objectIndex]);
        break;
    default:
        return;
    }

    while (targetParentIndex != CubismModel::CubismNoIndex_Parent)
    {
        if (targetParentIndex == currentOwnerIndex)
        {
            return;
        }

        targetParentIndex = GetModel()->GetPartParentPartIndex(targetParentIndex);
    }

    DrawOffscreen(_currentOffscreen);
    SubmitDrawToParentOffscreen(objectIndex, objectType);
}

void CubismRenderer_Axmol::AddOffscreen(csmInt32 offscreenIndex)
{
    if (_currentOffscreen != NULL && _currentOffscreen->GetOffscreenIndex() != offscreenIndex)
    {
        csmBool isParent = false;
        csmInt32 ownerIndex = GetModel()->GetOffscreenOwnerIndices()[offscreenIndex];
        csmInt32 parentIndex = GetModel()->GetPartParentPartIndex(ownerIndex);

        csmInt32 currentOffscreenOwnerIndex = GetModel()->GetOffscreenOwnerIndices()[_currentOffscreen->GetOffscreenIndex()];
        while (parentIndex != CubismModel::CubismNoIndex_Parent)
        {
            if (parentIndex == currentOffscreenOwnerIndex)
            {
                isParent = true;
                break;
            }
            parentIndex = GetModel()->GetPartParentPartIndex(parentIndex);
        }

        if (!isParent)
        {
            SubmitDrawToParentOffscreen(offscreenIndex, DrawableObjectType_Offscreen);
        }
    }

    CubismOffscreenRenderTarget_Axmol* offscreen = _offscreenList.At(offscreenIndex);
    if (offscreen->GetRenderTarget()->GetBufferWidth() != _modelRenderTargetWidth ||
        offscreen->GetRenderTarget()->GetBufferHeight() != _modelRenderTargetHeight)
    {
        offscreen->GetRenderTarget()->CreateOffscreenFrame(_modelRenderTargetWidth, _modelRenderTargetHeight);
    }
    if (!offscreen->GetRenderTarget()->IsValid())
    {
        offscreen->GetRenderTarget()->CreateOffscreenFrame(_modelRenderTargetWidth, _modelRenderTargetHeight);
    }

    CubismOffscreenRenderTarget_Axmol* oldOffscreen = offscreen->GetParentPartOffscreen();
    offscreen->SetOldOffscreen(oldOffscreen);

    ax::rhi::Texture* restoreColorBuffer = oldOffscreen != NULL
        ? oldOffscreen->GetRenderTarget()->GetColorBuffer()->getRHITexture()
        : _modelRenderTargets[0]->GetColorBuffer()->getRHITexture();

    offscreen->GetRenderTarget()->BeginDraw(_commandBuffer, restoreColorBuffer);
    const csmRectF offscreenViewport = offscreen->GetRenderTarget()->GetViewPortSize();
    _commandBuffer->Viewport(offscreenViewport.X, offscreenViewport.Y, offscreenViewport.Width, offscreenViewport.Height);
    offscreen->GetRenderTarget()->Clear(_commandBuffer, 0.0f, 0.0f, 0.0f, 0.0f);
    _currentOffscreen = offscreen;
}

void CubismRenderer_Axmol::DrawOffscreen(CubismOffscreenRenderTarget_Axmol* currentOffscreen)
{
    if (currentOffscreen == NULL)
    {
        return;
    }

    const csmInt32 offscreenIndex = currentOffscreen->GetOffscreenIndex();
    currentOffscreen->GetRenderTarget()->EndDraw(_commandBuffer);
    _currentOffscreen = currentOffscreen->GetOldOffscreen();

    CubismClippingContext_Axmol* clipContext = (_offscreenClippingManager != NULL)
        ? (*_offscreenClippingManager->GetClippingContextListForOffscreen())[offscreenIndex]
        : NULL;
    if (clipContext != NULL && IsUsingHighPrecisionMask())
    {
        DrawMaskForClippingContext(clipContext,
                                   _offscreenClippingManager,
                                   &_offscreenMaskFrameBuffer,
                                   _currentOffscreen != NULL
                                       ? _currentOffscreen->GetRenderTarget()->GetColorBuffer()->getRHITexture()
                                       : (_modelRenderTargets.GetSize() > 0 ? _modelRenderTargets[0]->GetColorBuffer()->getRHITexture() : _rendererProfile._lastColorBuffer));
    }
    SetClippingContextBufferForOffscreen(clipContext);
    IsCulling(GetModel()->GetOffscreenCulling(offscreenIndex) != 0);

    CubismOffscreenFrame_Axmol* destination = _currentOffscreen != NULL ? _currentOffscreen->GetRenderTarget() : _modelRenderTargets[0];
    const csmBlendMode offscreenBlendMode = GetModel()->GetOffscreenBlendModeType(offscreenIndex);
    ax::Texture2D* blendTexture = IsAdvancedBlendMode(offscreenBlendMode) ? CopyCurrentRenderTarget(destination) : NULL;

    csmFloat32 offscreenOpacity = GetModel()->GetOffscreenOpacity(offscreenIndex);
    CubismTextureColor baseColor(offscreenOpacity, offscreenOpacity, offscreenOpacity, offscreenOpacity);

    const CubismModelMultiplyAndScreenColor& overrideMultiplyAndScreenColor = GetModel()->GetOverrideMultiplyAndScreenColor();
    DrawRenderTargetMeshToCurrent(
        _offscreenDrawCommandBuffer[offscreenIndex]->GetCommandDraw(),
        currentOffscreen->GetRenderTarget()->GetColorBuffer(),
        ToCubismBlendMode(offscreenBlendMode),
        baseColor,
        overrideMultiplyAndScreenColor.GetOffscreenMultiplyColor(offscreenIndex),
        overrideMultiplyAndScreenColor.GetOffscreenScreenColor(offscreenIndex),
        offscreenBlendMode,
        blendTexture,
        GetModel()->GetOffscreenInvertedMask(offscreenIndex));
    SetClippingContextBufferForOffscreen(NULL);
}

void CubismRenderer_Axmol::SyncRenderTargetSizeWithCurrentViewport()
{
    const auto& viewport = GetCocos2dRenderer()->getViewport();
    SyncRenderTargetSize(viewport.w, viewport.h);
}

void CubismRenderer_Axmol::OnRenderViewResized()
{
    auto renderView = ax::Director::getInstance()->getRenderView();
    if (!renderView)
    {
        return;
    }

    const ax::Size& viewportSize = renderView->getViewportRect().size;
    SyncRenderTargetSize(viewportSize.width, viewportSize.height);
}

void CubismRenderer_Axmol::SyncRenderTargetSize(csmFloat32 width, csmFloat32 height)
{
    if (width <= 0.0f || height <= 0.0f)
    {
        return;
    }

    const csmUint32 renderTargetWidth =
        static_cast<csmUint32>(std::max<csmFloat32>(1.0f, std::round(width)));
    const csmUint32 renderTargetHeight =
        static_cast<csmUint32>(std::max<csmFloat32>(1.0f, std::round(height)));

    if (_modelRenderTargetWidth != renderTargetWidth || _modelRenderTargetHeight != renderTargetHeight)
    {
        SetRenderTargetSize(renderTargetWidth, renderTargetHeight);
    }
}

csmRectF CubismRenderer_Axmol::GetCurrentDrawViewport() const
{
    if (_currentOffscreen != NULL && _currentOffscreen->GetRenderTarget()->IsValid())
    {
        return _currentOffscreen->GetRenderTarget()->GetViewPortSize();
    }

    if (_modelRenderTargets.GetSize() > 0 && _modelRenderTargets[0]->IsValid())
    {
        return _modelRenderTargets[0]->GetViewPortSize();
    }

    return _rendererProfile._lastViewport;
}

void CubismRenderer_Axmol::BeforeDrawModelRenderTarget()
{
    _renderTargetCopyDrawCommandBufferIndex = 0;

    if (_modelRenderTargets.GetSize() == 0)
    {
        return;
    }

    for (csmUint32 i = 0; i < _modelRenderTargets.GetSize(); ++i)
    {
        if (_modelRenderTargets[i]->GetBufferWidth() != _modelRenderTargetWidth ||
            _modelRenderTargets[i]->GetBufferHeight() != _modelRenderTargetHeight)
        {
            _modelRenderTargets[i]->CreateOffscreenFrame(_modelRenderTargetWidth, _modelRenderTargetHeight);
        }
    }

    _modelRenderTargets[0]->BeginDraw(_commandBuffer, _rendererProfile._lastColorBuffer);
    const csmRectF modelViewport = _modelRenderTargets[0]->GetViewPortSize();
    _commandBuffer->Viewport(modelViewport.X, modelViewport.Y, modelViewport.Width, modelViewport.Height);
    _modelRenderTargets[0]->Clear(_commandBuffer, 0.0f, 0.0f, 0.0f, 0.0f);
}

void CubismRenderer_Axmol::AfterDrawModelRenderTarget()
{
    if (_modelRenderTargets.GetSize() == 0)
    {
        return;
    }

    _modelRenderTargets[0]->EndDraw(_commandBuffer);
    _commandBuffer->Viewport(_rendererProfile._lastViewport.X, _rendererProfile._lastViewport.Y,
                             _rendererProfile._lastViewport.Width, _rendererProfile._lastViewport.Height);

    CubismTextureColor baseColor = GetModelColor();
    baseColor.R *= baseColor.A;
    baseColor.G *= baseColor.A;
    baseColor.B *= baseColor.A;

    DrawRenderTargetToCurrent(_modelRenderTargets[0],
                              _renderTargetDrawCommandBuffer->GetCommandDraw(),
                              baseColor,
                              ax::rhi::BlendFactor::ONE,
                              ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA,
                              ax::rhi::BlendFactor::ONE,
                              ax::rhi::BlendFactor::ONE_MINUS_SRC_ALPHA);
}

ax::Texture2D* CubismRenderer_Axmol::CopyCurrentModelRenderTarget()
{
    return CopyCurrentRenderTarget(_modelRenderTargets.GetSize() > 0 ? _modelRenderTargets[0] : NULL);
}

ax::Texture2D* CubismRenderer_Axmol::CopyCurrentRenderTarget(CubismOffscreenFrame_Axmol* sourceTarget)
{
    const csmUint32 copyTargetIndex = 1 + _renderTargetCopyDrawCommandBufferIndex;
    if (copyTargetIndex >= _modelRenderTargets.GetSize())
    {
        return NULL;
    }
    if (sourceTarget == NULL)
    {
        return NULL;
    }

    CubismOffscreenFrame_Axmol* copyTarget = _modelRenderTargets[copyTargetIndex];
    copyTarget->BeginDraw(_commandBuffer, sourceTarget->GetColorBuffer()->getRHITexture());
    const csmRectF copyViewport = copyTarget->GetViewPortSize();
    _commandBuffer->Viewport(copyViewport.X, copyViewport.Y, copyViewport.Width, copyViewport.Height);
    copyTarget->Clear(_commandBuffer, 0.0f, 0.0f, 0.0f, 0.0f);

    CubismCommandBuffer_Axmol::DrawCommandBuffer* copyCommandBuffer = GetRenderTargetCopyDrawCommandBuffer();
    DrawRenderTargetToCurrent(sourceTarget,
                              copyCommandBuffer->GetCommandDraw(),
                              CubismTextureColor(1.0f, 1.0f, 1.0f, 1.0f),
                              ax::rhi::BlendFactor::ONE,
                              ax::rhi::BlendFactor::ZERO,
                              ax::rhi::BlendFactor::ONE,
                              ax::rhi::BlendFactor::ZERO);

    copyTarget->EndDraw(_commandBuffer);
    const csmRectF sourceViewport = sourceTarget->GetViewPortSize();
    _commandBuffer->Viewport(sourceViewport.X, sourceViewport.Y, sourceViewport.Width, sourceViewport.Height);

    return copyTarget->GetColorBuffer();
}

void CubismRenderer_Axmol::DrawRenderTargetMeshToCurrent(CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommand,
                                                         ax::Texture2D* texture,
                                                         CubismBlendMode colorBlendMode,
                                                         CubismTextureColor baseColor,
                                                         CubismTextureColor multiplyColor,
                                                         CubismTextureColor screenColor,
                                                         const csmBlendMode& sourceBlendMode,
                                                         ax::Texture2D* blendTexture,
                                                         csmBool invertedMask)
{
    CubismMatrix44 identity;
    identity.LoadIdentity();

    if (IsCulling())
    {
        _commandBuffer->SetCullMode(CubismCommandBuffer_Axmol::CullType_Back);
        _commandBuffer->SetOperationEnable(CubismCommandBuffer_Axmol::OperationType_Culling, true);
    }
    else
    {
        _commandBuffer->SetOperationEnable(CubismCommandBuffer_Axmol::OperationType_Culling, false);
    }
    _commandBuffer->SetWindingMode(CubismCommandBuffer_Axmol::WindingType_CounterClockWise);

    CubismShader_Axmol::GetInstance()->SetupShaderProgram(
        drawCommand,
        this,
        texture,
        4,
        const_cast<csmFloat32*>(ModelRenderTargetVertexArray),
        const_cast<csmFloat32*>(ModelRenderTargetUvArray),
        baseColor.A,
        colorBlendMode,
        baseColor,
        multiplyColor,
        screenColor,
        sourceBlendMode,
        blendTexture,
        true,
        identity,
        invertedMask);

    _commandBuffer->AddDrawCommand(drawCommand);
}

void CubismRenderer_Axmol::DrawRenderTargetToCurrent(CubismOffscreenFrame_Axmol* renderTarget,
                                                     CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommand,
                                                     CubismTextureColor baseColor,
                                                     ax::rhi::BlendFactor sourceRGB,
                                                     ax::rhi::BlendFactor destinationRGB,
                                                     ax::rhi::BlendFactor sourceAlpha,
                                                     ax::rhi::BlendFactor destinationAlpha)
{
    if (renderTarget == NULL || !renderTarget->IsValid())
    {
        return;
    }

    CubismShader_Axmol::GetInstance()->SetupRenderTargetShaderProgram(
        drawCommand,
        renderTarget->GetColorBuffer(),
        baseColor,
        sourceRGB,
        destinationRGB,
        sourceAlpha,
        destinationAlpha);

    _commandBuffer->AddDrawCommand(drawCommand);
}

void CubismRenderer_Axmol::DrawMesh(csmInt32 textureNo, csmInt32 indexCount, csmInt32 vertexCount,
    csmUint16* indexArray, csmFloat32* vertexArray, csmFloat32* uvArray, csmFloat32 opacity,
    CubismBlendMode colorBlendMode, csmBool invertedMask)
{
}

void CubismRenderer_Axmol::DrawMeshCocos2d(CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommand, csmInt32 textureNo, csmInt32 indexCount, csmInt32 vertexCount
                                        , csmUint16* indexArray, csmFloat32* vertexArray, csmFloat32* uvArray
                                        , csmFloat32 opacity, CubismBlendMode colorBlendMode
                                        , CubismTextureColor multiplyColor, CubismTextureColor screenColor
                                        , const Live2D::Cubism::Framework::csmBlendMode& sourceBlendMode
                                        , ax::Texture2D* blendTexture
                                        , csmBool invertedMask)
{
#ifndef CSM_DEBUG
    if (_textures[textureNo] == 0) return;    // モデルが参照するテクスチャがバインドされていない場合は描画をスキップする
#endif

    // 裏面描画の有効・無効
    if (IsCulling())
    {
        _commandBuffer->SetCullMode(CubismCommandBuffer_Axmol::CullType_Back);
        _commandBuffer->SetOperationEnable(CubismCommandBuffer_Axmol::OperationType_Culling, true);
    }
    else
    {
        _commandBuffer->SetOperationEnable(CubismCommandBuffer_Axmol::OperationType_Culling, false);
    }

    // Cubism SDK OpenGLはマスク・アートメッシュ共にCCWが表面
    _commandBuffer->SetWindingMode(CubismCommandBuffer_Axmol::WindingType_CounterClockWise);

    CubismTextureColor modelColorRGBA = GetModelColor();

    if (GetClippingContextBufferForMask() == NULL) // マスク生成時以外
    {
        if (GetModel()->IsBlendModeEnabled())
        {
            modelColorRGBA.R = IsPremultipliedAlpha() ? opacity : 1.0f;
            modelColorRGBA.G = IsPremultipliedAlpha() ? opacity : 1.0f;
            modelColorRGBA.B = IsPremultipliedAlpha() ? opacity : 1.0f;
            modelColorRGBA.A = opacity;
        }
        else
        {
            modelColorRGBA.A *= opacity;
            if (IsPremultipliedAlpha())
            {
                modelColorRGBA.R *= modelColorRGBA.A;
                modelColorRGBA.G *= modelColorRGBA.A;
                modelColorRGBA.B *= modelColorRGBA.A;
            }
        }
    }

    ax::Texture2D* drawTexture;   // シェーダに渡すテクスチャ

    // テクスチャマップからバインド済みテクスチャIDを取得
    // バインドされていなければダミーのテクスチャIDをセットする
    if(_textures[textureNo] != NULL)
    {
        drawTexture = _textures[textureNo];
    }
    else
    {
        drawTexture = NULL;
    }


    CubismShader_Axmol::GetInstance()->SetupShaderProgram(
        drawCommand, this, drawTexture, vertexCount, vertexArray, uvArray
        , opacity, colorBlendMode, modelColorRGBA, multiplyColor, screenColor, sourceBlendMode, blendTexture, IsPremultipliedAlpha()
        , GetMvpMatrix(), invertedMask
    );


    // ポリゴンメッシュを描画する
    _commandBuffer->AddDrawCommand(drawCommand);


    // 後処理
    SetClippingContextBufferForDraw(NULL);
    SetClippingContextBufferForMask(NULL);
}

CubismCommandBuffer_Axmol* CubismRenderer_Axmol::GetCommandBuffer()
{
    return _commandBuffer;
}

void CubismRenderer_Axmol::StartFrame(CubismCommandBuffer_Axmol* commandBuffer)
{
    _commandBuffer = commandBuffer;
}

void CubismRenderer_Axmol::EndFrame(CubismCommandBuffer_Axmol* commandBuffer)
{
}

CubismCommandBuffer_Axmol::DrawCommandBuffer* CubismRenderer_Axmol::GetDrawCommandBufferData(csmInt32 drawableIndex)
{
    return _drawableDrawCommandBuffer[drawableIndex];
}

void CubismRenderer_Axmol::SaveProfile()
{
    _rendererProfile.Save();
}

void CubismRenderer_Axmol::RestoreProfile()
{
    _rendererProfile.Restore();
}

void CubismRenderer_Axmol::BindTexture(csmUint32 modelTextureNo, ax::Texture2D* texture)
{
    _textures[modelTextureNo] = texture;
}

const csmMap<csmInt32, ax::Texture2D*>& CubismRenderer_Axmol::GetBindedTextures() const
{
    return _textures;
}

void CubismRenderer_Axmol::SetClippingMaskBufferSize(csmFloat32 width, csmFloat32 height)
{
    //FrameBufferのサイズを変更するためにインスタンスを破棄・再作成する
    CSM_DELETE_SELF(CubismClippingManager_Axmol, _clippingManager);
    CSM_DELETE_SELF(CubismClippingManager_Axmol, _offscreenClippingManager);
    _clippingManager = NULL;
    _offscreenClippingManager = NULL;

    if (GetModel()->IsUsingMasking())
    {
        _clippingManager = CSM_NEW CubismClippingManager_Axmol();

        _clippingManager->SetClippingMaskBufferSize(width, height);

        _clippingManager->Initialize(
            *GetModel(),
            1,
            CubismRenderer::DrawableObjectType_Drawable
        );
    }

    if (GetModel()->IsUsingMaskingForOffscreen())
    {
        _offscreenClippingManager = CSM_NEW CubismClippingManager_Axmol();
        _offscreenClippingManager->SetClippingMaskBufferSize(width, height);
        _offscreenClippingManager->Initialize(
            *GetModel(),
            1,
            CubismRenderer::DrawableObjectType_Offscreen
        );
    }
}

CubismVector2 CubismRenderer_Axmol::GetClippingMaskBufferSize() const
{
    return _clippingManager != NULL ? _clippingManager->GetClippingMaskBufferSize() : CubismVector2();
}

void CubismRenderer_Axmol::SetClippingContextBufferForMask(CubismClippingContext_Axmol* clip)
{
    _clippingContextBufferForMask = clip;
}

CubismClippingContext_Axmol* CubismRenderer_Axmol::GetClippingContextBufferForMask() const
{
    return _clippingContextBufferForMask;
}

void CubismRenderer_Axmol::SetClippingContextBufferForDraw(CubismClippingContext_Axmol* clip)
{
    _clippingContextBufferForDraw = clip;
}

CubismClippingContext_Axmol* CubismRenderer_Axmol::GetClippingContextBufferForDraw() const
{
    return _clippingContextBufferForDraw;
}

void CubismRenderer_Axmol::SetClippingContextBufferForOffscreen(CubismClippingContext_Axmol* clip)
{
    _clippingContextBufferForOffscreen = clip;
}

CubismClippingContext_Axmol* CubismRenderer_Axmol::GetClippingContextBufferForOffscreen() const
{
    return _clippingContextBufferForOffscreen;
}

}}}}

//------------ LIVE2D NAMESPACE ------------
