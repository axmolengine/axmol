// By halx99, This file is only workaround for build ios combined archs
#if defined(__arm__)
    #include "arm/jsimd_neon.S"
#elif defined(__arm64__)
    #include "arm64/jsimd_neon.S"
#endif
