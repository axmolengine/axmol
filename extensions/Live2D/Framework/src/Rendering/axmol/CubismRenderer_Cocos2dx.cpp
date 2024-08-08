/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#include "CubismRenderer_Cocos2dx.hpp"
#include "Math/CubismMatrix44.hpp"
#include "Type/csmVector.hpp"
#include "Model/CubismModel.hpp"
#include <float.h>
#include "renderer/backend/DriverBase.h"

USING_NS_AX;

#ifdef CSM_TARGET_WIN_GL
#include <Windows.h>
#endif

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

/*********************************************************************************************************************
*                                      CubismClippingManager_Cocos2dx
********************************************************************************************************************/
///< ファイルスコープの変数宣言
namespace {
const csmInt32 ColorChannelCount = 4;   ///< 実験時に1チャンネルの場合は1、RGBだけの場合は3、アルファも含める場合は4
}

CubismClippingManager_Cocos2dx::CubismClippingManager_Cocos2dx() :
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

CubismClippingManager_Cocos2dx::~CubismClippingManager_Cocos2dx()
{
    for (csmUint32 i = 0; i < _clippingContextListForMask.GetSize(); i++)
    {
        if (_clippingContextListForMask[i]) CSM_DELETE_SELF(CubismClippingContext, _clippingContextListForMask[i]);
        _clippingContextListForMask[i] = NULL;
    }

    // _clippingContextListForDrawは_clippingContextListForMaskにあるインスタンスを指している。上記の処理により要素ごとのDELETEは不要。
    for (csmUint32 i = 0; i < _clippingContextListForDraw.GetSize(); i++)
    {
        _clippingContextListForDraw[i] = NULL;
    }

    for (csmUint32 i = 0; i < _channelColors.GetSize(); i++)
    {
        if (_channelColors[i]) CSM_DELETE(_channelColors[i]);
        _channelColors[i] = NULL;
    }
}

void CubismClippingManager_Cocos2dx::Initialize(CubismModel& model, csmInt32 drawableCount, const csmInt32** drawableMasks, const csmInt32* drawableMaskCounts)
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
        CubismClippingContext* cc = FindSameClip(drawableMasks[i], drawableMaskCounts[i]);
        if (cc == NULL)
        {
            // 同一のマスクが存在していない場合は生成する
            cc = CSM_NEW CubismClippingContext(this, model, drawableMasks[i], drawableMaskCounts[i]);
            _clippingContextListForMask.PushBack(cc);
        }

        cc->AddClippedDrawable(i);

        _clippingContextListForDraw.PushBack(cc);

    }
}

CubismClippingContext* CubismClippingManager_Cocos2dx::FindSameClip(const csmInt32* drawableMasks, csmInt32 drawableMaskCounts) const
{
    // 作成済みClippingContextと一致するか確認
    for (csmUint32 i = 0; i < _clippingContextListForMask.GetSize(); i++)
    {
        CubismClippingContext* cc = _clippingContextListForMask[i];
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

void CubismClippingManager_Cocos2dx::SetupClippingContext(CubismModel& model, CubismRenderer_Cocos2dx* renderer, backend::TextureBackend* lastColorBuffer, csmRectF lastViewport)
{
    _currentFrameNo++;

    // 全てのクリッピングを用意する
    // 同じクリップ（複数の場合はまとめて１つのクリップ）を使う場合は１度だけ設定する
    csmInt32 usingClipCount = 0;
    for (csmUint32 clipIndex = 0; clipIndex < _clippingContextListForMask.GetSize(); clipIndex++)
    {
        // １つのクリッピングマスクに関して
        CubismClippingContext* cc = _clippingContextListForMask[clipIndex];

        // このクリップを利用する描画オブジェクト群全体を囲む矩形を計算
        CalcClippedDrawTotalBounds(model, cc);

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
            renderer->GetCommandBuffer()->Viewport(0, 0, renderer->_offscreenFrameBuffer.GetViewPortSize().Width, renderer->_offscreenFrameBuffer.GetViewPortSize().Height);

            // モデル描画時にDrawMeshNowに渡される変換（モデルtoワールド座標変換）
            CubismMatrix44 modelToWorldF = renderer->GetMvpMatrix();

            renderer->PreDraw(); // バッファをクリアする

            // _offscreenFrameBufferへ切り替え
            renderer->_offscreenFrameBuffer.BeginDraw(renderer->GetCommandBuffer(), lastColorBuffer);

            // マスクをクリアする
            // 1が無効（描かれない）領域、0が有効（描かれる）領域。（シェーダで Cd*Csで0に近い値をかけてマスクを作る。1をかけると何も起こらない）
            renderer->_offscreenFrameBuffer.Clear(renderer->GetCommandBuffer(), 1.0f, 1.0f, 1.0f, 1.0f);
        }

        // 各マスクのレイアウトを決定していく
        SetupLayoutBounds(renderer->IsUsingHighPrecisionMask() ? 0 : usingClipCount);

        // 実際にマスクを生成する
        // 全てのマスクをどの様にレイアウトして描くかを決定し、ClipContext , ClippedDrawContext に記憶する
        for (csmUint32 clipIndex = 0; clipIndex < _clippingContextListForMask.GetSize(); clipIndex++)
        {
            // --- 実際に１つのマスクを描く ---
            CubismClippingContext* clipContext = _clippingContextListForMask[clipIndex];
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
                    _tmpMatrix.TranslateRelative(layoutBoundsOnTex01->X, layoutBoundsOnTex01->Y); //new = [translate]
                    _tmpMatrix.ScaleRelative(scaleX, scaleY); //new = [translate][scale]
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

            if (!renderer->IsUsingHighPrecisionMask())
            {
                const csmInt32 clipDrawCount = clipContext->_clippingIdCount;
                for (csmInt32 i = 0; i < clipDrawCount; i++)
                {
                    const csmInt32 clipDrawIndex = clipContext->_clippingIdList[i];
                    CubismCommandBuffer_Cocos2dx::DrawCommandBuffer* drawCommandBufferData = clipContext->_clippingCommandBufferList->At(i);// [i];


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
                        model.GetDrawableTextureIndices(clipDrawIndex),
                        model.GetDrawableVertexIndexCount(clipDrawIndex),
                        model.GetDrawableVertexCount(clipDrawIndex),
                        const_cast<csmUint16*>(model.GetDrawableVertexIndices(clipDrawIndex)),
                        const_cast<csmFloat32*>(model.GetDrawableVertices(clipDrawIndex)),
                        reinterpret_cast<csmFloat32*>(const_cast<Core::csmVector2*>(model.GetDrawableVertexUvs(clipDrawIndex))),
                        model.GetDrawableOpacity(clipDrawIndex),
                        CubismRenderer::CubismBlendMode_Normal,   //クリッピングは通常描画を強制
                        false   // マスク生成時はクリッピングの反転使用は全く関係がない
                    );
                }
            }
        }

        if (!renderer->IsUsingHighPrecisionMask())
        {
            // --- 後処理 ---
            renderer->_offscreenFrameBuffer.EndDraw(renderer->GetCommandBuffer()); // 描画対象を戻す
            renderer->SetClippingContextBufferForMask(NULL);
            renderer->GetCommandBuffer()->Viewport(lastViewport.X, lastViewport.Y, lastViewport.Width, lastViewport.Height);
        }
    }
}

