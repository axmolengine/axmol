/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismModel.hpp"

namespace Live2D {  namespace Cubism {  namespace Framework {

/// ユーザデータの種類を指定するタイプ宣言
typedef CubismIdHandle  ModelUserDataType;

/**
* @brief ユーザデータの管理クラス
*
* ユーザデータをロード、管理、検索インターフェイス、解放までを行う。
*/
class CubismModelUserData
{
public:
    /**
    * @brief ユーザデータ構造体
    *
    * Jsonから読み込んだユーザデータを記録しておくための構造体
    */
    struct CubismModelUserDataNode
    {
        ModelUserDataType   TargetType;     ///< ユーザデータターゲットタイプ
        CubismIdHandle      TargetId;       ///< ユーザデータターゲットのID
        csmString           Value;          ///< ユーザデータ
    };

    /**
    * @brief インスタンスの作成
    *
    * インスタンスを作成する。
    *
    * @param[in]   buffer      userdata3.jsonが読み込まれいるバッファ
    * @param[in]   size        バッファのサイズ
    * @return      作成されたインスタンス
    */
    static CubismModelUserData* Create(const csmByte* buffer, csmSizeInt size);

    /**
    * @brief インスタンスの破棄
    *
    * インスタンスを破棄する。
    *
    * @param[in]   modelUserData      破棄するインスタンス
    */
    static void Delete(CubismModelUserData* modelUserData);

    /**
    * @brief デストラクタ
    *
    * ユーザーデータ構造体配列を解放する
    */
    virtual ~CubismModelUserData();

    /**
    * @brief ArtMeshのユーザデータのリストの取得
    *
    * ArtMeshのユーザデータのリストの取得する。
    *
    * @return      csmVectorのユーザデータリスト
    */
    const csmVector<const CubismModelUserDataNode*>& GetArtMeshUserDatas() const;

private:

    /**
    * @brief userdata3.jsonのパース
    *
    * userdata3.jsonをパースする。
    *
    * @param[in]   buffer          userdata3.jsonが読み込まれいるバッファ
    * @param[in]   size            バッファのサイズ
    */
    void ParseUserData(const csmByte* buffer, csmSizeInt size);

    csmVector<const CubismModelUserDataNode*>    _userDataNodes;        ///< ユーザデータ構造体配列
    csmVector<const CubismModelUserDataNode*>    _artMeshUserDataNodes; ///< 閲覧リスト保持
};
}}} //--------- LIVE2D NAMESPACE ------------
