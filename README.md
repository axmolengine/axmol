<p align="center"><a href="https://axmol.dev"><img width="160" src="https://axmol.dev/assets/img/logo.png" alt="Axmol logo"></a></p>

# Axmol

Axmol is an open-source, cross-platform game engine for 2D and 3D games. Build with C++ or Lua for mobile, desktop, WebAssembly, and Xbox via UWP.

[Try it online](#try-it-online) · [Get started](#get-started) · [Games made with Axmol](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol) · [简体中文](README_CN.md)

[![Build status](https://github.com/axmolengine/axmol/workflows/build/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Abuild)
[![Latest stable release](https://img.shields.io/github/v/release/axmolengine/axmol?label=v2%20LTS)](https://github.com/axmolengine/axmol/releases)
[![MIT license](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)
[![C++23 on v3 dev](https://img.shields.io/badge/v3%20dev-C%2B%2B23-8A2BE2.svg)](https://github.com/axmolengine/axmol/blob/dev/cmake/Modules/AXConfigDefine.cmake)

## Try it online

Run the [Axmol WebAssembly tests](https://axmol.netlify.app/wasm/cpp-tests/cpp-tests) or the [FairyGUI demo](https://axmol.netlify.app/wasm/fairygui-tests/fairygui-tests) in your browser. Explore [games and other projects made with Axmol](https://github.com/axmolengine/axmol/wiki/Made-in-Axmol).

## Choose a version

| Branch | Use it for | C++ standard |
| --- | --- | --- |
| [`release/2.x`](https://github.com/axmolengine/axmol/tree/release/2.x) | Stable v2 LTS releases and production projects | C++20 |
| [`dev`](https://github.com/axmolengine/axmol/tree/dev) (default) | v3 development and trying new features; APIs and experimental features may change | C++23 |

The [latest stable release](https://github.com/axmolengine/axmol/releases) is from v2 LTS. For v3 work, use `dev`; see the [v3 roadmap](https://github.com/axmolengine/axmol/discussions/2650). Contribution and branch rules are in [CONTRIBUTING.md](CONTRIBUTING.md).

## Get started

Install the compiler and [PowerShell 7](https://github.com/PowerShell/PowerShell/releases) for your platform, then follow the [setup and building guide](docs/DevSetup.md) for platform-specific prerequisites. For a stable v2 project:

```sh
git clone -b release/2.x https://github.com/axmolengine/axmol.git
cd axmol
./setup.ps1
```

Restart your terminal after setup, return to the cloned Axmol directory, then create and build a C++ project:

```sh
axmol new -p dev.axmol.hellocpp -d ./projects -l cpp HelloCpp
cd ./projects/HelloCpp
axmol
```

Use `-l lua` to create a Lua project. To try v3, clone the `dev` branch instead; its compiler requirements differ from v2. See the [full guide](docs/DevSetup.md) for Windows setup, other targets, and troubleshooting.

## What you can build

- **Across platforms:** iOS, Android, Windows, Linux, macOS, tvOS, WebAssembly, and Xbox through UWP.
- **2D and 3D:** graphics and scene APIs, [Box2D](https://github.com/axmolengine/axmol/wiki/2D-Physics-Engines-Information) for 2D physics, and Jolt for 3D physics on v3 dev.
- **Rendering and tools:** Metal, OpenGL and WebGL, with additional v3 backends described below; optional extensions include FairyGUI, ImGui, Spine, Live2D, and Effekseer. See the [extensions guide](https://github.com/axmolengine/axmol/wiki/Extensions).

See the [documentation](https://axmol.dev/manual/latest/), [tutorials](https://github.com/axmolengine/axmol/wiki/Tutorials), and [FAQ](https://github.com/axmolengine/axmol/wiki/FAQ) for details.

## What's changing in v3

The `dev` branch is under active development. Highlights are grouped by their current status:

- **Available in dev:** C++23, Jolt-based 3D physics, and high-DPI improvements for Windows, Linux, and WebAssembly.
- **Experimental:** Vulkan, D3D11, and D3D12 rendering backends, plus optional VR/OpenXR support.
- **Still in progress:** Box2D v3 integration and related 2D physics API work.

Follow the [v3 roadmap](https://github.com/axmolengine/axmol/discussions/2650) for the full list and progress. These v3 features are not claims about the stable v2 release.

## Background and migration

Axmol began in November 2019 as a fork of Cocos2d-x v4.0 and has continued to evolve with its contributors. If you have an existing Cocos2d-x project, see the [migration guide](https://github.com/axmolengine/axmol/wiki/Cocos2d%E2%80%90x-migration-guide) and [comparison](https://github.com/axmolengine/axmol/wiki/Axmol-vs-Cocos2d%E2%80%90x).

## Community and support

Ask questions in [GitHub Discussions](https://github.com/axmolengine/axmol/discussions), join [Discord](https://discord.gg/QjaQBhFVay), or read the [contribution guide](CONTRIBUTING.md) and [contributors list](AUTHORS.md). If Axmol is useful to you, a GitHub star helps others discover it. You can also [support development](https://axmol.dev/sponsor).

### Corporate Diamond sponsor

<a href="https://scorewarrior.com/?ad=axmol"><img src="https://cdn.prod.website-files.com/633da33305ac754156026dd8/63566f1edf5f0712f94f7f1b_sw-triangle-821890.svg" height="96" alt="Scorewarrior"></a>