void CubismClippingManager_Cocos2dx::CalcClippedDrawTotalBounds(CubismModel& model, CubismClippingContext* clippingContext)
{
    // 被クリッピングマスク（マスクされる描画オブジェクト）の全体の矩形
    csmFloat32 clippedDrawTotalMinX = FLT_MAX, clippedDrawTotalMinY = FLT_MAX;
    csmFloat32 clippedDrawTotalMaxX = -FLT_MAX, clippedDrawTotalMaxY = -FLT_MAX;

    // このマスクが実際に必要か判定する
    // このクリッピングを利用する「描画オブジェクト」がひとつでも使用可能であればマスクを生成する必要がある

    const csmInt32 clippedDrawCount = clippingContext->_clippedDrawableIndexList->GetSize();
    for (csmInt32 clippedDrawableIndex = 0; clippedDrawableIndex < clippedDrawCount; clippedDrawableIndex++)
    {
        // マスクを使用する描画オブジェクトの描画される矩形を求める
        const csmInt32 drawableIndex = (*clippingContext->_clippedDrawableIndexList)[clippedDrawableIndex];

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

void CubismClippingManager_Cocos2dx::SetupLayoutBounds(csmInt32 usingClipCount) const
{
    if (usingClipCount <= 0)
    {// この場合は一つのマスクターゲットを毎回クリアして使用する
        for (csmUint32 index = 0; index < _clippingContextListForMask.GetSize(); index++)
        {
            CubismClippingContext* cc = _clippingContextListForMask[index];
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
            CubismClippingContext* cc = _clippingContextListForMask[curClipIndex++];
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

                CubismClippingContext* cc = _clippingContextListForMask[curClipIndex++];
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

                CubismClippingContext* cc = _clippingContextListForMask[curClipIndex++];
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

                CubismClippingContext* cc = _clippingContextListForMask[curClipIndex++];
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
                CubismClippingContext* cc = _clippingContextListForMask[curClipIndex++];
                cc->_layoutChannelNo = 0;
                cc->_layoutBounds->X = 0.0f;
                cc->_layoutBounds->Y = 0.0f;
                cc->_layoutBounds->Width = 1.0f;
                cc->_layoutBounds->Height = 1.0f;
            }
        }
    }
}

CubismRenderer::CubismTextureColor* CubismClippingManager_Cocos2dx::GetChannelFlagAsColor(csmInt32 channelNo)
{
    return _channelColors[channelNo];
}

csmVector<CubismClippingContext*>* CubismClippingManager_Cocos2dx::GetClippingContextListForDraw()
{
    return &_clippingContextListForDraw;
}

void CubismClippingManager_Cocos2dx::SetClippingMaskBufferSize(csmFloat32 width, csmFloat32 height)
{
    _clippingMaskBufferSize = CubismVector2(width, height);
}

CubismVector2 CubismClippingManager_Cocos2dx::GetClippingMaskBufferSize() const
{
    return _clippingMaskBufferSize;
}

/*********************************************************************************************************************
*                                      CubismClippingContext
********************************************************************************************************************/
CubismClippingContext::CubismClippingContext(CubismClippingManager_Cocos2dx* manager, CubismModel& model, const csmInt32* clippingDrawableIndices, csmInt32 clipCount)
{
    _owner = manager;

    // クリップしている（＝マスク用の）Drawableのインデックスリスト
    _clippingIdList = clippingDrawableIndices;

    // マスクの数
    _clippingIdCount = clipCount;

    _layoutChannelNo = 0;

    _allClippedDrawRect = CSM_NEW csmRectF();
    _layoutBounds = CSM_NEW csmRectF();

    _clippedDrawableIndexList = CSM_NEW csmVector<csmInt32>();
    _clippingCommandBufferList = CSM_NEW csmVector<CubismCommandBuffer_Cocos2dx::DrawCommandBuffer*>;


    for (csmUint32 i = 0; i < _clippingIdCount; ++i)
    {
        const csmInt32 clippingId = _clippingIdList[i];
        CubismCommandBuffer_Cocos2dx::DrawCommandBuffer* drawCommandBuffer = NULL;
        const csmInt32 drawableVertexCount = model.GetDrawableVertexCount(clippingId);
        const csmInt32 drawableVertexIndexCount = model.GetDrawableVertexIndexCount(clippingId);
        const csmSizeInt vertexSize = sizeof(csmFloat32) * 2;


        drawCommandBuffer = CSM_NEW CubismCommandBuffer_Cocos2dx::DrawCommandBuffer();
        drawCommandBuffer->GetCommandDraw()->GetCommand()->setDrawType(ax::CustomCommand::DrawType::ELEMENT);
        drawCommandBuffer->GetCommandDraw()->GetCommand()->setPrimitiveType(ax::backend::PrimitiveType::TRIANGLE);
        drawCommandBuffer->CreateVertexBuffer(vertexSize, drawableVertexCount * 2);      // Vertices + UVs
        drawCommandBuffer->CreateIndexBuffer(drawableVertexIndexCount);


        _clippingCommandBufferList->PushBack(drawCommandBuffer);
    }
}

CubismClippingContext::~CubismClippingContext()
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

void CubismClippingContext::AddClippedDrawable(csmInt32 drawableIndex)
{

    _clippedDrawableIndexList->PushBack(drawableIndex);
}

CubismClippingManager_Cocos2dx* CubismClippingContext::GetClippingManager()
{
    return _owner;
}



/*********************************************************************************************************************
*                                      CubismDrawProfile_OpenGL
********************************************************************************************************************/
void CubismRendererProfile_Cocos2dx::Save()
{
    //-- push state --
    _lastScissorTest = GetCocos2dRenderer()->getScissorTest();
    _lastStencilTest = GetCocos2dRenderer()->getStencilTest();
    _lastDepthTest = GetCocos2dRenderer()->getDepthTest();
    _lastCullFace = GetCocos2dRenderer()->getCullMode();
    _lastWinding = GetCocos2dRenderer()->getWinding();


    // モデル描画直前のFBOとビューポートを保存
    // _lastColorBuffer = GetCocos2dRenderer()->getRenderTargetAttachment(backend::TargetBufferFlags::COLOR);
    // _lastDepthBuffer      = GetCocos2dRenderer()->getRenderTargetAttachment(backend::TargetBufferFlags::DEPTH);
    // _lastStencilBuffer    = GetCocos2dRenderer()->getRenderTargetAttachment(backend::TargetBufferFlags::STENCIL);
    // _lastRenderTargetFlag = GetCocos2dRenderer()->getRenderTargetFlag();
    // _lastColorBuffer  = nullptr; // unused
    _lastRenderTarget = GetCocos2dRenderer()->getRenderTarget();
	//_lastRenderTargetFlag = _lastRenderTarget->getTargetFlags();
	_lastColorBuffer  = _lastRenderTarget->_color[0].texture;
	_lastDepthBuffer = _lastRenderTarget->_depth.texture;
	_lastStencilBuffer = _lastRenderTarget->_stencil.texture;
    _lastViewport = csmRectF(GetCocos2dRenderer()->getViewport().x, GetCocos2dRenderer()->getViewport().y, GetCocos2dRenderer()->getViewport().w, GetCocos2dRenderer()->getViewport().h);
}

void CubismRendererProfile_Cocos2dx::Restore()
{
    GetCocos2dRenderer()->setScissorTest(_lastScissorTest);
    GetCocos2dRenderer()->setStencilTest(_lastStencilTest);
    GetCocos2dRenderer()->setDepthTest(_lastDepthTest);
    GetCocos2dRenderer()->setCullMode(_lastCullFace);
    GetCocos2dRenderer()->setWinding(_lastWinding);

    if (_lastRenderTarget)
    {
        // GetCocos2dRenderer()->setRenderTarget(
        //     _lastRenderTarget);  // GetCocos2dRenderer()->setRenderTargetParams(_lastRenderTargetFlag, _lastColorBuffer,
        //                          // _lastDepthBuffer, _lastStencilBuffer);

        CallbackCommand* cmd = GetCocos2dRenderer()->nextCallbackCommand();
        cmd->init(0);

        auto restoringRT = _lastRenderTarget;
		restoringRT->setColorAttachment(_lastColorBuffer);
		restoringRT->setDepthAttachment(_lastDepthBuffer);
        restoringRT->setStencilAttachment(_lastStencilBuffer);
		// restoringRT->setTargetFlags(_lastRenderTargetFlag);
        cmd->func        = [=]() -> void {
            GetCocos2dRenderer()->setRenderTarget(restoringRT);
        };
        // AddCommand([=]() -> void { GetCocos2dRenderer()->setRenderTarget(_lastRenderTarget); });
    }
    GetCocos2dRenderer()->setViewPort(_lastViewport.X, _lastViewport.Y, _lastViewport.Width, _lastViewport.Height);
}


/*********************************************************************************************************************
*                                       CubismShader_Cocos2dx
********************************************************************************************************************/
namespace {
    const csmInt32 ShaderCount = 19; ///< シェーダの数 = マスク生成用 + (通常 + 加算 + 乗算) * (マスク無 + マスク有 + マスク有反転 + マスク無の乗算済アルファ対応版 + マスク有の乗算済アルファ対応版 + マスク有反転の乗算済アルファ対応版)
    CubismShader_Cocos2dx* s_instance;
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
};

void CubismShader_Cocos2dx::ReleaseShaderProgram()
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

//----- フラグメントシェーダプログラム -----
// Normal & Add & Mult 共通
static const csmChar* FragShaderSrc = "custom/live2d_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcTegra = "custom/live2d_tegra_fs";
#endif

// Normal & Add & Mult 共通 （PremultipliedAlpha）
static const csmChar* FragShaderSrcPremultipliedAlpha = "custom/live2d_premultiplied_alpha_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcPremultipliedAlphaTegra = "custom/live2d_premultiplied_alpha_tegra_fs";
#endif

// Normal & Add & Mult 共通（クリッピングされたものの描画用）
static const csmChar* FragShaderSrcMask = "custom/live2d_mask_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcMaskTegra = "custom/live2d_mask_tegra_fs";
#endif

// Normal & Add & Mult 共通（クリッピングされて反転使用の描画用）
static const csmChar* FragShaderSrcMaskInverted = "custom/live2d_mask_inverted_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcMaskInvertedTegra = "custom/live2d_mask_inverted_tegra_fs";
#endif

// Normal & Add & Mult 共通（クリッピングされたものの描画用、PremultipliedAlphaの場合）
static const csmChar* FragShaderSrcMaskPremultipliedAlpha = "custom/live2d_mask_premultiplied_alpha_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcMaskPremultipliedAlphaTegra = "custom/live2d_mask_premultiplied_alpha_tegra_fs";
#endif

// Normal & Add & Mult 共通（クリッピングされて反転使用の描画用、PremultipliedAlphaの場合）
static const csmChar* FragShaderSrcMaskInvertedPremultipliedAlpha =
    "custom/live2d_mask_inverted_premultiplied_alpha_fs";
#if AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID
static const csmChar* FragShaderSrcMaskInvertedPremultipliedAlphaTegra = "custom/live2d_mask_inverted_premultiplied_alpha_tegra_fs";
#endif

CubismShader_Cocos2dx::CubismShader_Cocos2dx()
{ }

CubismShader_Cocos2dx::~CubismShader_Cocos2dx()
{
    ReleaseShaderProgram();
}

CubismShader_Cocos2dx* CubismShader_Cocos2dx::GetInstance()
{
    if (s_instance == NULL)
    {
        s_instance = CSM_NEW CubismShader_Cocos2dx();
    }
    return s_instance;
}

void CubismShader_Cocos2dx::DeleteInstance()
{
    if (s_instance)
    {
        CSM_DELETE_SELF(CubismShader_Cocos2dx, s_instance);
        s_instance = NULL;
    }
}

#ifdef CSM_TARGET_ANDROID_ES2
csmBool CubismShader_Cocos2dx::s_extMode = false;
csmBool CubismShader_Cocos2dx::s_extPAMode = false;
void CubismShader_Cocos2dx::SetExtShaderMode(csmBool extMode, csmBool extPAMode) {
    s_extMode = extMode;
    s_extPAMode = extPAMode;
}
#endif

void CubismShader_Cocos2dx::GenerateShaders()
{
    for (csmInt32 i = 0; i < ShaderCount; i++)
    {
        _shaderSets.PushBack(CSM_NEW CubismShaderSet());
    }

#ifdef CSM_TARGET_ANDROID_ES2
    if (s_extMode)
    {
        _shaderSets[0]->ShaderProgram = LoadShaderProgram(VertShaderSrcSetupMask, FragShaderSrcSetupMaskTegra);

        _shaderSets[1]->ShaderProgram = LoadShaderProgram(VertShaderSrc, FragShaderSrcTegra);
        _shaderSets[2]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMaskTegra);
        _shaderSets[3]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMaskInvertedTegra);
        _shaderSets[4]->ShaderProgram = LoadShaderProgram(VertShaderSrc, FragShaderSrcPremultipliedAlphaTegra);
        _shaderSets[5]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMaskPremultipliedAlphaTegra);
        _shaderSets[6]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMaskInvertedPremultipliedAlphaTegra);
    }
    else
    {
        _shaderSets[0]->ShaderProgram = LoadShaderProgram(VertShaderSrcSetupMask, FragShaderSrcSetupMask);

        _shaderSets[1]->ShaderProgram = LoadShaderProgram(VertShaderSrc, FragShaderSrc);
        _shaderSets[2]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMask);
        _shaderSets[3]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMaskInverted);
        _shaderSets[4]->ShaderProgram = LoadShaderProgram(VertShaderSrc, FragShaderSrcPremultipliedAlpha);
        _shaderSets[5]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMaskPremultipliedAlpha);
        _shaderSets[6]->ShaderProgram = LoadShaderProgram(VertShaderSrcMasked, FragShaderSrcMaskInvertedPremultipliedAlpha);
    }

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

