#include "MediaEngine.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
#    include "media/WmfMediaEngine.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#    include "media/MfMediaEngine.h"
// #elif defined(__APPLE__)
// #    include "media/AvfMediaEngine.h"
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
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<WmfMediaEngineFactory>());
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<MfMediaEngineFactory>());
// #elif defined(__APPLE__)
//     return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<AvfMediaEngineFactory>());
#else
    return nullptr;
#endif
}

NS_AX_END
