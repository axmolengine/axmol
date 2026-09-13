/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "axmol/media/MediaEngine.h"

#if defined(WINAPI_FAMILY)
#    if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP && !defined(AXME_USE_IMFME)
#        include "axmol/media/WmfMediaEngine.h"
#    else
#        include "axmol/media/MfMediaEngine.h"
#    endif
#elif defined(__APPLE__)
#    include "axmol/media/AvfMediaEngine.h"
#elif defined(__ANDROID__)
#    include "axmol/media/AndroidMediaEngine.h"
#endif

#if defined(AX_ENABLE_VLC_MEDIA)
#    include "axmol/media/VlcMediaEngine.h"
#endif

namespace tlx
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

}  // namespace tlx

namespace ax
{

std::unique_ptr<MediaEngineFactory> MediaEngineFactory::create()
{
#if defined(WINAPI_FAMILY)
#    if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP && !defined(AXME_USE_IMFME)
#        if defined(AX_ENABLE_MFMEDIA)
    return tlx::static_pointer_cast<MediaEngineFactory>(std::make_unique<WmfMediaEngineFactory>());
#        elif defined(AX_ENABLE_VLC_MEDIA)
    return tlx::static_pointer_cast<MediaEngineFactory>(std::make_unique<VlcMediaEngineFactory>());
#        endif
#    else
    return tlx::static_pointer_cast<MediaEngineFactory>(std::make_unique<MfMediaEngineFactory>());
#    endif
#elif defined(__APPLE__)
    return tlx::static_pointer_cast<MediaEngineFactory>(std::make_unique<AvfMediaEngineFactory>());
#elif defined(__ANDROID__)
    return tlx::static_pointer_cast<MediaEngineFactory>(std::make_unique<AndroidMediaEngineFactory>());
#elif defined(__linux__) && defined(AX_ENABLE_VLC_MEDIA)
    return tlx::static_pointer_cast<MediaEngineFactory>(std::make_unique<VlcMediaEngineFactory>());
#else
    return nullptr;
#endif
}

}  // namespace ax
