/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Type/CubismBasicType.hpp"
#include "Type/csmString.hpp"
#include "Type/csmVector.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

struct CubismId;

/**
 * @brief ID名の管理
 *
 * ID名を管理する。
 */
class CubismIdManager
{
    friend struct CubismId;

public:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismIdManager();

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    ~CubismIdManager();

    /**
     * @brief ID名をリストから登録
     *
     * ID名をリストから登録する。
     *
     * @param[in]   ids     ID名リスト
     * @param[in]   count   IDの個数
     */
    void RegisterIds(const csmChar** ids, csmInt32 count);

    /**
    * @brief ID名をリストから登録
    *
    * ID名をリストから登録する。
    *
    * @param[in]   ids     ID名リスト
    */
    void RegisterIds(const csmVector<csmString>& ids);

    /**
     * @brief ID名を登録
     *
     * ID名を登録する。
     *
     * @param[in]   id  ID名
     */
    const CubismId* RegisterId(const csmChar* id);

    /**
    * @brief ID名を登録
    *
    * ID名を登録する。
    *
    * @param[in]   id  ID名
    */
    const CubismId* RegisterId(const csmString& id);

    /**
     * @brief ID名からIDを取得する。
     *
     * ID名からIDを取得する。
     *
     * @param[in]   id  ID名
     *
     * @note 未登録のID名の場合、登録も行う。
     */
    const CubismId* GetId(const csmString& id);

    /**
    * @brief ID名からIDを取得する。
    *
    * ID名からIDを取得する。
    *
    * @param[in]   id  ID名
    *
    * @note 未登録のID名の場合、登録も行う。
    */
    const CubismId* GetId(const csmChar* id);

    /**
     * @brief ID名からIDの確認
     *
     * ID名からIDが登録されているかどうか確認する。
     *
     * @retval  true    存在する
     * @retval  false   存在しない
     */
    csmBool IsExist(const csmString& id) const;

    /**
    * @brief ID名からIDの確認
    *
    * ID名からIDが登録されているかどうか確認する。
    *
    * @retval  true    存在する
    * @retval  false   存在しない
    */
    csmBool IsExist(const csmChar* id) const;

private:
    CubismIdManager(const CubismIdManager&);
    CubismIdManager& operator=(const CubismIdManager&);

    /**
     * @brief ID名からIDを検索
     *
     * ID名からIDを検索する。
     *
     * @param[in]   id  ID名
     * @return  登録されているID。なければNULL。
     */
    CubismId* FindId(const csmChar* id) const;

    csmVector<CubismId*> _ids;      ///< 登録されているIDのリスト
};

}}}
