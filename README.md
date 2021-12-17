# adxe
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/1c5628dea478449ea0c6e1b0e30c3be9)](https://app.codacy.com/gh/adxeproject/adxe?utm_source=github.com&utm_medium=referral&utm_content=adxeproject/adxe&utm_campaign=Badge_Grade_Settings)
[![dev](https://img.shields.io/badge/v1.0.0-beta4-yellow.svg)](https://github.com/adxeproject/adxe/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/adxeproject/adxe/blob/master/LICENSE)
  
[![Windows Build Status](https://github.com/adxeproject/adxe/actions/workflows/windows-ci.yml/badge.svg)](https://github.com/adxeproject/adxe/actions/workflows/windows-ci.yml)
[![Android Build Status](https://github.com/adxeproject/adxe/workflows/android/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Aandroid)
[![iOS Build Status](https://github.com/adxeproject/adxe/workflows/ios/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Aios)
[![Linux Build Status](https://github.com/adxeproject/adxe/workflows/linux/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Alinux)
[![macOS Build Status](https://github.com/adxeproject/adxe/workflows/osx/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Aosx)  

**This is another more radical fork of *Cocos2d-x-4.0*, use OpenAL for all platforms, single texture multi GPU texture handler, C++ 14/17 and some more (see 'Highlight Features' for more info).**  

### View code with vscode online: 
- [![github1s](https://img.shields.io/badge/github1s-green.svg)](https://github1s.com/adxeproject/adxe)
- [![github1s](https://img.shields.io/badge/vscode.dev-green.svg)](https://vscode.dev/github/adxeproject/adxe)
  
  
**[简体中文](README_CN.md)**
  
### Purpose Summary
* C++ 14/17
* Focus on native game dev (quick starting, easy to use, fast)
* Bugfixes ASAP
* Usefull PRs from **you** are welcome (review/merge ASAP)
  
### Highlight Features
* Windows x64 build support
* Reimplement HttpClient based on yasio for process http request concorrently.
* **More clearly third-party libs ['Upstream-Version-License'](thirdparty/README.md) for easy to publish your commercial apps based on adxe. Also some links to third party libs which supporting adxe too.**
* Refactor AudioEngine, OpenAL for all platforms
  * [OpenAL Soft](https://github.com/kcat/openal-soft), pass -DBUILD_DEP_ALSOFT=ON to cmake to force enable it
  * [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL), if no ```BUILD_DEP_ALSOFT``` option specified, cmake script will choose it on osx/ios, even through it was mark as deprecated, but still available.
* Refactor UserDefault with [mio](https://github.com/mandreyel/mio)
* Modularize all optional extension, move from engine core to folder extensions
* Implement all .wav formats supported by ```OpenAL Soft```, such as MS-ADPCM, ADPCM, ...
* Use modern GL loader ```Glad```
* Google [angle](https://github.com/google/angle) renderer backend support
* C++ 14 standard
* IOS SDK 9.0 as minimal deployment
* Use fast pugixml
* Use [curl](https://github.com/curl/curl) for transferring data with URL syntax
* Use SAX parser for all plist file
* Spine-3.8 support
* Extension ```FairyGUI``` support
* ASTC 4x4/6x6/8x8 support (if hardware decoder not present, use software decoder)
* ETC2 RGB/RGBA support    (if hardware decoder not present, use software decoder)
* Supported 2D physics engines (see also [Wiki](https://github.com/adxeproject/adxe-wiki)):
  * Box2D
  * Box2D-optimized
  * Chipmunk2D 
* Supported 3D physics engines:
  * Bullet Physics SDK
* **ImGui integrated, easy to write game embedded tools, very ease to use, please read [ImGuiEXT](extensions/ImGuiEXT/README.md)**

Open [Wiki](https://github.com/adxeproject/adxe-wiki) for additional information and see [Milestones](https://github.com/adxeproject/adxe/milestones) for planed feature too.

### Quick Start

#### Common Requirement [Python](https://www.python.org/downloads/)
  * Python-2.7.17+, Python-3.7+ 

#### Prerequisites
  1. Enter ```adxe``` root directory
  2. Run ```python setup.py```, restart console after finish

#### Windows (64/32 bit  Visual Studio 2019/2022)
  1. Install [CMake](https://cmake.org/) 3.14+  
  2. Install Visual Studio 2019/2022 (it's recommend you use only this versions)  
  3. Execute follow command at command line (Console, Window Terminal or Powershell)
  
     ```cd adxe ```
     - for 32 bit Visual Studio 2019:
     ```cmake -S . -B build -G "Visual Studio 16 2019" -A Win32```
     - for 64 bit Visual Studio 2019:
     ```cmake -S . -B build -G "Visual Studio 16 2019" -A x64```
     - for 32 bit Visual Studio 2022:
     ```cmake -S . -B build -G "Visual Studio 17 2022" -A Win32```
     - for 64 bit Visual Studio 2022:
     ```cmake -S . -B build -G "Visual Studio 17 2022" -A x64```
  
  Build excecutable on command line (e.g. cpp-tests):
    ```msbuild .\build\adxe.sln -target:cpp-tests -maxCpuCount```

#### Android
  1. Install Android Studio 4.2+
  2. When first start Android Studio, It will guide you to install sdk and other tools, just install them
  3. Start Android and choose [Open an existing Android Studio Project] to open ```adxe\tests\cpp-tests\proj.android```
  4. Start Android Studio and Open [Tools][SDKManager], then switch to ```SDK Tools```, check the ```Show Package Details```, choose follow tools and click the button ```Apply``` to install them:  
     * Android SDK Platform 29 r5  
     * Android SDK Build-Tools 29.0.2  
     * NDK r19c+  
     * CMake 3.10+  
  5. Waiting for ```Gradle sync``` finish.
  6. Remark: If you use non-sdk provided CMake edition, you need download ```ninja``` from https://github.com/ninja-build/ninja/releases, and copy ```ninja.exe``` to cmake's bin directory

#### iOS
  1. Ensure xcode11+ & [cmake3.21+](https://github.com/Kitware/CMake/releases) installed, install cmake command line support: ```sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install```
  2. Execute follow command   
  ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```  
  3. Generate xcode project  
     - for any device   
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake```
     - for arm64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake -DCMAKE_OSX_ARCHITECTURES=arm64```
     - for armv7,arm64 combined:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake "-DCMAKE_OSX_ARCHITECTURES=armv7;arm64"```
     - for simulator x86_64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake -DCMAKE_OSX_ARCHITECTURES=x86_64``` 

  4. After cmake generate finish, you can open xcode project at ```build``` folder and run cpp-tests or other test targets.  
  5. Notes  
     - **The code sign required to run ios app on device, just change bundle identifier until the auto manage signing solved**  
     - **adxe only provide armv7, arm64, x86_64 prebuilt libraries for ios**

### New Project
- Cpp: `adxe new -p org.adxe.hellocpp -d D:\dev\projects\ -l cpp --portrait HelloCpp`
- Lua: `adxe new -p org.adxe.hellolua -d D:\dev\projects\ -l lua --portrait HelloLua`

### Notes
  * ThreadLocalStorage (TLS) 
    - ios x86 simulator ios>=10 and adxe no longer provide x86 libraries
    - ios x64 or devices (armv7, arm64) ios sdk>=9.0
    - the 'OpenAL Soft' maintained by kcat use TLS

### Reference links
  * Official Cocos2d-x: https://github.com/cocos2d/cocos2d-x

### Contributing guide
https://github.com/adxeproject/adxe/discussions/411

### The adxe Active Contributors

- @halx99
- @rh101
- @aismann
- @weiwest

### Fullly changes since cocos2d-x-4.0

- [HIGHLIGHT] Implement Windows WebView using WebView2 Edge Chromium, thanks to @rh101
- [HIGHLIGHT] Windows x64 build support
- [HIGHLIGHT] Support custom texture atlas formats, thanks to @rh101
- [HIGHLIGHT] Downloader realtime md5 checksum calculation support
- [HIGHLIGHT] Decompress astc parallel support
- [HIGHLIGHT] Reimplement HttpClient based on yasio for concurrent http request support
- [HIGHLIGHT] Improve Lua RTTI performance and less memory cost
- [HIGHLIGHT] Virtual File System support, thanks to @rh101 
- [HIGHLIGHT] Refactor lua loader, speed up 30%+
- [HIGHLIGHT] Update plainlua version to 5.4.3
- [HIGHLIGHT] Use Openal-Soft for all platform
- [HIGHLIGHT] Refactor UserDefault with mio, speed up 100x+
- [HIGHLIGHT] Implement all .wav formats supported by Openal-Soft, such as MS-ADPCM, ADPCM
- [HIGHLIGHT] Use modern GL loader Glad
- [HIGHLIGHT] Google angle renderer backend support
- [HIGHLIGHT] Update codebase to C++ 17 standard
- [HIGHLIGHT] Remove tinyxml2
- [HIGHLIGHT] Use fast pugixml
- [HIGHLIGHT] Spine-3.6~4.0 support
- [HIGHLIGHT] ASTC 4x4/6x6/8x8 support (if hardware decoder not present, use software decoder)
- [HIGHLIGHT] ETC2 RGB/RGBA support (if hardware decoder not present, use software decoder)
- [HIGHLIGHT] ImGui integrated for PC platforms
- [HIGHLIGHT] Extension FairyGUI support
- [HIGHLIGHT] Use curl for transferring data with URL syntax
- [HIGHLIGHT] Modularize all optional extension, move from engine core to folder extensions
- [HIGHLIGHT] Improve thirdparty libs building, 99% of them build from sources or github actions with latest toolchain, see also: `adxeproject/buildware`
- [HIGHLIGHT] Add new API `Director::setChildrenIndexerEnabled` for speed up getChildByTag & getChildByName support
- [HIGHLIGHT] Add new API `FontFreeType::setStreamParsingEnabled` for stream parsing support, it's very useful for reduce memory cost when load large .ttf font file
- [HIGHLIGHT] Remove all unnecessary `std::nothrow` stubs
- [NEW] Add int64_t/uint64_t support for `cocos2d::Value`
- [FIX] Fix uniform location mismatch when more than 1 spine with different shaders  
- [FIX] Fix imgui draw frame cause afterimage when game scene nothing to draw
- [FIX] Set global Z value of label debug layer to be the same as the parent label to fix display issue
- [FIX] Use TTF scaled metrics to calculate line height, thanks to @rh101
- [FIX] Fix Memory leak in ShaderModuleGL::getErrorLog(), thanks to @Xrysnow
- [FIX] Fix console output text encoding for win32
- [FIX] Fix charset process for fontName on win32
- [FIX] Fix Crash on ParticleBatchNode::updateProgramStateTexture() 
- [FIX] Fix Crash on SpriteBatchNode::appendChild when CC_SPRITE_DEBUG_DRAW==1
- [FIX] Fix Lua can't get `unsigned char` or `unsigned int` values of `ValueMap`
- [FIX] Fix crash on AudioEngine::end after the `Director` was destroyed
- [FIX] Fix font atlas will leak when it does not exist in the atlas cache, thanks to @rh101
- [FIX] Fix ScrollView shows incorrect position in vertical direction some time, thanks to @wzhengsen
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
- [REFINE] Rename command line to to `adxe`
- [REFINE] Update imgui to 1.84 WIP
- [REFINE] Rename thirdparty folder `external` to `thirdparty`
- [REFINE] Building plainlua as dll on windows always for debugger happy to debug app quickly
- [REFINE] Fix some audio test case can't hear sound
- [REFINE] Avoid OS_WINDOWS conflict with system macro
- [REFINE] Update OpenSSL to v3.0.1
- [REFINE] Update FreeType to latest
- [REFINE] Use Freetype SDF render when label distance field enabled
- [REFINE] Always enable string pooling for msvc on cmake
- [REFINE] Change DrawNode api color parameters from `Color4F` to low `Color4B`
- [REFINE] Improve DrawNode GC allocs
