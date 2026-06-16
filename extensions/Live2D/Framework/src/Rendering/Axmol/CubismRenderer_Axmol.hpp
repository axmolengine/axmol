/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "../CubismRenderer.hpp"
#include "../csmBlendMode.hpp"
#include "CubismFramework.hpp"
#include "CubismOffscreenSurface_Axmol.hpp"
#include "CubismCommandBuffer_Axmol.hpp"
#include "Math/CubismVector2.hpp"
#include "Type/csmVector.hpp"
#include "Type/csmRectF.hpp"
#include "Type/csmMap.hpp"

using namespace ax;

//------------ LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Rendering {

//  前方宣言
class CubismRenderer_Axmol;
class CubismClippingContext_Axmol;

class CubismOffscreenRenderTarget_Axmol
{
public:
    CubismOffscreenRenderTarget_Axmol();
    ~CubismOffscreenRenderTarget_Axmol();

    void SetOffscreenIndex(csmInt32 offscreenIndex);
    csmInt32 GetOffscreenIndex() const;
    CubismOffscreenFrame_Axmol* GetRenderTarget();
    const CubismOffscreenFrame_Axmol* GetRenderTarget() const;
    void SetParentPartOffscreen(CubismOffscreenRenderTarget_Axmol* parent);
    CubismOffscreenRenderTarget_Axmol* GetParentPartOffscreen() const;
    void SetOldOffscreen(CubismOffscreenRenderTarget_Axmol* oldOffscreen);
    CubismOffscreenRenderTarget_Axmol* GetOldOffscreen() const;

private:
    csmInt32 _offscreenIndex;
    CubismOffscreenFrame_Axmol* _renderTarget;
    CubismOffscreenRenderTarget_Axmol* _parentPartOffscreen;
    CubismOffscreenRenderTarget_Axmol* _oldOffscreen;
};

/**
 * @brief  クリッピングマスクの処理を実行するクラス
 *
 */
class CubismClippingManager_Axmol
{
    friend class CubismShader_Axmol;
    friend class CubismRenderer_Axmol;

private:

    /**
     * @brief カラーチャンネル(RGBA)のフラグを取得する
     *
     * @param[in]   channelNo   ->   カラーチャンネル(RGBA)の番号(0:R , 1:G , 2:B, 3:A)
     */
    CubismRenderer::CubismTextureColor* GetChannelFlagAsColor(csmInt32 channelNo);

    /**
     * @brief   マスクされる描画オブジェクト群全体を囲む矩形(モデル座標系)を計算する
     *
     * @param[in]   model            ->  モデルのインスタンス
     * @param[in]   clippingContext  ->  クリッピングマスクのコンテキスト
     */
    void CalcClippedDrawTotalBounds(CubismModel& model, CubismClippingContext_Axmol* clippingContext);
    void CalcClippedDrawTotalBounds(CubismModel& model, CubismClippingContext_Axmol* clippingContext, CubismRenderer::DrawableObjectType drawableObjectType);
    void CollectOffscreenChildDrawableIndexList(CubismModel& model, csmInt32 offscreenIndex, csmVector<csmInt32>& childDrawableIndexList);
    void CollectPartChildDrawableIndexList(CubismModel& model, csmInt32 partIndex, csmVector<csmInt32>& childDrawableIndexList);

    /**
     * @brief    コンストラクタ
     */
    CubismClippingManager_Axmol();

    /**
     * @brief    デストラクタ
     */
    virtual ~CubismClippingManager_Axmol();

    /**
     * @brief    マネージャの初期化処理<br>
     *           クリッピングマスクを使う描画オブジェクトの登録を行う
     *
     * @param[in]   model           ->  モデルのインスタンス
     * @param[in]   drawableCount   ->  描画オブジェクトの数
     * @param[in]   drawableMasks   ->  描画オブジェクトをマスクする描画オブジェクトのインデックスのリスト
     * @param[in]   drawableMaskCounts   ->  描画オブジェクトをマスクする描画オブジェクトの数
     */
    void Initialize(CubismModel& model, csmInt32 drawableCount, const csmInt32** drawableMasks, const csmInt32* drawableMaskCounts);
    void Initialize(CubismModel& model, csmInt32 maskBufferCount, CubismRenderer::DrawableObjectType drawableObjectType);

