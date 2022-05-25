# adxe

[![dev](https://img.shields.io/badge/v1.0.0-beta6-yellow.svg)](https://github.com/adxeproject/adxe/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/adxeproject/adxe/blob/master/LICENSE)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/1c5628dea478449ea0c6e1b0e30c3be9)](https://app.codacy.com/gh/adxeproject/adxe?utm_source=github.com&utm_medium=referral&utm_content=adxeproject/adxe&utm_campaign=Badge_Grade_Settings)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/adxeproject/adxe/pulls)
  
![issues](https://img.shields.io/github/issues/adxeproject/adxe?style=plastic)
![forks](https://img.shields.io/github/forks/adxeproject/adxe?style=plastic)
![stars](https://img.shields.io/github/stars/adxeproject/adxe?style=plastic)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/adxeproject/adxe?style=plastic)  
  
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
* Improve windows workflow, support linking with engine prebuilt libs, read [windows workflow guide](https://github.com/adxeproject/adxe/issues/564)
* Windows video player support (based on microsoft media foundation)
* Windows x64 build support
* Reimplement HttpClient based on yasio for concorrent http requests processing.
* **clearer third-party libs ['Upstream-Version-License'](thirdparty/README.md) for easier publishing of your commercial apps based on adxe. Also some links to third party libs which support adxe too.**
* Refactor AudioEngine, OpenAL for all platforms
  * [OpenAL Soft](https://github.com/kcat/openal-soft), pass -DAX_USE_ALSOFT=ON to cmake to force enabling it
  * [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL), if no ```AX_USE_ALSOFT``` option specified, cmake script will choose it on osx/ios, even though it was marked as deprecated, but still available.
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
* Supported 2D physics engines (see also [APPENDIX.md](APPENDIX.md)):
  * Box2D
  * Box2D-optimized
  * Chipmunk2D 
* Supported 3D physics engines:
  * Bullet Physics SDK
* **ImGui 1.87 integrated, easy to write game embedded tools, very easy to use, read [ImGui](extensions/ImGui/README.md) for more info**

[Read Full changes since cocos2d-x-4.0](CHANGELOG)

Open [APPENDIX.md](APPENDIX.md) for additional information and see [Milestones](https://github.com/adxeproject/adxe/milestones) for planed features too.

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
    ```msbuild .\build\adxe.sln -target:cpp_tests -maxCpuCount```

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
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$ADXE_ROOT/cmake/ios.mini.cmake```
     - for arm64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$ADXE_ROOT/cmake/ios.mini.cmake -DCMAKE_OSX_ARCHITECTURES=arm64```
     - for armv7,arm64 combined:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$ADXE_ROOT/cmake/ios.mini.cmake "-DCMAKE_OSX_ARCHITECTURES=armv7;arm64"```
     - for simulator x86_64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$ADXE_ROOT/cmake/ios.mini.cmake -DCMAKE_OSX_ARCHITECTURES=x86_64``` 

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
