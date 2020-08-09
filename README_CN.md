# EGNX
[![Build Status](https://travis-ci.com/c4games/engine-x.svg?branch=master)](https://travis-ci.com/c4games/engine-x)
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/4936wev2r2ot606s/branch/master?svg=true)](https://ci.appveyor.com/project/halx99/engine-x)

**这是另外一个基于 ```cocos2d-x-4.0``` 持续维护的分支, 全平台基于OpenAL, 删除不必要的平台相关, c++14/17...**
  
**[English](README.md)**
  
### 宗旨:
* C++14/17
* Forcus on native game dev only
* Fix bugs ASAP
* Review PR ASAP
* Excellent PRs from any guys are welcome, we will review & merge ASAP
  
### 主要特性:
* 重构AudioEngine, 全平台OpenAL, 在iOS平台可以通过```-DCC_USE_ALSOFT_ON_APPLE```切换为 [openal-soft](https://github.com/kcat/openal-soft), 苹果公司已经在iOS12将OpenAL framework标记为废弃的
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
  
### [Roadmap](https://github.com/c4games/engine-x/issues/1)
  
### 快速开始

#### 必要工具 [python](https://www.python.org/downloads/)
  * python-2.7.17+, python-3.7+都可以

#### Windows
  1. 安装CMake，要求3.6以上  
  2. 确保Visual Studio 2019以正确安装
  3. 执行下面的命令
  ```bat
  cd engine-x
  cmake -S . -B build -G "Visual Studio 16 2019" -A Win32
  ``` 
  4. 之后就可以用vs打开```engine-x/build/Cocos2d-x.sln```启动cpp-tests等测试工程了

#### Android
  1. 安装 Android Studio 3.5.3或更高版本
  2. 第一次启动Android Studio, 会引导安装一些工具，默认安装即可，如果不需要模拟器可以把```Virutal Device```勾选去掉
  2. 启动 Android Studio并打开engine-x\tests\cpp-tests\proj.android
  3. 通过 [Tools][SdkManagers] 打开sdk管理工具，选择如下工具后， 点击按钮 ```Apply``` 一路跟着引导安装即可:
     * Android SDK Build-Tools 29.0.2  
     * NDK r16b+  
     * CMake 3.10.2+  

  5. 等待Gradle sync完成后，Build APKs，安装运行  
  6. 如果报缺少Ninja, 从https://github.com/ninja-build/ninja/releases下载， 拷贝Ninja.exe到Cmake的bin目录即可

#### iOS
  1. 确保已安装xcode11+
  2. 安装brew: ```/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"```  
  brew安装完成后，确保如下工具安装:  
  ```sh
    brew update
    brew install git
    brew install cmake
    brew install autoconf
    brew install automake
    brew install libtool
  ```
  3. 执行如下命令确保cmake能成功生成xcode工程:  
  ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```  
  4. 生成xcode工程, 进入engine-x根目录执行如下命令:  
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
  5. 之后就可以用xcode打开, 选择cpp-tests编译运行

### 注意
  * ThreadLocalStorage线程本地存储
    - ios x86 simulator ios>=10
    - ios x64 or devices(armv7,arm64) ios sdk>=9.0
    - the openal-soft maintained by kcat use TLS

### QQ交流群

  点击链接加入群聊【engine-x交流群】：https://jq.qq.com/?_wv=1027&k=nvNmzOIY
  
### 参考链接
  * engine-x-3rd: https://github.com/c4games/engine-x-3rd
  * official v4: https://github.com/cocos2d/cocos2d-x