    /**
     * @brief   クリッピングコンテキストを作成する。モデル描画時に実行する。
     *
     * @param[in]   model        ->  モデルのインスタンス
     * @param[in]   renderer     ->  レンダラのインスタンス
     * @param[in]   lastFBO      ->  フレームバッファ
     * @param[in]   lastViewport ->  ビューポート
     */
    void SetupClippingContext(CubismModel& model,
                              CubismRenderer_Axmol* renderer,
                              rhi::Texture* lastColorBuffer,
                              csmRectF lastViewport,
                              CubismOffscreenFrame_Axmol* maskBuffer,
                              CubismRenderer::DrawableObjectType drawableObjectType,
                              csmBool isRightHanded,
                              const CubismMatrix44& mvp = CubismMatrix44());

    /**
     * @brief   既にマスクを作っているかを確認。<br>
     *          作っているようであれば該当するクリッピングマスクのインスタンスを返す。<br>
     *          作っていなければNULLを返す
     *
     * @param[in]   drawableMasks    ->  描画オブジェクトをマスクする描画オブジェクトのリスト
     * @param[in]   drawableMaskCounts ->  描画オブジェクトをマスクする描画オブジェクトの数
     * @return          該当するクリッピングマスクが存在すればインスタンスを返し、なければNULLを返す。
     */
    CubismClippingContext_Axmol* FindSameClip(const csmInt32* drawableMasks, csmInt32 drawableMaskCounts) const;

    /**
     * @brief   クリッピングコンテキストを配置するレイアウト。<br>
     *           ひとつのレンダーテクスチャを極力いっぱいに使ってマスクをレイアウトする。<br>
     *           マスクグループの数が4以下ならRGBA各チャンネルに１つずつマスクを配置し、5以上6以下ならRGBAを2,2,1,1と配置する。
     *
     * @param[in]   usingClipCount  ->  配置するクリッピングコンテキストの数
     */
    void SetupLayoutBounds(csmInt32 usingClipCount) const;

    /**
     * @brief   画面描画に使用するクリッピングマスクのリストを取得する
     *
     * @return  画面描画に使用するクリッピングマスクのリスト
     */
    csmVector<CubismClippingContext_Axmol*>* GetClippingContextListForDraw();
    csmVector<CubismClippingContext_Axmol*>* GetClippingContextListForOffscreen();

    /**
     *@brief  クリッピングマスクバッファのサイズを設定する
     *
     *@param  size -> クリッピングマスクバッファのサイズ
     *
     */
    void SetClippingMaskBufferSize(csmFloat32 width, csmFloat32 height);

    /**
     *@brief  クリッピングマスクバッファのサイズを取得する
     *
     *@return クリッピングマスクバッファのサイズ
     *
     */
    CubismVector2 GetClippingMaskBufferSize() const;

    csmInt32    _currentFrameNo;         ///< マスクテクスチャに与えるフレーム番号

    csmVector<CubismRenderer::CubismTextureColor*>  _channelColors;
    csmVector<CubismClippingContext_Axmol*>               _clippingContextListForMask;   ///< マスク用クリッピングコンテキストのリスト
    csmVector<CubismClippingContext_Axmol*>               _clippingContextListForDraw;   ///< 描画用クリッピングコンテキストのリスト
    csmVector<CubismClippingContext_Axmol*>               _clippingContextListForOffscreen;   ///< オフスクリーン用クリッピングコンテキストのリスト
    CubismVector2                                   _clippingMaskBufferSize; ///< クリッピングマスクのバッファサイズ（初期値:256）

    CubismMatrix44  _tmpMatrix;              ///< マスク計算用の行列
    CubismMatrix44  _tmpMatrixForMask;       ///< マスク計算用の行列
    CubismMatrix44  _tmpMatrixForDraw;       ///< マスク計算用の行列
    csmRectF        _tmpBoundsOnModel;       ///< マスク配置計算用の矩形

};

/**
 * @brief   クリッピングマスクのコンテキスト
 */
class CubismClippingContext_Axmol
{
    friend class CubismClippingManager_Axmol;
    friend class CubismShader_Axmol;
    friend class CubismRenderer_Axmol;

private:
    /**
     * @brief   引数付きコンストラクタ
     *
     */
    CubismClippingContext_Axmol(CubismClippingManager_Axmol* manager, CubismModel& model, const csmInt32* clippingDrawableIndices, csmInt32 clipCount);

