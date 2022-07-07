/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismJsonHolder.hpp"
#include "Utils/CubismJson.hpp"
#include "Model/CubismModel.hpp"
#include "Id/CubismIdManager.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D {  namespace Cubism {  namespace Framework {

class CubismModelUserDataJson : public CubismJsonHolder
{
public:
    /**
    * @brief コンストラクタ
    *
    * コンストラクタ。
    *
    * @param[in]   buffer  userdata3.jsonが読み込まれているバッファ
    * @param[in]   size    バッファのサイズ
    */
    CubismModelUserDataJson(const csmByte* buffer, csmSizeInt size);

    /**
    * @brief デストラクタ
    *
    * デストラクタ。
    */
    virtual ~CubismModelUserDataJson();

    /**
    * @brief ユーザデータ個数の取得
    *
    * userdata3.jsonに入っていたユーザデータの個数を取得する。
    *
    * @return  ユーザデータの個数
    */
    csmInt32 GetUserDataCount() const;

    /**
    * @brief ユーザデータ総文字列数の取得
    *
    * ユーザデータ総文字列数の取得する。
    *
    * @return  ユーザデータ総文字列数
    */
    csmInt32 GetTotalUserDataSize() const;

    /**
    * @brief ユーザデータのタイプの取得
    *
    * 指定番目のユーザデータのタイプの取得を取得する。
    *
    * @param[in]   i    インデックス
    *
    * @return      ユーザデータのタイプ
    */
    csmString GetUserDataTargetType(csmInt32 i) const;

    /**
    * @brief ユーザデータのターゲットIDの取得
    *
    * 指定番目のユーザデータのターゲットIDの取得を取得する。
    *
    * @param[in]   i    インデックス
    *
    * @return      ユーザデータターゲットID
    */
    CubismIdHandle GetUserDataId(csmInt32 i) const;

    /**
    * @brief ユーザデータの文字列の取得
    *
    * 指定番目のユーザデータの文字列の取得を取得する。
    *
    * @param[in]   i    インデックス
    *
    * @return      ユーザデータ
    */
    const csmChar* GetUserDataValue(csmInt32 i) const;
};

}}}
