# adxe

[![dev](https://img.shields.io/badge/v1.0.0-beta6-yellow.svg)](https://github.com/adxeproject/adxe/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/adxeproject/adxe/blob/master/LICENSE)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/1c5628dea478449ea0c6e1b0e30c3be9)](https://app.codacy.com/gh/adxeproject/adxe?utm_source=github.com&utm_medium=referral&utm_content=adxeproject/adxe&utm_campaign=Badge_Grade_Settings)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/adxeproject/adxe/pulls)
  
[![Windows Build Status](https://github.com/adxeproject/adxe/actions/workflows/windows-ci.yml/badge.svg)](https://github.com/adxeproject/adxe/actions/workflows/windows-ci.yml)
[![Android Build Status](https://github.com/adxeproject/adxe/workflows/android/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Aandroid)
[![iOS Build Status](https://github.com/adxeproject/adxe/workflows/ios/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Aios)
[![Linux Build Status](https://github.com/adxeproject/adxe/workflows/linux/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Alinux)
[![macOS Build Status](https://github.com/adxeproject/adxe/workflows/osx/badge.svg)](https://github.com/adxeproject/adxe/actions?query=workflow%3Aosx)  


**这是另外一个基于 *Cocos2d-x-4.0* 持续维护的分支, 全平台基于OpenAL, c++17...**
  
**[English](README.md)**
  
### 宗旨:
* C++17
* Focus on native game dev only
* Fix bugs ASAP
* Review PR ASAP
  
### 主要特性:
* Windows x64编译支持
* 基于yasio重构HttpClient以支持并发Http请求，不再需要sendImmidate接口
* 重构AudioEngine, 全平台OpenAL
  * [openal-soft](https://github.com/kcat/openal-soft), pass -DBUILD_DEP_ALSOFT=ON to cmake to force enable it
  * [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL), if no and ```BUILD_DEP_ALSOFT``` option specified, cmake script will choose it on osx/ios, even through it was mark as deprecated, but still avaiable.
* 重构UserDefault, 全平台基于内存映射文件 [mio](https://github.com/mandreyel/mio), 性能提升百倍
* 模块化所有引擎扩展库, 所有扩展库放到extensions目录下，如cocostudio, spine
* AudioEngine实现wav所有 ```openal-soft``` 支持的封装格式, 例如MS-ADPCM, ADPCM等
* 使用现代化的GL加载器glad代替glew
* 增加google angle渲染后端支持
* C++最低标准要求: C++17
* 设置ios下最低目标平台为ios9.0
* 使用更快的xml解析库pugixml代替tinyxml2
* Downloader全平台统一实现，均基于curl
* 全平台统一使用XML SAX解析plist文件, 移除Apple平台的实现
* Spine 3.8支持
* 新增FairyGUI支持
* 新增硬件压缩纹理格式ASTC 4x4/6x6/8x8支持，支持软解
* 新增硬件压缩纹理格式ETC2 RGB/RGBA支持，支持软解
* **ImGui集成，非常方便写游戏内嵌小工具，用法详见[ImGuiEXT](extensions/ImGuiEXT/README.md)**
  
### [Roadmap](https://github.com/adxeproject/adxe/issues/1)
  
### 快速开始

#### 必要工具 [python](https://www.python.org/downloads/)
  * python-2.7.17+, python-3.7+都可以

#### 准备步骤
  1. 进入引擎```adxe```根目录
  3. 执行```python setup.py```后重启控制台

#### Windows
  1. 安装CMake，要求3.14以上  
  2. 确保 Visual Studio 2019 已正确安装
  3. 执行下面的命令
  ```bat
  cd adxe
  cmake -S . -B build -G "Visual Studio 16 2019" -A Win32
  ``` 
  4. 之后就可以用vs打开```adxe/build/adxe.sln```启动cpp-tests等测试工程了

#### Android
  1. 安装 Android Studio 2021.1.1+
  2. 第一次启动Android Studio, 会引导安装一些工具，默认安装即可，如果不需要模拟器可以把```Virutal Device```勾选去掉
  2. 启动 Android Studio并打开adxe\tests\cpp-tests\proj.android
  3. 通过 [Tools][SdkManagers] 打开sdk管理工具，选择如下SDK Platforms和SDK Tools后， 点击按钮 ```Apply``` 一路跟着引导安装即可:
     * Android SDK Platform 29 r5  (在SDK Platforms页签)
     * Android SDK Build-Tools 29.0.2  
     * NDK r19c+  
     * CMake 3.10+  

  5. 等待Gradle sync完成后，Build APKs，安装运行  
  6. 如果使用非sdk自动安装的CMake版本, 需要下载[ninja](https://github.com/ninja-build/ninja/releases)， 并且拷贝```ninja.exe```到CMake的bin目录

#### iOS
  1. 确保已安装xcode11+和[cmake3.21+](https://github.com/Kitware/CMake/releases), 安装CMake命令行支持: ```sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install```
  2. 执行如下命令确保cmake能成功生成xcode工程:  
  ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```  
  3. 生成xcode工程, 进入adxe根目录执行如下命令之一:  
     - for any device:   
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake```
     - for arm64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake -DCMAKE_OSX_ARCHITECTURES=arm64```
     - for armv7,arm64 combined:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake "-DCMAKE_OSX_ARCHITECTURES=armv7;arm64"```
     - for simulator x86_64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake -DCMAKE_OSX_ARCHITECTURES=x86_64```

  4. 之后就可以用xcode打开, 选择cpp-tests编译运行

### 注意
  * ThreadLocalStorage线程本地存储
    - ios x86 simulator ios>=10 and adxe no longer provide x86 libraries
    - ios x64 or devices(armv7,arm64) ios sdk>=9.0
    - the openal-soft maintained by kcat use TLS

### QQ交流群

  点击链接加入群聊【Adxe 交流群】：https://jq.qq.com/?_wv=1027&k=nvNmzOIY
  
### 参考链接
  * official v4: https://gitee.com/mirrors/cocos2d-x
  * Git快速手册: https://github.com/adxeproject/adxe/wiki/Git-Guides

### adxe项目的活跃贡献者

- @halx99
- @rh101
- @aismann
- @weiwest

### 自Cocos2d-x-4.0以来完整改动列表

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
- [HIGHLIGHT] Use c++17 string_view instead `const std::string&`
- [NEW] Add int64_t/uint64_t support for `cocos2d::Value`
- [Fix] Fix Label overflow shrink bug
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
- [FIX] Fix Spine ClippingAttachment doesn't work
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
- [REFINE] Use `jni.hpp` to improve jni call without method signature GC allocs
