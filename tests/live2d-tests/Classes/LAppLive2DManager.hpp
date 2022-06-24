/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <Math/CubismMatrix44.hpp>
#include <Type/csmVector.hpp>
#include "LAppModel.hpp"

class LAppSprite;

/**
 * @brief サンプルアプリケーションにおいてCubismModelを管理するクラス<br>
 *         モデル生成と破棄、タップイベントの処理、モデル切り替えを行う。
 *
 */
class LAppLive2DManager
{

public:

    /**
     * @brief LAppModelのレンダリング先
     */
    enum SelectTarget
    {
        SelectTarget_None,                ///< デフォルトのフレームバッファにレンダリング
        SelectTarget_ModelFrameBuffer,    ///< LAppModelが各自持つフレームバッファにレンダリング
        SelectTarget_ViewFrameBuffer,     ///< LAppViewの持つフレームバッファにレンダリング
    };

    /**
     * @brief   クラスのインスタンス（シングルトン）を返す。<br>
     *           インスタンスが生成されていない場合は内部でインスタンを生成する。
     *
     * @return  クラスのインスタンス
     */
    static LAppLive2DManager* GetInstance();

    /**
     * @brief   クラスのインスタンス（シングルトン）を解放する。
     *
     */
    static void ReleaseInstance();

    /**
     * @brief   現在のシーンで保持しているモデルを返す
     *
     * @param[in]   no  モデルリストのインデックス値
     * @return      モデルのインスタンスを返す。インデックス値が範囲外の場合はNULLを返す。
     */
    LAppModel* GetModel(Csm::csmUint32 no) const;

    /**
     * @brief   現在のシーンで保持しているモデルの数を返す
     *
     * @return  モデルリストの数
     */
    Csm::csmUint32 GetModelCount() const;

    /**
     * @brief   現在のシーンで保持しているすべてのモデルを解放する
     *
     */
    void ReleaseAllModel();

    /**
     * @brief   モデル描画用のレンダラを再構築する。
     *
     */
    void RecreateRenderer() const;

    /**
     * @brief   モデル描画用のView行列をセットする。
     *
     * @param[in]   matrix   View行列
     *
     */
    void SetViewMatrix(Csm::CubismMatrix44* matrix);

    /**
     * @brief   画面をドラッグしたときの処理
     *
     * @param[in]   x   画面のX座標
     * @param[in]   y   画面のY座標
     */
    void OnDrag(Csm::csmFloat32 x, Csm::csmFloat32 y) const;

    /**
     * @brief   画面をタップしたときの処理
     *
     * @param[in]   x   画面のX座標
     * @param[in]   y   画面のY座標
     */
    void OnTap(Csm::csmFloat32 x, Csm::csmFloat32 y);

    /**
     * @brief   画面を更新するときの処理
     *          モデルの更新処理および描画処理を行う
     */
    void OnUpdate(Csm::Rendering::CubismCommandBuffer_Cocos2dx* commandBuffer) const;

    /**
     * @brief   次のシーンに切り替える<br>
     *           サンプルアプリケーションではモデルセットの切り替えを行う。
     */
    void NextScene();

    /**
     * @brief   シーンを切り替える<br>
     *           サンプルアプリケーションではモデルセットの切り替えを行う。
     */
    void ChangeScene(Csm::csmInt32 index);

    /**
     * @brief レンダリング先をデフォルト以外に切り替えた際の背景クリア色設定
     * @param[in]   r   赤(0.0~1.0)
     * @param[in]   g   緑(0.0~1.0)
     * @param[in]   b   青(0.0~1.0)
     */
    void SetRenderTargetClearColor(float r, float g, float b);

private:
    /**
     * @brief  コンストラクタ
     */
    LAppLive2DManager();

    /**
     * @brief  デストラクタ
     */
    virtual ~LAppLive2DManager();

    /**
     * @brief シェーダーを登録する。
     */
    void CreateShader();

    Csm::CubismMatrix44*        _viewMatrix;    ///< モデル描画に用いるView行列
    Csm::csmVector<LAppModel*>  _models;        ///< モデルインスタンスのコンテナ
    Csm::csmInt32               _sceneIndex;    ///< 表示するシーンのインデックス値

    // レンダリング先を別ターゲットにする方式の場合に使用
    SelectTarget _renderTarget;                 ///< レンダリング先の選択肢
    LAppSprite* _sprite;                        ///< テクスチャの単純描画クラス
    Csm::Rendering::CubismOffscreenFrame_Cocos2dx* _renderBuffer;   ///< モードによってはCubismモデル結果をこっちにレンダリング
    float _clearColor[4];                       ///< レンダリングターゲットのクリアカラー
    cocos2d::backend::Program* _program;

};