    /**
     * @brief   デストラクタ
     */
    virtual ~CubismClippingContext_Axmol();

    /**
     * @brief   このマスクにクリップされる描画オブジェクトを追加する
     *
     * @param[in]   drawableIndex   ->  クリッピング対象に追加する描画オブジェクトのインデックス
     */
    void AddClippedDrawable(csmInt32 drawableIndex);
    void AddClippedOffscreen(csmInt32 offscreenIndex);

    /**
     * @brief   このマスクを管理するマネージャのインスタンスを取得する。
     *
     * @return  クリッピングマネージャのインスタンス
     */
    CubismClippingManager_Axmol* GetClippingManager();

    csmBool _isUsing;                                ///< 現在の描画状態でマスクの準備が必要ならtrue
    const csmInt32* _clippingIdList;                 ///< クリッピングマスクのIDリスト
    csmInt32 _clippingIdCount;                       ///< クリッピングマスクの数
    csmInt32 _layoutChannelNo;                       ///< RGBAのいずれのチャンネルにこのクリップを配置するか(0:R , 1:G , 2:B , 3:A)
    csmInt32 _bufferIndex;                           ///< このマスクが割り当てられるレンダーテクスチャのインデックス
    csmRectF* _layoutBounds;                         ///< マスク用チャンネルのどの領域にマスクを入れるか(View座標-1..1, UVは0..1に直す)
    csmRectF* _allClippedDrawRect;                   ///< このクリッピングで、クリッピングされる全ての描画オブジェクトの囲み矩形（毎回更新）
    CubismMatrix44 _matrixForMask;                   ///< マスクの位置計算結果を保持する行列
    CubismMatrix44 _matrixForDraw;                   ///< 描画オブジェクトの位置計算結果を保持する行列
    csmVector<csmInt32>* _clippedDrawableIndexList;  ///< このマスクにクリップされる描画オブジェクトのリスト
    csmVector<csmInt32>* _clippedOffscreenIndexList; ///< このマスクにクリップされるオフスクリーンのリスト
    csmVector<CubismCommandBuffer_Axmol::DrawCommandBuffer*>* _clippingCommandBufferList;

    CubismClippingManager_Axmol* _owner;        ///< このマスクを管理しているマネージャのインスタンス
};

/**
 * @brief   Cocos2dx用のシェーダプログラムを生成・破棄するクラス<br>
 *           シングルトンなクラスであり、CubismShader_Axmol::GetInstance()からアクセスする。
 *
 */
class CubismShader_Axmol
{
    friend class CubismRenderer_Axmol;

private:
    /**
     * @brief   インスタンスを取得する（シングルトン）。
     *
     * @return  インスタンスのポインタ
     */
    static CubismShader_Axmol* GetInstance();

    /**
     * @brief   インスタンスを解放する（シングルトン）。
     */
    static void DeleteInstance();

    /**
    * @bref    シェーダープログラムとシェーダ変数のアドレスを保持する構造体
    *
    */
    struct CubismShaderSet
    {
        ax::rhi::Program* ShaderProgram;               ///< シェーダプログラムのアドレス
        const ax::rhi::VertexInputDesc* AttributePositionLocation{nullptr};   ///< シェーダプログラムに渡す変数のアドレス(Position)
        const ax::rhi::VertexInputDesc* AttributeTexCoordLocation{nullptr};   ///< シェーダプログラムに渡す変数のアドレス(TexCoord)
        ax::rhi::UniformLocation UniformMatrixLocation;        ///< シェーダプログラムに渡す変数のアドレス(Matrix)
        ax::rhi::UniformLocation UniformClipMatrixLocation;    ///< シェーダプログラムに渡す変数のアドレス(ClipMatrix)
        ax::rhi::UniformLocation SamplerTexture0Location;      ///< シェーダプログラムに渡す変数のアドレス(Texture0)
        ax::rhi::UniformLocation SamplerTexture1Location;      ///< シェーダプログラムに渡す変数のアドレス(Texture1)
        ax::rhi::UniformLocation SamplerBlendTextureLocation;  ///< シェーダプログラムに渡す変数のアドレス(BlendTexture)
        ax::rhi::UniformLocation UniformBaseColorLocation;     ///< シェーダプログラムに渡す変数のアドレス(BaseColor)
        ax::rhi::UniformLocation UniformMultiplyColorLocation; ///< シェーダプログラムに渡す変数のアドレス(MultiplyColor)
        ax::rhi::UniformLocation UniformScreenColorLocation;   ///< シェーダプログラムに渡す変数のアドレス(ScreenColor)
        ax::rhi::UniformLocation UniformBlendModeLocation;      ///< シェーダプログラムに渡す変数のアドレス(BlendMode)
        ax::rhi::UniformLocation UnifromChannelFlagLocation;   ///< シェーダプログラムに渡す変数のアドレス(ChannelFlag)
    };

