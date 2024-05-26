## axmol-2.1.3 May.26 2024

### Significant changes relative to 2.1.2:

- Implement cross-platform media controller for video playback by @rh101 in https://github.com/axmolengine/axmol/pull/1845
  - Adjust media control positioning if video aspect ratio is maintained by @rh101 in https://github.com/axmolengine/axmol/pull/1851
  - Allow media controls to be rotated left or right by 90 degrees by @rh101 in https://github.com/axmolengine/axmol/pull/1910
- Allow certain code modules to be removed from build process by @rh101 in https://github.com/axmolengine/axmol/pull/1769, i.e. remove 3d features by `-DAX_ENABLE_3D=OFF`
- New logging system with general log level and colored support
  - Implement new axmol log system based on fmtlib by @halx99 in https://github.com/axmolengine/axmol/pull/1732
  - Logging colorful support by @halx99 in https://github.com/axmolengine/axmol/pull/1735
  - Improve logging color style by @halx99 in https://github.com/axmolengine/axmol/pull/1739
  - Add support for verbose trace level logging by @rh101 in https://github.com/axmolengine/axmol/pull/1764
  - Removed the '\n' which creates a second (empty) line. by @aismann in https://github.com/axmolengine/axmol/pull/1746
- Add wasm EditBox support
- Simplify axmol cmdlines build command, now you can use command `axmol` instead `axmol build` to build your project
- Linking prebuilt xcframework for apple platforms, please do clean `AX_ROOT` by `axmol -c -u` if you tracking axmol engine git repo
- Add unit tests by @smilediver in https://github.com/axmolengine/axmol/pull/1862
  - https://github.com/axmolengine/axmol/pull/1870
  - https://github.com/axmolengine/axmol/pull/1871
- Add AXLOGD,AXLOGI,... functions for lua
- Add axmol cmdline build option to specify build optimize flag: `-O0`(Debug), `-O1`(MinSizeRel), `-O2`(RelWithDebInfo), `-O3`(Release)

### Breaking Changes

- Rename `ax::Ref` ==> `ax::Object`
- Remove `ax::log`, use `AXLOGD` instead
- Remove cmake function: `ax_uwp_set_all_targets_deploy_min_version`
- Remove deprecated pixel formats L8, A8, LA8 in https://github.com/axmolengine/axmol/pull/1839
- Move `axmol/build.ps1` to `axmol/tools/cmdline/build.ps1`
- Rename ax::ccMessageBox to ax::messageBox
- Move ax::ccNextPOT to ax::utils::nextPOT
- Remove unused API: ax::LuaLog
- Change parameter 'sharedRT' of RenderTexture::create to false

### Mark as deprecated

- `2d/TextFieldTTF`
- `ui/UITextField`, use `ui/TextFieldEx` instead

### BugFixes

- Fix fullscreen incorrect in retina display by @halx99, reported by @TyelorD
- Fix same uniforms in fragment and vertex shaders not working in Metal by @smilediver in https://github.com/axmolengine/axmol/pull/1808
- Fix android build by @smilediver in https://github.com/axmolengine/axmol/pull/1723
- Fix EditBox error by @binxiaojiao in https://github.com/axmolengine/axmol/pull/1755
- Fix InputEvent in fairyGUI onKeyDown and onKeyUp by @binxiaojiao in https://github.com/axmolengine/axmol/pull/1757
- Fix wasm FPS control not working by @binxiaojiao in https://github.com/axmolengine/axmol/pull/1762
- Fix texture recreation when GL context is lost by @smilediver in https://github.com/axmolengine/axmol/pull/1763
- Fix DebugDraw issue in 3D physics by @halx99
- Fix OpenGLState cache error, reported by @solan-solan in https://github.com/axmolengine/axmol/issues/1759
- Fix linux crash after close window by @halx99
- Fix setup.ps1 not update AX_ROOT in unix profile by @halx99
- Fix a regression introduced by 2.1.2 that template gradlew permission incorrect
- Fix build android with axmol cmdline arch not working
- Fix crash on iOS by @smilediver in https://github.com/axmolengine/axmol/pull/1799
- Fix typo: BLEND_CLOLOR ==> BLEND_COLOR
- Fix typo: RESERVE_SUBTRACT ==> REVERSE_SUBTRACT
- Fix video player crash on apple platforms, reported by @asnagni in https://github.com/axmolengine/axmol/issues/1815
- Fix an issue where ProgressTimer didn't properly set or cascade its opacity by @TyelorD in https://github.com/axmolengine/axmol/pull/1830
- Fix ClippingNode stencil needing global z set manually by @TyelorD in https://github.com/axmolengine/axmol/pull/1831
- Fix FileUtilsApple::getPathForDirectory() checking wrong path by smilediver in https://github.com/axmolengine/axmol/pull/1828
- Fix issue where UIWidget couldn't be reselected by @TyelorD in https://github.com/axmolengine/axmol/pull/1827
- Fix glfw crash on macOS for large mouse button ids by @smilediver in https://github.com/axmolengine/axmol/pull/1835
- Fix 1k/fetch.cmake not working on powershell 5.1
- Fix spine two color tint data not being copied across to backend buffer on first frame by @rh101 in https://github.com/axmolengine/axmol/pull/1875
- Fix FileUtils issues on macOS by @smilediver in https://github.com/axmolengine/axmol/pull/1863
- Fix memory corruption bug and resulting crash in the TextFieldTTF by @TyelorD in https://github.com/axmolengine/axmol/pull/1890
- Fix shaders not copying to final build on macOS for non Xcode builds by @smilediver in https://github.com/axmolengine/axmol/pull/1908
- Fix system font blurring by in @DelinWorks in https://github.com/axmolengine/axmol/pull/1907
- Fix android ImGui crash due to invalid pointer by @rh101 in https://github.com/axmolengine/axmol/pull/1920

