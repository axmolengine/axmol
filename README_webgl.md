# building wasm

Building axmol for target platform: webgl aka wasm.

After setup, just run follow command:

axmol build -p wasm

## TODO:

```cpp
#ifndef EMSCRIPTEN
        const FT_Int spread = DistanceMapSpread;
        FT_Property_Set(_FTlibrary, "sdf", "spread", &spread);
        FT_Property_Set(_FTlibrary, "bsdf", "spread", &spread);
#endif
```