    /**
     * @brief   privateなコンストラクタ
     */
    CubismShader_Axmol();

    /**
     * @brief   privateなデストラクタ
     */
    virtual ~CubismShader_Axmol();

    /**
     * @brief   シェーダプログラムの一連のセットアップを実行する
     *
     * @param[in]   renderer              ->  レンダラのインスタンス
     * @param[in]   textureId             ->  GPUのテクスチャID
     * @param[in]   vertexCount           ->  ポリゴンメッシュの頂点数
     * @param[in]   vertexArray           ->  ポリゴンメッシュの頂点配列
     * @param[in]   uvArray               ->  uv配列
     * @param[in]   opacity               ->  不透明度
     * @param[in]   colorBlendMode        ->  カラーブレンディングのタイプ
     * @param[in]   baseColor             ->  ベースカラー
     * @param[in]   isPremultipliedAlpha  ->  乗算済みアルファかどうか
     * @param[in]   matrix4x4             ->  Model-View-Projection行列
     * @param[in]   invertedMask           ->  マスクを反転して使用するフラグ
     */
    void SetupShaderProgram(CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommand, CubismRenderer_Axmol* renderer, ax::Texture2D* texture
                            , csmInt32 vertexCount, csmFloat32* vertexArray
                            , csmFloat32* uvArray, csmFloat32 opacity
                            , CubismRenderer::CubismBlendMode colorBlendMode
                            , CubismRenderer::CubismTextureColor baseColor
                            , CubismRenderer::CubismTextureColor multiplyColor
                            , CubismRenderer::CubismTextureColor screenColor
                            , const Live2D::Cubism::Framework::csmBlendMode& sourceBlendMode
                            , ax::Texture2D* blendTexture
                            , csmBool isPremultipliedAlpha, CubismMatrix44 matrix4x4
                            , csmBool invertedMask);

    void SetupRenderTargetShaderProgram(CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommand,
                                        ax::Texture2D* texture,
                                        CubismRenderer::CubismTextureColor baseColor,
                                        ax::rhi::BlendFactor sourceRGB,
                                        ax::rhi::BlendFactor destinationRGB,
                                        ax::rhi::BlendFactor sourceAlpha,
                                        ax::rhi::BlendFactor destinationAlpha);

    /**
     * @brief   シェーダプログラムを解放する
     */
    void ReleaseShaderProgram();

    /**
     * @brief   シェーダプログラムを初期化する
     */
    void GenerateShaders();

    /**
     * @brief   シェーダプログラムをロードしてアドレス返す。
     *
     * @param[in]   vertShaderSrc   ->  頂点シェーダのソース
     * @param[in]   fragShaderSrc   ->  フラグメントシェーダのソース
     *
     * @return  シェーダプログラムのアドレス
     */
    ax::rhi::Program* LoadShaderProgram(const csmChar* vertShaderPath, const csmChar* fragShaderPath);

    csmVector<CubismShaderSet*> _shaderSets;   ///< ロードしたシェーダプログラムを保持する変数

};

/**
 * @brief   Cubismモデルを描画する直前のCocos2dxのステートを保持・復帰させるクラス
 *
 */
class CubismRendererProfile_Axmol
{
    friend class CubismRenderer_Axmol;

private:
    /**
     * @biref   privateなコンストラクタ
     */
    CubismRendererProfile_Axmol() {};

    /**
     * @biref   privateなデストラクタ
     */
    virtual ~CubismRendererProfile_Axmol() {};

