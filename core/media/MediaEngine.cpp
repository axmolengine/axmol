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
#elif defined(__linux__) && defined(AX_ENABLE_VLC_MEDIA)
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

std::unique_ptr<MediaEngineFactory> CreatePlatformMediaEngineFactory()
{
#if defined(WINAPI_FAMILY)
#    if WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP && !defined(AXME_USE_IMFME)
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<WmfMediaEngineFactory>());
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
