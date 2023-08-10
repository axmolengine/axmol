<p align="center"><a href="https://axmolengine.github.io/axmol" target="_blank" rel="noopener noreferrer"><img width="160" src="docs/logo.png" alt="axmol logo"></a></p>

# Axmol引擎

## A Multi-platform Engine for Desktop, XBOX (UWP) and Mobile games. (A fork of Cocos2d-x-4.0)

[![dev](https://img.shields.io/github/v/release/axmolengine/axmol?include_prereleases&label=release)](https://github.com/axmolengine/axmol/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/axmolengine/axmol/blob/master/LICENSE)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/81fa1aba09ab41a98b949064b928d06e)](https://www.codacy.com/gh/axmolengine/axmol/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=axmolengine/axmol&amp;utm_campaign=Badge_Grade)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/axmolengine/axmol/pulls)
[![QQ Group](https://img.shields.io/badge/QQGroup-join-green.svg)](https://jq.qq.com/?_wv=1027&k=nvNmzOIY)


> ⚠️⚠️⚠️ NOTICE ⚠️⚠️⚠️
>
> 本分支处于早期开阶段, 主要目标是:

> - OpenGL3/GLESv3支持
> - 废弃GLES2设备支持
> - Mesh实例化渲染支持
> - 使用`glslcc`作为新的引擎着色器工作流，开发者使用ESSL310或GLSL450语法编写shader, 引擎自动使用`glslcc`编译为目标支持平台语法： 
>   - Desktop GL: GLSL330
>   - GLESv3: ESSL300
>   - Metal: MSL
> - 增强shader缓存管理和加载机制，加载API更加易用
> - 其他更多优化，请查看: #1279


**[English](README.md)** 

## 支持平台列表
| Build | Status (github) |
|-------|-----------------|
| Windows(win32,winuwp,win32-clang)|[![Win32 CI](https://github.com/axmolengine/axmol/actions/workflows/windows.yml/badge.svg)](https://github.com/axmolengine/axmol/actions/workflows/windows.yml)|
| Android | [![Android Build Status](https://github.com/axmolengine/axmol/workflows/android/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aandroid)|
| iOS |[![iOS Build Status](https://github.com/axmolengine/axmol/workflows/ios/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aios)|
| tvOS |[![tvOS Build Status](https://github.com/axmolengine/axmol/workflows/tvos/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Atvos)|
| Linux |[![Linux Build Status](https://github.com/axmolengine/axmol/workflows/linux/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Alinux)|
| macOS |[![macOS Build Status](https://github.com/axmolengine/axmol/workflows/osx/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aosx)|

## 渲染后端

- OpenGL 3.3+ for Linux, macOS and Win32
- OpenGL ES 3.0+ for Android and iOS
- Metal for macOS and iOS
- ANGLE ES 3.0+ for Win32 and UWP

## 新的媒体播放器

新的媒体播放器`ui::MediaPlayer`(原命: `ui::VideoPlayer`) 将视频渲染为纹理，使得游戏对视频渲染控制更加灵活，就像普通UI一样，而无需考虑层级问题。这得益于重新升级的跨平台`MediaEngine`, 灵感来自微软的媒体基础库(Microsoft Media Foundation): `IMFMediaEngine`，目前已支持axmol引擎支持的所有平台。

|  Target Platform      | Video Compress Format | Video Pixel Format   | Container Format | Backend            |
|-----------------------|-----------------------|----------------------|----------------- |--------------------|
| Windows Desktop       | H264, HEVC, VP90      | YUY2, NV12, RGB32    | .mp4,.mkv,.webm  | IMFMediaSession    |
| Windows UWP           | H264, HEVC, VP90      | BGR32                | .mp4,.mkv,.webm  | IMFMediaEngine     |
| Apple macOS           | H264, HEVC(hvc1)      | NV12, BGR32          | .mp4             | AVFoundation       |
| Apple tvOS            | H264, HEVC(hvc1)      | NV12, BGR32          | .mp4             | AVFoundation       |
| Apple iOS             | H264, HEVC(hvc1)      | NV12, BGR32          | .mp4             | AVFoundation       |
| Android               | H264, HEVC            | NV12                 | .mp4,.mkv,.webm  | ExoPlayer2         |
| Linux                 | H264, HEVC            | NV12                 | .mp4,.mkv,.webm  | libVLC             |


## 构建

请查看: [Dev setup instructions](docs/DevSetup.md).
  
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


### 参考链接

- axmol引擎预编译库生成工具: [build1k](https://github.com/axmolengine/build1k), 
- axmol专用shader编译工具: [axmolengine/glslcc](https://github.com/axmolengine/glslcc), 
- 一些基于axmol引擎的开源项目: https://github.com/axmolengine/axmol/discussions/694
- official v4: https://gitee.com/mirrors/cocos2d-x
- Git快速手册: https://github.com/axmolengine/axmol/discussions/411

### axmol项目活跃状态

![Alt](https://repobeats.axiom.co/api/embed/6fcb8168a3af91ba9e797a1f14a3c2edc42ac56a.svg "Repobeats analytics image")