    /**
     * @brief   Cocos2dxのステートを保持する
     */
    void Save();

    /**
     * @brief   保持したCocos2dxのステートを復帰させる
     *
     */
    void Restore();

    csmBool _lastScissorTest;             ///< モデル描画直前のGL_VERTEX_ATTRIB_ARRAY_ENABLEDパラメータ
    csmBool _lastBlend;                   ///< モデル描画直前のGL_SCISSOR_TESTパラメータ
    csmBool _lastStencilTest;             ///< モデル描画直前のGL_STENCIL_TESTパラメータ
    csmBool _lastDepthTest;               ///< モデル描画直前のGL_DEPTH_TESTパラメータ
    ax::CullMode _lastCullFace;                ///< モデル描画直前のGL_CULL_FACEパラメータ
    ax::Winding _lastWinding;
    ax::rhi::Texture* _lastColorBuffer;                         ///< モデル描画直前のフレームバッファ
    ax::rhi::Texture* _lastDepthBuffer;
    ax::rhi::Texture* _lastStencilBuffer;
    //ax::RenderTargetFlag _lastRenderTargetFlag;
    ax::rhi::RenderTarget* _lastRenderTarget;
    csmRectF _lastViewport;                 ///< モデル描画直前のビューポート
};

/**
 * @brief   Cocos2dx用の描画命令を実装したクラス
 *
 */
class CubismRenderer_Axmol : public CubismRenderer
{
    friend class CubismRenderer;
    friend class CubismClippingManager_Axmol;
    friend class CubismShader_Axmol;

public:
    /**
     * @brief    レンダラの初期化処理を実行する<br>
     *           引数に渡したモデルからレンダラの初期化処理に必要な情報を取り出すことができる
     *
     * @param[in]  model -> モデルのインスタンス
     */
    void Initialize(Framework::CubismModel* model) override;
    void Initialize(Framework::CubismModel* model, csmInt32 maskBufferCount) override;

    /**
     * @brief   OpenGLテクスチャのバインド処理<br>
     *           CubismRendererにテクスチャを設定し、CubismRenderer中でその画像を参照するためのIndex値を戻り値とする
     *
     * @param[in]   modelTextureNo  ->  セットするモデルテクスチャの番号
     * @param[in]   texture     ->  バックエンドテクスチャ
     *
     */
    void BindTexture(csmUint32 modelTextureNo, ax::Texture2D* texture);

    /**
     * @brief   OpenGLにバインドされたテクスチャのリストを取得する
     *
     * @return  テクスチャのアドレスのリスト
     */
    const csmMap<csmInt32, ax::Texture2D*>& GetBindedTextures() const;

    /**
     * @brief  クリッピングマスクバッファのサイズを設定する<br>
     *         マスク用のFrameBufferを破棄・再作成するため処理コストは高い。
     *
     * @param[in]  width -> クリッピングマスクバッファの横サイズ
     * @param[in]  height -> クリッピングマスクバッファの縦サイズ
     *
     */
    void SetClippingMaskBufferSize(csmFloat32 width, csmFloat32 height);

    /**
     * @brief  クリッピングマスクバッファのサイズを取得する
     *
     * @return クリッピングマスクバッファのサイズ
     *
     */
    CubismVector2 GetClippingMaskBufferSize() const;


    static CubismCommandBuffer_Axmol* GetCommandBuffer();

    static void StartFrame(CubismCommandBuffer_Axmol* commandBuffer);

    static void EndFrame(CubismCommandBuffer_Axmol* commandBuffer);

protected:
    /**
     * @brief   コンストラクタ
     */
    CubismRenderer_Axmol(csmUint32 width, csmUint32 height);

    /**
     * @brief   デストラクタ
     */
    virtual ~CubismRenderer_Axmol();

    /**
     * @brief   モデルを描画する実際の処理
     *
     */
    void DoDrawModel() override;

