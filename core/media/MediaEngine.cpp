#include "MediaEngine.h"

#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
#    include "media/WmfMediaEngine.h"
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
#    include "media/MfMediaEngine.h"
#endif

namespace axstd
{
// static_pointer_cast overload for std::shared_ptr
using std::static_pointer_cast;

// dynamic_pointer_cast overload for std::shared_ptr
using std::dynamic_pointer_cast;

// const_pointer_cast overload for std::shared_ptr
using std::const_pointer_cast;

// reinterpret_pointer_cast overload for std::shared_ptr
template <class T, class U>
std::shared_ptr<T> reinterpret_pointer_cast(const std::shared_ptr<U>& r)
{
    (void)reinterpret_cast<T*>(static_cast<U*>(0));

    typedef typename std::shared_ptr<T>::element_type E;

    E* p = reinterpret_cast<E*>(r.get());
    return std::shared_ptr<T>(r, p);
}

// static_pointer_cast overload for std::unique_ptr
template <class T, class U>
std::unique_ptr<T> static_pointer_cast(std::unique_ptr<U>&& r)
{
    (void)static_cast<T*>(static_cast<U*>(0));

    typedef typename std::unique_ptr<T>::element_type E;

    return std::unique_ptr<T>(static_cast<E*>(r.release()));
}

// dynamic_pointer_cast overload for std::unique_ptr
template <class T, class U>
std::unique_ptr<T> dynamic_pointer_cast(std::unique_ptr<U>&& r)
{
    (void)dynamic_cast<T*>(static_cast<U*>(0));

    // BOOST_STATIC_ASSERT_MSG(boost::has_virtual_destructor<T>::value, "The target of dynamic_pointer_cast must have a
    // virtual destructor.");

    T* p = dynamic_cast<T*>(r.get());
    if (p)
        r.release();
    return std::unique_ptr<T>(p);
}

// const_pointer_cast overload for std::unique_ptr
template <class T, class U>
std::unique_ptr<T> const_pointer_cast(std::unique_ptr<U>&& r)
{
    (void)const_cast<T*>(static_cast<U*>(0));

    typedef typename std::unique_ptr<T>::element_type E;

    return std::unique_ptr<T>(const_cast<E*>(r.release()));
}

// reinterpret_pointer_cast overload for std::unique_ptr
template <class T, class U>
std::unique_ptr<T> reinterpret_pointer_cast(std::unique_ptr<U>&& r)
{
    (void)reinterpret_cast<T*>(static_cast<U*>(0));

    typedef typename std::unique_ptr<T>::element_type E;

    return std::unique_ptr<T>(reinterpret_cast<E*>(r.release()));
}
}  // namespace axstd

NS_AX_BEGIN

std::unique_ptr<MediaEngineFactory> CreatePlatformMediaEngineFactory()
{
#if AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<WmfMediaEngineFactory>());
#elif AX_TARGET_PLATFORM == AX_PLATFORM_WINRT
    return axstd::static_pointer_cast<MediaEngineFactory>(std::make_unique<MfMediaEngineFactory>());
#else
    return nullptr;
#endif
}

NS_AX_END
