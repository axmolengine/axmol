/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "CubismFramework.hpp"
#include <string.h>

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief   文字列クラス<br>
 *           コンシューマゲーム機等でSTLの組み込みを避けるための実装。<br>
 *           std::string の簡易版マルチバイト文字列未対応
 */
class csmString
{
public:
    /**
     * @brief   コンストラクタ
     *
     */
    csmString();

    /**
     * @brief   引数付きコンストラクタ
     *
     * @param   c   ->  コピーする文字列
     */
    csmString(const csmChar* c);

    /**
     * @brief   引数付きコンストラクタ
     *
     * @param   c       ->  文字列
     * @param   length  ->  文字列からコピーする長さ
     */
    csmString(const csmChar* c, csmInt32 length);

    /**
     * @brief   引数付きコンストラクタ
     *
     * @param[in]   s   ->  文字列
     */
    csmString(const csmString& s);

    /**
     * @brief   引数付きコンストラクタ
     *
     * @param[in]   c       ->  文字列
     * @param[in]   length  ->  文字列からコピーする長さ
     * @param[in]   usePtr  ->  trueなら引数のポインタを保持する（インスタンスと共に破棄される）<br> falseなら内部でコピーを実行する
     */
    csmString(const csmChar* c, csmInt32 length, csmBool usePtr);

    /**
     * @brief   デストラクタ
     *
     */
    virtual ~csmString();

    /**
     * @brief =演算子のオーバーロード(csmString型)
     */
    csmString& operator=(const csmString& s);

    /**
     * @brief =演算子のオーバーロード(csmChar型)
     */
    csmString& operator=(const csmChar* c);

    /**
     * @brief ==演算子のオーバーロード(csmString型)
     */
    csmBool operator==(const csmString& s) const;

    /**
     * @brief ==演算子のオーバーロード(csmChar型)
     */
    csmBool operator==(const csmChar* c) const;

    /**
     * @brief <演算子のオーバーロード(csmString型)
     */
    csmBool operator<(const csmString& s) const;

    /**
     * @brief <演算子のオーバーロード(csmChar型)
     */
    csmBool operator<(const csmChar* c) const;

    /**
     * @brief >演算子のオーバーロード(csmString型)
     */
    csmBool operator>(const csmString& s) const;

    /**
     * @brief >演算子のオーバーロード(csmChar型)
     */
    csmBool operator>(const csmChar* c) const;

    /**
     * @brief +演算子のオーバーロード(csmString型)
     */
    csmString operator+(const csmString& s) const;

    /**
     * @brief +演算子のオーバーロード(csmChar型)
     */
    csmString operator+(const csmChar* c) const;

    /**
     * @brief +=演算子のオーバーロード(csmString型)
     */
    csmString& operator+=(const csmString& s);

    /**
     * @brief +=演算子のオーバーロード(csmChar型)
     */
    csmString& operator+=(const csmChar* c);

    /**
     * @brief  文字列を後方に追加する
     *
     * @param[in]   c   ->  文字列
     * @param[in]   length  ->  追加する文字列の長さ
     * @return 更新された文字列
     */
    csmString& Append(const csmChar* c, csmInt32 length);

    /**
     * @brief   文字サイズを拡張して文字を埋める
     *
     * @param[in]   length   ->  拡張する文字数
     * @param[in]   v        ->  埋める文字
     * @return 更新された文字列
     */
    csmString& Append(csmInt32 length, const csmChar v);

    /**
     * @brief   文字列の長さを返す
     *
     * @return  文字列の長さ
     */
    csmInt32 GetLength() const { return _length; }

    /**
     * @brief   C言語文字列としてのポインタを取得する
     *
     * @return   C言語文字列のポインタ
     */
    const csmChar* GetRawString() const;

    /**
     * @brief   ポインタを解放する
     */
    void Clear();

    /**
     * @brief   ハッシュコードを取得する
     *
     * @return  ハッシュコード
     */
    csmInt32 GetHashcode();


protected:

    /**
     * @brief   文字列をコピーする. 終端に\0を追加する
     *
     * @param[in]   c   ->  文字列
     * @param[in]   length  ->  コピーする文字数
     */
    void Copy(const csmChar* c, csmInt32 length);

    /**
     * @brief   csmStringインスタンスの初期化関数。文字列のセットとハッシュコードの算出を行う。
     *
     * @param[in]   c       ->  文字列のポインタ
     * @param[in]   length  ->  文字列の長さ
     * @param[in]   usePtr  ->  trueなら第一引数のポインタを保持する(インスタンスと共に破棄される)。<br> falseなら内部でコピーを実行する
     */
    void Initialize(const csmChar* c, csmInt32 length, csmBool usePtr);

    /**
     * @brief   文字列からハッシュ値を生成して返す
     *
     * @param[in]   c       ->  文字列
     * @param[in]   length  ->  ハッシュ値の長さ
     * @return      文字列から生成したハッシュ値
     */
    csmInt32 CalcHashcode(const csmChar* c, csmInt32 length);

private:
    static const csmInt32 SmallLength = 64; ///< この長さ-1未満の文字列は内部バッファを使用
    static const csmInt32 DefaultSize = 10; ///< デフォルトの文字数
    static csmInt32 s_totalInstanceNo;      ///< 通算のインスタンス番号
    csmChar* _ptr;                          ///< 文字型配列のポインタ
    csmInt32 _length;                       ///< 半角文字数（メモリ確保は最後に0が入るため_length+1）
    csmInt32 _hashcode;                     ///< インスタンスに当てられたハッシュ値
    csmInt32 _instanceNo;                   ///< インスタンスに割り当てられた番号

    csmChar _small[SmallLength];            ///< 文字列の長さがSmallLength-1未満の場合はこちらを使用

    /**
     * @brief 文字列が空かどうか？
     *
     * 文字列が空かどうか確認する。
     *
     * @retval  true    空の文字列
     * @retval  false   値が設定されている
     */
    csmBool IsEmpty() const;

    /**
     * @brief 文字列を空に設定
     *
     * 文字列を空に設定する。
     *
     * @note ポインタに対してfree()はしない。
     */
    void SetEmpty();

    /**
     * @brief   C言語文字列としてのポインタを取得する
     *
     * @return   C言語文字列のポインタ
     */
    csmChar* WritePointer();
};
}}}

//------------------------- LIVE2D NAMESPACE -----------

