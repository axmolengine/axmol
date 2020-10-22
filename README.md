# cocos-re - Cocos2dx Refine
[![Build Status](https://travis-ci.com/c4games/cocos-re.svg?branch=master)](https://travis-ci.com/c4games/cocos-re)
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/yyf7c9w0k0b6c33b/branch/master?svg=true)](https://ci.appveyor.com/project/halx99/cocos-re)
[![dev](https://img.shields.io/badge/v1.0.0-alpha18-yellow.svg)](https://github.com/c4games/cocos-re/releases)

**This is another more radical fork of *Cocos2d-x-4.0*, use OpenAL for all platforms, single texture multi GPU texture handler, C++14/17, etc.**  
  
**[简体中文](README_CN.md)**
  
### Purpose Summary:
* C++14/17
* Focus on native game dev (quick starting, easy to use, fast)
* Bugfixes ASAP
* Usefull PRs from **you** are welcome (review/merge ASAP)
  
### Highlight Features:
* Refactor AudioEngine, OpenAL for all platforms
  * [openal-soft](https://github.com/kcat/openal-soft), pass -DBUILD_EXT_ALSOFT=ON to cmake to force enable it
  * [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL), if no ```BUILD_EXT_ALSOFT``` option specified, cmake script will choose it on osx/ios, even through it was mark as deprecated, but still available.
* Refactor UserDefault with [mio](https://github.com/mandreyel/mio)
* Modularize all optional extension, move from engine core to folder extensions
* Implement all .wav formats supported by ```openal-soft```, such as MS-ADPCM, ADPCM, ...
* Use modern gl loader ```glad``` (instead of glew)
* Add google angle renderer backend support
* C++14 standard
* IOS SDK 9.0 as minimal deployment
* Use fast pugixml
* Using curl for transferring data with URL syntax
* Use SAX parser for all plist file
* Spine-3.8 support
* Add engine extension ```FairyGUI``` support
* Add ASTC 4x4/6x6/8x8 support, if hardware decoder not present, use software decoder
* Add ETC2 RGB/RGBA support, if hardware decoder not present, use software decoder
* **ImGui integrated, easy to write game embedded tools, very ease to use, please read [ImGuiEXT](extensions/ImGuiEXT/README.md)**

### [Roadmap](https://github.com/c4games/cocos-re/issues/1)

### Quick Start
#### Common Requirement [python](https://www.python.org/downloads/)
  * python-2.7.17+, python-3.7+ 

#### Windows
  1. Install [CMake](https://cmake.org/) 3.6+  
  2. Install Visual Studio 2019 build(we strong recommend you use this version)  
  3. Execute follow command at command line(Console, Window Terminal or Powershell)
  ```bat
  cd cocos-re\
  cmake -S . -B build -G "Visual Studio 16 2019" -A Win32
  ```
  

#### Android
  1. Install Android Studio 4.0+
  2. When first start Android Studio, It will guide you to install sdk and other tools, just install them
  3. Start Android and choose [Open an existing Android Studio Project] to open ```cocos-re\tests\cpp-tests\proj.android```
  4. Start Android Studio and Open [Tools][SDKManager], then switch to ```SDK Tools```, check the ```Show Package Details```, choose follow tools and click the button ```Apply``` to install them:  
     * Android SDK Platform 28 r3  
     * Android SDK Build-Tools 29.0.2  
     * NDK r16b+  
     * CMake 3.6  
    
  5. Waiting for ```Gradle sync``` finish.
  
  6. Remark: If you use non-sdk provided CMake edition, you need download ```ninja``` from https://github.com/ninja-build/ninja/releases, and copy ```ninja.exe``` to cmake's bin directory

#### iOS
  1. Ensure xcode11+ & [cmake3.6+](https://github.com/Kitware/CMake/releases) installed, install cmake command line support: ```sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install```
  2. Execute follow command   
  ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```  
  3. Generate xcode project
  ```sh
    # for device arm64
    cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake

    # for device combined armv7,arm64
    # cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake "-DCMAKE_OSX_ARCHITECTURES=armv7;arm64"

    # for simulator x86_64
    # cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake -DCMAKE_OSX_SYSROOT=iphonesimulator
  ```
  4. After cmake generate finish, you can open xcode project at ```build``` folder and run cpp-tests or other test targets.  

  5. Notes  
    a. **The code sign required to run ios app on device, just change bundle identifier until the auto manage siging solved**  
    b. **cocos-re only provide armv7,arm64,x86_64 prebuilt libraries for ios**

### Pitfalls
  * ThreadLocalStorage(TLS) 
    - ios x86 simulator ios>=10
    - ios x64 or devices(armv7,arm64) ios sdk>=9.0
    - the openal-soft maintained by kcat use TLS

### Reference links
  * cocos-re: https://github.com/c4games/cocos-re-3rd
  * official v4: https://github.com/cocos2d/cocos2d-x
