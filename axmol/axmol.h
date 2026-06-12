/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#pragma once

#include "axmol/axmolver.h"

//
// all axmol include files
//
#include "axmol/base/Config.h"

// base
#include "axmol/base/AutoreleasePool.h"
#include "axmol/base/Environment.h"
#include "axmol/base/Logging.h"
#include "axmol/base/Data.h"
#include "axmol/base/Director.h"
#include "axmol/base/InputDelegate.h"
#include "axmol/base/InputSystem.h"
#include "axmol/base/Map.h"
#include "axmol/base/Profiling.h"
#include "axmol/base/Properties.h"
#include "axmol/base/Object.h"
#include "axmol/base/RefPtr.h"
#include "axmol/base/Scheduler.h"
#include "axmol/base/UserDefault.h"
#include "axmol/base/Value.h"
#include "axmol/base/Vector.h"
#include "axmol/base/ZipUtils.h"
#include "axmol/base/base64.h"
#include "axmol/base/Config.h"
#include "axmol/base/Macros.h"
#include "axmol/base/Types.h"
#include "axmol/base/text_utils.h"
#include "axmol/base/Utils.h"

// EventDispatcher
#include "axmol/base/AccelerationEvent.h"
#include "axmol/base/CustomEvent.h"
#include "axmol/base/EventDispatcher.h"
#include "axmol/base/FocusEvent.h"
#include "axmol/base/KeyboardEvent.h"
#include "axmol/base/AccelerationEventListener.h"
#include "axmol/base/CustomEventListener.h"
#include "axmol/base/FocusEventListener.h"
#include "axmol/base/KeyboardEventListener.h"
#include "axmol/base/ControllerEventListener.h"
#include "axmol/base/PointerEventListener.h"
#include "axmol/base/ControllerEvent.h"
#include "axmol/base/Controller.h"
#include "axmol/base/PointerEvent.h"
#include "axmol/base/EventType.h"

// math
#include "axmol/math/AffineTransform.h"
#include "axmol/math/Math.h"
#include "axmol/math/Vertex.h"
#include "axmol/math/Mat4.h"
#include "axmol/math/MathUtil.h"
#include "axmol/math/Quaternion.h"
#include "axmol/math/Vec2.h"
#include "axmol/math/Vec3.h"
#include "axmol/math/Vec4.h"

// actions
#include "axmol/2d/Action.h"
#include "axmol/2d/ActionCamera.h"
#include "axmol/2d/ActionCatmullRom.h"
#include "axmol/2d/ActionEase.h"
#include "axmol/2d/ActionGrid.h"
#include "axmol/2d/ActionGrid3D.h"
#include "axmol/2d/ActionInstant.h"
#include "axmol/2d/ActionInterval.h"
#include "axmol/2d/ActionManager.h"
#include "axmol/2d/ActionPageTurn3D.h"
#include "axmol/2d/ActionProgressTimer.h"
#include "axmol/2d/ActionTiledGrid.h"
#include "axmol/2d/ActionTween.h"
#include "axmol/2d/TweenFunction.h"
#include "axmol/2d/ActionCoroutine.h"

// 2d nodes
#include "axmol/2d/AtlasNode.h"
#include "axmol/2d/ClippingNode.h"
#include "axmol/2d/ClippingRectangleNode.h"
#include "axmol/2d/DrawNode.h"
#include "axmol/2d/FontFNT.h"
#include "axmol/2d/FontFreeType.h"
#include "axmol/2d/Label.h"
#include "axmol/2d/LabelAtlas.h"
#include "axmol/2d/Layer.h"
#include "axmol/2d/Menu.h"
#include "axmol/2d/MenuItem.h"
#include "axmol/2d/MotionStreak.h"
#include "axmol/scene/Node.h"
#include "axmol/2d/NodeGrid.h"
#include "axmol/2d/ParticleBatchNode.h"
#include "axmol/2d/ParticleExamples.h"
#include "axmol/2d/ParticleSystem.h"
#include "axmol/2d/ParticleSystemQuad.h"
#include "axmol/2d/ProgressTimer.h"
#include "axmol/2d/ProtectedNode.h"
#include "axmol/2d/RenderTexture.h"
#include "axmol/scene/Scene.h"
#include "axmol/2d/Transition.h"
#include "axmol/2d/TransitionPageTurn.h"
#include "axmol/2d/TransitionProgress.h"

// 2d utils
#include "axmol/scene/Camera.h"
#include "axmol/scene/CameraBackgroundBrush.h"
#include "axmol/2d/Grid.h"
#include "axmol/2d/Light.h"