#else

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
#endif

    // SetupMask
    _shaderSets[0]->AttributePositionLocation = _shaderSets[0]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[0]->AttributeTexCoordLocation = _shaderSets[0]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[0]->SamplerTexture0Location = _shaderSets[0]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[0]->UniformClipMatrixLocation = _shaderSets[0]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[0]->UnifromChannelFlagLocation = _shaderSets[0]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[0]->UniformBaseColorLocation = _shaderSets[0]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常
    _shaderSets[1]->AttributePositionLocation = _shaderSets[1]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[1]->AttributeTexCoordLocation = _shaderSets[1]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[1]->SamplerTexture0Location = _shaderSets[1]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[1]->UniformMatrixLocation = _shaderSets[1]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[1]->UniformBaseColorLocation = _shaderSets[1]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常（クリッピング）
    _shaderSets[2]->AttributePositionLocation = _shaderSets[2]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[2]->AttributeTexCoordLocation = _shaderSets[2]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[2]->SamplerTexture0Location = _shaderSets[2]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[2]->SamplerTexture1Location = _shaderSets[2]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[2]->UniformMatrixLocation = _shaderSets[2]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[2]->UniformClipMatrixLocation = _shaderSets[2]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[2]->UnifromChannelFlagLocation = _shaderSets[2]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[2]->UniformBaseColorLocation = _shaderSets[2]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常（クリッピング・反転）
    _shaderSets[3]->AttributePositionLocation = _shaderSets[3]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[3]->AttributeTexCoordLocation = _shaderSets[3]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[3]->SamplerTexture0Location = _shaderSets[3]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[3]->SamplerTexture1Location = _shaderSets[3]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[3]->UniformMatrixLocation = _shaderSets[3]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[3]->UniformClipMatrixLocation = _shaderSets[3]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[3]->UnifromChannelFlagLocation = _shaderSets[3]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[3]->UniformBaseColorLocation = _shaderSets[3]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常（PremultipliedAlpha）
    _shaderSets[4]->AttributePositionLocation = _shaderSets[4]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[4]->AttributeTexCoordLocation = _shaderSets[4]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[4]->SamplerTexture0Location = _shaderSets[4]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[4]->UniformMatrixLocation = _shaderSets[4]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[4]->UniformBaseColorLocation = _shaderSets[4]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常（クリッピング、PremultipliedAlpha）
    _shaderSets[5]->AttributePositionLocation = _shaderSets[5]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[5]->AttributeTexCoordLocation = _shaderSets[5]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[5]->SamplerTexture0Location = _shaderSets[5]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[5]->SamplerTexture1Location = _shaderSets[5]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[5]->UniformMatrixLocation = _shaderSets[5]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[5]->UniformClipMatrixLocation = _shaderSets[5]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[5]->UnifromChannelFlagLocation = _shaderSets[5]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[5]->UniformBaseColorLocation = _shaderSets[5]->ShaderProgram->getUniformLocation("u_baseColor");

    // 通常（クリッピング・反転、PremultipliedAlpha）
    _shaderSets[6]->AttributePositionLocation = _shaderSets[6]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[6]->AttributeTexCoordLocation = _shaderSets[6]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[6]->SamplerTexture0Location = _shaderSets[6]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[6]->SamplerTexture1Location = _shaderSets[6]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[6]->UniformMatrixLocation = _shaderSets[6]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[6]->UniformClipMatrixLocation = _shaderSets[6]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[6]->UnifromChannelFlagLocation = _shaderSets[6]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[6]->UniformBaseColorLocation = _shaderSets[6]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算
    _shaderSets[7]->AttributePositionLocation = _shaderSets[7]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[7]->AttributeTexCoordLocation = _shaderSets[7]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[7]->SamplerTexture0Location = _shaderSets[7]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[7]->UniformMatrixLocation = _shaderSets[7]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[7]->UniformBaseColorLocation = _shaderSets[7]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算（クリッピング）
    _shaderSets[8]->AttributePositionLocation = _shaderSets[8]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[8]->AttributeTexCoordLocation = _shaderSets[8]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[8]->SamplerTexture0Location = _shaderSets[8]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[8]->SamplerTexture1Location = _shaderSets[8]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[8]->UniformMatrixLocation = _shaderSets[8]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[8]->UniformClipMatrixLocation = _shaderSets[8]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[8]->UnifromChannelFlagLocation = _shaderSets[8]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[8]->UniformBaseColorLocation = _shaderSets[8]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算（クリッピング・反転）
    _shaderSets[9]->AttributePositionLocation = _shaderSets[9]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[9]->AttributeTexCoordLocation = _shaderSets[9]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[9]->SamplerTexture0Location = _shaderSets[9]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[9]->SamplerTexture1Location = _shaderSets[9]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[9]->UniformMatrixLocation = _shaderSets[9]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[9]->UniformClipMatrixLocation = _shaderSets[9]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[9]->UnifromChannelFlagLocation = _shaderSets[9]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[9]->UniformBaseColorLocation = _shaderSets[9]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算（PremultipliedAlpha）
    _shaderSets[10]->AttributePositionLocation = _shaderSets[10]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[10]->AttributeTexCoordLocation = _shaderSets[10]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[10]->SamplerTexture0Location = _shaderSets[10]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[10]->UniformMatrixLocation = _shaderSets[10]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[10]->UniformBaseColorLocation = _shaderSets[10]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算（クリッピング、PremultipliedAlpha）
    _shaderSets[11]->AttributePositionLocation = _shaderSets[11]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[11]->AttributeTexCoordLocation = _shaderSets[11]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[11]->SamplerTexture0Location = _shaderSets[11]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[11]->SamplerTexture1Location = _shaderSets[11]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[11]->UniformMatrixLocation = _shaderSets[11]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[11]->UniformClipMatrixLocation = _shaderSets[11]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[11]->UnifromChannelFlagLocation = _shaderSets[11]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[11]->UniformBaseColorLocation = _shaderSets[11]->ShaderProgram->getUniformLocation("u_baseColor");

    // 加算（クリッピング・反転、PremultipliedAlpha）
    _shaderSets[12]->AttributePositionLocation = _shaderSets[12]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[12]->AttributeTexCoordLocation = _shaderSets[12]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[12]->SamplerTexture0Location = _shaderSets[12]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[12]->SamplerTexture1Location = _shaderSets[12]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[12]->UniformMatrixLocation = _shaderSets[12]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[12]->UniformClipMatrixLocation = _shaderSets[12]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[12]->UnifromChannelFlagLocation = _shaderSets[12]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[12]->UniformBaseColorLocation = _shaderSets[12]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算
    _shaderSets[13]->AttributePositionLocation = _shaderSets[13]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[13]->AttributeTexCoordLocation = _shaderSets[13]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[13]->SamplerTexture0Location = _shaderSets[13]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[13]->UniformMatrixLocation = _shaderSets[13]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[13]->UniformBaseColorLocation = _shaderSets[13]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算（クリッピング）
    _shaderSets[14]->AttributePositionLocation = _shaderSets[14]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[14]->AttributeTexCoordLocation = _shaderSets[14]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[14]->SamplerTexture0Location = _shaderSets[14]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[14]->SamplerTexture1Location = _shaderSets[14]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[14]->UniformMatrixLocation = _shaderSets[14]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[14]->UniformClipMatrixLocation = _shaderSets[14]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[14]->UnifromChannelFlagLocation = _shaderSets[14]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[14]->UniformBaseColorLocation = _shaderSets[14]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算（クリッピング・反転）
    _shaderSets[15]->AttributePositionLocation = _shaderSets[15]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[15]->AttributeTexCoordLocation = _shaderSets[15]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[15]->SamplerTexture0Location = _shaderSets[15]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[15]->SamplerTexture1Location = _shaderSets[15]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[15]->UniformMatrixLocation = _shaderSets[15]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[15]->UniformClipMatrixLocation = _shaderSets[15]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[15]->UnifromChannelFlagLocation = _shaderSets[15]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[15]->UniformBaseColorLocation = _shaderSets[15]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算（PremultipliedAlpha）
    _shaderSets[16]->AttributePositionLocation = _shaderSets[16]->ShaderProgram->getAttributeLocation( "a_position");
    _shaderSets[16]->AttributeTexCoordLocation = _shaderSets[16]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[16]->SamplerTexture0Location = _shaderSets[16]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[16]->UniformMatrixLocation = _shaderSets[16]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[16]->UniformBaseColorLocation = _shaderSets[16]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算（クリッピング、PremultipliedAlpha）
    _shaderSets[17]->AttributePositionLocation = _shaderSets[17]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[17]->AttributeTexCoordLocation = _shaderSets[17]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[17]->SamplerTexture0Location = _shaderSets[17]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[17]->SamplerTexture1Location = _shaderSets[17]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[17]->UniformMatrixLocation = _shaderSets[17]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[17]->UniformClipMatrixLocation = _shaderSets[17]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[17]->UnifromChannelFlagLocation = _shaderSets[17]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[17]->UniformBaseColorLocation = _shaderSets[17]->ShaderProgram->getUniformLocation("u_baseColor");

    // 乗算（クリッピング・反転、PremultipliedAlpha）
    _shaderSets[18]->AttributePositionLocation = _shaderSets[18]->ShaderProgram->getAttributeLocation("a_position");
    _shaderSets[18]->AttributeTexCoordLocation = _shaderSets[18]->ShaderProgram->getAttributeLocation("a_texCoord");
    _shaderSets[18]->SamplerTexture0Location = _shaderSets[18]->ShaderProgram->getUniformLocation("u_tex0");
    _shaderSets[18]->SamplerTexture1Location = _shaderSets[18]->ShaderProgram->getUniformLocation("u_tex1");
    _shaderSets[18]->UniformMatrixLocation = _shaderSets[18]->ShaderProgram->getUniformLocation("u_matrix");
    _shaderSets[18]->UniformClipMatrixLocation = _shaderSets[18]->ShaderProgram->getUniformLocation("u_clipMatrix");
    _shaderSets[18]->UnifromChannelFlagLocation = _shaderSets[18]->ShaderProgram->getUniformLocation("u_channelFlag");
    _shaderSets[18]->UniformBaseColorLocation = _shaderSets[18]->ShaderProgram->getUniformLocation("u_baseColor");
}

