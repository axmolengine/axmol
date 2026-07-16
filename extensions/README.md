# The Axmol extensions

## DragonBones - Deprecrated

- Upstream: https://github.com/DragonBones/DragonBonesCPP
- Version: 4.5
- License: MIT

## Effekseer (OFF default)

- [![Upstream](https://img.shields.io/github/v/release/effekseer/Effekseer?label=Upstream)](https://github.com/effekseer/EffekseerForCocos2d-x)
- https://github.com/effekseer/EffekseerForCocos2d-x
- https://github.com/effekseer/Effekseer
- Version: 1.70
- License: MIT

## GUI - Deprecrated, should only used for the axmol tests (e.g. cpp-tests)

- Upstream: https://github.com/axmolengine/axmol
- Version: axmol-1.0
- License: MIT

## ImGui

- [![Upstream](https://img.shields.io/github/v/release/ocornut/imgui?label=Upstream)](https://github.com/ocornut/imgui)
- Version: 1.92.8-docking
- License: MIT
  
## Inspector

- Upstream: https://github.com/axmolengine/axmol
- Version: axmol-2.1
- License: MIT

## JSONDefault

- Upstream: https://github.com/axmolengine/axmol
- Version: axmol-2.1
- License: MIT

## Live2D (OFF default)

- Upstream: https://github.com/axmolengine/live2d-axmol
- Version: 5.5
- License: Live2D Proprietary Software License
- Limitation: The Live2D Core SDK does not provide prebuilt libraries for WinUWP or WebAssembly, so these platforms are not supported.

## Particle3D

- Upstream: https://github.com/axmolengine/axmol
- Version: axmol-1.0
- License: MIT

## SDFGen

- Upstream: https://github.com/axmolengine/axmol
- Version: axmol-2.1
- License: MIT

## Asset-Manager

- Upstream: https://github.com/axmolengine/axmol
- Version: axmol-1.0
- License: MIT

## CocosStudio

- Upstream: https://github.com/axmolengine/axmol
- Version: axmol-1.0
- License: MIT

## FairyGUI

- Upstream: https://github.com/axmolengine/axmol
- original repo: https://github.com/fairygui/FairyGUI-cocos2dx
- Version: axmol-1.0
- License: MIT

## physics-nodes

- Upstream: https://github.com/axmolengine/axmol
- Version: axmol-3.0
- License: MIT

## scripting/lua

- Upstream: https://github.com/axmolengine/axmol
- Version: axmol-1.0
- License: MIT

## spine

- Upstream: https://github.com/axmolengine/spine-axmol
- Version: 4.3-ce7b5d9 (Until Jun.13 2026), with modifications:
  - `String.h`: Added an unsafe String(std::string_view) constructor to bypass redundant strlen and memory copy operations.
  - `Interpolation.h`: Removes Interpolation’s inheritance from spine::SpineObject to prevent destructor calls from accessing a released AxmolSpineExtension during shutdown.
- License: Spine Runtimes License