    /**
     * @brief   [オーバーライド]<br>
     *           描画オブジェクト（アートメッシュ）を描画する。<br>
     *           ポリゴンメッシュとテクスチャ番号をセットで渡す。
     *
     * @param[in]   textureNo       ->  描画するテクスチャ番号
     * @param[in]   indexCount      ->  描画オブジェクトのインデックス値
     * @param[in]   vertexCount     ->  ポリゴンメッシュの頂点数
     * @param[in]   indexArray      ->  ポリゴンメッシュのインデックス配列
     * @param[in]   vertexArray     ->  ポリゴンメッシュの頂点配列
     * @param[in]   uvArray         ->  uv配列
     * @param[in]   opacity         ->  不透明度
     * @param[in]   colorBlendMode  ->  カラー合成タイプ
     * @param[in]   invertedMask     ->  マスク使用時のマスクの反転使用
     *
     */

    void DrawMesh(csmInt32 textureNo, csmInt32 indexCount, csmInt32 vertexCount, csmUint16* indexArray, csmFloat32* vertexArray, csmFloat32* uvArray, csmFloat32 opacity, CubismBlendMode colorBlendMode, csmBool invertedMask);

    void DrawMeshCocos2d(CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommand, csmInt32 textureNo, csmInt32 indexCount, csmInt32 vertexCount
                  , csmUint16* indexArray, csmFloat32* vertexArray, csmFloat32* uvArray
                  , csmFloat32 opacity, CubismBlendMode colorBlendMode
                  , CubismTextureColor multiplyColor, CubismTextureColor screenColor
                  , const Live2D::Cubism::Framework::csmBlendMode& sourceBlendMode
                  , ax::Texture2D* blendTexture
                  , csmBool invertedMask);

    CubismCommandBuffer_Axmol::DrawCommandBuffer* GetDrawCommandBufferData(csmInt32 drawableIndex);

private:
    // Prevention of copy Constructor
    CubismRenderer_Axmol(const CubismRenderer_Axmol&);
    CubismRenderer_Axmol& operator=(const CubismRenderer_Axmol&);

    /**
     * @brief   レンダラが保持する静的なリソースを解放する<br>
     *           Cocos2dxの静的なシェーダプログラムを解放する
     */
    static void DoStaticRelease();

    /**
     * @brief   描画開始時の追加処理。<br>
     *           モデルを描画する前にクリッピングマスクに必要な処理を実装している。
     */
    void PreDraw();

    /**
     * @brief   描画完了後の追加処理。
     *
     */
    void PostDraw(){};

    /**
     * @brief   モデル描画直前のCocos2dxのステートを保持する
     */
    virtual void SaveProfile() override;

    /**
     * @brief   モデル描画直前のCocos2dxのステートを保持する
     */
    virtual void RestoreProfile() override;

    void BeforeDrawModelRenderTarget() override;
    void AfterDrawModelRenderTarget() override;

    ax::Texture2D* CopyCurrentModelRenderTarget();
    ax::Texture2D* CopyCurrentRenderTarget(CubismOffscreenFrame_Axmol* sourceTarget);
    CubismCommandBuffer_Axmol::DrawCommandBuffer* CreateRenderTargetDrawCommandBuffer();
    CubismCommandBuffer_Axmol::DrawCommandBuffer* GetRenderTargetCopyDrawCommandBuffer();
    void SyncRenderTargetSize(csmFloat32 width, csmFloat32 height);
    void SyncRenderTargetSizeWithCurrentViewport();
    void OnRenderViewResized();
    csmRectF GetCurrentDrawViewport() const;
    void DrawRenderTargetToCurrent(CubismOffscreenFrame_Axmol* renderTarget,
                                   CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommand,
                                   CubismTextureColor baseColor,
                                   ax::rhi::BlendFactor sourceRGB,
                                   ax::rhi::BlendFactor destinationRGB,
                                   ax::rhi::BlendFactor sourceAlpha,
                                   ax::rhi::BlendFactor destinationAlpha);
    void DrawRenderTargetMeshToCurrent(CubismCommandBuffer_Axmol::DrawCommandBuffer::DrawCommand* drawCommand,
                                       ax::Texture2D* texture,
                                       CubismBlendMode colorBlendMode,
                                       CubismTextureColor baseColor,
                                       CubismTextureColor multiplyColor,
                                       CubismTextureColor screenColor,
                                       const csmBlendMode& sourceBlendMode,
                                       ax::Texture2D* blendTexture,
                                       csmBool invertedMask);
    void DrawMaskForClippingContext(CubismClippingContext_Axmol* clipContext,
                                    CubismClippingManager_Axmol* clippingManager,
                                    CubismOffscreenFrame_Axmol* maskBuffer,
                                    rhi::Texture* restoreColorBuffer);
    void SetupParentOffscreens(const CubismModel* model, csmInt32 offscreenCount);
    void DrawObjectLoop();
    void RenderObject(csmInt32 objectIndex, DrawableObjectType objectType);
    void DrawDrawable(csmInt32 drawableIndex);
    void AddOffscreen(csmInt32 offscreenIndex);
    void DrawOffscreen(CubismOffscreenRenderTarget_Axmol* currentOffscreen);
    void SubmitDrawToParentOffscreen(csmInt32 objectIndex, DrawableObjectType objectType);