void CubismShader_Cocos2dx::SetupShaderProgram(CubismCommandBuffer_Cocos2dx::DrawCommandBuffer::DrawCommand* drawCommand, CubismRenderer_Cocos2dx* renderer, ax::Texture2D* texture
                                                , csmInt32 vertexCount, csmFloat32* vertexArray
                                                , csmFloat32* uvArray, csmFloat32 opacity
                                                , CubismRenderer::CubismBlendMode colorBlendMode
                                                , CubismRenderer::CubismTextureColor baseColor
                                                , csmBool isPremultipliedAlpha, CubismMatrix44 matrix4x4
                                                , csmBool invertedMask)
{
    if (_shaderSets.GetSize() == 0)
    {
        GenerateShaders();
    }

    ax::backend::BlendDescriptor* blendDescriptor = drawCommand->GetBlendDescriptor();
    ax::PipelineDescriptor* pipelineDescriptor = drawCommand->GetPipelineDescriptor();

    ax::backend::ProgramState* programState = pipelineDescriptor->programState;
    VertexLayout* layout                    = nullptr;

    if (renderer->GetClippingContextBufferForMask() != NULL) // マスク生成時
    {
        CubismShaderSet* shaderSet = _shaderSets[ShaderNames_SetupMask];

        if (!programState)
        {
            programState = new ax::backend::ProgramState(shaderSet->ShaderProgram);
        }

        layout = programState->getMutableVertexLayout();

        //テクスチャ設定
        programState->setTexture(shaderSet->SamplerTexture0Location, 0, texture->getBackendTexture());

        // 頂点配列の設定
        layout->setAttrib("a_position", shaderSet->AttributePositionLocation, ax::backend::VertexFormat::FLOAT2, 0, false);
        // テクスチャ頂点の設定
        layout->setAttrib("a_texCoord", shaderSet->AttributeTexCoordLocation, ax::backend::VertexFormat::FLOAT2,
                          sizeof(csmFloat32) * 2, false);

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

        blendDescriptor->sourceRGBBlendFactor = ax::backend::BlendFactor::ZERO;
        blendDescriptor->destinationRGBBlendFactor = ax::backend::BlendFactor::ONE_MINUS_SRC_COLOR;
        blendDescriptor->sourceAlphaBlendFactor = ax::backend::BlendFactor::ZERO;
        blendDescriptor->destinationAlphaBlendFactor = ax::backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
    }
    else // マスク生成以外の場合
    {
        const csmBool masked = renderer->GetClippingContextBufferForDraw() != NULL;  // この描画オブジェクトはマスク対象か
        const csmInt32 offset = (masked ? ( invertedMask ? 2 : 1 ) : 0) + (isPremultipliedAlpha ? 3 : 0);

        CubismShaderSet* shaderSet;
        switch (colorBlendMode)
        {
        case CubismRenderer::CubismBlendMode_Normal:
        default:
            shaderSet = _shaderSets[ShaderNames_Normal + offset];
            blendDescriptor->sourceRGBBlendFactor = ax::backend::BlendFactor::ONE;
            blendDescriptor->destinationRGBBlendFactor = ax::backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
            blendDescriptor->sourceAlphaBlendFactor = ax::backend::BlendFactor::ONE;
            blendDescriptor->destinationAlphaBlendFactor = ax::backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
            break;

        case CubismRenderer::CubismBlendMode_Additive:
            shaderSet = _shaderSets[ShaderNames_Add + offset];
            blendDescriptor->sourceRGBBlendFactor = ax::backend::BlendFactor::ONE;
            blendDescriptor->destinationRGBBlendFactor = ax::backend::BlendFactor::ONE;
            blendDescriptor->sourceAlphaBlendFactor = ax::backend::BlendFactor::ZERO;
            blendDescriptor->destinationAlphaBlendFactor = ax::backend::BlendFactor::ONE;
            break;

        case CubismRenderer::CubismBlendMode_Multiplicative:
            shaderSet = _shaderSets[ShaderNames_Mult + offset];
            blendDescriptor->sourceRGBBlendFactor = ax::backend::BlendFactor::DST_COLOR;
            blendDescriptor->destinationRGBBlendFactor = ax::backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
            blendDescriptor->sourceAlphaBlendFactor = ax::backend::BlendFactor::ZERO;
            blendDescriptor->destinationAlphaBlendFactor = ax::backend::BlendFactor::ONE;
            break;
        }

        if (!programState)
        {
            programState = new ax::backend::ProgramState(shaderSet->ShaderProgram);
        }
        layout = programState->getMutableVertexLayout();

        // 頂点配列の設定
        layout->setAttrib("a_position", shaderSet->AttributePositionLocation, ax::backend::VertexFormat::FLOAT2, 0,
                          false);
        // テクスチャ頂点の設定
        layout->setAttrib("a_texCoord", shaderSet->AttributeTexCoordLocation, ax::backend::VertexFormat::FLOAT2,
                          sizeof(csmFloat32) * 2, false);

        if (masked)
        {
            // frameBufferに書かれたテクスチャ
            ax::Texture2D* tex = renderer->_offscreenFrameBuffer.GetColorBuffer();

            programState->setTexture(shaderSet->SamplerTexture1Location, 1, tex->getBackendTexture());

            // View座標をClippingContextの座標に変換するための行列を設定
            programState->setUniform(shaderSet->UniformClipMatrixLocation,
                                     renderer->GetClippingContextBufferForDraw()->_matrixForDraw.GetArray(),
                                     sizeof(float) * 16);

            // 使用するカラーチャンネルを設定
            const csmInt32 channelNo = renderer->GetClippingContextBufferForDraw()->_layoutChannelNo;
            CubismRenderer::CubismTextureColor* colorChannel = renderer->GetClippingContextBufferForDraw()->GetClippingManager()->GetChannelFlagAsColor(channelNo);
            csmFloat32 colorFlag[4] = { colorChannel->R, colorChannel->G, colorChannel->B, colorChannel->A };
            programState->setUniform(shaderSet->UnifromChannelFlagLocation, colorFlag, sizeof(float) * 4);
        }

        //テクスチャ設定
        programState->setTexture(shaderSet->SamplerTexture0Location, 0, texture->getBackendTexture());

        //座標変換
        programState->setUniform(shaderSet->UniformMatrixLocation, matrix4x4.GetArray(), sizeof(float) * 16);

        csmFloat32 base[4] = { baseColor.R, baseColor.G, baseColor.B, baseColor.A };
        programState->setUniform(shaderSet->UniformBaseColorLocation, base, sizeof(float) * 4);
    }

    if (layout)
        layout->setStride(sizeof(csmFloat32) * 4);

    blendDescriptor->blendEnabled = true;
    pipelineDescriptor->programState = programState;
}

