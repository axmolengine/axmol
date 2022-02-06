#pragma once

#include <Unknwn.h>
#include <wrl/client.h>

namespace MFUtils
{

template <typename _Ty>
class TComPtr : public Microsoft::WRL::ComPtr<_Ty>
{
    using _Mytype = TComPtr<InterfaceType>;
public:
    static _Mytype MakeObject()
    {
        _Mytype ptr;
        InterfaceType** pp = &ptr;
        *(pp)              = new InterfaceType();
        return ptr;
    }
};

const char* SubTypeToString(const GUID& SubType);
const char* MajorTypeToString(const GUID& MajorType);
const char* ResultToString(HRESULT hr);
}  // namespace MFUtils