    /**
     * @brief   マスクテクスチャに描画するクリッピングコンテキストをセットする。
     */
    void SetClippingContextBufferForMask(CubismClippingContext_Axmol* clip);

    /**
     * @brief   マスクテクスチャに描画するクリッピングコンテキストを取得する。
     *
     * @return  マスクテクスチャに描画するクリッピングコンテキスト
     */
    CubismClippingContext_Axmol* GetClippingContextBufferForMask() const;

    /**
     * @brief   画面上に描画するクリッピングコンテキストをセットする。
     */
    void SetClippingContextBufferForDraw(CubismClippingContext_Axmol* clip);

    /**
     * @brief   画面上に描画するクリッピングコンテキストを取得する。
     *
     * @return  画面上に描画するクリッピングコンテキスト
     */
    CubismClippingContext_Axmol* GetClippingContextBufferForDraw() const;
    void SetClippingContextBufferForOffscreen(CubismClippingContext_Axmol* clip);
    CubismClippingContext_Axmol* GetClippingContextBufferForOffscreen() const;


    csmMap<csmInt32, ax::Texture2D*>            _textures;                      ///< モデルが参照するテクスチャとレンダラでバインドしているテクスチャとのマップ
    csmVector<csmInt32>                 _sortedDrawableIndexList;       ///< 描画オブジェクトのインデックスを描画順に並べたリスト
    csmVector<csmInt32>                 _sortedObjectsIndexList;
    csmVector<DrawableObjectType>       _sortedObjectsTypeList;
    csmVector<CubismOffscreenRenderTarget_Axmol*> _offscreenList;
    CubismOffscreenRenderTarget_Axmol*  _currentOffscreen;
    CubismRendererProfile_Axmol     _rendererProfile;               ///< OpenGLのステートを保持するオブジェクト
    CubismClippingManager_Axmol*    _clippingManager;               ///< クリッピングマスク管理オブジェクト
    CubismClippingManager_Axmol*    _offscreenClippingManager;      ///< オフスクリーン用クリッピングマスク管理オブジェクト
    CubismClippingContext_Axmol*              _clippingContextBufferForMask;  ///< マスクテクスチャに描画するためのクリッピングコンテキスト
    CubismClippingContext_Axmol*              _clippingContextBufferForDraw;  ///< 画面上描画するためのクリッピングコンテキスト
    CubismClippingContext_Axmol*              _clippingContextBufferForOffscreen; ///< オフスクリーン描画用のクリッピングコンテキスト

    CubismOffscreenFrame_Axmol      _offscreenFrameBuffer;          ///< マスク描画用のフレームバッファ
    CubismOffscreenFrame_Axmol      _offscreenMaskFrameBuffer;      ///< オフスクリーンマスク描画用のフレームバッファ
    csmVector<CubismOffscreenFrame_Axmol*> _modelRenderTargets;     ///< 5.5ブレンド用モデル描画バッファ
    csmVector<CubismCommandBuffer_Axmol::DrawCommandBuffer*>  _drawableDrawCommandBuffer;
    csmVector<CubismCommandBuffer_Axmol::DrawCommandBuffer*>  _offscreenDrawCommandBuffer;
    CubismCommandBuffer_Axmol::DrawCommandBuffer* _renderTargetDrawCommandBuffer;
    csmVector<CubismCommandBuffer_Axmol::DrawCommandBuffer*> _renderTargetCopyDrawCommandBuffers;
    csmUint32 _renderTargetCopyDrawCommandBufferIndex;
    ax::CustomEventListener* _windowResizeListener;
};

}}}}
//------------ LIVE2D NAMESPACE ------------
