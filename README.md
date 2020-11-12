# Engine-x
[![Android Build Status](https://github.com/c4games/engine-x/workflows/android/badge.svg)](https://github.com/c4games/engine-x/actions?query=workflow%3Aandroid)
[![iOS Build Status](https://github.com/c4games/engine-x/workflows/ios/badge.svg)](https://github.com/c4games/engine-x/actions?query=workflow%3Aios)
[![Windows Build Status](https://github.com/c4games/engine-x/workflows/win32/badge.svg)](https://github.com/c4games/engine-x/actions?query=workflow%3Awin32)
[![Linux Build Status](https://github.com/c4games/engine-x/workflows/linux/badge.svg)](https://github.com/c4games/engine-x/actions?query=workflow%3Alinux)
[![macOS Build Status](https://github.com/c4games/engine-x/workflows/osx/badge.svg)](https://github.com/c4games/engine-x/actions?query=workflow%3Aosx)  

[![dev](https://img.shields.io/badge/v1.0.0-alpha19-yellow.svg)](https://github.com/c4games/engine-x/releases)

**This is another more radical fork of *Cocos2d-x-4.0*, use OpenAL for all platforms, single texture multi GPU texture handler, C++14/17, etc.**  
  
**[简体中文](README_CN.md)**
  
### Purpose Summary:
* C++14/17
* Focus on native game dev (quick starting, easy to use, fast)
* Bugfixes ASAP
* Usefull PRs from **you** are welcome (review/merge ASAP)
  
### Highlight Features:
* **More clearly thirdparty libs Upstram & Version & License for easy to publish your commercial apps based on engine-x**
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

### [Roadmap](https://github.com/c4games/engine-x/issues/1)



### Quick Start
#### Common Requirement [python](https://www.python.org/downloads/)
  * python-2.7.17+, python-3.7+ 

#### Prerequisites
  1. Enter ```engine-x``` root directory
  2. Run ```python download-deps.py```
  3. Run ```python setup.py```, restart console after finish

#### Windows
  1. Install [CMake](https://cmake.org/) 3.14+  
  2. Install Visual Studio 2019 build(we strong recommend you use this version)  
  3. Execute follow command at command line(Console, Window Terminal or Powershell)
  ```bat
  cd engine-x\
  cmake -S . -B build -G "Visual Studio 16 2019" -A Win32
  ```
  

#### Android
  1. Install Android Studio 4.0+
  2. When first start Android Studio, It will guide you to install sdk and other tools, just install them
  3. Start Android and choose [Open an existing Android Studio Project] to open ```engine-x\tests\cpp-tests\proj.android```
  4. Start Android Studio and Open [Tools][SDKManager], then switch to ```SDK Tools```, check the ```Show Package Details```, choose follow tools and click the button ```Apply``` to install them:  
     * Android SDK Platform 29 r5  
     * Android SDK Build-Tools 29.0.2  
     * NDK r16b+  
     * CMake 3.10+  
    
  5. Waiting for ```Gradle sync``` finish.
  
  6. Remark: If you use non-sdk provided CMake edition, you need download ```ninja``` from https://github.com/ninja-build/ninja/releases, and copy ```ninja.exe``` to cmake's bin directory

#### iOS
  1. Ensure xcode11+ & [cmake3.14+](https://github.com/Kitware/CMake/releases) installed, install cmake command line support: ```sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install```
  2. Execute follow command   
  ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```  
  3. Generate xcode project
  ```sh
    # for any device
    cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake

    # for device combined armv7,arm64
    # cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake "-DCMAKE_OSX_ARCHITECTURES=armv7;arm64"

    # for simulator x86_64
    # cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake -DCMAKE_OSX_SYSROOT=iphonesimulator -DCMAKE_OSX_ARCHITECTURES=x86_64
  ```
  4. After cmake generate finish, you can open xcode project at ```build``` folder and run cpp-tests or other test targets.  

  5. Notes  
    a. **The code sign required to run ios app on device, just change bundle identifier until the auto manage siging solved**  
    b. **engine-x only provide armv7,arm64,x86_64 prebuilt libraries for ios**

### Pitfalls
  * ThreadLocalStorage(TLS) 
    - ios x86 simulator ios>=10
    - ios x64 or devices(armv7,arm64) ios sdk>=9.0
    - the openal-soft maintained by kcat use TLS

### Reference links
  * engine-x-3rd: https://github.com/c4games/engine-x-3rd
  * official v4: https://github.com/cocos2d/cocos2d-x
