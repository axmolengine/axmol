/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "2d/CCRenderTexture.h"
#include <CubismFramework.hpp>
#include <Model/CubismUserModel.hpp>
#include <ICubismModelSetting.hpp>
#include <Type/csmRectF.hpp>
#include <Rendering/axmol/CubismOffscreenSurface_Cocos2dx.hpp>
#include "LAppDefine.hpp"
#include "AppDelegate.h"

/**
 * @brief ユーザーが実際に使用するモデルの実装クラス<br>
 *         モデル生成、機能コンポーネント生成、更新処理とレンダリングの呼び出しを行う。
 *
 */
class LAppModel : public Csm::CubismUserModel
{
public:
    /**
     * @brief コンストラクタ
     */
    LAppModel();

    /**
     * @brief デストラクタ
     *
     */
    virtual ~LAppModel();

    /**
     * @brief model3.jsonが置かれたディレクトリとファイルパスからモデルを生成する
     *
     */
    void LoadAssets(const Csm::csmChar dir[], const  Csm::csmChar fileName[]);

    /**
     * @brief レンダラを再構築する
     *
     */
    void ReloadRnederer();

    /**
     * @brief   モデルの更新処理。モデルのパラメータから描画状態を決定する。
     *
     */
    void Update();

    /**
     * @brief   モデルを描画する処理。モデルを描画する空間のView-Projection行列を渡す。
     *
     * @param[in]  matrix  View-Projection行列
     */
    void Draw(Csm::Rendering::CubismCommandBuffer_Cocos2dx* commandBuffer, Csm::CubismMatrix44& matrix);

