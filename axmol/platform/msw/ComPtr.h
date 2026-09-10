/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <wrl/client.h>

namespace ax
{
using Microsoft::WRL::ComPtr;

template <typename T, unsigned int N>
inline void SafeRelease(T (&resourceBlock)[N])
{
    for (unsigned int i = 0; i < N; i++)
    {
        SafeRelease(resourceBlock[i]);
    }
}

template <typename T>
inline void SafeRelease(T& resource)
{
    if (resource)
    {
        resource->Release();
        resource = nullptr;
    }
}
}  // namespace ax
