/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Type/csmString.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework { namespace Utils{
class CubismString
{
public:
    /**
     * @brief   標準出力の書式を適用した文字列を取得する。
     *
     * @param[in]   format  ->  標準出力の書式指定文字列
     * @param[in]   ...     ->  書式指定文字列に渡す文字列
     * @return  書式を適用した文字列
     */
    static csmString GetFormatedString(const csmChar* format, ...);

    /**
     * @brief   textがstartWordで始まっているかどうかを返す
     * @param[in]   text        ->  検査対象の文字列
     * @param[in]   startWord   ->  比較対象の文字列
     * @retval  false   ->  textがstartWordで始まっている
     * @retval  true    ->  textがstartWordで始まっていない
     */
    static csmBool IsStartsWith(const csmChar* text, const csmChar* startWord);


    /***
     * @brief   position位置の文字から数字を解析する。
     *
     * @param[in]   string -> 文字列
     * @param[in]   length -> 文字列の長さ
     * @param[in]   position  -> 解析したい文字の位置
     * @param[out]  outEndPos   ->  一文字も読み込まなかった場合はエラー値(-1)が入る
     * @return      解析結果の数値
     */
    static csmFloat32 StringToFloat(const csmChar* string, csmInt32 length, csmInt32 position, csmInt32* outEndPos);

private:
    // コンストラクタ・デストラクタ呼び出し不可な静的クラスにする
    CubismString();
};
}}}}

//--------- LIVE2D NAMESPACE ------------
