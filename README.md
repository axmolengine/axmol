# EGNX
[![Build Status](https://travis-ci.com/c4games/engine-x.svg?branch=master)](https://travis-ci.com/c4games/engine-x)
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/4936wev2r2ot606s/branch/master?svg=true)](https://ci.appveyor.com/project/halx99/engine-x)

**This is another more radical fork of ```cocos2d-x v4```, use OpneAL for all platforms, single texture multi GPU texture handler, C++14/17 etc.**  
  
**[简体中文](README_CN.md)**
  
### Purpose Summary:
* C++14/17
* Focus on native game dev
* Bugfixes ASAP
* Review/Merge PR ASAP
* Excellent PRs from **you** are welcome.
  
### Highlight Features:
* Refactor AudioEngine, OpenAL for all platforms
  * [openal-soft](https://github.com/kcat/openal-soft), pass -DBUILD_EXT_ALSOFT=ON to cmake to force enable it
  * [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL), if no ```BUILD_EXT_ALSOFT``` option specified, cmake script will choose it on osx/ios, even through it was mark as deprecated, but still available.
* Refactor UserDefault with [mio](https://github.com/mandreyel/mio), very fast
* Modularize all optional extension, all move from engine core to folder extensions
* Implement all .wav formats supported by ```openal-soft```, such as MS-ADPCM, ADPCM...
* Use modern gl loader ```glad``` to instead glew
* Add google angle renderer backend support
* C++14 standard
* IOS SDK 9.0 as minimal deployment
* Use fast pugixml instead of tinyxml2
* Using curl for transferring data with URL syntax
* Use SAX parser for all plist file, remove apple platform spec for getValueMapFromFile stubs
* Spine-3.8 support
* Add engine extension ```FairyGUI``` support

### [Roadmap](https://github.com/c4games/engine-x/issues/1)

### Quick Start
#### Common Requirement [python](https://www.python.org/downloads/)
  * python-2.7.17+, python-3.7+ also works

#### Windows
  1. Install [CMake](https://cmake.org/) 3.6+  
  2. Install Visual Studio 2019 build(we strong recommend you install this version)  
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
     * Android SDK Platform 28 r3  
     * Android SDK Build-Tools 29.0.2  
     * NDK r16b+  
     * CMake 3.6  
    
  5. Waiting for ```Gradle sync``` finish.
  
  6. Remark: If you use non-sdk provided CMake edition, you need download ```ninja``` from https://github.com/ninja-build/ninja/releases, and copy ```ninja.exe``` to cmake's bin directory

#### iOS
  1. Ensure xcode11+ & **cmake-3.18.2+** installed
  2. Install brew: ```/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"```  
  when finish, install follow tools:
  ```sh
      brew update
      brew install git
      brew install autoconf
      brew install automake
      brew install libtool
  ```
  3. Execute follow command   
  ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```  
  4. Generate xcode project
  ```sh
    cmake -S . -B build -GXcode -DCMAKE_SYSTEM_NAME=iOS
  ```
  5. After cmake generate finish, you can open xcode project at ```build``` folder and run cpp-tests or other test targets.  

  6. Notes  
    a. **If you don't want upgrade your cmake to 3.18.2, please [click here](https://github.com/c4games/engine-x/wiki/Build-ios-device-with-CMake--version-less-than-3.18.2) to see xcode generate command.**  
    b. **The code sign required to run ios app on device, just change bundle identifier until the auto manage siging solved**

### Pitfalls
  * ThreadLocalStorage(TLS) 
    - ios x86 simulator ios>=10
    - ios x64 or devices(armv7,arm64) ios sdk>=9.0
    - the openal-soft maintained by kcat use TLS

### Reference links
  * engine-x-3rd: https://github.com/c4games/engine-x-3rd
  * official v4: https://github.com/cocos2d/cocos2d-x
