// By halx99, This file is only workaround for build ios combined archs
#if defined(__APPLE__)
    #if defined(__arm__)
        #include "arm/jsimd.c"
    #elif defined(__arm64__)
        #include "arm64/jsimd.c"
    #endif
#endif
