<p align="center"><a href="https://axmol.dev"><img width="160" src="https://axmol.dev/assets/img/logo.png" alt="Axmol 标志"></a></p>

# Axmol

Axmol 是采用 MIT 许可证的开源跨平台游戏引擎，支持使用 C++ 或 Lua 开发 2D 和 3D 游戏，并发布到移动端、桌面端、WebAssembly，以及通过 UWP 发布到 Xbox。

[在线体验](#在线体验) · [快速开始](#快速开始) · [Axmol 作品](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol) · [English](README.md)

[![构建状态](https://github.com/axmolengine/axmol/workflows/build/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Abuild)
[![最新稳定版](https://img.shields.io/github/v/release/axmolengine/axmol?label=latest%20stable)](https://github.com/axmolengine/axmol/releases)
[![MIT 许可证](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![v3 dev 使用 C++23](https://img.shields.io/badge/v3%20dev-C%2B%2B23-8A2BE2.svg)](https://github.com/axmolengine/axmol/blob/dev/cmake/Modules/AXConfigDefine.cmake)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/axmolengine/axmol)

## 在线体验

在浏览器中运行 [Axmol WebAssembly 测试](https://axmol.netlify.app/wasm/cpp-tests/cpp-tests)或 [FairyGUI 演示](https://axmol.netlify.app/wasm/fairygui-tests/fairygui-tests)，还可以查看[使用 Axmol 制作的游戏和其他项目](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol)。

## 迈向 v3 beta1

`dev` 分支上的 v3 beta1 主要功能开发已基本完成，目前重点是最后的整理和发布准备工作。

- **已集成：** C++23、Box2D v3 和 Jolt 物理、与 Metal 和 GL 并存的 Vulkan/D3D11/D3D12 渲染后端，以及 Windows、Linux 和 WebAssembly 的高分屏改进。
- **实验性功能：** 可选的 VR/OpenXR 支持。

`dev` 分支已按以下组合默认启用渲染后端，一次构建可包含多个后端。

| 平台 | 默认启用的后端 |
| --- | --- |
| Windows | D3D11、D3D12、GL、Vulkan |
| Linux | Vulkan、GL |
| Android | Vulkan、GL |
| WinUWP | D3D12、D3D11、GL |
| iOS / macOS | Metal |

GL 按平台指 OpenGL 或 OpenGL ES；WebAssembly 使用 WebGL。配置方式见[渲染构建选项](CMakeOptions.md)，进展见 [v3 路线图](https://github.com/axmolengine/axmol/discussions/2650)。上述 v3 能力不代表稳定版 v2 已支持。

## 选择版本

| 分支 | 适用场景 | C++ 标准 |
| --- | --- | --- |
| [`dev`](https://github.com/axmolengine/axmol/tree/dev)（默认分支） | v3 开发及新功能试用；API 和实验性功能可能变化 | C++23 |
| [`release/2.x`](https://github.com/axmolengine/axmol/tree/release/2.x) | 稳定的 v2 LTS 版本及正式项目 | C++20 |

v3 beta1 正在筹备中，`dev` 仍是 v3 的预发布分支。[最新稳定发行版](https://github.com/axmolengine/axmol/releases)属于 v2 LTS。贡献和分支规则见 [CONTRIBUTING.md](CONTRIBUTING.md)。

## 快速开始

安装支持 C++23 的编译器和 [PowerShell 7](https://github.com/PowerShell/PowerShell/releases)，并先阅读[安装与构建指南](docs/DevSetup.md)中的平台要求。体验默认 `dev` 分支的 v3：

```sh
git clone https://github.com/axmolengine/axmol.git
cd axmol
./setup.ps1
```

运行安装脚本后重启终端，回到刚克隆的 Axmol 目录，再创建并构建 C++ 项目：

```sh
axmol new -p dev.axmol.hellocpp -d ./projects -l cpp HelloCpp
cd ./projects/HelloCpp
axmol
```

使用 `-l lua` 可创建 Lua 项目。正式项目如需使用稳定的 v2 LTS，请改为克隆 `release/2.x` 分支，其要求为 C++20。Windows 环境准备、其他平台构建及常见问题见[完整指南](docs/DevSetup.md)。

## 可以构建什么

- **跨平台：** iOS、Android、Windows、Linux、macOS、tvOS、WebAssembly，以及通过 UWP 支持 Xbox。
- **2D 与 3D：** 图形与场景 API；v3 dev 已集成 Box2D v3 用于 2D 物理、Jolt 用于 3D 物理。
- **扩展：** FairyGUI、ImGui、Spine、Live2D 和 Effekseer，详见[扩展介绍](https://github.com/axmolengine/axmol/wiki/Extensions)。

更多内容请参阅[文档](https://axmol.dev/manual/latest/)、[教程](https://github.com/axmolengine/axmol/wiki/Tutorials)和[常见问题](https://github.com/axmolengine/axmol/wiki/FAQ)。

## 项目背景与迁移

Axmol 于 2019 年 11 月从 Cocos2d-x v4.0 分支开始，并在贡献者的参与下持续演进。已有 Cocos2d-x 项目的开发者可参考[迁移指南](https://github.com/axmolengine/axmol/wiki/Cocos2d%E2%80%90x-migration-guide)和[从 Cocos2d-x v4.0 到 Axmol v3 的重大变化](https://github.com/axmolengine/axmol/wiki/Axmol-v3-VS-Cocos2d%E2%80%90x-4.0)。

## 社区与支持

[![Discord](https://img.shields.io/discord/1099599084895088670?label=discord)](https://discord.gg/QjaQBhFVay)
[![欢迎贡献 PR](https://img.shields.io/badge/PRs-welcome-blue.svg)](CONTRIBUTING.md)
[![AtomGit stars](https://atomgit.com/axmol/axmol/star/badge.svg)](https://atomgit.com/axmol/axmol)

国内开发者也可访问 [AtomGit 镜像](https://atomgit.com/axmol/axmol)。

欢迎在 [GitHub Discussions](https://github.com/axmolengine/axmol/discussions) 提问，加入 [Discord](https://discord.gg/QjaQBhFVay) 或[QQ 群](https://jq.qq.com/?_wv=1027&k=nvNmzOIY)，并阅读[贡献指南](CONTRIBUTING.md)和[贡献者名单](AUTHORS.md)。如果 Axmol 对你有帮助，GitHub star 能帮助更多人发现项目。你也可以[支持项目开发](https://axmol.dev/sponsor)。

### 企业钻石级赞助者

<a href="https://scorewarrior.com/?utm_source=axmol"><img src="https://cdn.prod.website-files.com/633da33305ac754156026dd8/63566f1edf5f0712f94f7f1b_sw-triangle-821890.svg" height="96" alt="Scorewarrior"></a>

## 项目活跃度

查看 [awesome-cpp](https://github.com/fffaraz/awesome-cpp)。

![Axmol 仓库活跃度](https://repobeats.axiom.co/api/embed/6fcb8168a3af91ba9e797a1f14a3c2edc42ac56a.svg "Repobeats analytics")
