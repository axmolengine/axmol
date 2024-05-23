<p align="center"><a href="https://axmol.dev" target="_blank" rel="noopener noreferrer"><img width="160" src="docs/logo.png" alt="axmol logo"></a></p>

# Axmol Engine

## A Multi-platform Engine for Mobile, Desktop and Xbox.

[Axmol Engine](https://axmol.dev) 是一个开源的专为移动、桌面和XBox设备设计的 C++ 多平台引擎。该项目于2019年11月作为`Cocos2d-X-4.0`的一个分支启动。

请 [访问我们的 Wiki](https://github.com/axmolengine/axmol/wiki) 了解关于 Axmol 的更多信息。

***

[![Latest Release](https://img.shields.io/github/v/release/axmolengine/axmol?label=release)](https://github.com/axmolengine/axmol/releases)
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

[英文版本 / English](README.md)

***

## 构建

* [下载最新版本](https://github.com/axmolengine/axmol/releases) / [镜像仓库](https://gitee.com/simdsoft/axmol)
* [如何安装](docs/DevSetup.md)

如果你有一个 Cocos2d-x 项目, 可以很容易迁移到 Axmol。 我们准备了 [迁移指导](https://github.com/axmolengine/axmol/wiki/Cocos2d%E2%80%90x-migration-guide)。

## 教程和文档

* [API文档](https://axmol.dev/manual/latest/)
* [Axmol Wiki](https://github.com/axmolengine/axmol/wiki)
* [教程](https://github.com/axmolengine/axmol/wiki/Tutorials)
* [常见问题](https://github.com/axmolengine/axmol/wiki/FAQ)

## 引擎概要

Axmol 引擎在 Cocos2d-x v4.0 的基础上进行迭代和改进。现在速度更快，能力更强，同时保持轻量级。 [请查看对比差异](https://github.com/axmolengine/axmol/wiki/Axmol-vs-Cocos2d%E2%80%90x)。

**支持平台**:

* 移动设备: iOS, Android
* 桌面设备: Windows, Linux, macOS, tvOS
* 主机设备: Xbox (Windows通用平台)
* Web: 主流浏览器 (Chrome，Edge，Firefox) (在线预览: [Axmol tests](https://axmol.netlify.app/wasm/cpp-tests/cpp-tests) / [FairyGUI tests](https://axmol.netlify.app/wasm/fairygui-tests/fairygui-tests))

**支持语言**:

* C++
* Lua

**渲染后端**:

* Metal： macOS、iOS、tvOS
* OpenGL 3.3+： Linux、macOS、Win32
* OpenGL ES 2.0+： Android
* OpenGL ES 3.0+： iOS
* ANGLE GLES 3.0+： Win32、 UWP
* WebGL 2.0 (OpenGL ES 3.0): WASM

**支持架构**:

* iOS、tvOS (x64, arm64)
* Android (x86, x64, armv7, arm64)
* Windows (x86, x64)
* Linux (x64)
* OSX (x64, arm64)
* UWP (x64, arm64)
* Wasm32

**支持2D物理引擎** ([更多相关信息](https://github.com/axmolengine/axmol/wiki/2D-Physics-Engines-informations)):

* Box2D
* Chipmunk2D

**支持3D物理引擎**:

* Bullet Physics SDK

**支持扩展**:

* FairyGUI
* ImGUI
* Spine
* Live2D
* Effekseer
* 更多 [扩展 Wiki 页](https://github.com/axmolengine/axmol/wiki/Extensions)

[由我们的社区创建的更多扩展](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol) 

## 特性

一些亮点:

- **图形驱动升级：GLES3.0、OpenGL3.3核心模式支持，3D模型实例化渲染支持，全新跨平台 Shader 工作流**。
- **全新媒体播放器**: 基于 MediaEngine 的视频纹理渲染，支持所有 Native 平台. [详见我们的 Wiki 页](https://github.com/axmolengine/axmol/wiki/Media-Player)。
- **WebAssembly 支持** (by @nowasm) - Preview: [Axmol tests](https://axmol.netlify.app/wasm/cpp-tests/cpp-tests)
- **Windows 通用** (UWP) Xbox 主机平台支持。
- **Apple M1 和 Android x64 支持** (by @pietpukkel)。
- **Windows x64 构建支持**。
- **Windows SIMD 加速指令矩阵计算支持**。
- 支持链接引擎预编译库。 详见 [Windows 工作流指导](https://github.com/axmolengine/axmol/issues/564)。
- 重构 AudioEngine，所有平台均使用 OpenAL 并新增 `.wav` 音频格式支持。
- 使用内存映射文件 [mio](https://github.com/mandreyel/mio) 重构 UserDefault，速度提升百倍。 
- 使用 yasio 重构 HttpClient 支持并发请求。
- 使用 yasio 重构 WebSocket。
- 模块化所有扩展，所有扩展均可选编译。
- Windows 下使用 Google [Angle](https://github.com/google/angle) 作为默认渲染后端，提升设备兼容性。
- 使用更加现代的 GL 加载器：`Glad`。
- 使用 [pugixml](https://pugixml.org) 作为引擎唯一 XML 解析和生成库。
- 使用 [curl](https://github.com/curl/curl) 作为 Downloader 全平台实现库。
- 使用 SAX 解析 .plist 文件。
- 新增 ASTC 4x4 / 6x6 / 8x8 格式支持 (如果硬件不支持，则使用SIMD加速指令集进并行解码)。
- 新增 ETC2 RGB / RGBA 格式支持 (如果硬件不支持，则使用 ANGLE 提供的 API 进行软解)。
 
[请阅读相对于 Cocos2d-x-4.0 的完整改动列表](CHANGELOG.md)。

版本计划，请查看[里程碑](https://github.com/axmolengine/axmol/milestones)。

## 贡献

如何贡献:

* 非常感谢建议、错误修复和改进
  - [贡献说明](CONTRIBUTING.md)
  - [贡献者列表](AUTHORS.md)
* 请不要忘记将主题 axmol 和 axmolengine 添加到您创建的任何相关存储库中
* 在 GitHub 上添加星也将有助于 Axmol 的成长。谢谢你!

如果您发现 Axmol 引擎有帮助，请考虑在 [Open Collective](https://opencollective.com/axmol) 或 [GitHub Sponsors](https://github.com/sponsors/axmolengine) 赞助，非常感谢！

## 社区

我们的社区正在成长！请加入我们：
 * [GitHub 论坛](https://github.com/axmolengine/axmol/discussions) (提问和讨论引擎的首选方式)
 * [Discord](https://discord.gg/QjaQBhFVay)
 * [Reddit](https://www.reddit.com/r/axmol/)
 * [点击加入QQ交流群](https://jq.qq.com/?_wv=1027&k=nvNmzOIY)

要查看我们的一些社区作品，请访问我们的 Axmol 展示页：[Made in Axmol](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol)

## 参考链接

- [1kiss](https://github.com/simdsoft/1kiss): 一个由 PowerShell 编写的能够自动配置各平台编译工具链的跨平台构建工具。
- [axmolengine/glslcc](https://github.com/axmolengine/glslcc): 一个 glslcc 的分支, 用于 Axmol 引擎跨平台 Shader 编译工作流。
- [Cocos2d-x](https://github.com/cocos2d/cocos2d-x)：原 Cocos2d-x 官方引擎仓库。

## Axmol 活跃状态

![Alt](https://repobeats.axiom.co/api/embed/6fcb8168a3af91ba9e797a1f14a3c2edc42ac56a.svg "Repobeats analytics image")
