/**
 * Copyright(c) Live2D Inc. All rights reserved.
 *
 * Use of this source code is governed by the Live2D Open Software license
 * that can be found at https://www.live2d.com/eula/live2d-open-software-license-agreement_en.html.
 */

#pragma once

#include <CubismFramework.hpp>
#include <ICubismAllocator.hpp>

/**
 * @brief メモリアロケーションを実装するクラス。
 *
 * メモリ確保・解放処理のインターフェースの実装。
 * フレームワークから呼び出される。
 *
 */
class LAppAllocator : public Csm::ICubismAllocator
{
    void* Allocate(const Csm::csmSizeType size);

    void Deallocate(void* memory);

    void* AllocateAligned(const Csm::csmSizeType size, const Csm::csmUint32 alignment);

    void DeallocateAligned(void* alignedMemory);
};
