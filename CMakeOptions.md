# The axmol CMake options

## The options for axmol engine
- AX_BUILD_TESTS: whether build test porojects: cpp-tests, lua-tests, fairygui-tests, default: `TRUE`
- AX_ENABLE_XXX for core feature: 
  - AX_ENABLE_MSEDGE_WEBVIEW2: whether enable msedge webview2, default: `TRUE`
  - AX_ENABLE_MFMEDIA: whether enable microsoft media foundation for windows video player support, default: `TRUE`
- AX_USE_XXX:
  - AX_USE_ALSOFT: whether use openal-soft for all platforms
    - Apple platform: Use openal-soft instead system deprecated: `OpenAL.framework`
    - Other platforms: Always use openal-soft even this option not enabled
  - AX_USE_COMPAT_GL: whether use compat gl as renderer backend, default: `FALSE`
    - windows: whether use angleproject as GLES2 backend
    - apple: whether use GLES instead Metal backend
  - AX_USE_LUAJIT: whether use luajit, default: `FALSE`, use plainlua
- AX_ENABLE_EXT_XXX for extensions
  - AX_ENABLE_EXT_GUI: the traditional GUI extension, default: `TRUE`
  - AX_ENABLE_EXT_ASSETMANAGER: the assetmanager extension, default: `TRUE`
  - AX_ENABLE_EXT_PARTICLE3D: the particle3d extension, default: `TRUE`
  - AX_ENABLE_EXT_PHYSICS_NODE: the physics_node extension, default: `TRUE`
  - AX_ENABLE_EXT_SPINE: the spine extension, default: `TRUE`
  - AX_ENABLE_EXT_DRAGONBONES: the dragonbones extension: `TRUE`
  - AX_ENABLE_EXT_COCOSTUDIO: the cocosstudio extension for load .csb: `TRUE`
  - AX_ENABLE_EXT_FAIRYGUI: the fairygui extension, default: `TRUE`
  - AX_ENABLE_EXT_IMGUI: the imgui extension, only support PC platforms, default: `TRUE` 
  - AX_ENABLE_EXT_LIVE2D: the imgui extension, only support PC platforms, default: `FALSE` 
  - AX_ENABLE_EXT_EFFEKSEER: the imgui extension, only support PC platforms, default: `FALSE` 
- AX_WITH_XXX: usually user don't need care it

## The options for axmol apps

- AX_PREBUILT_DIR: specific the prebuilt dir (relative to `AX_ROOT`), it's very useful for fast linking apps with prebuilt engine libs