### Improvements

- Update prebuilt tool `glslcc` to 1.9.5, macos-arm64 support and built for macos-10.15+
- Improve MediaPlayer: handle video rotation properly
- Disable c++20 char8_t
- Improve build system, set rpath properly for platforms: linux, apple
- HttpClient: add support for PATCH method by @smilediver in https://github.com/axmolengine/axmol/pull/1722
- Add -f switch info to axmol command help output by @rh101 in https://github.com/axmolengine/axmol/pull/1729
- Adapt DrawNodeEx to the changes on: Move color types to math/Color.h axmolengine#1741 by @aismann in https://github.com/axmolengine/axmol/pull/1760
- Add Node World Space Positioning by @DelinWorks in https://github.com/axmolengine/axmol/pull/1743
- Move color types to math/Color.h by @halx99 in https://github.com/axmolengine/axmol/pull/1741
- Removing EditBox should not result in logging an error message by @rh101 in https://github.com/axmolengine/axmol/pull/1740
- Update LUA config for new build options by @rh101 in https://github.com/axmolengine/axmol/pull/1772
- Update TLD axmol.org -> axmol.dev
- Rename folder thirdparty ==> 3rdparty
- Update spine to 4.1-54fac9d
- Update axmol cmdline preferred cmake to 3.29.3+ for supress xcode 15 duplicated linking warnings
- Remove unnecessary cmake option: `AX_VS_DEPLOYMENT_TARGET`
- Make FileStream open file for write share flags same with cstd
- Fix some compile warnings
- Add ttf fallback render support
- Add cmake option: `AX_EXT_HINT` to set default extensions state
- Add axmol cmdline option `-dm` to dump compiler preprocessors before build
- Display cmake command used by build script in console by @rh101 in https://github.com/axmolengine/axmol/pull/1834
- Add Borderless Windowed Mode by @TyelorD in https://github.com/axmolengine/axmol/pull/1826
- Add CC_USE_CULLING Alias to cocos2d.h by @TyelorD in https://github.com/axmolengine/axmol/pull/1825
- Prevent repeated loading of plist files when loading csb files by @tkzcfc in https://github.com/axmolengine/axmol/pull/1844
- Remove Info.plist from mac Resources by @martinking71 https://github.com/axmolengine/axmol/pull/1849
- Add libvlc prebuilt entry CMakeLists.txt
- Add ability to create console apps by @smilediver in https://github.com/axmolengine/axmol/pull/1859
- Add support for ensuring sprite content size does not change on new texture by @rh101 in https://github.com/axmolengine/axmol/pull/1897
- Remove obsolete `box2d-optimized` support by @aismann in https://github.com/axmolengine/axmol/pull/1913
- Add macOS, Linux support for `tools/ci/genbindings.ps1`
- Fix `axmol` cmdline not raise error when cmake build fail
- Migrate wasm ci from appveyor to github actions
- Set AX_USE_METAL and AX_USE_GL to 1 if defined by @smilediver in https://github.com/axmolengine/axmol/pull/1916
- Update the CMake minimum version requirement to 3.23.0+ by @rh101 in https://github.com/axmolengine/axmol/pull/1918
- Add x86_64 to example list of possible target architectures for android by @rh101 in https://github.com/axmolengine/axmol/pull/1919
- Enable lua `bit` module for all lua versions supported by axmol
- Fix passing non null terminated strings for null terminated params by @smilediver in https://github.com/axmolengine/axmol/pull/1924
- Fix sol problem in emplace method by @paulocoutinhox in https://github.com/axmolengine/axmol/pull/1927
- Fix descriptor typo by @paulocoutinhox in https://github.com/axmolengine/axmol/pull/1928
- Do image processing to reverse premultiplied alpha in separate thread by @rh101 in https://github.com/axmolengine/axmol/pull/1929


