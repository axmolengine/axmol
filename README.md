# adxe

[![dev](https://img.shields.io/badge/v1.0.0-beta5-yellow.svg)](https://github.com/adxeproject/adxe/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/adxeproject/adxe/blob/master/LICENSE)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/1c5628dea478449ea0c6e1b0e30c3be9)](https://app.codacy.com/gh/adxeproject/adxe?utm_source=github.com&utm_medium=referral&utm_content=adxeproject/adxe&utm_campaign=Badge_Grade_Settings)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/adxeproject/adxe/pulls)
  
[![Windows Build Status](https://github.com/adxeproject/adxe/actions/workflows/windows-ci.yml/badge.svg)](https://github.com/adxeproject/adxe/actions/workflows/windows-ci.yml)
[![Android Build Status](https://github.com/adxeproject/adxe/workflows/android/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Aandroid)
[![iOS Build Status](https://github.com/adxeproject/adxe/workflows/ios/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Aios)
[![Linux Build Status](https://github.com/adxeproject/adxe/workflows/linux/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Alinux)
[![macOS Build Status](https://github.com/adxeproject/adxe/workflows/osx/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Aosx)  

**This is another more radical fork of *Cocos2d-x-4.0*, it has Full Support OpenAL for all platforms, single texture multi GPU texture handler, C++ 17 and more! (see 'Highlighted Features' for more info).**  

### View code with vscode online: 
- [![github1s](https://img.shields.io/badge/github1s-green.svg)](https://github1s.com/adxeproject/adxe)
- [![vscode.dev](https://img.shields.io/badge/vscode.dev-green.svg)](https://vscode.dev/github/adxeproject/adxe)
  
  
**[简体中文](README_CN.md)**
  
### Purpose Summary
* C++ 17
* Focuses on native game dev (easy to use, fast deployment, intuitive)
* Bugfixes ASAP
  
### Highlighted Features
* Windows x64 build support
* Reimplement HttpClient based on yasio for concorrent http requests processing.
* **clearer third-party libs ['Upstream-Version-License'](thirdparty/README.md) for easier publishing of your commercial apps based on adxe. Also some links to third party libs which support adxe too.**
* Refactor AudioEngine, OpenAL for all platforms
  * [OpenAL Soft](https://github.com/kcat/openal-soft), pass -DBUILD_DEP_ALSOFT=ON to cmake to force enabling it
  * [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL), if no ```BUILD_DEP_ALSOFT``` option specified, cmake script will choose it on osx/ios, even though it was marked as deprecated, but still available.
* Refactor UserDefault with [mio](https://github.com/mandreyel/mio)
* Modularize all optional extensions, move from engine core folder to an extensions folder
* Implement all .wav formats supported by ```OpenAL Soft```, such as MS-ADPCM, ADPCM, ...
* Use a modern GL loader ```Glad```
* Google [angle](https://github.com/google/angle) renderer backend support
* C++ 17 standard
* IOS SDK 9.0 as minimal deployment
* Use fast pugixml
* Use [curl](https://github.com/curl/curl) for transferring data with URL syntax
* Use SAX parser for all plist files
* Spine-3.8 support
* Extension ```FairyGUI``` support
* ASTC 4x4/6x6/8x8 support (if hardware decoding is not supported, then software decoding is used)
* ETC2 RGB/RGBA support    (if hardware decoding is not supported, then software decoding is used)
* Supported 2D physics engines (see also [Wiki](https://github.com/adxeproject/adxe-wiki)):
  * Box2D
  * Box2D-optimized
  * Chipmunk2D 
* Supported 3D physics engines:
  * Bullet Physics SDK
* **ImGui 1.87 integrated, easy to write game embedded tools, very easy to use, read [ImGuiEXT](extensions/ImGuiEXT/README.md) for more info**

Open [Wiki](https://github.com/adxeproject/adxe-wiki) for additional information and see [Milestones](https://github.com/adxeproject/adxe/milestones) for planed features too.

### Quick Start

#### Common Requirement [Python](https://www.python.org/downloads/)
  * Python-2.7.17+, Python-3.7+ 

#### Prerequisites
  1. Enter ```adxe``` root directory
  2. Run ```python setup.py```, restart the console after it has finished for environment variables to take effect

#### Windows (64/32 bit  Visual Studio 2019/2022)
  1. Install [CMake](https://cmake.org/) 3.14+  
  2. Install Visual Studio 2019/2022 (it's recommended that you only use these versions)  
  3. Execute the following commands in a command line (Console, Window Terminal or Powershell)
  
     ```cd adxe ```
     - for 32 bit Visual Studio 2019:
     ```cmake -S . -B build -G "Visual Studio 16 2019" -A Win32```
     - for 64 bit Visual Studio 2019:
     ```cmake -S . -B build -G "Visual Studio 16 2019" -A x64```
     - for 32 bit Visual Studio 2022:
     ```cmake -S . -B build -G "Visual Studio 17 2022" -A Win32```
     - for 64 bit Visual Studio 2022:
     ```cmake -S . -B build -G "Visual Studio 17 2022" -A x64```
  
  Build excecutable in a command line (e.g. cpp-tests):
    ```msbuild .\build\adxe.sln -target:cpp-tests -maxCpuCount```

#### Android
  1. Install Android Studio 2021.1.1+
  2. When starting Android Studio for the first time, It will guide you to install the SDK and other tools, just install them
  3. Start Android and choose [Open an existing Android Studio Project] and select ```adxe\tests\cpp-tests\proj.android```
  4. Start Android Studio and Open [Tools][SDKManager], then switch to ```SDK Tools```, check the ```Show Package Details```, choose the following tools and click the button ```Apply``` to install them:  
     * Android SDK Platform 29 r5  
     * Android SDK Build-Tools 29.0.2  
     * NDK r19c+  
     * CMake 3.10+  
  5. Wait for ```Gradle sync``` finish.
  6. Note: If you use non-sdk provided CMake edition, you will need to download ```ninja``` from https://github.com/ninja-build/ninja/releases, and copy ```ninja.exe``` to cmake's bin directory

#### iOS
  1. Ensure xcode11+ & [cmake3.21+](https://github.com/Kitware/CMake/releases) are installed, install cmake command line support: ```sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install```
  2. Execute the following command   
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

  4. After cmake finishes generating, you can open the xcode project at ```build``` folder and run cpp-tests or other test targets.  
  5. Notes  
     - **The code signing is required to run the ios app on your device, just change the bundle identifier until the auto manage signing is solved**  
     - **adxe only provides armv7, arm64, x86_64 prebuilt libraries for ios**

### New Project
- Cpp: `adxe new -p org.adxe.hellocpp -d D:\dev\projects\ -l cpp --portrait HelloCpp`
- Lua: `adxe new -p org.adxe.hellolua -d D:\dev\projects\ -l lua --portrait HelloLua`

### Notes
  * ThreadLocalStorage (TLS) 
    - ios x86 simulator ios>=10 and adxe no longer provide x86 libraries
    - ios x64 or devices (armv7, arm64) ios sdk>=9.0
    - the 'OpenAL Soft' maintained by kcat uses TLS

### Reference links
  * Official Cocos2d-x Repo: https://github.com/cocos2d/cocos2d-x

### Contributing guide
https://github.com/adxeproject/adxe/discussions/411

### The adxe Active Contributors

- @halx99
- @rh101
- @aismann
- @weiwest

![Alt](https://repobeats.axiom.co/api/embed/e46f10898672224fde3b102d81a28482164faf2e.svg "Repobeats analytics image")

### Full changes since cocos2d-x-4.0

- [HIGHLIGHT] Improve spine RTTI compare performance when compiler string pooling is enabled
- [HIGHLIGHT] Implement Windows WebView using WebView2 Edge Chromium, thanks to @rh101
- [HIGHLIGHT] Windows x64 build support
- [HIGHLIGHT] Support custom texture atlas formats, thanks to @rh101
- [HIGHLIGHT] Downloader realtime md5 checksum calculation support
- [HIGHLIGHT] Decompress astc parallel support
- [HIGHLIGHT] Reimplement HttpClient based on yasio for concurrent http request support
- [HIGHLIGHT] Improve Lua RTTI performance and increase memory efficiency
- [HIGHLIGHT] Virtual File System support, thanks to @rh101 
- [HIGHLIGHT] Refactor lua loader, gain 30%+ speed
- [HIGHLIGHT] Update plainlua version to 5.4.3
- [HIGHLIGHT] Use Openal-Soft for all platforms
- [HIGHLIGHT] Refactor UserDefault with mio, gain 100x+ speed
- [HIGHLIGHT] Implement all .wav formats supported by Openal-Soft, such as MS-ADPCM, ADPCM
- [HIGHLIGHT] Use a modern GL loader Glad
- [HIGHLIGHT] Google angle renderer backend support
- [HIGHLIGHT] Update codebase to C++ 17 standard
- [HIGHLIGHT] Remove tinyxml2
- [HIGHLIGHT] Use fast pugixml
- [HIGHLIGHT] Spine-3.6~4.0 support
- [HIGHLIGHT] ASTC 4x4/6x6/8x8 support (if hardware decoding is not present, software decoding will be used instead)
- [HIGHLIGHT] ETC2 RGB/RGBA support (if hardware decoding is not present, software decoding will be used instead)
- [HIGHLIGHT] ImGui integrated for PC platforms
- [HIGHLIGHT] Extension FairyGUI support
- [HIGHLIGHT] Use curl for transferring data with URL syntax
- [HIGHLIGHT] Modularize all optional extension, move from engine core folder to an extensions folder
- [HIGHLIGHT] Improve thirdparty library building, 99% of builds are from sources or github actions with latest toolchain, see also: `adxeproject/buildware`
- [HIGHLIGHT] Added a new API `Director::setChildrenIndexerEnabled` for speeding up getChildByTag & getChildByName support
- [HIGHLIGHT] Added a new API `FontFreeType::setStreamParsingEnabled` for stream parsing support, it's very useful for reducing memory cost when loading a large .ttf font file
- [HIGHLIGHT] Remove all unnecessary `std::nothrow` stubs
- [HIGHLIGHT] Use c++17 string_view instead `const std::string&`
- [NEW] Added int64_t/uint64_t support for `cocos2d::Value`
- [Fix] Fixed Label overflow shrink bug
- [FIX] Fixed uniform location mismatch when having more than 1 spine with different shaders  
- [FIX] Fixed imgui draw frame cause afterimage when game scene nothing to draw
- [FIX] Set global Z value of label debug layer to be the same as the parent label to fix display issues
- [FIX] Using TTF scaled metrics to calculate line height, thanks to @rh101
- [FIX] Fixed Memory leak in ShaderModuleGL::getErrorLog(), thanks to @Xrysnow
- [FIX] Fixed console output text encoding for win32
- [FIX] Fixed charset processing for fontName on win32
- [FIX] Fixed A crash on ParticleBatchNode::updateProgramStateTexture() 
- [FIX] Fixed A crash on SpriteBatchNode::appendChild when CC_SPRITE_DEBUG_DRAW==1
- [FIX] Fixed A bug where Lua can't get `unsigned char` or `unsigned int` values of a `ValueMap`
- [FIX] Fixed A crash on AudioEngine::end after the `Director` has been destroyed
- [FIX] Fixed font atlas memory leaking when it doesn't exist in the atlas cache, thanks to @rh101
- [FIX] Fixed A bug where ScrollView randomly shows in an incorrect position vertically, thanks to @wzhengsen
- [FIX] Fixed Spine ClippingAttachment not working
- [REFINE] CCValue move construct and assign with std::string support
- [REFINE] Improve windows dev workflow, use working directory instead of copying resources to build binary directory manually
- [REFINE] Add lua debug project file to lua project template
- [REFINE] Remove lua 64bit spec search path, since we can compile compatible bytecode for both plainlua and luajit
- [REFINE] Put stats labels to a safe origin to make sure we can see it integral at some mobile device
- [REFINE] Improve cmake scripts
- [REFINE] Replace deprecated repo `jcenter` with `mavenCentral`
- [REFINE] Lua-5.4 compatible
- [REFINE] Improve cmdline-tools
- [REFINE] Ensure label underline node has the same global Z value as the label node to fix visibility issues when global Z value is not 0
- [REFINE] Improve android astc support check
- [REFINE] Improve the "Physics part" (#379), thanks to @aismann 
- [REFINE] Rename command line to `adxe`
- [REFINE] Update imgui to 1.84 WIP
- [REFINE] Rename thirdparty folder `external` to `thirdparty`
- [REFINE] Force building plainlua as dll on windows for the debugger, better for debuging apps quickly
- [REFINE] Fix some audio test cases where sound can't be heard
- [REFINE] Avoid OS_WINDOWS conflict with system macro
- [REFINE] Update OpenSSL to v3.0.1
- [REFINE] Update FreeType to latest
- [REFINE] Use Freetype SDF rendering when label distance field is enabled
- [REFINE] Force enable string pooling for msvc on cmake
- [REFINE] Change DrawNode api color parameters from `Color4F` to low `Color4B`
- [REFINE] Improve DrawNode GC allocations
- [REFINE] Use `jni.hpp` to improve jni calls without method signature GC allocations
