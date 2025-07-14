# NOTE: Changing the value of AX_EXT_HINT after cmake build files have been created
# will not update all other options that are using AX_EXT_HINT.  You must delete the
# {build_dir}/CMakeCache.txt file and then re-run the `axmol build ...` command in
# order for the new setting to take effect. Alternatively, you can also delete the
# entire build folder and then re-run the `axmol build` command.
# set(AX_EXT_HINT ON CACHE BOOL "The default extensions hint" FORCE)

# The following are by default set the same as the AX_EXT_HINT flag above - default: ON
# Uncomment the lines below to set them individually
# set(AX_ENABLE_EXT_LUA OFF CACHE BOOL "Build lua libraries" FORCE)
# set(AX_ENABLE_EXT_GUI OFF CACHE BOOL "Build extension GUI" FORCE)
# set(AX_ENABLE_EXT_ASSETMANAGER OFF CACHE BOOL "Build extension asset-manager" FORCE)
# set(AX_ENABLE_EXT_SPINE OFF CACHE BOOL "Build extension spine" FORCE)
# set(AX_ENABLE_EXT_DRAGONBONES OFF CACHE BOOL "Build extension DragonBones" FORCE)
# set(AX_ENABLE_EXT_COCOSTUDIO OFF CACHE BOOL "Build extension cocostudio" FORCE)
# set(AX_ENABLE_EXT_FAIRYGUI OFF CACHE BOOL "Build extension FairyGUI" FORCE)
# set(AX_ENABLE_EXT_IMGUI OFF CACHE BOOL "Build extension ImGui" FORCE)
# set(AX_ENABLE_EXT_JSONDEFAULT OFF CACHE BOOL "Build extension JSONDefault" FORCE)

# 3D Support - - default: ON
# set(AX_ENABLE_3D ON CACHE BOOL "Build 3D support" FORCE)
# These depend on AX_EXT_HINT & AX_ENABLE_3D
# set(AX_ENABLE_EXT_PARTICLE3D ON CACHE BOOL "Build extension Particle3D" FORCE)
# set(AX_ENABLE_3D_PHYSICS ON CACHE BOOL "Build Physics3D support" FORCE)
# set(AX_ENABLE_NAVMESH ON CACHE BOOL "Build NavMesh support" FORCE)

# Physics Support - - default: ON
# set(AX_ENABLE_PHYSICS ON CACHE BOOL "Build Physics support" FORCE)
# These depend on AX_EXT_HINT & AX_ENABLE_PHYSICS
# set(AX_ENABLE_EXT_PHYSICS_NODE ON CACHE BOOL "Build extension physics-nodes" FORCE)

# These depend on AX_EXT_HINT & AX_ENABLE_EXT_IMGUI - default: ON
# set(AX_ENABLE_EXT_INSPECTOR ON CACHE BOOL "Enable extension Inspector" FORCE)
# set(AX_ENABLE_EXT_SDFGEN ON CACHE BOOL "Build extension SDFGen" FORCE)

# The follow options are set individually - default: OFF
# set(AX_ENABLE_EXT_LIVE2D OFF CACHE BOOL "Build extension Live2D" FORCE)
# set(AX_ENABLE_EXT_EFFEKSEER OFF CACHE BOOL "Build extension Effekseer" FORCE)

# Code modules that can be disabled - default: ON
# set(AX_ENABLE_AUDIO ON CACHE BOOL "Build audio support" FORCE)
# set(AX_ENABLE_WEBSOCKET ON CACHE BOOL "Build Websocket client based on yasio" FORCE)
# set(AX_ENABLE_HTTP ON CACHE BOOL "Build HTTP client based on yasio" FORCE)
# set(AX_ENABLE_OPUS ON CACHE BOOL "Build with opus support" FORCE)

# WEBVIEW2 - default: ON for WIN32 and WINRT
# set(AX_ENABLE_MSEDGE_WEBVIEW2 ON CACHE BOOL "Disable msedge webview2")
