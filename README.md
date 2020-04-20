# engine-x
[![Build Status](https://travis-ci.com/c4games/engine-x.svg?branch=master)](https://travis-ci.com/c4games/engine-x)
[![Windows Build Status](https://ci.appveyor.com/api/projects/status/4936wev2r2ot606s/branch/master?svg=true)](https://ci.appveyor.com/project/halx99/engine-x)

This is another more radical fork of cocos2d-x game engine, use opanal for all platforms, single texture multi gpu texture handler, c++17...  
  
The readme of cocos2d-x v4: https://github.com/simdsoft/engine-x/blob/master/README.ccv4.md  
  
  
### Goals summary:
1. C++17
2. forcus on native game dev only
3. Remove unnecessary sources
4. Fix bugs ASAP
5. Review PR ASAP
6. If you have any other excellent goals, welcome
7. Excellent PRs from any guys are welcome, I will review & merge ASAP

### preprocessors notes: 

```txt
CC_STATIC  
  
_USRDLL  
_USEGUIDLL  
_USREXDLL  
_EXPORT_DLL_  
  
_USRSTUDIODLL  
_USRSTUDIOPDLL  
```

### Refactor core & extensions libs
- [ ] engine-x-core.dll(node tree, multi-renderer-backends, filesystem, audio, platform spec, and other):
  - [x] 1. Refactor audio engine, OpenAL for all platforms.
  - [x] 2. Remove SimpleAudio
  - [ ] 3. Remove offthread file operation
  - [x] 4. Remove getSutiableFOpen, avoid reduce performance, there is another better solution for support: ```just doesn't convert path to utf-8 in FileUtils manually implementation```
  - [x] 5. Refactor CCDownloader, curl for all platforms, don't use scheduler to retrive progress info, make it more reuseable
  - [ ] 7. Refactor engine-x-3rd, all compile as vs2019 MSVC 19.22.27905.0
  - [x] 8. minizip-1.2.0
  - [x] 9. ccMessageBox
  - [x] 10. BMP support with stb_image
  - [x] 11. Remove unused libuv
  - [ ] 12. Reorder child support
  - [x] 13. Remove tinyxml2 dep, use pugixml instead
  - [x] 14. Use xsxml for SAX parser only
  - [x] 15. Multi meta textures v1.0
  - [x] 16. ProgramStateRegistry
  - [ ] 17. Optimize CCFileUtils
  - [ ] 18. ANGLE support.
  - [x] 19. Use curl for HttpClient all platforms
  - [ ]  20. AES encrypt system
  - [x] 21. Remove xxtea
  - [x] 22. Remove rapidxml dep
  - [x] 23. fix etc1 alpha test case, content size incorrect
  - [ ] 24. Since getSuitableFopen removed, don't convert to utf-8 for win32, or in the future, windows 10 will support utf-8 file path, such as: For Chinese system language users:
在 控制面板-区域-更改系统区域设置 中
勾选 Beta版：使用 Unicode UTF=8 提供全球语言支持(U) 选项
  - [x] 25. ASTC 4x4, 8x8 support for all platforms.
  - [x] 26. Refine backend sources.
  - [x] 27. Refactor UserDefault based on file mapping with aes-cfb encrypt support.
  - [x] 28. Fix OpenGL error 0x501 when load incorrect & unused shader.
  - [x] 29. Fix OpenGL error 0x501 when use compressd texture on device, caused by backend TextureGL: initWithZeros
  - [ ] XX1. Cache ProgramState or use object_pool
  - [ ] XX2. Remove socket.io websockets, use yasio for socket connection solution
  - [ ] XX3. Fully streaming play for all media file: .mp4, .mp3, .ogg, .wav
  - [ ] XX4. Streaming load for .ttf with freetype2
  - [x] XX5. Remove CCB(already removed in v4)
  - [ ] XX6. Sync 3rd bin
  - [ ] XX7. Purely offthread load texture, use shared gl context
  - [ ] XX8. Use git submodule to manage 3rd libs.  
  
- [ ] Upgrade 3rd stubs to latest stable edition for all platforms
  - [ ] zlib to 1.2.11  
  
--------------------------------------------------------------------------    
- [ ] engine-x.ui.dll: The original v3-ui framework may remove in the future
- [ ] engine-x.ext.lib: old GUI ui(could be remove), physics-nodes, pu3d
- [ ] engine-x.dragonbones.lib
- [ ] engine-x.ccs20.dll: x-studio
- [ ] engine-x.ccs21.dll: x-studio
- [ ] engine-x.xs.dll: x-studio
- [ ] engine-x.spine.lib  
----------------------------------------------------------------------------  
- [ ] engine-x-lua.lib:
  - [x] Lua53 compat
  - [ ] make lua test works well
  - [ ] Remvoe luasocket, use yasio for socket connection solution
---------------------------------------------------------------------------  
!!! Custom APK support, don't compress .mp4, .mp3, .ogg, .wav, .ttf
---------------------------------------------------------------------------  
!!!! future of future:
   - [ ] Use sol2 for binding framework instead tolua++
   - [ ] Use bgfx for multi renderer backend support



### 快速开始
#### windows
  安装CMake，要求3.6以上<br>
  根据安装的 Visual Studio 版本，执行下面的命令，解决方案就生成在 build 目录下了。<br>
  打开cpp-tests.sln，编译运行。<br>
  ```
  cd engine-x\tests\cpp-tests
  cmake -S .\ -B .\build -G “Visual Studio 14 2015 Win32”
  or 
  cmake -S .\ -B .\build -G “Visual Studio 16 2019” -A Win32
  ``` 

#### android
  安装 Android Studio (推荐3.5.3)，打开 SDKManager，安装下列工具。<br>
  打开project，目录在engine-x\tests\cpp-tests\proj.android<br>
  等待Gradle sync完成后，Build APKs，安装运行<br>
  ```
    LLDB 推荐3.1
    CMake 推荐 3.10.2
    NDK 推荐 20.1
  ```
  
  如果报缺少Ninja, 从https://github.com/ninja-build/ninja/releases下载， 拷贝Ninja.exe到Cmake的bin目录即可

#### ios
  确保最新版xcode已安装
  安装brew: ```/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"```  
  brew安装完成后，确保如下工具安装:  
  ```sh
    brew update
    brew install git
    brew install cmake
    brew install autoconf
    brew install automake
    brew install libtool
  ```
  执行如下命令确保cmake能成功生成xcode工程:  
  ```sudo xcode-select -switch /Applications/Xcode.app/Contents/Developer```  
  生成xcode工程, 进入engine-x根目录执行如下命令:  
  ```sh
    mkdir build
    cd build
    cmake .. -GXcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_SYSROOT=iphonesimulator
  ```
  之后就可以用xcode打开, 选择cpp-tests编译运行，其他targets目前均无法编译或运行
  