ax::backend::Program* CubismShader_Cocos2dx::LoadShaderProgram(const csmChar* vertShaderPath,
                                                               const csmChar* fragShaderPath)
{
    // cocos2dx対応
    // Create shader program.
    return ProgramManager::getInstance()->loadProgram(vertShaderPath, fragShaderPath);
}

/*********************************************************************************************************************
 *                                      CubismRenderer_Cocos2dx
 ********************************************************************************************************************/

#ifdef CSM_TARGET_ANDROID_ES2
void CubismRenderer_Cocos2dx::SetExtShaderMode(csmBool extMode, csmBool extPAMode)
{
    CubismShader_Cocos2dx::SetExtShaderMode(extMode, extPAMode);
    CubismShader_Cocos2dx::DeleteInstance();
}

void CubismRenderer_Cocos2dx::ReloadShader()
{
    CubismShader_Cocos2dx::DeleteInstance();
}
#endif

CubismRenderer* CubismRenderer::Create()
{
    return CSM_NEW CubismRenderer_Cocos2dx();
}

void CubismRenderer::StaticRelease()
{
    CubismRenderer_Cocos2dx::DoStaticRelease();
}

namespace
{
    CubismCommandBuffer_Cocos2dx*       _commandBuffer;
}

CubismRenderer_Cocos2dx::CubismRenderer_Cocos2dx() : _clippingManager(NULL)
                                                     , _clippingContextBufferForMask(NULL)
                                                     , _clippingContextBufferForDraw(NULL)
{
    // テクスチャ対応マップの容量を確保しておく.
    _textures.PrepareCapacity(32, true);
}