### sdks & tools updates

- emsdk: 3.1.53 ==> 3.1.60
- AGP: 8.2.1 ==> 8.2.2
- androidx.media3: 1.0.2 ==> 1.2.1
- glslcc: 1.9.4 ==> 1.9.5

### 3rdparty updates

- simdjson: 3.7.0 ==> 3.9.2
- flatbuffers: 2.0.8 ==> 24.3.25
- curl: 8.6.0 ==> 8.7.1
- glad: 2.0.5 ==> 2.0.6
- yasio: 4.2.1 ==> 4.2.2
- llhttp: 9.2.0 ==> 9.2.1
- libwebp: 1.3.2 ==> 1.4.0
- astcenc: 4.7.0 ==> 4.8.0
- stb_image: 2.28 ==> 2.29
- luajit: 2.1-9cc2e42 ==> 2.1-d06beb0
- c-ares: 1.25.0 ==> 1.29.0
- imgui: 1.90.4 ==> 1.90.6

## axmol-2.1.2 Feb.25 2024

### BugFixes

- Fix screen size incorrect in retina display by @halx99 in https://github.com/axmolengine/axmol/pull/1692, reported by @IcemarkUK
- Fix view size incorrect when frameZoomFactor not 1.0 by @halx99 in https://github.com/axmolengine/axmol/pull/1694
- Fix lua build for Android, iOS and Windows by @rh101 in https://github.com/axmolengine/axmol/pull/1650
- Fix missing return statement by @rh101 in https://github.com/axmolengine/axmol/pull/1657
- Fix cross fade transition by @rh101 in https://github.com/axmolengine/axmol/pull/1664
- Fix Update CONTRIBUTING.md by @grif-on in https://github.com/axmolengine/axmol/pull/1673
- Fix crash during scene transition due to invalid camera by @rh101 in https://github.com/axmolengine/axmol/pull/1676
- Fix trim leading and trailing whitespace characters for RichText XML text by @rh101 in https://github.com/axmolengine/axmol/pull/1687
- Fix can't start lua project on iOS by @halx99
- Fix lua build for Android, iOS by @bintester in https://github.com/axmolengine/axmol/pull/1709
- Fix #1704, control android cmake options by engine axutils.gradle by @halx99 in https://github.com/axmolengine/axmol/pull/1705
- Fix #1703, unzip don't rely on zlib DLL export by @halx99
- Fix OSX sandbox network issue by @halx99, reported by @IcemarkUK
- Fix c-ares link issue
- Fix DrawNode luabinding color type mismatch by @halx99, reported by @qq461787436

### Improvements

