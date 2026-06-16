/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "csmVector.hpp"
#include "Utils/CubismDebug.hpp"

//--------- LIVE2D NAMESPACE ------------
namespace Live2D { namespace Cubism { namespace Framework {

//========================テンプレートの宣言==============================

/**
 * @brief   ソート比較関数の型定義<br>
 *
 * leftがright の関係が left < right の場合 true を返すようにしてください
 * leftがright の関係が left >= right の場合 false を返すようにしてください
 */
template<class T>
using SortCompareFunction = csmBool(*)(T left, T right);

/**
 * @brief   ベクター型（可変配列型）<br>
 *           コンシューマゲーム機等でSTLの組み込みを避けるための実装。std::vector の簡易版
 *
 */
class csmVectorSort
{
public:
    /**
     * @brief   マージソート
     *
     * @param[in]   list    ->  ソート対象の配列
     * @param[in]   begin   ->  ソート開始インデックス
     * @param[in]   end     ->  ソート終了インデックス
     * @param[in]   compare ->  比較関数
     */
    template<class T>
    static void MergeSort(csmVector<T>& list, csmInt32 begin, csmInt32  end, SortCompareFunction<T> compare);

    /**
     * @brief   マージソート
     *
     * @param[in]   list    ->  ソート対象の配列
     * @param[in]   begin   ->  ソート開始イテレータ
     * @param[in]   end     ->  ソート終了イテレータ
     * @param[in]   compare ->  比較関数
     */
    template<class T>
    static void MergeSort(typename csmVector<T>::iterator begin, typename csmVector<T>::iterator end, SortCompareFunction<T> compare);

    /**
     * @brief   マージソート
     *
     * @param[in]   list    ->  ソート対象の配列
     * @param[in]   begin   ->  ソート開始イテレータ
     * @param[in]   end     ->  ソート終了イテレータ
     * @param[in]   compare ->  比較関数
     */
    template<class T>
    static void MergeSort(typename csmVector<T>::const_iterator begin, typename csmVector<T>::const_iterator end, SortCompareFunction<T> compare);

private:
    /**
     * @brief   2つのソート済み部分配列 [left...mid] と [mid+1...right] をマージする関数
     *
     * @param[in]   list    ->  ソート対象の配列
     * @param[in]   left    ->  1つ目の部分配列の先頭インデックス
     * @param[in]   mid     ->  1つ目の部分配列の末尾インデックス
     * @param[in]   right   ->  2つ目の部分配列の末尾インデックス
     * @param[in]   compare ->  比較関数
     */
    template<class T>
    static void Merge(csmVector<T>& list, csmInt32 left, csmInt32 mid, csmInt32 right, SortCompareFunction<T> compare);

};

//========================テンプレートの定義==============================

// マージソートの再帰関数
template<class T>
void csmVectorSort::MergeSort(csmVector<T>& list, const csmInt32 begin, const csmInt32 end, SortCompareFunction<T> compare)
{
    if (!compare)
    {
        return; // 比較関数が指定されていない場合は何もしない
    }

    if (begin + 1 < end)
    {
        const csmInt32 mid = (end + begin) / 2;
        MergeSort(list, begin, mid, compare);  // 左半分をソート
        MergeSort(list, mid, end, compare);  // 右半分をソート
        Merge(list, begin, mid, end, compare);    // ソート済みの左右をマージ
    }
}

// マージソートの再帰関数
template<class T>
void csmVectorSort::MergeSort(const typename csmVector<T>::iterator begin, const typename csmVector<T>::iterator end, SortCompareFunction<T> compare)
{
    if (begin._vector != end._vector)
    {
        return; // beginとendが同じベクターを指していない場合は何もしない
    }

    MergeSort(*begin._vector, begin._index, end._index, compare);
}

// マージソートの再帰関数
template<class T>
void csmVectorSort::MergeSort(const typename csmVector<T>::const_iterator begin, const typename csmVector<T>::const_iterator end, SortCompareFunction<T> compare)
{
    if (begin._vector != end._vector)
    {
        return; // beginとendが同じベクターを指していない場合は何もしない
    }

    MergeSort(*begin._vector, begin._index, end._index, compare);
}

// 2つのソート済み部分配列 [left...mid] と [mid+1...right] をマージする関数
template<class T>
void csmVectorSort::Merge(csmVector<T>& list, const csmInt32 left, const csmInt32 mid, const csmInt32 right, SortCompareFunction<T> compare)
{
        const csmInt32 n1 = mid - left;
        const csmInt32 n2 = right - mid;

        // 一時配列を作成
        csmVector<T> tmpListL(n1);
        csmVector<T> tmpListR(n2);

        for (csmInt32 i = 0; i < n1; ++i)
        {
            tmpListL[i] = list[left + i];
        }
        for (csmInt32 i = 0; i < n2; ++i)
        {
            tmpListR[i] = list[mid + i];
        }

        // 二分割配列をマージ
        csmInt32 i = 0;
        csmInt32 j = 0;
        csmInt32 k = left;
        while (i < n1 && j < n2)
        {
            if (compare(tmpListL[i], tmpListR[j]))
            {
                list[k++] = tmpListL[i++];
            }
            else
            {
                list[k++] = tmpListR[j++];
            }
        }

        // 残った要素をコピー
        while (i < n1)
        {
            list[k++] = tmpListL[i++];
        }
        while (j < n2)
        {
            list[k++] = tmpListR[j++];
        }
}
}}}
//--------- LIVE2D NAMESPACE ------------
