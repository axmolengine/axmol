/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmol.dev/

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 ****************************************************************************/

#include "MediaEngine.h"

#if defined(WINAPI_FAMILY)
#    if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP && !defined(AXME_USE_IMFME)
#        include "media/WmfMediaEngine.h"
#    else
#        include "media/MfMediaEngine.h"
#    endif
#elif defined(__APPLE__)
#    include "media/AvfMediaEngine.h"
#elif defined(__ANDROID__)
#    include "media/AndroidMediaEngine.h"
#endif

#if defined(AX_ENABLE_VLC_MEDIA)
#    include "media/VlcMediaEngine.h"
#endif

namespace axstd
{
// static_pointer_cast overload for std::shared_ptr
using std::static_pointer_cast;

// static_pointer_cast overload for std::unique_ptr
template <class T, class U>
std::unique_ptr<T> static_pointer_cast(std::unique_ptr<U>&& r)
{
    (void)static_cast<T*>(static_cast<U*>(0));

    typedef typename std::unique_ptr<T>::element_type E;

    return std::unique_ptr<T>(static_cast<E*>(r.release()));
}

}  // namespace axstd

NS_AX_BEGIN

std::unique_ptr<MediaEngineFactory> MediaEngineFactory::create()
{
#if defined(WINAPI_FAMILY)
#    if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP && !defined(AXME_USE_IMFME)
#        if defined(AX_ENABLE_MFMEDIA)
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<WmfMediaEngineFactory>());
#        elif defined(AX_ENABLE_VLC_MEDIA)
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<VlcMediaEngineFactory>());
#        endif
#    else
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<MfMediaEngineFactory>());
#    endif
#elif defined(__APPLE__)
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<AvfMediaEngineFactory>());
#elif defined(__ANDROID__)
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<AndroidMediaEngineFactory>());
#elif defined(__linux__) && defined(AX_ENABLE_VLC_MEDIA)
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<VlcMediaEngineFactory>());
#else
    return nullptr;
#endif
}

NS_AX_END
