this is a fork of axmol engine.
add webgl build surport.

build step:
1. run emsdk_env.bat

2. emcmake cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
   
3. ninja






todo:
1.
#ifndef EMSCRIPTEN
        const FT_Int spread = DistanceMapSpread;
        FT_Property_Set(_FTlibrary, "sdf", "spread", &spread);
        FT_Property_Set(_FTlibrary, "bsdf", "spread", &spread);
#endif
