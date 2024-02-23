<p align="center"><a href="https://axmol.org" target="_blank" rel="noopener noreferrer"><img width="160" src="docs/logo.png" alt="axmol logo"></a></p>

# Axmol Engine

## A Multi-platform Engine for Mobile, Desktop and Xbox.

[Axmol Engine](https://axmol.org) 是一个开源的专为移动设备，桌面设备和XBox设备设计的C++多平台引擎。该项目于2019年11月作为`Cocos2d-X-4.0`的一个分支启动

请 [访问我们的Wiki](https://github.com/axmolengine/axmol/wiki) 了解关于 Axmol 的更多信息.

***

[![Latest Release](https://img.shields.io/github/v/release/axmolengine/axmol?include_prereleases&label=release)](https://github.com/axmolengine/axmol/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/81fa1aba09ab41a98b949064b928d06e)](https://www.codacy.com/gh/axmolengine/axmol/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=axmolengine/axmol&amp;utm_campaign=Badge_Grade)
[![cxxstd](https://img.shields.io/badge/cxxstd-c++20-8A2BE2.svg)](https://github.com/axmolengine/axmol)  

[![Build Status](https://github.com/axmolengine/axmol/workflows/build/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Abuild)
[![WASM Build Status](https://ci.appveyor.com/api/projects/status/4slq965fwhbaidnc?svg=true&passingText=wasm%20-%20passing)](https://ci.appveyor.com/project/halx99/axmol)

![issues](https://img.shields.io/github/issues/axmolengine/axmol?style=plastic)
![forks](https://img.shields.io/github/forks/axmolengine/axmol?style=plastic)
![stars](https://img.shields.io/github/stars/axmolengine/axmol?style=plastic)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/axmolengine/axmol?style=plastic)  

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/axmolengine/axmol/pulls)
[![Discord](https://img.shields.io/discord/1099599084895088670?label=discord)](https://discord.gg/QjaQBhFVay)

[Chinese ver. / 简体中文](README_CN.md)

***

## 构建

* [下载最新版本](https://github.com/axmolengine/axmol/releases) / [镜像仓库](https://gitee.com/simdsoft/axmol)
* [如何安装](docs/DevSetup.md)

如果你有一个 Cocos2d-x 项目, 可以很容易迁移到 Axmol。 我们准备了 [迁移引导](https://github.com/axmolengine/axmol/wiki/Cocos2d%E2%80%90x-migration-guide).

## 教程和文档

* [API文档](https://axmol.org/manual/latest/)
* [Axmol Wiki](https://github.com/axmolengine/axmol/wiki)
* [教程](https://github.com/axmolengine/axmol/wiki/Tutorials)
* [常见问题](https://github.com/axmolengine/axmol/wiki/FAQ)

## 概要

Axmol 引擎在 Cocos2d-x v4.0 的基础上经i行了迭代和改进。现在速度更快，能力更强，同时保持轻量级. [请查看对比差异](https://github.com/axmolengine/axmol/wiki/Axmol-vs-Cocos2d%E2%80%90x).

**支持平台**:
* 移动设备: iOS, Android
* 桌面设备: Windows, Linux, macOS, tvOS
* 主机设备: Xbox (Windows通用平台)
* Web: WebAssembly (预览版: [Axmol tests](https://axmol.netlify.app/wasm/cpp-tests/cpp-tests) / [FairyGUI tests](https://axmol.netlify.app/wasm/fairygui-tests/fairygui-tests))

**支持语言**:
* C++
* Lua

**渲染后端**:
* Metal for macOS and iOS
* OpenGL 3.3+ for Linux, macOS and Win32
* OpenGL ES 2.0+ for Android
* OpenGL ES 3.0+ for iOS
* ANGLE GLES 3.0+ for Win32 and UWP
* WebGL 2.0 (OpenGL ES 3.0): WASM

**支持架构**:
* iOS (x64, arm64)
* Android (x86, x64, armv7, arm64)
* Windows (x86, x64)
* Linux (x64)
* OSX (x64, arm64)
* UWP (x64, arm64)
* wasm32

**支持2D物理引擎** ([更多细节请查看](https://github.com/axmolengine/axmol/wiki/2D-Physics-Engines-informations)):
* Box2D
* Box2D-optimized
* Chipmunk2D

**支持3D物理引擎**:
* Bullet Physics SDK

**支持扩展**:
* FairyGUI
* ImGUI
* Spine
* Live2D
* Effekseer
* And more! Check our [Extensions Wiki Page](https://github.com/axmolengine/axmol/wiki/Extensions)

[More extensions created by our community](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol) 

## 特性

一些亮点:

- **New MediaPlayer**: render video as texture2D using MediaEngine. [More info in our Wiki](https://github.com/axmolengine/axmol/wiki/Media-Player).
- Windows video player support (based on Microsoft Media Foundation)
- **WebAssembly support** (by @nowasm) - Preview: [Axmol tests](https://axmol.netlify.app/wasm/cpp-tests/cpp-tests)
- **Universal Windows Platform** (UWP) support for Xbox consoles
- **Apple M1 and Android x64 support** (by @pietpukkel)
- **Windows x64 build support**
- Improved Windows workflow, supporting linking with engine prebuilt libs. Please read the [Windows workflow guide](https://github.com/axmolengine/axmol/issues/564).
- Refactored AudioEngine, OpenAL for all platforms:
    - [OpenAL Soft](https://github.com/kcat/openal-soft) (pass `-DAX_USE_ALSOFT=ON` to CMake to force enabling it)
    - [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL) (if no `AX_USE_ALSOFT` option specified, cmake script will choose it on OSX/iOS/tvOS, even though it was marked as deprecated is still available)
- Implemented all .wav formats supported by OpenAL Soft (MS-ADPCM, ADPCM, etc.)
- Refactored UserDefault with [mio](https://github.com/mandreyel/mio)
- [Upstream Version License](thirdparty/README.md) - Third-party:
    - Third-party license overview, for easier publishing of your commercial apps based on Axmol framework.
    - Some links to third party libs which support axmol too.
- [Upstream Version License](extensions/README.md) - Extensions:
    - Extensions license overview, for easier publishing of your commercial apps based on Axmol framework.
- Reimplemented HttpClient based on yasio for concurrent http requests processing.
- Modularized all optional extensions, move from engine core folder to an extensions folder.
- Google [Angle](https://github.com/google/angle) renderer backend support
- Using GL loader Glad.
- Using [pugixml](https://pugixml.org)
- Using [curl](https://github.com/curl/curl) for transferring data with URL syntax
- Using SAX parser for .plist files
- ASTC 4x4 / 6x6 / 8x8 support (if hardware decoding is not supported, then software decoding is used)
- ETC2 RGB / RGBA support (if hardware decoding is not supported, then software decoding is used)
- iOS / tvOS SDK 11.0 as minimal deployment.
 
Please read the [full list of changes since Cocos2d-x v4.0](CHANGELOG.md). 

See the [Milestones](https://github.com/axmolengine/axmol/milestones) for planned features.

## 贡献

如何贡献:

* 非常感谢建议、错误修复和改进
  - [贡献说明](CONTRIBUTING.md)
  - [贡献者列表](AUTHORS.md)
* 请不要忘记将主题添加 axmol 到 axmolengine 您创建的任何相关存储库中
* 在 GitHub 上添加星也将有助于 Axmol 的成长。谢谢你

如果您发现 Axmol 引擎有帮助，请考虑在 [Open Collective](https://opencollective.com/axmol) 或 in [GitHub Sponsors](https://github.com/sponsors/axmolengine)赞助，非常感谢！

## 社区

我们的社区正在成长！请加入我们：
 * [GitHub 轮胎](https://github.com/axmolengine/axmol/discussions) (提问和讨论引擎的首选方式)
 * [Discord](https://discord.gg/QjaQBhFVay)
 * [Reddit](https://www.reddit.com/r/axmol/)

要查看我们的一些社区作品，请查看我们的 Axmol 展示：[Made in Axmol](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol)

## 参考链接

- [1kiss](https://github.com/simdsoft/1kiss): A cross-platform one step build powershell script, with auto setup general dependent tools.
- [axmolengine/glslcc](https://github.com/axmolengine/glslcc): A fork of glslcc, for Axmol new shader workflow tool.
- [Official Cocos2d-x](https://github.com/cocos2d/cocos2d-x)

## Axmol 活跃状态

![Alt](https://repobeats.axiom.co/api/embed/6fcb8168a3af91ba9e797a1f14a3c2edc42ac56a.svg "Repobeats analytics image")
