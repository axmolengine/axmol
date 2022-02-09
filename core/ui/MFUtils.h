#pragma once

#include <Unknwn.h>
#include <wrl/client.h>

namespace MFUtils
{

template <typename _Ty>
using TComPtr = Microsoft::WRL::ComPtr<_Ty>;

template <typename _Ty>
inline TComPtr<_Ty> MakeComInstance()
{
    TComPtr<_Ty> obj;
    _Ty** ppv = &obj;
    *ppv      = new _Ty();
    return obj;
}

template <typename _Ty>
inline TComPtr<_Ty> ReferencedPtrToComPtr(_Ty* ptr)
{
    TComPtr<_Ty> obj;
    _Ty** ppv = &obj;
    *ppv      = ptr;
    return obj;
}

}  // namespace MFUtils
