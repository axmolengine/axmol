/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

class CubismModel;

/**
 * @brief Mocデータの管理
 *
 * Mocデータの管理を行うクラス。
 */
class CubismMoc
{
    friend class CubismModel;
public:
    /**
     * @brief バッファからMocデータの作成
     *
     * バッファからMocファイルを読み取り、Mocデータを作成する。
     *
     * @param[in]   mocBytes    Mocファイルのバッファ
     * @param[in]   size        バッファのサイズ
     */
    static CubismMoc* Create(const csmByte* mocBytes, csmSizeInt size);

    /**
     * @brief Mocデータを削除
     *
     * Mocデータを削除する。
     */
    static void Delete(CubismMoc* moc);

    /**
     * @brief モデルを作成
     *
     * モデルを作成する。
     *
     * @return  Mocデータから作成されたモデル
     */
    CubismModel* CreateModel();

    /**
     * @brief モデルを削除
     *
     * モデルを削除する。
     *
     * @param[in]   model   対象のモデル
     */
    void DeleteModel(CubismModel* model);

private:
    /**
     * @brief コンストラクタ
     *
     * コンストラクタ。
     */
    CubismMoc(Core::csmMoc* moc);

    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~CubismMoc();

    Core::csmMoc*     _moc;             ///< Mocデータ
    csmInt32          _modelCount;      ///< Mocデータから作られたモデルの個数
};

}}}
