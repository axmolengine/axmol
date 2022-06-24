/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Model/CubismModel.hpp"
#include "Id/CubismId.hpp"
#include "Type/csmVector.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief 呼吸機能
 *
 * 呼吸機能を提供する。
 */
class CubismBreath
{
public:
    /**
     * @brief 呼吸のパラメータ情報
     *
     * 呼吸のパラメータ情報。
     */
    struct BreathParameterData
    {
        /**
         * @brief コンストラクタ
         *
         * コンストラクタ。
         */
        BreathParameterData()
                             : ParameterId(NULL)
                             , Offset(0.0f)
                             , Peak(0.0f)
                             , Cycle(0.0f)
                             , Weight(0.0f)
        { }

        /**
         * @brief コンストラクタ
         *
         * コンストラクタ。
         *
         * @param[in]   parameterId     呼吸をひもづけるパラメータID
         * @param[in]   offset          呼吸を正弦波としたときの、波のオフセット
         * @param[in]   peak            呼吸を正弦波としたときの、波の高さ
         * @param[in]   cycle           呼吸を正弦波としたときの、波の周期
         * @param[in]   weight          パラメータへの重み
         */
        BreathParameterData(CubismIdHandle parameterId, csmFloat32 offset, csmFloat32 peak, csmFloat32 cycle, csmFloat32 weight)
            : ParameterId(parameterId)
            , Offset(offset)
            , Peak(peak)
            , Cycle(cycle)
            , Weight(weight)
        { }

        CubismIdHandle ParameterId;             ///< 呼吸をひもづけるパラメータID
        csmFloat32 Offset;                      ///< 呼吸を正弦波としたときの、波のオフセット
        csmFloat32 Peak;                        ///< 呼吸を正弦波としたときの、波の高さ
        csmFloat32 Cycle;                       ///< 呼吸を正弦波としたときの、波の周期
        csmFloat32 Weight;                      ///< パラメータへの重み
    };

    /**
     * @brief インスタンスの作成
     *
     * インスタンスを作成する。
     */
    static CubismBreath* Create();


    /**
     * @brief インスタンスの破棄
     *
     * インスタンスを破棄する。
     *
     * @param[in]   instance    対象のCubismBreath
     */
    static void Delete(CubismBreath* instance);

    /**
     * @brief 呼吸のパラメータのひもづけ
     *
     * 呼吸のパラメータをひもづける。
     *
     * @param[in]   breathParameters    呼吸をひもづけたいパラメータのリスト
     */
    void SetParameters(const csmVector<BreathParameterData>& breathParameters);


    /**
     * @brief 呼吸にひもづいているパラメータの取得
     *
     * 呼吸にひもづいているパラメータを取得する。
     *
     * @return  呼吸にひもづいているパラメータのリスト
     */
    const csmVector<BreathParameterData>& GetParameters() const;


    /**
     * @brief モデルのパラメータの更新
     *
     * モデルのパラメータを更新する。
     *
     * @param[in]   model   対象のモデル
     * @param[in]   deltaTimeSeconds   デルタ時間[秒]
     */
    void UpdateParameters(CubismModel* model, csmFloat32 deltaTimeSeconds);

private:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismBreath();

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismBreath();

    csmVector<BreathParameterData> _breathParameters;           ///< 呼吸にひもづいているパラメータのリスト
    csmFloat32 _currentTime;                                    ///< 積算時間[秒]
};

}}}
