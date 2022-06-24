/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "ACubismMotion.hpp"
#include "Utils/CubismJson.hpp"
#include "Model/CubismModel.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief 表情のモーション
 *
 * 表情のモーションクラス。
 */
class CubismExpressionMotion : public ACubismMotion
{
private:
    /**
     * @brief 表情パラメータ値の計算方式
     *
     */
    enum ExpressionBlendType
    {
        ExpressionBlendType_Add = 0,        ///< 加算
        ExpressionBlendType_Multiply = 1,   ///< 乗算
        ExpressionBlendType_Overwrite = 2   ///< 上書き
    };

public:
    /**
     * @brief 表情のパラメータ情報
     *
     * 表情のパラメータ情報の構造体。
     */
    struct ExpressionParameter
    {
        CubismIdHandle      ParameterId;        ///< パラメータID
        ExpressionBlendType BlendType;          ///< パラメータの演算種類
        csmFloat32          Value;              ///< 値
    };

    /**
     * @brief インスタンスの作成
     *
     * インスタンスを作成する。
     *
     * @param[in]   buf     expファイルが読み込まれているバッファ
     * @param[in]   size    バッファのサイズ
     * @return  作成されたインスタンス
     */
    static CubismExpressionMotion* Create(const csmByte* buf, csmSizeInt size);

    /**
    * @brief モデルのパラメータの更新の実行
    *
    * モデルのパラメータ更新を実行する。
    *
    * @param[in]   model               対象のモデル
    * @param[in]   userTimeSeconds     デルタ時間の積算値[秒]
    * @param[in]   weight              モーションの重み
    * @param[in]   motionQueueEntry    CubismMotionQueueManagerで管理されているモーション
    */
    virtual void DoUpdateParameters(CubismModel* model, csmFloat32 userTimeSeconds, csmFloat32 weight, CubismMotionQueueEntry* motionQueueEntry);

private:
    CubismExpressionMotion();
    virtual ~CubismExpressionMotion();

    csmVector<ExpressionParameter> _parameters;         ///< 表情のパラメータ情報リスト
};

}}}
