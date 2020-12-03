// By halx99, This file is only workaround for build ios combined archs
#if defined(__arm__)
    #include "arm/jsimd_neon.S"
#else // assume arm64
    #include "arm64/jsimd_neon.S"
#endif
