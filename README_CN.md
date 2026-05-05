<p align="center"><a href="https://axmol.dev" target="_blank" rel="noopener noreferrer"><img width="160" src="https://axmol.dev/assets/img/logo.png" alt="axmol logo"></a></p>

# Axmol Engine

## A Multi-platform Engine for Desktop, XBOX (UWP), WebAssembly and Mobile games.

[Axmol Engine](https://axmol.dev) 是一个开源的专为移动、桌面和 XBox 设备设计的 C++ 多平台引擎。该项目于 2019 年 11 月作为 `Cocos2d-X-4.0` 的一个分支启动。

请 [访问我们的 Wiki](https://github.com/axmolengine/axmol/wiki) 了解关于 Axmol 的更多信息。

***

[![Build Status](https://github.com/axmolengine/axmol/workflows/build/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Abuild)

[![Latest Release](https://img.shields.io/github/v/release/axmolengine/axmol?label=release)](https://github.com/axmolengine/axmol/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/81fa1aba09ab41a98b949064b928d06e)](https://www.codacy.com/gh/axmolengine/axmol/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=axmolengine/axmol&amp;utm_campaign=Badge_Grade)
[![cxxstd](https://img.shields.io/badge/cxxstd-c++20-8A2BE2.svg)](https://github.com/axmolengine/axmol)  

![issues](https://img.shields.io/github/issues/axmolengine/axmol?style=plastic)
![forks](https://img.shields.io/github/forks/axmolengine/axmol?style=plastic)
![stars](https://img.shields.io/github/stars/axmolengine/axmol?style=plastic)
![G-Star](https://atomgit.com/axmol/axmol/star/badge.svg)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/axmolengine/axmol?style=plastic)  

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/axmolengine/axmol/pulls)
[![QQ Group](https://pub.idqqimg.com/wpa/images/group.png)](https://jq.qq.com/?_wv=1027&k=nvNmzOIY)
[![awesome-cpp](https://badgen.net/badge/icon/awesome-cpp/pink?icon=awesome&label&color)](https://github.com/fffaraz/awesome-cpp)
[![ossinsight](https://badgen.net/badge/icon/ossinsight/pink?icon=awesome&label&color)](https://ossinsight.io/collections/game-engine/)

[英文版本 / English](README.md)

***

## 📢 分支说明

> **重要提示**
> - **`dev` 分支**：作为 **v3** 版本的开发分支，处于持续开发与变更阶段，可能包含不稳定或实验性功能。**各类PR（新功能、功能增强、非关键bug修复等）均优先提交至该分支**，仅v2 LTS版本的关键修复可例外。
> - **`release/2.x` 分支**：已进入**稳定维护阶段**，**2.11.x 为 v2 系列最后一个 LTS 长期支持版本**；该分支仅接收关键bug修复与安全补丁，依旧是生产环境的推荐分支，可保障正式项目的稳定性与兼容性。
>
> 如需参与 v3 版本开发、测试新特性，或提交非关键修复类PR，请使用 `dev` 分支；生产环境部署仍可使用 `release/2.x` 分支，享受LTS版本的稳定维护保障。
>
> ⚠️ **请勿向 `release/2.x` 分支的PR中执行全文件 `clang-format` 格式化，或进行大规模代码排版调整操作；同时，非关键的功能开发、功能增强类PR也请勿提交至该分支。**
> 大规模格式化改动会显著增加代码合并冲突，提升代码审查的难度并消耗额外时间；而基于该分支的LTS维护定位，非关键类PR也无法被接受。
> 任何违反上述规则的PR **将不会被接收**。

***

## ⚡️构建

* 下载: [官方最新发布](https://github.com/axmolengine/axmol/releases) / [AtomGit 镜像仓库](https://atomgit.com/axmol/axmol) / [SourceForge 镜像](https://sourceforge.net/projects/axmol-engine.mirror) / [Gitee 镜像仓库](https://gitee.com/simdsoft/axmol)
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

* Vulkan: Windows, Linux, Android (axmol-v3 版本新增)
* D3D12: Windows and UWP (axmol-v3 版本新增)
* D3D11: Windows and UWP (axmol-v3 版本新增)
* Metal： macOS、iOS、tvOS
* OpenGL 3.3+： Linux、macOS、Win32
* OpenGL ES 2.0+： Android
* OpenGL ES 3.0+： iOS
* ANGLE GLES 3.0+： Win32、 UWP
* WebGL 2.0 (OpenGL ES 3.0): WASM

**支持架构**:

* iOS、tvOS (x64, arm64)
* Android (x86, x64, armv7, arm64)
* Windows (x86, x64, arm64)
* Linux (x64, arm64)
* OSX (x64, arm64)
* UWP (x64, arm64)
* Wasm32, Wasm64(axmo-v3 版本新增)

*注意: Axmol v3 开始提供对 Linux 和 Windows 的 arm64 构建支持*

**支持2D物理引擎** ([更多相关信息](https://github.com/axmolengine/axmol/wiki/2D-Physics-Engines-informations)):

* Box2D

**支持3D物理引擎**:

* JoltPhysics

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

- **新增 Vulkan 渲染后端支持** (axmol-v3)
- **新增 D3D12 渲染后端支持** (axmol-v3)
- **新增 D3D11 渲染后端支持** (axmol-v3)
- **新增桌面平台 Windows/Linux/WebAssembly 高分屏(HiDPI) 支持** (axmol-v3)
- **图形驱动升级：GLES3.0、OpenGL3.3核心模式支持，3D模型实例化渲染支持，全新跨平台 Shader 工作流**。
- **全新媒体播放器**: 基于 MediaEngine 的视频纹理渲染，支持所有 Native 平台. [详见我们的 Wiki 页](https://github.com/axmolengine/axmol/wiki/Media-Player)。
- **WebAssembly 支持** (by @nowasm) - Preview: [Axmol tests](https://axmol.netlify.app/wasm/cpp-tests/cpp-tests)
- **Windows 通用** (UWP) Xbox 主机平台支持。
- **Apple M1 和 Android x64 支持** (by @pietpukkel)。
- **Windows x64 构建支持**。
- **Windows SIMD 加速指令矩阵计算支持**。
- **强大的跨平台构建系统**。
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
- 从旧项目迁移 VR 渲染支持，并加以增强 (axmol-v3)
 
[请阅读相对于 Cocos2d-x-4.0 的完整改动列表](CHANGELOG.md)。

版本计划，请查看[里程碑](https://github.com/axmolengine/axmol/milestones)。

## 贡献

如何贡献:

* 非常感谢建议、错误修复和改进
  - [贡献说明](CONTRIBUTING.md)
  - [贡献者列表](AUTHORS.md)
* 请不要忘记将主题 axmol 和 axmolengine 添加到您创建的任何相关存储库中
* 在 GitHub 上添加星也将有助于 Axmol 的成长。谢谢你!

如果您发现 Axmol 引擎有帮助，请考虑在 [https://axmol.dev/donate](https://axmol.dev/donate) 赞助，非常感谢！

## Sponsors

#### Platinum Sponsors
<p align="left">
  <a href="https://scorewarrior.com/?ad=axmol" target=_blank><img
    src="https://cdn.prod.website-files.com/633da33305ac754156026dd8/63566f1edf5f0712f94f7f1b_sw-triangle-821890.svg" height="120"
  /></a>
</p>

<p>&nbsp;</p>

#### More Sponsors

Please visit: [https://axmol.dev/donate](https://axmol.dev/donate)

## 社区

我们的社区正在成长！请加入我们：
 * [GitHub 论坛](https://github.com/axmolengine/axmol/discussions) (提问和讨论引擎的首选方式)
 * [Discord](https://discord.gg/QjaQBhFVay)
 * [Reddit](https://www.reddit.com/r/axmol/)
 * [点击加入QQ交流群](https://jq.qq.com/?_wv=1027&k=nvNmzOIY)

要查看我们的一些社区作品，请访问我们的 Axmol 展示页：[Made in Axmol](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol)

## 参考链接

- [1kiss](https://github.com/simdsoft/1kiss): 一个由 PowerShell 编写的能够自动配置各平台编译工具链的跨平台构建工具。
- [axslcc](https://github.com/axmolengine/axslcc): Axmol 引擎专用跨平台 Shader 编译器（fork 自 `glslcc`)。
- [Cocos2d-x](https://github.com/cocos2d/cocos2d-x)：原 Cocos2d-x 官方引擎仓库。

## Axmol 活跃状态

![Alt](https://repobeats.axiom.co/api/embed/6fcb8168a3af91ba9e797a1f14a3c2edc42ac56a.svg "Repobeats analytics image")
