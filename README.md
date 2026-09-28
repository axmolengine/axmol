<p align="center"><a href="https://axmol.dev"><img width="160" src="https://axmol.dev/assets/img/logo.png" alt="Axmol logo"></a></p>

# Axmol

Axmol is an MIT-licensed, cross-platform game engine for 2D and 3D games. Build with C++ or Lua for mobile, desktop, WebAssembly, and Xbox via UWP.

[Try it online](#try-it-online) · [Get started](#get-started) · [Games made with Axmol](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol) · [简体中文](README_CN.md)

[![Build status](https://github.com/axmolengine/axmol/workflows/build/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Abuild)
[![Latest stable release](https://img.shields.io/github/v/release/axmolengine/axmol?label=latest%20stable)](https://github.com/axmolengine/axmol/releases)
[![MIT license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++23 on v3 dev](https://img.shields.io/badge/v3%20dev-C%2B%2B23-8A2BE2.svg)](https://github.com/axmolengine/axmol/blob/dev/cmake/Modules/AXConfigDefine.cmake)
[![Ask DeepWiki](https://deepwiki.com/badge.svg)](https://deepwiki.com/axmolengine/axmol)

## Try it online

Run the [Axmol WebAssembly tests](https://axmol.netlify.app/wasm/cpp-tests/cpp-tests) or the [FairyGUI demo](https://axmol.netlify.app/wasm/fairygui-tests/fairygui-tests) in your browser. Explore [games and other projects made with Axmol](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol).

## Toward v3 beta1

Major feature development for v3 beta1 is largely complete on `dev`. Current work focuses on final cleanup and release preparation.

- **Integrated:** C++23, Box2D v3 and Jolt physics, Vulkan/D3D11/D3D12 rendering backends alongside Metal and GL, and high-DPI improvements for Windows, Linux, and WebAssembly.
- **Experimental:** optional VR/OpenXR support.

The following rendering backends are enabled by default on `dev`. A build can include multiple backends.

| Platform | Default-enabled backends |
| --- | --- |
| Windows | D3D11, D3D12, GL, Vulkan |
| Linux | Vulkan, GL |
| Android | Vulkan, GL |
| WinUWP | D3D12, D3D11, GL |
| iOS / macOS | Metal |

GL refers to OpenGL or OpenGL ES depending on the platform. WebAssembly uses WebGL. See [rendering build options](CMakeOptions.md) for configuration and the [v3 roadmap](https://github.com/axmolengine/axmol/discussions/2650) for progress. These v3 capabilities do not describe the stable v2 release.

## Choose a version

| Branch | Use it for | C++ standard |
| --- | --- | --- |
| [`dev`](https://github.com/axmolengine/axmol/tree/dev) (default) | v3 development and trying new features; APIs and experimental features may change | C++23 |
| [`release/2.x`](https://github.com/axmolengine/axmol/tree/release/2.x) | Stable v2 LTS releases and production projects | C++20 |

v3 beta1 is in preparation, and `dev` remains the v3 pre-release branch. The [latest stable release](https://github.com/axmolengine/axmol/releases) is from v2 LTS. Contribution and branch rules are in [CONTRIBUTING.md](CONTRIBUTING.md).

## Get started

Install a C++23 compiler and [PowerShell 7](https://github.com/PowerShell/PowerShell/releases), then follow the [setup and building guide](docs/DevSetup.md) for platform-specific prerequisites. To try v3 from the default `dev` branch:

```sh
git clone https://github.com/axmolengine/axmol.git
cd axmol
./setup.ps1
```

Restart your terminal after setup, return to the cloned Axmol directory, then create and build a C++ project:

```sh
axmol new -p dev.axmol.hellocpp -d ./projects -l cpp HelloCpp
cd ./projects/HelloCpp
axmol
```

Use `-l lua` to create a Lua project. For a production project on stable v2 LTS, clone `release/2.x` instead; it requires C++20. See the [full guide](docs/DevSetup.md) for Windows setup, other targets, and troubleshooting.

## What you can build

- **Across platforms:** iOS, Android, Windows, Linux, macOS, tvOS, WebAssembly, and Xbox through UWP.
- **2D and 3D:** graphics and scene APIs, with Box2D v3 for 2D physics and Jolt for 3D physics on v3 dev.
- **Extensions:** FairyGUI, ImGui, Spine, Live2D, and Effekseer. See the [extensions guide](https://github.com/axmolengine/axmol/wiki/Extensions).

See the [documentation](https://axmol.dev/manual/latest/), [tutorials](https://github.com/axmolengine/axmol/wiki/Tutorials), and [FAQ](https://github.com/axmolengine/axmol/wiki/FAQ) for details.

## Background and migration

Axmol began in November 2019 as a fork of Cocos2d-x v4.0 and has continued to evolve with its contributors. If you have an existing Cocos2d-x project, see the [migration guide](https://github.com/axmolengine/axmol/wiki/Cocos2d%E2%80%90x-migration-guide) and [major changes from Cocos2d-x v4.0 to Axmol v3](https://github.com/axmolengine/axmol/wiki/Axmol-v3-VS-Cocos2d%E2%80%90x-4.0).

## Community and support

[![Discord](https://img.shields.io/discord/1099599084895088670?label=discord)](https://discord.gg/QjaQBhFVay)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](CONTRIBUTING.md)

Ask questions in [GitHub Discussions](https://github.com/axmolengine/axmol/discussions), join [Discord](https://discord.gg/QjaQBhFVay), or read the [contribution guide](CONTRIBUTING.md) and [contributors list](AUTHORS.md). If Axmol is useful to you, a GitHub star helps others discover it. You can also [support development](https://axmol.dev/sponsor).

### Corporate Diamond sponsor

<a href="https://scorewarrior.com/?utm_source=axmol"><img src="https://cdn.prod.website-files.com/633da33305ac754156026dd8/63566f1edf5f0712f94f7f1b_sw-triangle-821890.svg" height="96" alt="Scorewarrior"></a>

## Project activity

Explore [awesome-cpp](https://github.com/fffaraz/awesome-cpp).

![Axmol repository activity](https://repobeats.axiom.co/api/embed/6fcb8168a3af91ba9e797a1f14a3c2edc42ac56a.svg "Repobeats analytics")
