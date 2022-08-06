# Axis游戏引擎

[![dev](https://img.shields.io/github/v/release/axis-project/axis?include_prereleases&label=release)](https://github.com/axis-project/axis/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/axis-project/axis/blob/master/LICENSE)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/81fa1aba09ab41a98b949064b928d06e)](https://www.codacy.com/gh/axis-project/axis/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=axis-project/axis&amp;utm_campaign=Badge_Grade)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/axis-project/axis/pulls)
  
[![Windows Build Status](https://github.com/axis-project/axis/actions/workflows/windows-ci.yml/badge.svg)](https://github.com/axis-project/axis/actions/workflows/windows-ci.yml)
[![Android Build Status](https://github.com/axis-project/axis/workflows/android/badge.svg)](https://github.com/axis-project/axis/actions?query=workflow%3Aandroid)
[![iOS Build Status](https://github.com/axis-project/axis/workflows/ios/badge.svg)](https://github.com/axis-project/axis/actions?query=workflow%3Aios)
[![Linux Build Status](https://github.com/axis-project/axis/workflows/linux/badge.svg)](https://github.com/axis-project/axis/actions?query=workflow%3Alinux)
[![macOS Build Status](https://github.com/axis-project/axis/workflows/osx/badge.svg)](https://github.com/axis-project/axis/actions?query=workflow%3Aosx)  


**这是另外一个基于 *Cocos2d-x-4.0* 持续维护的分支, 全平台基于OpenAL, c++17...**
  
**[English](README.md)**
  
### 宗旨:
* C++17
* Focus on native game dev only
* Fix bugs ASAP
* Review PR ASAP
  
### 主要特性:
* 增强Windows下工作流，支持链接引擎预编译库, 用法请查看: [windows workflow guide](https://github.com/axis-project/axis/issues/564)
* Windows 视频播放支持
* Windows x64编译支持
* 基于yasio重构HttpClient以支持并发Http请求，不再需要sendImmidate接口
* 重构AudioEngine, 全平台OpenAL
  * [openal-soft](https://github.com/kcat/openal-soft), pass -DAX_USE_ALSOFT=ON to cmake to force enable it
  * [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL), if no and ```AX_USE_ALSOFT``` option specified, cmake script will choose it on osx/ios, even through it was mark as deprecated, but still avaiable.
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
* **ImGui集成，非常方便写游戏内嵌小工具，用法详见[ImGui](extensions/ImGui/README.md)**
* 完整改动列表，请查看[CHANGELOG](CHANGELOG)
  
### 快速开始

#### 必要工具 [python](https://www.python.org/downloads/)
  * python-2.7.17+, python-3.7+都可以

#### 准备步骤
  1. 进入引擎```axis```根目录
  3. 执行```python setup.py```后重启控制台

#### Windows
  1. 安装CMake，要求3.14以上  
  2. 确保 Visual Studio 2019 已正确安装
  3. 执行下面的命令
  ```bat
  cd axis
  cmake -S . -B build -G "Visual Studio 16 2019" -A Win32
  ``` 
  4. 之后就可以用vs打开```axis/build/axis.sln```启动cpp-tests等测试工程了

#### Android
  1. 安装 Android Studio 2021.1.1+
  2. 第一次启动Android Studio, 会引导安装一些工具，默认安装即可，如果不需要模拟器可以把```Virutal Device```勾选去掉
  2. 启动 Android Studio并打开axis\tests\cpp-tests\proj.android
  3. 通过 [Tools][SdkManagers] 打开sdk管理工具，选择如下SDK Platforms和SDK Tools后， 点击按钮 ```Apply``` 一路跟着引导安装即可:
     * Android SDK Platform 33  (在SDK Platforms页签)  
     * Android Gradle Plugin (AGP) 7.2.0  
     * Android SDK Build-Tools 30.0.3 和 AGP 版本对应  
     * Gradle 7.4.2  
     * NDK r23c+  
     * CMake 3.22.1+  

  5. 等待Gradle sync完成后，Build APKs，安装运行  
  6. 如果使用非sdk自动安装的CMake版本, 需要下载[ninja](https://github.com/ninja-build/ninja/releases)， 并且拷贝```ninja.exe```到CMake的bin目录

#### iOS
  1. 确保已安装xcode12+和[cmake3.21+](https://github.com/Kitware/CMake/releases), 安装CMake命令行支持: ```sudo "/Applications/CMake.app/Contents/bin/cmake-gui" --install```
  2. 执行如下命令确保cmake能成功生成xcode工程:  
  ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```  
  3. 生成xcode工程, 进入axis根目录执行如下命令之一:  
     - for arm64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AXIS_ROOT/cmake/ios.mini.cmake -DCMAKE_OSX_ARCHITECTURES=arm64```
     - for armv7,arm64 combined:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AXIS_ROOT/cmake/ios.mini.cmake "-DCMAKE_OSX_ARCHITECTURES=armv7;arm64"```
     - for simulator x86_64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AXIS_ROOT/cmake/ios.mini.cmake -DCMAKE_OSX_ARCHITECTURES=x86_64```

  4. 之后就可以用xcode打开, 选择cpp-tests编译运行

### 一些基于axis有趣的项目

- https://github.com/solan-solan/HeightMap/tree/smooth_lod_passing
- https://github.com/wzhengsen/StarryX
- https://github.com/aismann/SimpleSnake
- https://github.com/EugenyN/TanksKombat

### 注意
  * ThreadLocalStorage线程本地存储
    - ios x86 simulator ios>=10 and axis no longer provide x86 libraries
    - ios x64 or devices(armv7,arm64) ios sdk>=9.0
    - the openal-soft maintained by kcat use TLS

### QQ交流群

  点击链接加入群聊【axis 交流群】：https://jq.qq.com/?_wv=1027&k=nvNmzOIY
  
### 参考链接
  * official v4: https://gitee.com/mirrors/cocos2d-x
  * Git快速手册: https://github.com/axis-project/axis/discussions/411

### Axis项目活跃状态

![Alt](https://repobeats.axiom.co/api/embed/e46f10898672224fde3b102d81a28482164faf2e.svg "Repobeats analytics image")
