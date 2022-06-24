/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Utils/CubismJson.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {
    /**
     * @brief CubismJsonのインスタンス生成や有効性のチェック処理を抽象化したクラス
     *
     * 各JsonパーサにおいてCubismJsonのインスタンス生成や
     * 有効性のチェック処理を共通化するためのインターフェース。
     *
     */
    class CubismJsonHolder
    {
    public:
        /**
         * @brief コンストラクタ
         *
         * コンストラクタ。
         */
        CubismJsonHolder()
            : _json(NULL)
        { }

        /**
         * @brief デストラクタ
         *
         * デストラクタ。
         */
        virtual ~CubismJsonHolder()
        { }

        /**
         * @brief CubismJsonの有効性チェック
         *
         * @retval       true  -> Jsonファイルが正常に読み込めた
         * @retval       false -> Jsonファイルが読み込めなかった。もしくは、存在しない
         */
        csmBool IsValid()
        {
            return _json;
        }

    protected:
        /**
         * @brief CubismJsonのインスタンスを生成する
         *
         * Util:CubismJsonクラスのCreate関数を呼んで
         * CubismJsonのインスタンスを生成する。
         *
         */
        void CreateCubismJson(const csmByte* buffer, csmSizeInt size)
        {
            _json = Utils::CubismJson::Create(buffer, size);

            if (!IsValid())
            {
                CubismLogError("[CubismJsonHolder] Invalid Json document.");
            }
        };

        /**
         * @brief CubismJsonのインスタンスを破棄する
         *
         * Util:CubismJsonクラスのDelete関数を呼んで
         * CubismJsonのインスタンスを破棄する。
         *
         */
        void DeleteCubismJson()
        {
            Utils::CubismJson::Delete(_json);
            _json = NULL;
        }

        Utils::CubismJson* _json;   /// CubismJsonの実体
    };
}}}
