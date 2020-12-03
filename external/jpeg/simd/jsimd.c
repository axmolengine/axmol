// By halx99, This file is only workaround for build ios combined archs
#if defined(__arm__)
    #include "arm/jsimd.c"
#else // assume arm64
    #include "arm64/jsimd.c"
#endif