- Change extension folder structure to reduce file name conflicts by @rh101 in https://github.com/axmolengine/axmol/pull/1647
- Committing luabindings for commit 9f8962d by @axmol-bot in https://github.com/axmolengine/axmol/pull/1648
- Use different global names and #undef the #defines to stay unity build friendly. by @j-jorge in https://github.com/axmolengine/axmol/pull/1653
- Add AX_USE_ALSOFT_STATIC CMake option to compile and link OpenAL Soft as a static library. by @j-jorge in https://github.com/axmolengine/axmol/pull/1654
- Ability to disable default implementations of HTTP and WebSocket clients by @rh101 in https://github.com/axmolengine/axmol/pull/1656
- Remove duplicate CODE_SIGN_IDENTITY that is setting invalid value by @rh101 in https://github.com/axmolengine/axmol/pull/1666
- Disable unused extensions in the FairyGUI and Live2D tests to speed up builds by @rh101 in https://github.com/axmolengine/axmol/pull/1665
- Update README.md by @danialias in https://github.com/axmolengine/axmol/pull/1668
- Spelling and formatting corrections for .md files by @danialias in https://github.com/axmolengine/axmol/pull/1671
- Update CONTRIBUTING.md by @grif-on in https://github.com/axmolengine/axmol/pull/1670
- Solved broken links in README.md by @danialias in https://github.com/axmolengine/axmol/pull/1677
- Ensure correct video mode settings are applied when creating view by @rh101 in https://github.com/axmolengine/axmol/pull/1678
- Solved more broken links in README.md by @grif-on in https://github.com/axmolengine/axmol/pull/1680
- Some additions and corrections for readme files. by @danialias in https://github.com/axmolengine/axmol/pull/1682
- RichText enhancements by @rh101 in https://github.com/axmolengine/axmol/pull/1683
- Add -j to specify build cpu count  support by @halx99 in https://github.com/axmolengine/axmol/pull/1688
- Ensure valid whitespace is not trimmed when html parsing mode is used by @rh101 in https://github.com/axmolengine/axmol/pull/1689
- Add support for window close button calback by @lich426 in https://github.com/axmolengine/axmol/pull/1697
- RichText and ScrollView enhancements by @rh101 in https://github.com/axmolengine/axmol/pull/1696
- Improve project templates by @halx99 in https://github.com/axmolengine/axmol/pull/1713
- [Android] Allow setting certain defines without modifying engine source by @rh101 in https://github.com/axmolengine/axmol/pull/1710
- Improve dll support by @halx99 in https://github.com/axmolengine/axmol/pull/1706
- Don't use fmt header only by @halx99 in https://github.com/axmolengine/axmol/pull/1707
- Don't use yasio header only by @halx99

### sdks updates

- cppwinrt: 2.0.230706.1 ==> 2.0.240111.5

### 3rdparty updates

- glad: 2.0.4 ==> 2.0.5
- yasio: 4.2.0 ==> 4.2.1
- llhttp: 9.1.3 == > 9.2.0
- oboe: 1.8.0 ==> 1.8.1
- libpng: 1.6.42 ==> 1.6.43
- glfw: git b4c3ef9 ==> 3.4
- simdjson: 3.6.4 ==> 3.7.0
- imgui: 1.90.1 ==> 1.90.4

## axmol-2.1.1 Feb.2 2024

### BugFixes

- Fix shaders not copying to final build on macOS for non Xcode builds by @smilediver
- Fix `Scheduler::pauseTarget` can't timer targets, reported by @shaniamjad6
- Fix build lua projects with engine prebuilts lib doesn't work, reported by @grif-on
- Fix GUI ScrollView clipping bug and add lua support by @bintester
- Ensure scissor state is set to the correct value prior to a buffer clear by @rh101
- Fix memory leaks when app exit on non-embed systems by @halx99 and @lich426
- Fix can't generate lua bindings for set/get Node localZOrder, reporeted by @bintester
- Fix lua template include incorrect module path, reported by @grif-on
- Fix build win32 with clang error
- Fix ci build-site download unexpected artifacts from appveyor

### Breaking changes

- Rename android entrypoint: `cocos_android_app_init` ==> `axmol_android_app_init`

### Improvements

- Add some utils functions and lua support by @bintester

### 3rdparties updates

- simdjson: 3.6.3 ==> 3.6.4
- curl: 8.5.0 ==> 8.6.0
- openssl: 3.0.12 ==> 3.0.13
- yasio: 4.1.4 ==> 4.2.0(5.0.0)
- zlib: 1.3 ==> 1.3.1
- jpeg-turbo: 3.0.1 ==> 3.0.2
- libpng: 1.6.40 ==> 1.6.42
- luajit: 2.1-43d0a19 ==> 2.1-9cc2e42


## axmol-2.1.0 Jan.20 2024

### Significant changes relative to 2.0.0:

