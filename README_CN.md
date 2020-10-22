# cocos-re - Cocos2dx Refine
[![Build Status](https://travis-ci.com/c4games/cocos-re.svg?branch=master)](https://travis-ci.com/c4games/cocos-re)
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/yyf7c9w0k0b6c33b/branch/master?svg=true)](https://ci.appveyor.com/project/halx99/cocos-re)
[![dev](https://img.shields.io/badge/v1.0.0-alpha19-yellow.svg)](https://github.com/c4games/cocos-re/releases)

**这是另外一个基于 *Cocos2d-x-4.0* 持续维护的分支, 全平台基于OpenAL, c++14/17...**
  
**[English](README.md)**
  
### 宗旨:
* C++14/17
* Forcus on native game dev only
* Fix bugs ASAP
* Review PR ASAP
* Excellent PRs from any guys are welcome, we will review & merge ASAP
  
### 主要特性:
* 重构AudioEngine, 全平台OpenAL
  * [openal-soft](https://github.com/kcat/openal-soft), pass -DBUILD_EXT_ALSOFT=ON to cmake to force enable it
  * [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL), if no and ```BUILD_EXT_ALSOFT``` option specified, cmake script will choose it on osx/ios, even through it was mark as deprecated, but still avaiable.
* 重构UserDefault, 全平台基于内存映射文件 [mio](https://github.com/mandreyel/mio), 性能提升百倍
* 模块化所有引擎扩展库, 所有扩展库放到extensions目录下，如cocostudio, spine
* AudioEngine实现wav所有 ```openal-soft``` 支持的封装格式, 例如MS-ADPCM, ADPCM等
* 使用现代化的GL加载器glad代替glew
* 增加google angle渲染后端支持
* 设置默认C++标准为C++14
* 设置ios下最低目标平台为ios9.0
* 使用更快的xml解析库pugixml代替tinyxml2
* HttpClient, Downloader全平台统一实现，均基于curl
* 全平台统一使用XML SAX解析plist文件, 移除Apple平台的实现
* Spine 3.8支持
* 新增FairyGUI支持
* 新增硬件压缩纹理格式ASTC 4x4/6x6/8x8支持，支持软解
* 新增硬件压缩纹理格式ETC2 RGB/RGBA支持，支持软解
* **ImGui集成，非常方便写游戏内嵌小工具，用法详见[ImGuiEXT](extensions/ImGuiEXT/README.md)**
  
### [Roadmap](https://github.com/c4games/cocos-re/issues/1)
  
### 快速开始

#### 必要工具 [python](https://www.python.org/downloads/)
  * python-2.7.17+, python-3.7+都可以

#### Windows
  1. 安装CMake，要求3.6以上  
  2. 确保Visual Studio 2019以正确安装
  3. 执行下面的命令
  ```bat
  cd cocos-re
  cmake -S . -B build -G "Visual Studio 16 2019" -A Win32
  ``` 
  4. 之后就可以用vs打开```cocos-re/build/cocos-re.sln```启动cpp-tests等测试工程了

#### Android
  1. 安装 Android Studio 4.0或更高版本
  2. 第一次启动Android Studio, 会引导安装一些工具，默认安装即可，如果不需要模拟器可以把```Virutal Device```勾选去掉
  2. 启动 Android Studio并打开cocos-re\tests\cpp-tests\proj.android
  3. 通过 [Tools][SdkManagers] 打开sdk管理工具，选择如下SDK Platforms和SDK Tools后， 点击按钮 ```Apply``` 一路跟着引导安装即可:
     * Android SDK Platform 28 r3  (在SDK Platforms页签)
     * Android SDK Build-Tools 29.0.2  
     * NDK r16b+  
     * CMake 3.6  

  5. 等待Gradle sync完成后，Build APKs，安装运行  
  6. 如果使用非sdk自动安装的CMake版本, 需要从https://github.com/ninja-build/ninja/releases下载 ```ninja```， 并且拷贝```ninja.exe```到CMake的bin目录

#### iOS
  1. 确保已安装xcode11+和[cmake3.6+](https://github.com/Kitware/CMake/releases), 安装CMake命令行支持: ```sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install```
  2. 执行如下命令确保cmake能成功生成xcode工程:  
  ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```  
  3. 生成xcode工程, 进入cocos-re根目录执行如下命令:  
  ```sh
    # for device arm64
    cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake

    # for device combined armv7,arm64
    # cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake "-DCMAKE_OSX_ARCHITECTURES=armv7;arm64"

    # for simulator x86_64
    # cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=cmake/ios.mini.cmake -DCMAKE_OSX_SYSROOT=iphonesimulator
  ```
  4. 之后就可以用xcode打开, 选择cpp-tests编译运行

### 注意
  * ThreadLocalStorage线程本地存储
    - ios x86 simulator ios>=10
    - ios x64 or devices(armv7,arm64) ios sdk>=9.0
    - the openal-soft maintained by kcat use TLS

### QQ交流群

  点击链接加入群聊【cocos-re交流群】：https://jq.qq.com/?_wv=1027&k=nvNmzOIY
  
### 参考链接
  * cocos-re-3rd: https://github.com/c4games/cocos-re-3rd
  * official v4: https://gitee.com/mirrors/cocos2d-x
