// The stdfs workaround header, if not avaiable on current system, use ghc::filesystem
#pragma once

/* ios < 13 or ndk < 22, ghc as workaround */
#if (defined(__IPHONE_OS_VERSION_MIN_REQUIRED) && __IPHONE_OS_VERSION_MIN_REQUIRED < 130000) || \
    (defined(__NDK_MAJOR__) && __NDK_MAJOR__ < 22)
#    include "ghc/filesystem.hpp"
namespace stdfs = ghc::filesystem;
#else
#    include <filesystem>
namespace stdfs = std::filesystem;
#endif