- Migrate Windows UWP from C++/CX to [cppwinrt](https://github.com/microsoft/cppwinrt) for c++20 codebase by @halx99
- Refactor Target Platforms Enums
- Update AGP to 8.2.1 Android Studio 2023.1.1+
  - Update android API to 34
- Add support for enabling TTF SDF rendering globally by @halx99
- Add extension: SDFGen to generate prebaked font atlas by @halx99
- Add API: `FontAtlasCache::preloadFontAtlas` to support label SDF rendering with prebaked atlas by @halx99
- Add API `Device::getPixelRatio` by @halx99
- Add ImGui android DPI scaling support by @halx99
- Add winuwp FPS control support by @halx99
- Add API: `compressGZ`, `decompressGZ` by @halx99
- Combine `backend::Device` and `backend::DeviceInfo` into `backend::DriverBase` by @halx99
- Improve axmol console scripts by @halx99
- Rebase main repo, repo size reduce from 1.1GB+ to ~80MB by @halx99
  - Add simple cmake pkg manager aka `1k/fetch.cmake` to download all prebuilts,optionals,sample-assets at cmake config step
  - pkg config is in `$AX_ROOT/manifest.json`
  - Download cache dir is `$AX_ROOT/cache`
  - Also support simple mirror to gitee.com for china region via create a empty file name with `.gitee` in dir `$AX_ROOT/1k/`
- Add extension: JSONDefault by @sarooptech
- Add virtual file system support to WICImageLoader by @rh101
- Add support for Richtext vertical alignment by @rh101
- Add support for ActionCoroutine like Unity by @lich426
- Add API: `DrawNode::drawPie` by @aismann
- Add support for window resized and positioned by @lich426
- Add extension: Inspector by @iAndyHD3
- Add *experimental* WebSocket support for both wasm and native platforms by @halx99 & @rh101
- Add extension: DrawNodeEx by @aismann
- Add fairygui lua support by @Samoongeer
- Enable Mat4 SSE Istructions acceleration by @halx99
- Add dotnet API styled class `JsonWriter` based on rapidjson by @halx99
- Add donation page
- Add multi UserDefault support by @armanhossiny
- Add fixedUpdate method to Scene by @lich426

### Mark as deprecated, and will be removed in next release axmol-2.2.0
- 32bit build
- Extensions
    - DragonBones
    - GUI
- Zwoptex (Commented on cpp-test)

### BugFixes

- Fix translated metal shader texture order does not follow GLSL binding order
- Fix GLES3 shader scalar array layout incorrect
- Fix Android ImGUI coord conversions
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
- Fix build cannot correctly handle '-' on the project name
- Fix TMX Camera Culling by @DelinWorks
- Fix cmake project generation on platforms other than Android by @rh101
- Fix UILayout crash problem by @aismann
- Fix replace scene missing nullcheck for next scene by @armanhossiny
- Fix a crash caused by missing android view nullcheck by @Arvant
- Fix shader cache leak by @j-jorge
- Fix AXASSERT() not working with more complex msg arguments by @smilediver
- Fix assertion fail when re-create axmol imgui backend by @Samoongeer
- Fix AudioPlayer seek to incorrect frame after setTime invoked by @GuoliangWang


### Improvements

- Improve `setup.ps1`, auto set windows `powershell.exe` execution policy to `Bypass`
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
- Include hpp and c files in CMakeLists.txt of cpp template by @@lich426
- Improve gradle scripts, config each build-variants for easy add custom build-variant, refer #1571
- Allow to override AX_USE_COMPAT_GL from user's `CMakeLists.txt` by @smilediver
- Fix android gradle AGP8 warnings, and rename keystore props:
  - RELEASE_STORE_FILE ==> KEY_STORE_FILE
  - RELEASE_KEY_STORE_PASSWORD ==> KEY_STORE_PASSWORD
  - RELEASE_KEY_ALIAS ==> KEY_ALIAS
  - RELEASE_KEY_PASSWORD ==> KEY_PASSWORD
- Enable template projects' `aidl` by default for In-app purchases by @armanhossiny

### Breaking changes

- Rename glview to correct representative name: `OpenGLView` ==> `GLView` by @paulocoutinhox

### 3rdparties updates

- astcenc: 4.5.0 ==> 4.7.0
- c-ares: 1.19.1 ==> 1.25.0
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
- yasio: 4.1.0 ==> 4.1.4
- fmt: 10.1.1 ==> 10.2.1
- imgui: 1.89.8 ==> 1.90.1

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
- Use ANGLE(d3d11) as default renderer library on win32
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
- [HIGHLIGHT] Improve thirdparty libs building, 100% of them build from sources or github actions with latest toolchain, see also: `simdsoft/1kiss`
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
