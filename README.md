<p align="center"><a href="https://axmolengine.github.io/axmol" target="_blank" rel="noopener noreferrer"><img width="160" src="docs/logo.png" alt="axmol logo"></a></p>

# Axmol Engine

## A Multi-platform Engine for Desktop, XBOX (UWP) and Mobile games. (A fork of Cocos2d-x-4.0)

[![dev](https://img.shields.io/github/v/release/axmolengine/axmol?include_prereleases&label=release)](https://github.com/axmolengine/axmol/releases)
[![LICENSE](https://img.shields.io/badge/license-MIT-blue.svg)](https://github.com/axmolengine/axmol/blob/master/LICENSE)
[![Codacy Badge](https://app.codacy.com/project/badge/Grade/81fa1aba09ab41a98b949064b928d06e)](https://www.codacy.com/gh/axmolengine/axmol/dashboard?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=axmolengine/axmol&amp;utm_campaign=Badge_Grade)
[![cxxstd](https://img.shields.io/badge/cxxstd-c++20-8A2BE2.svg)](https://github.com/axmolengine/axmol)
[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-blue.svg)](https://github.com/axmolengine/axmol/pulls)
[![Discord](https://img.shields.io/discord/1099599084895088670?label=discord)](https://discord.com/channels/1099599084895088670)
  
![issues](https://img.shields.io/github/issues/axmolengine/axmol?style=plastic)
![forks](https://img.shields.io/github/forks/axmolengine/axmol?style=plastic)
![stars](https://img.shields.io/github/stars/axmolengine/axmol?style=plastic)
![GitHub code size in bytes](https://img.shields.io/github/languages/code-size/axmolengine/axmol?style=plastic)  


**[简体中文](README_CN.md)**

## Supported Platforms

| Build | Status (github) |
|-------|-----------------|
| Windows(win32,winuwp,win32-clang)|[![Win32 CI](https://github.com/axmolengine/axmol/actions/workflows/windows.yml/badge.svg)](https://github.com/axmolengine/axmol/actions/workflows/windows.yml)|
| Android | [![Android Build Status](https://github.com/axmolengine/axmol/workflows/android/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aandroid)|
| iOS |[![iOS Build Status](https://github.com/axmolengine/axmol/workflows/ios/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aios)|
| tvOS |[![tvOS Build Status](https://github.com/axmolengine/axmol/workflows/tvos/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Atvos)|
| Linux |[![Linux Build Status](https://github.com/axmolengine/axmol/workflows/linux/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Alinux)|
| macOS |[![macOS Build Status](https://github.com/axmolengine/axmol/workflows/osx/badge.svg)](https://github.com/axmolengine/axmol/actions?query=workflow%3Aosx)|
| WASM(experimental) |[![WASM Build Status](https://ci.appveyor.com/api/projects/status/4slq965fwhbaidnc?svg=true&passingText=wasm%20-%20passing)](https://ci.appveyor.com/project/halx99/axmol)|

## Building

View the [Dev setup instructions](docs/DevSetup.md).

## Axmol WebAssembly preview
https://axmol.netlify.app/

## Renderer Backends

- Metal for macOS and iOS
- OpenGL 3.3+ for Linux, macOS and Win32
- OpenGL ES 2.0+ for Android
- OpenGL ES 3.0+ for iOS
- ANGLE GLES 3.0+ for Win32 and UWP
- WebGL 2.0(OpenGL ES 3.0): WASM (experimental)

## New MediaPlayer

The new `ui::MediaPlayer`(renamed from `ui::VideoPlayer`) render video as Texture2D with designed corss-platform MediaEngine, the MediaEngine inspired from microsoft
media foundation `IMFMediaEngine`, the `MediaPlayer` support all native platforms.

|  Target Platform      | Video Compress Format | Video Pixel Format   | Container Format | Backend            |
|-----------------------|-----------------------|----------------------|----------------- |--------------------|
| Windows Desktop       | H264, HEVC, VP90      | YUY2, NV12, RGB32    | .mp4,.mkv,.webm  | IMFMediaSession    |
| Windows UWP           | H264, HEVC, VP90      | BGR32                | .mp4,.mkv,.webm  | IMFMediaEngine     |
| Apple macOS           | H264, HEVC(hvc1)      | NV12, BGR32          | .mp4             | AVFoundation       |
| Apple tvOS            | H264, HEVC(hvc1)      | NV12, BGR32          | .mp4             | AVFoundation       |
| Apple iOS             | H264, HEVC(hvc1)      | NV12, BGR32          | .mp4             | AVFoundation       |
| Android               | H264, HEVC            | NV12                 | .mp4,.mkv,.webm  | ExoPlayer2         |
| Linux                 | H264, HEVC            | NV12                 | .mp4,.mkv,.webm  | libVLC             |
| WASM                  | NA                    | NA                   | NA               | NA                 |

## Highlighted Features

- WebAssembly support(experimental) by @nowasm
- Windows UWP support, refer to: https://github.com/axmolengine/axmol/pull/1108
- Add apple M1, android x64 support, contributed by @pietpukkel
- Improve windows workflow, support linking with engine prebuilt libs, read [windows workflow guide](https://github.com/axmolengine/axmol/issues/564)
- Windows video player support (based on microsoft media foundation)
- Windows x64 build support
- Reimplement HttpClient based on yasio for concurrent http requests processing.
- ['Upstream-Version-License'](thirdparty/README.md) Third-party
  - Third-party license overview for easier publishing of your commercial apps based on axmol framework.
  - Some links to third party libs which support axmol too.
- ['Upstream-Version-License'](extensions/README.md) Extensions
  - Extensions license overview for easier publishing of your commercial apps based on axmol framework.
  - Spine support
  - ```FairyGUI``` support
  - DragonBones support
  - Live2D support
  - **ImGui integrated, easy to write game embedded tools, very easy to use, read [ImGui](extensions/ImGui/README.md) for more info**
- Refactor AudioEngine, OpenAL for all platforms
  - [OpenAL Soft](https://github.com/kcat/openal-soft), pass -DAX_USE_ALSOFT=ON to cmake to force enabling it
  - [OpenAL.framework](https://opensource.apple.com/tarballs/OpenAL), if no ```AX_USE_ALSOFT``` option specified, cmake script will choose it on osx/ios/tvos, even though it was marked as deprecated, but still available.
- Refactor UserDefault with [mio](https://github.com/mandreyel/mio)
- Modularize all optional extensions, move from engine core folder to an extensions folder
- Implement all .wav formats supported by ```OpenAL Soft```, such as MS-ADPCM, ADPCM, ...
- Use a modern GL loader ```Glad```
- Google [angle](https://github.com/google/angle) renderer backend support
- IOS/TVOS SDK 11.0 as minimal deployment
- Use fast pugixml
- Use [curl](https://github.com/curl/curl) for transferring data with URL syntax
- Use SAX parser for all plist files
- ASTC 4x4/6x6/8x8 support (if hardware decoding is not supported, then software decoding is used)
- ETC2 RGB/RGBA support    (if hardware decoding is not supported, then software decoding is used)
- Supported 2D physics engines (see also [APPENDIX.md](APPENDIX.md)):
  - Box2D
  - Box2D-optimized
  - Chipmunk2D
- Supported 3D physics engines:
  - Bullet Physics SDK

[Read Full changes since cocos2d-x-4.0](CHANGELOG.md)

Open [APPENDIX.md](APPENDIX.md) for additional information and see [Milestones](https://github.com/axmolengine/axmol/milestones) for planed features too.

## Reference links

- [build1k](https://github.com/axmolengine/build1k), the axmol prebuilt build-bot
- [axmolengine/glslcc](https://github.com/axmolengine/glslcc), a fork of glslcc for axmol new shader workflow tool
- [Interesting related](https://github.com/axmolengine/axmol/discussions/694)
- [Axmols contributing guide](https://github.com/axmolengine/axmol/discussions/411)
- [Official Cocos2d-x](https://github.com/cocos2d/cocos2d-x)

## The axmol Active Stats

![Alt](https://repobeats.axiom.co/api/embed/6fcb8168a3af91ba9e797a1f14a3c2edc42ac56a.svg "Repobeats analytics image")
