/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Type/csmString.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismIdManager;

/**
 * @brief パラメータ名・パーツ名・Drawable名を保持
 *
 * パラメータ名・パーツ名・Drawable名を保持するクラス。
 */
struct CubismId
{
    friend class CubismIdManager;

    /**
     * @brief ID名を取得
     *
     * ID名を取得する。
     */
    const csmString& GetString() const;

private:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismId();

    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     *
     * @param[in] id ID名
     */
    CubismId(const csmChar* id);

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    ~CubismId();

    CubismId(const CubismId& c);
    CubismId& operator=(const CubismId& c);

    csmBool operator==(const CubismId& c) const;
    csmBool operator!=(const CubismId& c) const;

    csmString _id;      ///< ID名
};

typedef const CubismId* CubismIdHandle;

}}}
