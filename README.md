# EGNX
[![Build Status](https://travis-ci.com/c4games/engine-x.svg?branch=master)](https://travis-ci.com/c4games/engine-x)
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/4936wev2r2ot606s/branch/master?svg=true)](https://ci.appveyor.com/project/halx99/engine-x)

**This is another more radical fork of ```cocos2d-x v4```, use opanal for all platforms, single texture multi gpu texture handler, c++14/17...**  
  
**[简体中文](README_CN.md)**
  
### Purpose Summary:
* C++14/17
* Forcus on native game dev only
* Remove unnecessary sources
* Fix bugs ASAP
* Review PR ASAP
* If you have any other excellent goals, welcome
* Excellent PRs from any guys are welcome, I will review & merge ASAP
  
### Highlight Features:
* Refactor AudioEngine, OpenAL for all platforms, and on iOS, can switch to [openal-soft](https://github.com/kcat/openal-soft) through ```-DCC_USE_ALSOFT_ON_APPLE``` since Apple mark OpenAL framework ```deprecated``` on iOS12
* Refactor UserDefault with [mio](https://github.com/mandreyel/mio), very fast
* Modularize all optional extension, all move from engine core to folder extensions
* Add engine extension 'fairygui' support
* Implement all .wav formats supported by ```openal-soft```, such as MS-ADPCM, ADPCM...
* Use modern gl loader ```glad``` to instead glew
* Add google angle renderer backend support
* Set default C++ standard to 14
* Set min deploy target ios sdk to 9.0
* Remove tinyxml2, use more fast pugixml instead
* Use curl for HttpClient,Downloader on all platforms
* Use SAX parser for all plist file, remove apple platform spec for getValueMapFromFile stubs

### [Roadmap](https://github.com/c4games/engine-x/issues/1)

### Quick Start
#### Windows
  1. Install [CMake](https://cmake.org/) 3.6+  
  2. Install Visual Studio 2019 build(we strong recommand you install this version)  
  3. Execute follow command at command line(Console, Window Terminal or Powershell)
  ```bat
  cd engine-x\
  cmake -S . -B build -G "Visual Studio 16 2019" -A Win32
  ```
  

#### Android
  1. Install Android Studio 3.5.3+
  2. Start Android and choose [Open an existing Android Studio Project] to open ```engine-x\tests\cpp-tests\proj.android```
  3. Start Android Studio and Open [Tools][SDKManager] and install sdk tools:  
    LLDB 3.1+  
    CMake 3.10.2+  
    NDK 20.1+  

  4. Waiting for ```Gradle sync``` finish.
  
  5. Remark: If missing Ninja, could be download from https://github.com/ninja-build/ninja/releases, then copy Ninja.exe to Cmake's bin directory

#### iOS
  1. Ensure xcode11+ installed
  2. Install brew: ```/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"```  
  when finish, install follow tools:
  ```sh
      brew update
      brew install git
      brew install cmake
      brew install autoconf
      brew install automake
      brew install libtool
  ```
  3. Execute follow command   
  ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```  
  4. Generate xcode project
  ```sh
    mkdir engine-x/build
    cd engine-x/build
    cmake .. -GXcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphonesimulator
    # for simulator64
    # cmake .. -G Xcode -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=SIMULATOR64 -DENABLE_ARC=0 -DENABLE_BITCODE=0
    # for (armv7, armv7s, arm64)
    # cmake .. -G Xcode -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=OS -DENABLE_ARC=0 -DENABLE_BITCODE=0
    # for device 64
    # cmake .. -G Xcode -DCMAKE_TOOLCHAIN_FILE=../cmake/ios.toolchain.cmake -DPLATFORM=OS64 -DENABLE_ARC=0 -DENABLE_BITCODE=0
  ```
  5. After cmake generate finish, you can open xcode project at ```build``` folder and run cpp-tests or other test targets.

### Pitfalls
  * ThreadLocalStorage(TLS) 
    - ios x86 simulator ios>=10
    - ios x64 or devices(armv7,arm64) ios sdk>=9.0
    - the openal-soft maintained by kcat use TLS

### Reference links
  * engine-x-3rd: https://github.com/c4games/engine-x-3rd
  * official v4: https://github.com/cocos2d/cocos2d-x
