/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include "Type/CubismBasicType.hpp"

namespace Live2D { namespace Cubism { namespace Framework {

/**
 * @brief メモリアロケーションを抽象化したクラス.
 *
 * メモリ確保・解放処理をプラットフォーム側で実装して
 * フレームワークから呼び出すためのインターフェース。
 *
 */
class ICubismAllocator
{
public:
    /**
     * @brief デストラクタ
     *
     * デストラクタ。
     */
    virtual ~ICubismAllocator() {}

    /**
     * @brief アラインメント制約なしのヒープ・メモリーを確保します。
     *
     * @param[in]  size   確保するバイト数
     *
     * @return     成功すると割り当てられたメモリのアドレス。 そうでなければ '0'を返す。
     */
    virtual void* Allocate(const csmSizeType size) = 0;

    /**
     * @brief アラインメント制約なしのヒープ・メモリーを解放します。
     *
     * @param[in]  memory   解放するメモリのアドレス
     *
     */
    virtual void Deallocate(void* memory) = 0;


    /**
     * @brief アラインメント制約ありのヒープ・メモリーを確保します。
     *
     * @param[in]  size       確保するバイト数
     * @param[in]  alignment  メモリーブロックのアラインメント幅
     *
     * @return     成功すると割り当てられたメモリのアドレス。 そうでなければ '0'を返す。
     */
    virtual void* AllocateAligned(const csmSizeType size, const csmUint32 alignment) = 0;

    /**
     * @brief アラインメント制約ありのヒープ・メモリーを解放します。
     *
     * @param[in]  alignedMemory       解放するメモリのアドレス
     *
     */
    virtual void DeallocateAligned(void* alignedMemory) = 0;

};
}}}
