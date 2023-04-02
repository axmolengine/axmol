<p align="center"><a href="https://axmolengine.github.io/axmol" target="_blank" rel="noopener noreferrer"><img width="160" src="docs/logo.png" alt="axmol logo"></a></p>

# Axmol引擎

## A Multi-platform Engine for Desktop, XBOX (UWP) and Mobile games. (A radical fork of Cocos2d-x-4.0)

[![dev](https://img.shields.io/github/v/release/axmolengine/axmol?include_prereleases&label=release)](https://github.com/axmolengine/axmol/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/axmolengine/axmol/blob/master/LICENSE)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/81fa1aba09ab41a98b949064b928d06e)](https://www.codacy.com/gh/axmolengine/axmol/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=axmolengine/axmol&amp;utm_campaign=Badge_Grade)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/axmolengine/axmol/pulls)

 
**[English](README.md)** 

## 支持平台列表
| Build | Status (github) |
|-------|-----------------|
| Windows Desktop|[![Win32 CI](https://github.com/axmolengine/axmol/actions/workflows/windows.yml/badge.svg)](https://github.com/axmolengine/axmol/actions/workflows/windows.yml)|
| Windows Desktop(Clang)|[![Win32 CI](https://github.com/axmolengine/axmol/actions/workflows/windows-clang.yml/badge.svg)](https://github.com/axmolengine/axmol/actions/workflows/windows-clang.yml)|
| Windows UWP|[![Win32 CI](https://github.com/axmolengine/axmol/actions/workflows/winuwp.yml/badge.svg)](https://github.com/axmolengine/axmol/actions/workflows/winuwp.yml)|
| Android | [![Android Build Status](https://github.com/axmolengine/axmol/workflows/android/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aandroid)|
| iOS |[![iOS Build Status](https://github.com/axmolengine/axmol/workflows/ios/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aios)|
| tvOS |[![tvOS Build Status](https://github.com/axmolengine/axmol/workflows/tvos/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Atvos)|
| Linux |[![Linux Build Status](https://github.com/axmolengine/axmol/workflows/linux/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Alinux)|
| macOS |[![macOS Build Status](https://github.com/axmolengine/axmol/workflows/osx/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aosx)|

## 基于VideoTexture的视频播放引擎

|  Platform             |    MediaEngine    | Video Compress Format | Video Pixel Format   | Backend            |
|-----------------------|-------------------|-----------------------|----------------------|--------------------|
| Windows Desktop       |    complete       | H264, HEVC, VP90      | YUY2, NV12, RGB32    | IMFMediaSession    |
| Windows UWP           |    complete       | H264, HEVC, VP90      | BGR32                | IMFMediaEngine     |
| Apple macOS           |    complete       | H264, HEVC(hvc1)      | NV12, BGR32          | AVFoundation       |
| Apple tvOS            |     complete      | H264, HEVC(hvc1)      | NV12, BGR32          | AVFoundation       |
| Apple iOS             |    in progress    | H264, HEVC(hvc1)      | NV12, BGR32          | AVFoundation       |
| Android               |    planned        | H264                  | RGB32                |                    |

### 关于预编译库

- 所有预编译库均通过[buildware](https://github.com/axmolengine/buildware)使用GitHub Actions自动构建
- 预编译库支持架构
  - windows: x86,x64
  - linux: x64
  - macos: x64, arm64(M1芯片)
  - android: armv7, arm64, x86, x64
  - ios: arm64, x64
  - tvos: arm64, x64

### 宗旨:
* C++17/20
* 专注C++
* Fix bugs ASAP
* Review PR ASAP
  
### 主要特性:

* Windows UWP 支持，请参考: https://github.com/axmolengine/axmol/pull/1108
* 增加tvos支持
* 增强Windows下工作流，支持链接引擎预编译库, 用法请查看: [windows workflow guide](https://github.com/axmolengine/axmol/issues/564)
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
* C++最低标准要求: C++17/20
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
  * python-3.7+

#### 准备步骤
  1. 进入引擎```axmol```根目录
  3. 执行```python setup.py```后重启控制台

#### Windows
  1. 安装CMake，要求3.22以上  
  2. 确保 Visual Studio 2019/2022 已正确安装
  3. 执行下面的命令
      - PC: ```cmake -S . -B build -G "Visual Studio 16 2019"```

      - UWP: ```cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_SYSTEM_NAME=WindowsStore "-DCMAKE_SYSTEM_VERSION=10.0" "-DAX_VS_DEPLOYMENT_TARGET=10.0.17763.0"```
  4. 之后就可以用vs打开```axmol/build/axmol.sln```启动cpp-tests等测试工程了

#### Android
  1. 安装 Android Studio 2021.1.1+
  2. 第一次启动Android Studio, 会引导安装一些工具，默认安装即可，如果不需要模拟器可以把```Virutal Device```勾选去掉
  2. 启动 Android Studio并打开axmol\tests\cpp-tests\proj.android
  3. 通过 [Tools][SdkManagers] 打开sdk管理工具，选择如下SDK Platforms和SDK Tools后， 点击按钮 ```Apply``` 一路跟着引导安装即可:
     * Android SDK Platform 33  (在SDK Platforms页签)  
     * Android Gradle Plugin (AGP) 7.2.2  
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
  3. 生成xcode工程, 进入axmol根目录执行如下命令之一:  
     - for ios arm64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=OS64```
     - for ios armv7,arm64 combined:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=OS```
     - for ios simulator x86_64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=SIMULATOR64```
     - for tvos arm64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=TVOS```
     - for tvos simulator x86_64:  
     ```cmake -S . -B build -GXcode -DCMAKE_TOOLCHAIN_FILE=$AX_ROOT/cmake/ios.toolchain.cmake -DPLATFORM=SIMULATOR_TVOS```
     - for macos x86_64(Intel)
     ```cmake -S . -B build -GXcode -DCMAKE_OSX_ARCHITECTURES=x86_64```
     - for macos arm64(M1)
     ```cmake -S . -B build -GXcode -DCMAKE_OSX_ARCHITECTURES=arm64```

  4. 之后就可以用xcode打开, 选择cpp-tests编译运行

### 一些基于axmol有趣的项目

- https://github.com/solan-solan/HeightMap/tree/smooth_lod_passing
- https://github.com/wzhengsen/StarryX
- https://github.com/aismann/SimpleSnake
- https://github.com/EugenyN/TanksKombat

### 注意
  * ThreadLocalStorage线程本地存储
    - ios x86 simulator ios>=10 and axmol no longer provide x86 libraries
    - ios x64 or devices arm64 ios sdk>=9.0
    - the openal-soft maintained by kcat use TLS

### QQ交流群

  点击链接加入群聊【axmol 交流群】：https://jq.qq.com/?_wv=1027&k=nvNmzOIY
  
### 参考链接
  * official v4: https://gitee.com/mirrors/cocos2d-x
  * Git快速手册: https://github.com/axmolengine/axmol/discussions/411

### axmol项目活跃状态

![Alt](https://repobeats.axiom.co/api/embed/6fcb8168a3af91ba9e797a1f14a3c2edc42ac56a.svg "Repobeats analytics image")
