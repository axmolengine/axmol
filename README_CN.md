<p align="center"><a href="https://axmol.dev"><img width="160" src="https://axmol.dev/assets/img/logo.png" alt="Axmol 标志"></a></p>

# Axmol

Axmol 是开源的跨平台游戏引擎，支持使用 C++ 或 Lua 开发 2D 和 3D 游戏，并发布到移动端、桌面端、WebAssembly，以及通过 UWP 发布到 Xbox。

[在线体验](#在线体验) · [快速开始](#快速开始) · [Axmol 作品](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol) · [English](README.md)

[![构建状态](https://github.com/axmolengine/axmol/workflows/build/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Abuild)
[![最新稳定版](https://img.shields.io/github/v/release/axmolengine/axmol?label=v2%20LTS)](https://github.com/axmolengine/axmol/releases)
[![MIT 许可证](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![v3 dev 使用 C++23](https://img.shields.io/badge/v3%20dev-C%2B%2B23-8A2BE2.svg)](https://github.com/axmolengine/axmol/blob/dev/cmake/Modules/AXConfigDefine.cmake)

## 在线体验

在浏览器中运行 [Axmol WebAssembly 测试](https://axmol.netlify.app/wasm/cpp-tests/cpp-tests)或 [FairyGUI 演示](https://axmol.netlify.app/wasm/fairygui-tests/fairygui-tests)，还可以查看[使用 Axmol 制作的游戏和其他项目](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol)。

## 选择版本

| 分支 | 适用场景 | C++ 标准 |
| --- | --- | --- |
| [`release/2.x`](https://github.com/axmolengine/axmol/tree/release/2.x) | 稳定的 v2 LTS 版本及正式项目 | C++20 |
| [`dev`](https://github.com/axmolengine/axmol/tree/dev)（默认分支） | v3 开发及新功能试用；API 和实验性功能可能变化 | C++23 |

[最新稳定发行版](https://github.com/axmolengine/axmol/releases)属于 v2 LTS。开发或体验 v3 请使用 `dev` 分支，并查看 [v3 路线图](https://github.com/axmolengine/axmol/discussions/2650)。贡献和分支规则见 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 快速开始

安装适用于目标平台的编译器和 [PowerShell 7](https://github.com/PowerShell/PowerShell/releases)。各平台的环境要求请先阅读[安装与构建指南](docs/DevSetup.md)。创建稳定的 v2 项目：

```sh
git clone -b release/2.x https://github.com/axmolengine/axmol.git
cd axmol
./setup.ps1
```

运行安装脚本后重启终端，回到刚克隆的 Axmol 目录，再创建并构建 C++ 项目：

```sh
axmol new -p dev.axmol.hellocpp -d ./projects -l cpp HelloCpp
cd ./projects/HelloCpp
axmol
```

使用 `-l lua` 可创建 Lua 项目。体验 v3 时改为克隆 `dev` 分支，其编译器要求与 v2 不同。Windows 环境准备、其他平台构建及常见问题见[完整指南](docs/DevSetup.md)。

## 可以构建什么

- **跨平台：** iOS、Android、Windows、Linux、macOS、tvOS、WebAssembly，以及通过 UWP 支持 Xbox。
- **2D 与 3D：** 图形与场景 API；2D 物理使用 [Box2D](https://github.com/axmolengine/axmol/wiki/2D-Physics-Engines-Information)，v3 dev 的 3D 物理使用 Jolt。
- **渲染与扩展：** Metal、OpenGL 和 WebGL；v3 的其他渲染后端见下文。可选扩展包括 FairyGUI、ImGui、Spine、Live2D 和 Effekseer，详见[扩展介绍](https://github.com/axmolengine/axmol/wiki/Extensions)。

更多内容请参阅[文档](https://axmol.dev/manual/latest/)、[教程](https://github.com/axmolengine/axmol/wiki/Tutorials)和[常见问题](https://github.com/axmolengine/axmol/wiki/FAQ)。

## v3 进展

`dev` 分支仍在积极开发中。以下按目前状态列出部分重要变化：

- **已在 dev 分支提供：** C++23、基于 Jolt 的 3D 物理，以及 Windows、Linux 和 WebAssembly 的高分屏改进。
- **实验性功能：** Vulkan、D3D11、D3D12 渲染后端，以及可选的 VR/OpenXR 支持。
- **仍在开发：** Box2D v3 集成及相关 2D 物理 API 工作。

完整项目和进展见 [v3 路线图](https://github.com/axmolengine/axmol/discussions/2650)。上述 v3 功能不代表稳定版 v2 已支持。

## 项目背景与迁移

Axmol 于 2019 年 11 月从 Cocos2d-x v4.0 分支开始，并在贡献者的参与下持续演进。已有 Cocos2d-x 项目的开发者可参考[迁移指南](https://github.com/axmolengine/axmol/wiki/Cocos2d%E2%80%90x-migration-guide)和[差异对比](https://github.com/axmolengine/axmol/wiki/Axmol-vs-Cocos2d%E2%80%90x)。

## 社区与支持

欢迎在 [GitHub Discussions](https://github.com/axmolengine/axmol/discussions) 提问，加入 [Discord](https://discord.gg/QjaQBhFVay) 或[QQ 群](https://jq.qq.com/?_wv=1027&k=nvNmzOIY)，并阅读[贡献指南](CONTRIBUTING.md)和[贡献者名单](AUTHORS.md)。如果 Axmol 对你有帮助，GitHub star 能帮助更多人发现项目。你也可以[支持项目开发](https://axmol.dev/sponsor)。

### 企业钻石级赞助者

<a href="https://scorewarrior.com/?ad=axmol"><img src="https://cdn.prod.website-files.com/633da33305ac754156026dd8/63566f1edf5f0712f94f7f1b_sw-triangle-821890.svg" height="96" alt="Scorewarrior"></a>
