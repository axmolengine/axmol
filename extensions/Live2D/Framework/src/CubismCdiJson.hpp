/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismJsonHolder.hpp"
#include "Utils/CubismJson.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D {  namespace Cubism {  namespace Framework {

class CubismCdiJson : public CubismJsonHolder
{
public:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param[in]   buffer  cdi3.jsonが読み込まれているバッファ
     * @param[in]   size    バッファのサイズ
     */
    CubismCdiJson(const csmByte* buffer, csmSizeInt size);

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismCdiJson();

    // Parameters

    csmInt32 GetParametersCount();

    const csmChar* GetParametersId(csmInt32 index);

    const csmChar* GetParametersGroupId(csmInt32 index);

    const csmChar* GetParametersName(csmInt32 index);

    // ParameterGroups

    csmInt32 GetParameterGroupsCount();

    const csmChar* GetParameterGroupsId(csmInt32 index);

    const csmChar* GetParameterGroupsGroupId(csmInt32 index);

    const csmChar* GetParameterGroupsName(csmInt32 index);

    // Parts

    csmInt32 GetPartsCount();

    const csmChar* GetPartsId(csmInt32 index);

    const csmChar* GetPartsName(csmInt32 index);


private:
    /**
     * @brief        パラメータのキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistParameters() const;

    /**
     * @brief        パラメータグループのキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistParameterGroups() const;

    /**
     * @brief        パーツのキーが存在するかどうかを確認する
     *
     * @retval       true  -> キーが存在する
     * @retval       false -> キーが存在しない
     */
    csmBool IsExistParts() const;
};

}}}