CubismRenderer_Cocos2dx::~CubismRenderer_Cocos2dx()
{
    CSM_DELETE_SELF(CubismClippingManager_Cocos2dx, _clippingManager);
}

void CubismRenderer_Cocos2dx::DoStaticRelease()
{
#ifdef CSM_TARGET_WINGL
    s_isInitializeGlFunctionsSuccess = false;     ///< 初期化が完了したかどうか。trueなら初期化完了
    s_isFirstInitializeGlFunctions = true;        ///< 最初の初期化実行かどうか。trueなら最初の初期化実行
#endif
    CubismShader_Cocos2dx::DeleteInstance();
}

void CubismRenderer_Cocos2dx::Initialize(CubismModel* model)
{
    if (model->IsUsingMasking())
    {
        _clippingManager = CSM_NEW CubismClippingManager_Cocos2dx();  //クリッピングマスク・バッファ前処理方式を初期化
        _clippingManager->Initialize(
            *model,
            model->GetDrawableCount(),
            model->GetDrawableMasks(),
            model->GetDrawableMaskCounts()
        );

        _offscreenFrameBuffer.CreateOffscreenFrame(_clippingManager->GetClippingMaskBufferSize().X, _clippingManager->GetClippingMaskBufferSize().Y);
    }

    _sortedDrawableIndexList.Resize(model->GetDrawableCount(), 0);

    _drawableDrawCommandBuffer.Resize(model->GetDrawableCount());

    for (csmInt32 i = 0; i < _drawableDrawCommandBuffer.GetSize(); ++i)
    {
        const csmInt32 drawableVertexCount = model->GetDrawableVertexCount(i);
        const csmInt32 drawableVertexIndexCount = model->GetDrawableVertexIndexCount(i);
        const csmSizeInt vertexSize = sizeof(csmFloat32) * 2;

        _drawableDrawCommandBuffer[i] = CSM_NEW CubismCommandBuffer_Cocos2dx::DrawCommandBuffer();
        _drawableDrawCommandBuffer[i]->GetCommandDraw()->GetCommand()->setDrawType(ax::CustomCommand::DrawType::ELEMENT);
        _drawableDrawCommandBuffer[i]->GetCommandDraw()->GetCommand()->setPrimitiveType(ax::backend::PrimitiveType::TRIANGLE);
        _drawableDrawCommandBuffer[i]->CreateVertexBuffer(vertexSize, drawableVertexCount * 2);      // Vertices + UVs

        if (drawableVertexIndexCount > 0)
        {
            _drawableDrawCommandBuffer[i]->CreateIndexBuffer(drawableVertexIndexCount);
        }
    }


    CubismRenderer::Initialize(model);  //親クラスの処理を呼ぶ
}

