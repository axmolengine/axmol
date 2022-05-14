# The adxe thirdparty libraries
## angle
- Upstream: https://github.com/google/angle
- Version: git 2.1-e37380e (18155) on branch `chromium/4951` @Google Chrome 101.0.4951.41
- License: BSD WITH The ANGLE Project Authors

## astc
- Upstream: https://github.com/ARM-software/astc-encoder
- Version: 3.7
- License: Apache-2.0

## Box2D
- Upstream: https://github.com/erincatto/box2d
- Version: 2.4.1
- License: MIT

## Box2D-optimized
- Upstream: https://github.com/mtsamis/box2d-optimized
- Version: 7ef46c1
- License: MIT

## Bullet
- Upstream: https://github.com/bulletphysics/bullet3
- Version: 3.24
- License: zlib
- Update method: Compare `adxe/thirdparty/bullet` with `bullet3/src`

## c-ares
- Upstream: https://github.com/c-ares/c-ares
- Version: git 1.18.1-c25d4eb (2138)
- License: MIT

## Chipmunk2D
- Upstream: https://github.com/slembcke/Chipmunk2D
- Version: git 7.0.3-0cb05e7 {until Dec 16, 2021}
- License: MIT

## Clipper
- Upstream: https://sourceforge.net/projects/polyclipping/files/
- Version: 6.4.2
- License: BSL-1.0

## ConvertUTF
- Upstream: https://github.com/llvm/llvm-project
- Version: git 2946cd7 without LLVM-specific wrappers of the functions
- License: Apache-2.0 WITH LLVM-exception

## curl
- Upstream: https://github.com/curl/curl
- Version: 7.83.1
- License: Curl (MIT/X)

## flatbuffers
- Upstream: https://github.com/google/flatbuffers
- Version: 2.0.0
- License: Apache-2.0

## {fmt}
- Upstream: https://github.com/fmtlib/fmt
- Version: git 8.1.1-9ff91b1 (5518)
- License: MIT

## FreeType
- Upstream: https://www.freetype.org/
- Version: 2.12.1
- License: BSD-style (The FreeType Project)

## Glad
- Upstream: https://github.com/Dav1dde/glad
- Version: 0.1.34
- License: MIT

## glfw
- Upstream: https://github.com/glfw/glfw
- Version: git df8d7bc on 31 Dec 2021 (4398)
- License: zlib

## glsl-optimizer
- Upstream: https://github.com/aras-p/glsl-optimizer
- Version: git d78c3d2
- License: MIT

## jni.hpp
- Upstream: https://github.com/mapbox/jni.hpp
- Version: v4.0.0
- License: MIT-like

## jpeg (libjpeg-turbo)
- Upstream: https://github.com/libjpeg-turbo/libjpeg-turbo
- Version: 2.1.3
- License: BSD-style (IJG,BSD-3-Clause,zlib)

## kcp
- Upstream: https://github.com/skywind3000/kcp
- Version: 1.7
- License: MIT

## llhttp
- Upstream: https://github.com/nodejs/llhttp
- Version: 6.0.6
- License: MIT

## lua
- plainlua
  - Upstream: http://www.lua.org/
  - Version: 5.4.4
  - License: MIT (http://www.lua.org/license.html)
  
- sol2
  - Upstream: https://github.com/ThePhD/sol2
  - Version: 2.20.6 with vs2019 compile fixes
  - License: MIT

- luajit
  - Upstream: https://github.com/LuaJIT/LuaJIT
  - Version: git 2.1.0-91bc6b8 (2562)
  - License: MIT

- tolua
  - Upstream: https://github.com/LuaDist/toluapp
  - Version: 1.0.93
  - License: MIT
  
- lua-cjson
  - Upstream: https://github.com/simdsoft/lua-cjson
  - Version: 2.1.1
  - License: MIT

## lz4
- Upstream: https://github.com/lz4/lz4
- Version: git 1.9.3-bdc9d3b (2541)
- License: BSD 2-Clause

## minimp3
- Upstream: https://github.com/lieff/minimp3
- Version: git ca7c706(316)
- License: CC0-1.0

## mio
- Upstream: https://github.com/mandreyel/mio
- Version: git 8c0d3c7 (125)
- License: MIT

## ogg & vorbis
- ogg
  - Upstream: https://github.com/xiph/ogg 
  - Version: git 1.3.5
  - License: BSD-3-Clause

- vorbis
  - Upstream: https://github.com/xiph/vorbis
  - Version: git 1.3.7-83a82dd (1569)
  - License: BSD-3-Clause

## OpenAL Soft
- Upstream: https://github.com/kcat/openal-soft
- Version: 1.22.0
- License: LGPL-2.1

## OpenSSL
- Upstream: https://github.com/openssl/openssl
- Version: 3.0.3
- License: Apache-2.0

## png
- Upstream: https://github.com/glennrp/libpng
- Version: git 1.6.38-a37d483 (4098) with arm64 neon detect fix
- License: PNG Reference Library License version 2

## poly2tri
- Upstream: https://github.com/jhasse/poly2tri
- Version: git 3380f5c  (until 25 Mar 2022)
- License: BSD-3-Clause

## pugixml
- Upstream: https://github.com/zeux/pugixml
- Version: 1.12.1
- License: MIT

## rapidjson
- Upstream: https://github.com/Tencent/rapidjson
- Version: 1.1.0
- License: MIT

## recast
- Upstream: https://github.com/recastnavigation/recastnavigation
- Version: git 65b314a (747) with dtCrowd::getEditableAgentAnim
- License: zlib

## robin-map
- Upstream: https://github.com/Tessil/robin-map
- Version: 1.0.1 with modified for more compatible with stl
    - Modify `robin_map::iterator->second` to mutable same with `stl map/unordered_map`
    - Forward `robin_map::operator[]` key by `std::forward` same with `stl map/unordered_map`
- License: MIT

## stb (stb_image)
- Upstream: https://github.com/nothings/stb
- Version: 2.27
- License: MIT

## tinydir
- Upstream: https://github.com/cxong/tinydir
- Version: git 1.2.4-ec6bff2 (168)
- License: BSD-2-Clause

## unzip (minizip-1.2)
- Upstream: https://github.com/simdsoft/mz12
- Version: 1.2 with bugfixs & improvements
- License: zlib

## uthash 
- Upstream: https://github.com/troydhanson/uthash
- Version: 2.3.0
- License: BSD license (https://troydhanson.github.io/uthash/license.html)

## webp
- Upstream: https://github.com/webmproject/libwebp
- Version: 1.2.2
- License: Google Inc

## xsbase
- Upstream: https://github.com/simdsoft/xsbase
- Version: 1.0
- License: MIT

## xxHash
- Upstream: https://github.com/Cyan4973/xxHash
- Version: 0.8.1
- License: BSD-2-Clause

## yaml-cpp
- Upstream: https://github.com/jbeder/yaml-cpp
- Version: 0.7.0
- License: MIT

## yasio
- Upstream: https://github.com/yasio/yasio
- Version: 3.39.4
- License: MIT WITH Anti-996

## zlib
- Upstream: https://github.com/madler/zlib
- Version: 1.2.12
- License: zlib

## libiconv
- Upstream: https://savannah.gnu.org/projects/libiconv
- Version: 1.16.0
- License: LGPL-2.1

## ntcvt
- Upstream: https://github.com/simdsoft/ntcvt
- Version: git-8422188 (7)
- License: Apache-2.0

## Some third party libs supporting adxe too:
   - Live2D for cocos2d-x, with lua binding:  https://github.com/Xrysnow/cocos2d-x-live2d

