# axmol-2.1.0 ?? 2023

### Significant changes relative to 2.0.0:

- Migrate Windows UWP from C++/CX to [cppwinrt](https://github.com/microsoft/cppwinrt) for c++20 codebase
- Refactor Target Platfroms Enums
- Update AGP to 8.x Android Studio 2022.3.1+
- Add support enable TTF SDF rendering globally
- Add extension: SDFGen to generate prebake font atlas
- Add API: `FontAtlasCache::preloadFontAtlas` to support label SDF rendering with prebake atlas
- Add API `Device::getPixelRatio`
- Add ImGui android DPI scaling support
- Add winuwp FPS control support
- Add API: `compressGZ`, `decompressGZ`
- Combine `backend::Device` and `backend::DeviceInfo` into `backend::DriverBase`
- Improve axmol console scripts
- Rebase main repo, repo size reduce from 1.1GB+ to ~80MB
  - Add simple cmake pkg manager aka `1k/fetch.cmake` to download all prebuilts,optionals,sample-assets at cmake config step
  - pkg config is in `$AX_ROOT/manifest.json`
  - Download cache dir is `$AX_ROOT/cache`
  - Also support simple mirror to gitee.com for china region via create a empty file name with `.gitee` in dir `$AX_ROOT/1k/`
- Add extension: JSONDefault by @sarooptech
- Add virtual file system support to WICImageLoader by @rh101
- Add support for Richtext vertical alignment by @rh101
- Update android API to 34
- Add support for ActionCoroutine like Unity by @lich426
- Add API: `DrawNode::drawPie` by @aismann
- Add support for window resized and positioned by @lich426
- Add extension: Inspector by @iAndyHD3

### BugFixs

- Fix MSL texture order does not follow GLSL binding order
- Fix GLES3 shader scalar array layout incorrect
- Fix Android ImGUI coord convertions
- Fix Lua build on windows
- Fix `axmol new` not support absolute path
- Fix some ImGui issues
  - Fix crash
  - Fix wasm touch event not handle properly
  - Fix add/remove render loop problem
- Fix wasm HelloLua/lua_tests error
- Fix wasm render system font aliasing problem, caused by incorrect premultiply alpha
- Fix text rendering layout incorrect due to kernings not multiply content scale factor
- Fix wasm RandomHelper always return 0 problem
- Fix label outline not work on metal graphics backend
- Fix command `axmol build` can't build android on macos
- Fix cmake may geneate 2 targets for sync axslc folder
- Fix windows prebuilt cache issue
- Fix Lua `FileUtils getDataFromFile` may return path of file
- Fix issue where RichText URL touch listener does not activate on touch by @rh101
- Fix for missing scripts and resources in lua tests project by @rh101
- Fix for UWP build issues by @rh101
- Fix UWP issues in `EditBoxWinRT` by @rh101
- Fix for TextField keyboard not opening on receiving focus on android by @rh101
- Fix plist data and version by @paulocoutinhox
- Fix build cannot correct handle '-' on the project name
- Fix TMX Camera Culling by @DelinWorks
- Fix Fix cmake project generation on platforms other than Android by @rh101

### Improvements

- Reduce memory usage when using getFileMD5Hash function by @rh101
- Improve install-pwsh.sh
- Apple platforms require cmake-3.28.0
- Rename `AX_USE_SHARED_PREBUILT` to `BUILD_SHARED_LIBS` in AXLinkHelper.cmake
- Rename GLFW view name to `axmol2`
- Remove Apple Controller deprecated .gamePad check branch
- Remove dep: libzip-dev for linux
- Auto setup llvm for win32 target platform
- Optimize Label memory size, reduce 40 bytes
- Support adding custom properties to the inspector for derived types by @rh101
- Use std containers instead uthash
- Imgui and Inspector enhancements by @rh101
- [Android] Reset the OpenGL state cache if the context is lost by @rh101
- [Android] Fix corrupted FPS stats font textures on OpenGL context lost by @rh101
- Update Live2D to work with Axmol 2.x by @rh101
- [Android] Enable support for restarting application process if OpenGL context is lost by @rh101
- Make AxmolEditBox invisible by default by @rh101
- [UWP] Use Package to get app version information by @rh101
- Improve `class DrawNode` by @aismann
- [CMake] Use execute_process instead of deprecated exec_program by @rh101
- Move fmt folder to Thirdparty folder within project by @rh101
- Remove duplicated libz for ios/tvos
- Improve info.plist for apple platforms by @paulocoutinhox
- Improve android build.gradle by @paulocoutinhox
- Fix large number of generated warnings by @rh101
- Add useful chipmunk function to `axmol's internal physics engine` by @DelinWorks
- Replace 'CCAssert' with 'AXASSERT' by @aismann
- Improve the default templates by @aismann
- Call Android native renderer while opengl context is still valid by @rh101
- Make `FileUtils::openFileStream` a const method by @rh101
- Improve cmake scripts for managing 3rd libs
- Add `Get TMXTilesetInfo` by name support by @lich426

### Break changes

- Rename glview to correct representative name: `OpenGLView` ==> `GLView` by @paulocoutinhox

### 3rdparties updates

- astcenc: 4.5.0 ==> 4.6.1
- c-ares: 1.19.1 ==> 1.23.0 (1.24.0 available)
- clipper2: 1.2.2 ==> 1.3.0
- curl: 8.2.1 ==> 8.5.0
- jpeg-turbo: 3.0.0 ==> 3.0.1
- llhttp: 9.0.1 ==> 9.1.3
- ntcvt: 1.0.0 ==> 1.0.1
- oboe: 1.7.0 ==> 1.8.0
- openssl: 3.0.10 ==> 3.0.12
- rapidjson: 1.0.0 ==> 1.0.0-b4a6da3
- simdjson(NEW): 3.6.3
- webp: 1.3.1 ==> 1.3.2
- yasio: 4.1.0 ==> 4.3.0

Notes:

- WASM require win8.1+ on windows

## axmol-2.0.0 Sep.9 2023

-  Add *experimental* WebAssembly build support(WebGL 2.0) by [@nowasm](https://github.com/nowasm)
    - List of known issues maybe *help wanted*
        - WebGL context lost not handled yet
     - A demo avaiable on axmol gh-pages: [`cpp-tests.html`](https://axmolengine.github.io/axmol/wasm/cpp_tests/cpp_tests.html), build from branch `dev`
     - Note:  Follow functionals require wasm pthread support, and needs server enable https://web.dev/coop-coep/ response with specified header, and by default thread support was enabled by axmol client build commands
         - AudioEngine
         - ASTC multi-thread software decoder
         - `TextureCache::addImageAsync`
         - `MeshRenderer::createAsync`
         -  Extension: live2d not support due to it's sdk core not opensource and not contains wasm prebuilt libs
     - Quick build and run `cpp_tests` on local machine, the command use `emrun` to preview, and it's response with specified headers which wasm pthread support required, so above functionals with multi-threading works well.
         - `axmol run -p wasm -xb "--target,cpp_tests"`
     - Add 2 cmake options: `AX_WASM_THREADS`, `AX_WASM_SHELL_FILE`, refer to [CMakeOptions.md](CMakeOptions.md)
- OpenGL3/GLES3 support, add new cmake option `AX_GLES_PROFILE` to control GLES profile, refer to [#1279](https://github.com/axmolengine/axmol/issues/1279)
- Mesh instancing draw support
- Use [glslcc](https://github.com/axmolengine/glslcc)(a spriv-corss & glslang wrapper tool) as new shader workflow, write shader by ESSL310, build to target platforms: 
    - MSL for Apple(macOS/iOS/tvOS)
    - ESSL300 for Android, ANGLE
    - GLSL330 for Desktop GL
    - GLSL100 for Android old devices e.g android-4.2
    - Restrict ESSL310 input shader: please only define 1 uniform block at per shader stage
- Improve ProgramManager for loading custom shader program more easy
- Improve text rendering, label SDF outline support
- Improve FileStream, handle large 4GB+ files
- Improve ASTC hardware support detection
- Use ANGLE as default renderer library on win32
- Thirdparty Updates
    - ANGLE: 113.0.5672.128 ==> 115.0.5790.173
    - curl: 8.1.2 ==> 8.2.1
    - fmtlib: 10.0.0 ==> 10.1.0
    - FreeType: 2.13.0 ==> 2.13.2
    - llhttp: 8.1.1 ==> 9.0.1
    - OpenSSL: 3.0.9 ==> 3.0.10
    - webp: 1.3.0 ==> 1.3.1
    - xxHash: 0.8.1 ==> 0.8.2
    - zlib: 1.2.13 ==> 1.3
- API changes:
    - CHANGED: The prototype of [`ProgramManager::registerCustomProgram`](https://axmolengine.github.io/axmol/manual/latest/d1/db2/classax_1_1backend_1_1_program_manager.html#ab0c9fa9f0ebef5f20a868e2ea6cdc631) was changed
    - ADDED: `ProgramManager::loadProgram`
    - REMOVED: `ProgramManager::getCustomProgram`
    - REMOVED: `CommandBuffer::setLineWidth`
- Many other improvements
  
  
## axmol-1.0.1 Sep.8 2023

- [NEW] Add [`install-pwsh.sh`](https://github.com/axmolengine/axmol/blob/dev/install-pwsh.sh) to make installing powershell on `macOS`, `Ubuntu`, `ArchLinux` more easier
- [NEW] **Axmol console next**, now almost powershell (except luabinding generator required python3)
  - The axmol console commands `axmol run`, `axmol deploy` aslo support `winuwp`
  - *The axmol console commands not only used by axmol, it's could be used to build any other project based on cmake build system*, i.e, goto any cmake based project directory, then run command: ```axmol build -p win32 -xb "--target, <your_project_cmake_target>"```
  
- [NEW] `ArchLinux` now official supported by axmol community, means, with 2 steps: 
  1. `install-pwsh.sh`
  2. `pwsh .\setup.ps1`
then you can compile for targets: `linux` and `android` (apk) by commands in your axmol's project root directory
  - `axmol build -p linux` for linux
  - `axmol build -p android -a arm64` for android
- [REFINE] Lock android ndk revision to **r23c**
  
  
axmol-1.0.0  Aug.9 2023

- [HIGHLIGHT] Reimplemented MediaPlayer support play media(video/audio) files for all platforms, and ui::VideoPlayer was renamed to ui::MediaPlayer
- [HIGHLIGHT] Windows Store UWP apps support
- [HIGHLIGHT] Add build windows with llvm-clang + ninja support
- [HIGHLIGHT] Add extensions `Effekseer` support
- [HIGHLIGHT] Replace all string map with `robin_map` which support heterogeneous lookup
- [HIGHLIGHT] Add precompile_headers all platforms support, thanks to @crazyhappygame
- [HIGHLIGHT] Refactor `VertexLayout` sharing mechanism
- [HIGHLIGHT] Refactor shader program management: link when use, rename `ProgramCache` to `ProgramManager`
- [HIGHLIGHT] Improve MeshRenderer #879 thanks @solan-solan (#904) 
- [HIGHLIGHT] Improve model loader, auto set texture for .obj file when texture path not specified
- [HIGHLIGHT] Add apple M1, android x64 support, contributed by @pietpukkel
- [HIGHLIGHT] Add apple tvos support, thanks to @paulocoutinhox
- [HIGHLIGHT] Add Particle System Animations, Alpha Masks & Improvements, thanks to @DelinWorks
- [HIGHLIGHT] Add wireframe rendering and function classification improvements, thanks to @DelinWorks
- [HIGHLIGHT] Improve text rendering
- [HIGHLIGHT] Improve windows workflow, support linking with engine prebuilt libs
- [HIGHLIGHT] Improve spine RTTI compare performance when compiler string pooling enabled
- [HIGHLIGHT] Implement Windows WebView using WebView2 Edge Chromium, thanks to @rh101
- [HIGHLIGHT] Windows x64 build support
- [HIGHLIGHT] Support custom texture atlas formats, thanks to @rh101
- [HIGHLIGHT] Downloader realtime md5 checksum calculation support
- [HIGHLIGHT] Decompress astc parallel support
- [HIGHLIGHT] Reimplement HttpClient based on yasio for concurrent http request support
- [HIGHLIGHT] Improve Lua RTTI performance and less memory cost
- [HIGHLIGHT] Virtual File System support, thanks to @rh101 
- [HIGHLIGHT] Refactor lua loader, speed up 30%+
- [HIGHLIGHT] Update plainlua version to 5.4.4
- [HIGHLIGHT] Use Openal-Soft for all platform
- [HIGHLIGHT] Refactor UserDefault with mio, speed up 100x+
- [HIGHLIGHT] Implement all .wav formats supported by Openal-Soft, such as MS-ADPCM, ADPCM
- [HIGHLIGHT] Use modern GL loader glad (v2.0.2)
- [HIGHLIGHT] Add google angle (version 5672) renderer backend support for windows
- [HIGHLIGHT] Update codebase to C++ 17/20 standard
- [HIGHLIGHT] Remove tinyxml2
- [HIGHLIGHT] Use fast pugixml
- [HIGHLIGHT] Spine-3.6~4.1 support, default is 4.1
- [HIGHLIGHT] ASTC 4x4/6x6/8x8 support (if hardware decoder not present, use software decoder)
- [HIGHLIGHT] ETC2 RGB/RGBA support (if hardware decoder not present, use software decoder)
- [HIGHLIGHT] ImGui integrated for Android platform, thanks to @rh101
- [HIGHLIGHT] ImGui integrated for PC platforms
- [HIGHLIGHT] Add extension `FairyGUI` support
- [HIGHLIGHT] Add extension `Live2D` support
- [HIGHLIGHT] Use curl for transferring data with URL syntax
- [HIGHLIGHT] Modularize all optional extension, move from engine core to folder extensions
- [HIGHLIGHT] Improve thirdparty libs building, 100% of them build from sources or github actions with latest toolchain, see also: `axmolengine/buildware`
- [HIGHLIGHT] Add new API `Director::setChildrenIndexerEnabled` for speed up getChildByTag & getChildByName support
- [HIGHLIGHT] Add new API `FontFreeType::setStreamParsingEnabled` for stream parsing support . It's very useful for reducing memory costs when loading a large .ttf font file
- [HIGHLIGHT] Remove all unnecessary `std::nothrow` stubs
- [HIGHLIGHT] Use c++17 string_view instead `const std::string&`
- [HIGHLIGHT] Improve code quality (stage1.Oct) (#916) 
- [HIGHLIGHT] Remove CC prefix from code file name
- [NEW] Add closeKeyboard() method for EditBox, thanks to @theunwisewolf
- [NEW] Added automake and automake to dependency list, thanks to @clawfinger
- [NEW] Add primitive type support for mesh materials, thanks to @DelinWorks
- [NEW] Add int64_t/uint64_t support for `cocos2d::Value`
- [NEW] Add support for scaling for RichElementImage, thanks to @theunwisewolf
- [NEW] Add fmtlib 10.0.0
- [NEW] Add llhttp 8.1.0
- [NEW] Add return type 'ResourceData' for lua bindings, thanks to @tkzcfc
- [NEW] Add spine binary support for lua bindings, thanks to @tkzcfc
- [NEW] Add GL_RGBA32F GL backend support by @solan-solan
- [NEW] CI: add upload android apk support by @crazyhappygame
- [NEW] Add Anchored Sprite Class by @DelinWorks
- [NEW] Update app icon to Axmol logo by @rh101
- [NEW] Add HSV/HSL types support by @DelinWorks
- [NEW] Add support for haptic feedback on iOS and Android devices by @maksg
- [FIX] Fix for UIEditBox password on Mac/iOS (Montery+/15+), thanks to @Niborski
- [Fix] Fix to ensure Component::onEnter() is called (#867), thanks to @rh101
- [Fix] Fix Label overflow shrink bug
- [FIX] Fix uniform location mismatch when more than 1 spine with different shaders  
- [FIX] Fix imgui draw frame cause afterimage when game scene nothing to draw
- [FIX] Set global Z value of label debug layer to be the same as the parent label to fix display issue
- [FIX] Use TTF scaled metrics to calculate line height, thanks to @rh101
- [FIX] Fix Memory leak in ShaderModuleGL::getErrorLog(), thanks to @Xrysnow
- [FIX] Fix console output text encoding for win32
- [FIX] Fix charset process for fontName on win32
- [FIX] Fix crash on ParticleBatchNode::updateProgramStateTexture() 
- [FIX] Fix crash on SpriteBatchNode::appendChild when CC_SPRITE_DEBUG_DRAW==1
- [FIX] Fix Lua can't get `unsigned char` or `unsigned int` values of `ValueMap`
- [FIX] Fix crash on AudioEngine::end after the `Director` was destroyed
- [FIX] Fix font atlas will leak when it does not exist in the atlas cache, thanks to @rh101
- [FIX] Fix ScrollView shows incorrect position in vertical direction some time, thanks to @wzhengsen
- [FIX] Fix Spine ClippingAttachment doesn't work
- [FIX] Fix render problem with boss.obj
- [FIX] Fix downloader crash in lua coroutine, thanks to @tkzcfc
- [FIX] Fix memory leak (VertexLayout) at `Program` constructor
- [FIX] Fix missed edge case when resizing the memorymapped file in UserDefault that could cause a crash, thanks to  @theunwisewolf
- [FIX] Fix http post with uri params
- [FIX] Fix extension issue - FairyGUI BMFont don't load, reported by @theSlyest
- [FIX] Fix download task creation failure Program crash, reported by @tkzcfc
- [FIX] Fix charset process in glfwCharCallback (cocos2dx-all)
- [FIX] Fix positionColorLengthTexture.vert shader compilation, thanks to @DelinWorks
- [FIX] Fixed resize on Windows by @Niborski
- [FIX] Trim leading space from wrapped lines of text in RichText by @rh101
- [FIX] Fix TileMap OpenGL pixel-tolerance by @DelinWorks
- [FIX] Fix TileMap flipped/rotated animated tiles incorrect rendering by @DelinWorks
- [FIX] Fix TiledMap sprite position by @rh101
- [FIX] Fix for incorrect tile sprite setup on creation by @rh101
- [FIX] Fix return reference to local temporary value by @rh101
- [FIX] Fix TMX Tiled Map CSV, BASE64, & ZLIB incorrect parsing by @DelinWorks
- [FIX] Fix to ensure new line is correctly added to RichText by @rh101
- [FIX] Fix tilemap camera move and zoom culling by @DelinWorks
- [FIX] Fix some audio test case can't hear sound
- [FIX] Fix RenderTexture memory leak and increased memory usage when initWithWidthAndHeight is called by @rh101
- [REFINE] Improved error handling in shader initialization by @CodeAndWeb
- [REFINE] Move getWroldPosition from Widget to Node class, thanks to @rh101
- [REFINE] Improve `FileUtils::addSearchPath`, thanks to @tkzcfc
- [REFINE] Allow the forced reformatting of RichText contents, thanks to @rh101
- [REFINE] Upgrade android toolsets to ndk-r23 LTS
- [REFINE] CCValue move construct and assign with std::string support
- [REFINE] Improve windows dev workflow, use working directory instead copy resources to build binary directory
- [REFINE] Add lua debug project file to lua project template
- [REFINE] Remove lua 64bit spec search path, since we can compile compatible bytecode for both plainlua and luajit
- [REFINE] Put stats labels to safe origin to make sure we can see it integral at some mobile device
- [REFINE] Improve cmake scripts
- [REFINE] Replace deprecated repo `jcenter` with `mavenCentral`
- [REFINE] Lua-5.4 compatible
- [REFINE] Improve cmdline-tools
- [REFINE] Ensure label underline node has the same global Z value as the label node to fix visibility issue when global Z is not 0
- [REFINE] Improve android astc support check
- [REFINE] Improve the "Physics part" (#379), thanks to @aismann 
- [REFINE] Rename command line to to `axmol`
- [REFINE] Update imgui to 1.88
- [REFINE] Rename thirdparty folder `external` to `thirdparty`
- [REFINE] Building plainlua as dll on windows always for debugger happy to debug app quickly
- [REFINE] Avoid OS_WINDOWS conflict with system macro
- [REFINE] Update OpenSSL to v3.0.x latest
- [REFINE] Update FreeType to latest
- [REFINE] Use Freetype SDF render when label distance field enabled
- [REFINE] Always enable string pooling for msvc on cmake
- [REFINE] Change DrawNode api color parameters from `Color4F` to low `Color4B`
- [REFINE] Improve DrawNode GC allocs
- [REFINE] Use `jni.hpp` to improve jni call without method signature GC allocs
- [REFINE] Rename is64BitIOSDevice to is64BitMobileDevice
- [REFINE] Fix Android screen blank issue after unlocking device with application running, thanks to @rh101
- [REFINE] Update zlib to v1.2.13
- [REFINE] Add explicit cmake options
- [REFINE] Migrate cross platform ci checks to github actions
- [REFINE] Replace `push_back` with `emplace_back`
- [REFINE] Improve Camera, thanks to @DelinWorks
- [REFINE] Use jpeg-turbo for jpg image decoding
- [REFINE] Update clipper1 to clipper2 1.x
- [REFINE] Update flatbuffers to v2.0.8
- [REFINE] Change Program::getActiveAttributes return type to reference
- [REFINE] Rename `Sprite3D` to `MeshRenderer`
- [REFINE] Improve AutoPolygon::generateTriangles performance (#905)
- [REFINE] Add many box2d testbed, thanks to @aismann
- [REFINE] Improve c++11 for-loop performance with Universal References ([#760](https://github.com/axmolengine/axmol/pull/760)), thanks to @aismann
- [REFINE] New engine logo & splash
- [REFINE] Improve gradle scripts
- [REFINE] Replace use of deprecated sprintf with snprintf, thanks to @euler0
- [REFINE] Fix some warnings compilation in linux gcc 12.2.0, thanks to @Joilnen
- [REFINE] Add linux prebuilt support, thanks to @AsakuraMizu
- [REFINE] Fix wanings implicit conversion loses and use of deprecated function, thanks to @euler0
- [REFINE] Clarify the order of steps required to create and build new projects, thanks to @rh101
- [REFINE] Add a pool for GroupCommand rendering commands, thanks to @rh101
- [REFINE] Improve ci scripts, thanks to @crazyhappygame
- [REFINE] Improve lua and android cmake, thanks to @crazyhappygame
- [REFINE] Update libpng to 1.6.39
- [REFINE] Improve cpp template, thanks to @aismann
- [REFINE] Improve drawCircle rendering for big ring, thanks to @aismann
- [REFINE] Add windows dll build checks for GH actions
- [REFINE] Improve test naming by @DelinWorks
- [REFINE] Scrollbar visibility fix for when auto hide is disabled by @rh101
- [REFINE] Improve richtext word wrapping by @rh101
- [REFINE] Tidy project tree and full symlink support
- [REFINE] Impore Metal Renderer Dirty Detection by @Yehsam23
- [REFINE] Improve Android performance
- [REFINE] Add 1 overload AudioEngine::play2d for time offset support
- [REFINE] Improve exit flow
- [REFINE] Use ipv4 address preferred for console command server
- [REFINE] Reopen alsoft device when default device changed
- [REFINE] Factor in protected nodes when creating global Z node priority map for event dispatcher by @rh101
  
  
cocos2d-x-4.0  Dec.3 2019

- [HIGHLIGHT]     Support metal
- [HIGHLIGHT]     Use CMake for all platforms
- [REFINE]        Update glfw to 3.3
- [REFINE]        Update minizip to 1.2
- [CHANGE]        remove deprecated functions
- [CHANGE]        remove h5 engine and JSB
- [CHANGE]        remove tiff
- [CHANGE]        remove `experimental` namespace
- [FIX]           system font can not work correctly on macOS 15
- [FIX]           TextField can not work if using system input to get Chinese characters with iOS13
- [FIX]           UIWebView uses WKWebView instead
- [FIX]           VideoPlayer uses AVPlayerController instead
- [FX]            lua crashes on 64it devcices 