void CubismRenderer_Cocos2dx::PreDraw()
{
    _commandBuffer->SetOperationEnable(CubismCommandBuffer_Cocos2dx::OperationType_ScissorTest, false);
    _commandBuffer->SetOperationEnable(CubismCommandBuffer_Cocos2dx::OperationType_StencilTest, false);
    _commandBuffer->SetOperationEnable(CubismCommandBuffer_Cocos2dx::OperationType_DepthTest, false);


    //異方性フィルタリング。プラットフォームのOpenGLによっては未対応の場合があるので、未設定のときは設定しない
    if (GetAnisotropy() > 0.0f)
    {
        // Not supported.
    }
}


void CubismRenderer_Cocos2dx::DoDrawModel()
{
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
                static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().X), static_cast<csmUint32>(_clippingManager->GetClippingMaskBufferSize().Y));
        }

        _clippingManager->SetupClippingContext(*GetModel(), this, _rendererProfile._lastColorBuffer, _rendererProfile._lastViewport);
    }

    // 上記クリッピング処理内でも一度PreDrawを呼ぶので注意!!
    PreDraw();

    const csmInt32 drawableCount = GetModel()->GetDrawableCount();
    const csmInt32* renderOrder = GetModel()->GetDrawableRenderOrders();

    // インデックスを描画順でソート
    for (csmInt32 i = 0; i < drawableCount; ++i)
    {
        const csmInt32 order = renderOrder[i];
        _sortedDrawableIndexList[order] = i;
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
    for (csmInt32 i = 0; i < drawableCount; ++i)
    {
        const csmInt32 drawableIndex = _sortedDrawableIndexList[i];

        // Drawableが表示状態でなければ処理をパスする
        if (!GetModel()->GetDrawableDynamicFlagIsVisible(drawableIndex))
        {
            continue;
        }

        // クリッピングマスク
        CubismClippingContext* clipContext = (_clippingManager != NULL)
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
                    CubismCommandBuffer_Cocos2dx::DrawCommandBuffer* drawCommandBufferMask = clipContext->_clippingCommandBufferList->At(index);

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
                        GetModel()->GetDrawableTextureIndices(clipDrawIndex),
                        GetModel()->GetDrawableVertexIndexCount(clipDrawIndex),
                        GetModel()->GetDrawableVertexCount(clipDrawIndex),
                        const_cast<csmUint16*>(GetModel()->GetDrawableVertexIndices(clipDrawIndex)),
                        const_cast<csmFloat32*>(GetModel()->GetDrawableVertices(clipDrawIndex)),
                        reinterpret_cast<csmFloat32*>(const_cast<Core::csmVector2*>(GetModel()->GetDrawableVertexUvs(clipDrawIndex))),
                        GetModel()->GetDrawableOpacity(clipDrawIndex),
                        CubismRenderer::CubismBlendMode_Normal,   //クリッピングは通常描画を強制
                        false // マスク生成時はクリッピングの反転使用は全く関係がない
                    );
                }
            }

            {
                // --- 後処理 ---
                _offscreenFrameBuffer.EndDraw(_commandBuffer);
                SetClippingContextBufferForMask(NULL);
                _commandBuffer->Viewport(_rendererProfile._lastViewport.X, _rendererProfile._lastViewport.Y, _rendererProfile._lastViewport.Width, _rendererProfile._lastViewport.Height);

                PreDraw(); // バッファをクリアする
            }
        }

        CubismCommandBuffer_Cocos2dx::DrawCommandBuffer::DrawCommand* drawCommandDraw = _drawableDrawCommandBuffer[drawableIndex]->GetCommandDraw();

        // クリッピングマスクをセットする
        SetClippingContextBufferForDraw(clipContext);

        IsCulling(GetModel()->GetDrawableCulling(drawableIndex) != 0);

        if (GetModel()->GetDrawableVertexIndexCount(drawableIndex) <= 0)
        {
            continue;
        }

        DrawMeshCocos2d(
            drawCommandDraw,
            GetModel()->GetDrawableTextureIndices(drawableIndex),
            GetModel()->GetDrawableVertexIndexCount(drawableIndex),
            GetModel()->GetDrawableVertexCount(drawableIndex),
            const_cast<csmUint16*>(GetModel()->GetDrawableVertexIndices(drawableIndex)),
            const_cast<csmFloat32*>(GetModel()->GetDrawableVertices(drawableIndex)),
            reinterpret_cast<csmFloat32*>(const_cast<Core::csmVector2*>(GetModel()->GetDrawableVertexUvs(drawableIndex))),
            GetModel()->GetDrawableOpacity(drawableIndex),
            GetModel()->GetDrawableBlendMode(drawableIndex),
            GetModel()->GetDrawableInvertedMask(drawableIndex) // マスクを反転使用するか
        );
    }

    //
    PostDraw();

}