// include
#include "axmol/base/Protocols.h"

// renderer
#include "axmol/renderer/CallbackCommand.h"
#include "axmol/renderer/CustomCommand.h"
#include "axmol/renderer/GroupCommand.h"
#include "axmol/renderer/Material.h"
#include "axmol/renderer/Pass.h"
#include "axmol/renderer/QuadCommand.h"
#include "axmol/renderer/RenderCommand.h"
#include "axmol/renderer/RenderCommandPool.h"
#include "axmol/renderer/RenderState.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/renderer/Technique.h"
#include "axmol/renderer/Texture2D.h"
#include "axmol/renderer/TextureCube.h"
#include "axmol/renderer/TextureCache.h"
#include "axmol/renderer/TrianglesCommand.h"
#include "axmol/renderer/Shaders.h"

// physics2d
#include "axmol/physics/physics-2d.h"

// physics3d
#include "axmol/physics/physics-3d.h"

// platform
#include "axmol/platform/Common.h"
#include "axmol/platform/Device.h"
#include "axmol/platform/FileUtils.h"
#include "axmol/platform/FileStream.h"
#include "axmol/platform/Image.h"
#include "axmol/platform/PlatformConfig.h"
#include "axmol/platform/PlatformMacros.h"
#include "axmol/platform/SAXParser.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
#    include "axmol/platform/ios/Application-ios.h"
#    include "axmol/platform/ios/StdC-ios.h"
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#    include "axmol/platform/android/Application-android.h"
#    include "axmol/platform/android/StdC-android.h"
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#    include "axmol/platform/win32/Application-win32.h"
#    include "axmol/platform/win32/StdC-win32.h"
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)
#    include "axmol/platform/winrt/Application-winrt.h"
#    include "axmol/platform/winrt/StdC-winrt.h"
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
#    include "axmol/platform/mac/Application-mac.h"
#    include "axmol/platform/mac/StdC-mac.h"
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#    include "axmol/platform/linux/Application-linux.h"
#    include "axmol/platform/linux/StdC-linux.h"
#elif (AX_TARGET_PLATFORM == AX_PLATFORM_WASM)
#    include "axmol/platform/wasm/Application-wasm.h"
#    include "axmol/platform/wasm/StdC-wasm.h"
#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_WASM

#include "axmol/platform/RenderView.h"

#if AX_ENABLE_GL
#    include "axmol/platform/GL.h"
#endif

// script_support
#include "axmol/base/ScriptSupport.h"

// sprite_nodes
#include "axmol/2d/Animation.h"
#include "axmol/2d/AnimationCache.h"
#include "axmol/2d/Sprite.h"
#include "axmol/2d/AnchoredSprite.h"
#include "axmol/2d/AutoPolygon.h"
#include "axmol/2d/SpriteBatchNode.h"
#include "axmol/2d/SpriteFrame.h"
#include "axmol/2d/SpriteFrameCache.h"

// textures
#include "axmol/renderer/TextureAtlas.h"

// tilemap_parallax_nodes
#include "axmol/2d/ParallaxNode.h"
#include "axmol/2d/TMXObjectGroup.h"
#include "axmol/2d/TMXXMLParser.h"
#include "axmol/2d/TileMapAtlas.h"
#include "axmol/2d/FastTMXLayer.h"
#include "axmol/2d/FastTMXTiledMap.h"

// component
#include "axmol/scene/Component.h"
#include "axmol/scene/ComponentContainer.h"

// 3d
#include "axmol/3d/AABB.h"
#include "axmol/3d/Animate3D.h"
#include "axmol/3d/Animation3D.h"
#include "axmol/3d/AttachNode.h"
#include "axmol/3d/BillBoard.h"
#include "axmol/3d/Frustum.h"
#include "axmol/3d/Mesh.h"
#include "axmol/3d/MeshSkin.h"
#include "axmol/3d/MotionStreak3D.h"
#include "axmol/3d/MeshVertexIndexData.h"
#include "axmol/3d/OBB.h"
#include "axmol/3d/Plane.h"
#include "axmol/3d/Ray.h"
#include "axmol/3d/Skeleton3D.h"
#include "axmol/3d/Skybox.h"
#include "axmol/3d/MeshRenderer.h"
#include "axmol/3d/MeshMaterial.h"
#include "axmol/3d/Terrain.h"
#include "axmol/3d/VertexInputBinding.h"

namespace ax
{

AX_DLL const char* axmolVersion();

}