    /**
     * @brief   引数で指定したモーションの再生を開始する。
     *
     * @param[in]   group                       モーショングループ名
     * @param[in]   no                          グループ内の番号
     * @param[in]   priority                    優先度
     * @param[in]   onFinishedMotionHandler     モーション再生終了時に呼び出されるコールバック関数。NULLの場合、呼び出されない。
     * @return                                  開始したモーションの識別番号を返す。個別のモーションが終了したか否かを判定するIsFinished()の引数で使用する。開始できない時は「-1」
     */
    Csm::CubismMotionQueueEntryHandle StartMotion(const Csm::csmChar* group, Csm::csmInt32 no, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    /**
     * @brief   ランダムに選ばれたモーションの再生を開始する。
     *
     * @param[in]   group                       モーショングループ名
     * @param[in]   priority                    優先度
     * @param[in]   onFinishedMotionHandler     モーション再生終了時に呼び出されるコールバック関数。NULLの場合、呼び出されない。
     * @return                                  開始したモーションの識別番号を返す。個別のモーションが終了したか否かを判定するIsFinished()の引数で使用する。開始できない時は「-1」
     */
    Csm::CubismMotionQueueEntryHandle StartRandomMotion(const Csm::csmChar* group, Csm::csmInt32 priority, Csm::ACubismMotion::FinishedMotionCallback onFinishedMotionHandler = NULL);

    /**
     * @brief   引数で指定した表情モーションをセットする
     *
     * @param   expressionID    表情モーションのID
     */
    void SetExpression(const Csm::csmChar* expressionID);

    /**
     * @brief   ランダムに選ばれた表情モーションをセットする
     *
     */
    void SetRandomExpression();

    /**
    * @brief   イベントの発火を受け取る
    *
    */
    virtual void MotionEventFired(const Live2D::Cubism::Framework::csmString& eventValue);

    /**
     * @brief    当たり判定テスト。<br>
     *            指定IDの頂点リストから矩形を計算し、座標が矩形範囲内か判定する。
     *
     * @param[in]   hitAreaName     当たり判定をテストする対象のID
     * @param[in]   x               判定を行うX座標
     * @param[in]   y               判定を行うY座標
     */
    virtual Csm::csmBool HitTest(const Csm::csmChar* hitAreaName, Csm::csmFloat32 x, Csm::csmFloat32 y);

    Csm::csmRectF GetDrawableArea(Csm::csmInt32 drawableIndex, const Csm::CubismMatrix44& vpMatrix, const Csm::CubismVector2& windowSize) const;
    const Csm::csmVector<Csm::csmRectF>& GetHitAreas(const Csm::CubismMatrix44& vpMatrix, const Csm::CubismVector2& windowSize);
    const Csm::csmVector<Csm::csmRectF>& GetUserDataAreas(const Csm::CubismMatrix44& vpMatrix, const Csm::CubismVector2& windowSize);

    /**
     * @brief   別ターゲットに描画する際に使用するバッファ・スプライトを作成し、
     *           スプライトをSampleSceneへ登録する
     */
    void MakeRenderingTarget();

    /**
     * @brief   別ターゲットに描画する際のスプライト色設定
     * @param[in]   r   赤(0.0~1.0)
     * @param[in]   g   緑(0.0~1.0)
     * @param[in]   b   青(0.0~1.0)
     * @param[in]   a   α(0.0~1.0)
     */
    void SetSpriteColor(float r, float g, float b, float a);


protected:
    /**
     *  @brief  モデルを描画する処理。モデルを描画する空間のView-Projection行列を渡す。
     *
     */
    void DoDraw();

private:
    /**
     * @brief model3.jsonからモデルを生成する。<br>
     *         model3.jsonの記述に従ってモデル生成、モーション、物理演算などのコンポーネント生成を行う。
     *
     * @param[in]   setting     ICubismModelSettingのインスタンス
     *
     */
    void SetupModel(Csm::ICubismModelSetting* setting);

    /**
     * @brief OpenGLのテクスチャユニットにテクスチャをロードする
     *
     */
    void SetupTextures();

    /**
     * @brief   モーションデータをグループ名から一括でロードする。<br>
     *           モーションデータの名前は内部でModelSettingから取得する。
     *
     * @param[in]   group  モーションデータのグループ名
     */
    void PreloadMotionGroup(const Csm::csmChar* group);

    /**
     * @brief   モーションデータをグループ名から一括で解放する。<br>
     *           モーションデータの名前は内部でModelSettingから取得する。
     *
     * @param[in]   group  モーションデータのグループ名
     */
    void ReleaseMotionGroup(const Csm::csmChar* group) const;

    /**
    * @brief すべてのモーションデータの解放
    *
    * すべてのモーションデータを解放する。
    */
    void ReleaseMotions();

    /**
    * @brief すべての表情データの解放
    *
    * すべての表情データを解放する。
    */
    void ReleaseExpressions();

    Csm::ICubismModelSetting* _modelSetting;        ///< モデルセッティング情報
    Csm::csmString _modelHomeDir;                   ///< モデルセッティングが置かれたディレクトリ
    Csm::csmFloat32 _userTimeSeconds;               ///< デルタ時間の積算値[秒]

    Csm::csmVector<Csm::CubismIdHandle> _eyeBlinkIds; ///< モデルに設定されたまばたき機能用パラメータID
    Csm::csmVector<Csm::CubismIdHandle> _lipSyncIds;  ///< モデルに設定されたリップシンク機能用パラメータID

    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _motions;       ///< 読み込まれているモーションのリスト
    Csm::csmMap<Csm::csmString, Csm::ACubismMotion*>   _expressions;   ///< 読み込まれている表情のリスト

    Csm::csmVector<Csm::csmRectF>_hitArea;
    Csm::csmVector<Csm::csmRectF>_userArea;

    const Csm::CubismId* _idParamAngleX;            ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamAngleY;            ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamAngleZ;            ///< パラメータID: ParamAngleX
    const Csm::CubismId* _idParamBodyAngleX;        ///< パラメータID: ParamBodyAngleX
    const Csm::CubismId* _idParamEyeBallX;          ///< パラメータID: ParamEyeBallX
    const Csm::CubismId* _idParamEyeBallY;          ///< パラメータID: ParamEyeBallXY

    Csm::Rendering::CubismOffscreenFrame_Cocos2dx*  _renderBuffer;  ///< モードによってはCubismOffscreenFrameのテクスチャを描画
    ax::RenderTexture* _renderSprite;          ///< _renderBufferを描画するスプライト
    float _clearColor[4];                           ///< _renderBufferをクリアする際の色
    Csm::csmVector<ax::Texture2D*> _loadedTextures;
};