void CubismRenderer_Cocos2dx::DrawMesh(csmInt32 textureNo, csmInt32 indexCount, csmInt32 vertexCount,
    csmUint16* indexArray, csmFloat32* vertexArray, csmFloat32* uvArray, csmFloat32 opacity,
    CubismBlendMode colorBlendMode, csmBool invertedMask)
{
}

void CubismRenderer_Cocos2dx::DrawMeshCocos2d(CubismCommandBuffer_Cocos2dx::DrawCommandBuffer::DrawCommand* drawCommand, csmInt32 textureNo, csmInt32 indexCount, csmInt32 vertexCount
                                        , csmUint16* indexArray, csmFloat32* vertexArray, csmFloat32* uvArray
                                        , csmFloat32 opacity, CubismBlendMode colorBlendMode, csmBool invertedMask)
{
#ifndef CSM_DEBUG
    if (_textures[textureNo] == 0) return;    // モデルが参照するテクスチャがバインドされていない場合は描画をスキップする
#endif

    // 裏面描画の有効・無効
    if (IsCulling())
    {
        _commandBuffer->SetOperationEnable(CubismCommandBuffer_Cocos2dx::OperationType_Culling, true);
    }
    else
    {
        _commandBuffer->SetOperationEnable(CubismCommandBuffer_Cocos2dx::OperationType_Culling, false);
    }

    // Cubism SDK OpenGLはマスク・アートメッシュ共にCCWが表面
    _commandBuffer->SetWindingMode(CubismCommandBuffer_Cocos2dx::WindingType_CounterClockWise);

    CubismTextureColor modelColorRGBA = GetModelColor();

    if (GetClippingContextBufferForMask() == NULL) // マスク生成時以外
    {
        modelColorRGBA.A *= opacity;
        if (IsPremultipliedAlpha())
        {
            modelColorRGBA.R *= modelColorRGBA.A;
            modelColorRGBA.G *= modelColorRGBA.A;
            modelColorRGBA.B *= modelColorRGBA.A;
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


    CubismShader_Cocos2dx::GetInstance()->SetupShaderProgram(
        drawCommand, this, drawTexture, vertexCount, vertexArray, uvArray
        , opacity, colorBlendMode, modelColorRGBA, IsPremultipliedAlpha()
        , GetMvpMatrix(), invertedMask
    );


    // ポリゴンメッシュを描画する
    _commandBuffer->AddDrawCommand(drawCommand);


    // 後処理
    SetClippingContextBufferForDraw(NULL);
    SetClippingContextBufferForMask(NULL);
}

CubismCommandBuffer_Cocos2dx* CubismRenderer_Cocos2dx::GetCommandBuffer()
{
    return _commandBuffer;
}

void CubismRenderer_Cocos2dx::StartFrame(CubismCommandBuffer_Cocos2dx* commandBuffer)
{
    _commandBuffer = commandBuffer;
}

void CubismRenderer_Cocos2dx::EndFrame(CubismCommandBuffer_Cocos2dx* commandBuffer)
{
}

CubismCommandBuffer_Cocos2dx::DrawCommandBuffer* CubismRenderer_Cocos2dx::GetDrawCommandBufferData(csmInt32 drawableIndex)
{
    return _drawableDrawCommandBuffer[drawableIndex];
}

void CubismRenderer_Cocos2dx::SaveProfile()
{
    _rendererProfile.Save();
}

void CubismRenderer_Cocos2dx::RestoreProfile()
{
    _rendererProfile.Restore();
}

void CubismRenderer_Cocos2dx::BindTexture(csmUint32 modelTextureNo, ax::Texture2D* texture)
{
    _textures[modelTextureNo] = texture;
}

const csmMap<csmInt32, ax::Texture2D*>& CubismRenderer_Cocos2dx::GetBindedTextures() const
{
    return _textures;
}

void CubismRenderer_Cocos2dx::SetClippingMaskBufferSize(csmFloat32 width, csmFloat32 height)
{
    //FrameBufferのサイズを変更するためにインスタンスを破棄・再作成する
    CSM_DELETE_SELF(CubismClippingManager_Cocos2dx, _clippingManager);

    _clippingManager = CSM_NEW CubismClippingManager_Cocos2dx();

    _clippingManager->SetClippingMaskBufferSize(width, height);

    _clippingManager->Initialize(
        *GetModel(),
        GetModel()->GetDrawableCount(),
        GetModel()->GetDrawableMasks(),
        GetModel()->GetDrawableMaskCounts()
    );
}

CubismVector2 CubismRenderer_Cocos2dx::GetClippingMaskBufferSize() const
{
    return _clippingManager->GetClippingMaskBufferSize();
}

void CubismRenderer_Cocos2dx::SetClippingContextBufferForMask(CubismClippingContext* clip)
{
    _clippingContextBufferForMask = clip;
}

CubismClippingContext* CubismRenderer_Cocos2dx::GetClippingContextBufferForMask() const
{
    return _clippingContextBufferForMask;
}

void CubismRenderer_Cocos2dx::SetClippingContextBufferForDraw(CubismClippingContext* clip)
{
    _clippingContextBufferForDraw = clip;
}

CubismClippingContext* CubismRenderer_Cocos2dx::GetClippingContextBufferForDraw() const
{
    return _clippingContextBufferForDraw;
}

}}}}

//------------ LIVE2D NAMESPACE ------------
