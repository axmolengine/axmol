/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmolengine.github.io/

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

#ifndef __AXMOL_H__
#define __AXMOL_H__

#include "axmolver.h"

//
// all axmol include files
//
#include "base/Config.h"

// base
#include "base/AsyncTaskPool.h"
#include "base/AutoreleasePool.h"
#include "base/Configuration.h"
#include "base/Logging.h"
#include "base/Data.h"
#include "base/Director.h"
#include "base/IMEDelegate.h"
#include "base/IMEDispatcher.h"
#include "base/Map.h"
#include "base/NS.h"
#include "base/Profiling.h"
#include "base/Properties.h"
#include "base/Object.h"
#include "base/RefPtr.h"
#include "base/Scheduler.h"
#include "base/UserDefault.h"
#include "base/Value.h"
#include "base/Vector.h"
#include "base/ZipUtils.h"
#include "base/base64.h"
#include "base/Config.h"
#include "base/Macros.h"
#include "base/Types.h"
#include "base/UTF8.h"
#include "base/Utils.h"

// EventDispatcher
#include "base/EventAcceleration.h"
#include "base/EventCustom.h"
#include "base/EventDispatcher.h"
#include "base/EventFocus.h"
#include "base/EventKeyboard.h"
#include "base/EventListenerAcceleration.h"
#include "base/EventListenerCustom.h"
#include "base/EventListenerFocus.h"
#include "base/EventListenerKeyboard.h"
#include "base/EventListenerMouse.h"
#include "base/EventListenerController.h"
#include "base/EventListenerTouch.h"
#include "base/EventMouse.h"
#include "base/EventController.h"
#include "base/Controller.h"
#include "base/EventTouch.h"
#include "base/EventType.h"

// math
#include "math/AffineTransform.h"
#include "math/Math.h"
#include "math/Vertex.h"
#include "math/Mat4.h"
#include "math/MathUtil.h"
#include "math/Quaternion.h"
#include "math/Vec2.h"
#include "math/Vec3.h"
#include "math/Vec4.h"

// actions
#include "2d/Action.h"
#include "2d/ActionCamera.h"
#include "2d/ActionCatmullRom.h"
#include "2d/ActionEase.h"
#include "2d/ActionGrid.h"
#include "2d/ActionGrid3D.h"
#include "2d/ActionInstant.h"
#include "2d/ActionInterval.h"
#include "2d/ActionManager.h"
#include "2d/ActionPageTurn3D.h"
#include "2d/ActionProgressTimer.h"
#include "2d/ActionTiledGrid.h"
#include "2d/ActionTween.h"
#include "2d/TweenFunction.h"
#include "2d/ActionCoroutine.h"

// 2d nodes
#include "2d/AtlasNode.h"
#include "2d/ClippingNode.h"
#include "2d/ClippingRectangleNode.h"
#include "2d/DrawNode.h"
#include "2d/FontFNT.h"
#include "2d/FontFreeType.h"
#include "2d/Label.h"
#include "2d/LabelAtlas.h"
#include "2d/Layer.h"
#include "2d/Menu.h"
#include "2d/MenuItem.h"
#include "2d/MotionStreak.h"
#include "2d/Node.h"
#include "2d/NodeGrid.h"
#include "2d/ParticleBatchNode.h"
#include "2d/ParticleExamples.h"
#include "2d/ParticleSystem.h"
#include "2d/ParticleSystemQuad.h"
#include "2d/ProgressTimer.h"
#include "2d/ProtectedNode.h"
#include "2d/RenderTexture.h"
#include "2d/Scene.h"
#include "2d/Transition.h"
#include "2d/TransitionPageTurn.h"
#include "2d/TransitionProgress.h"

// 2d utils
#include "2d/Camera.h"
#include "2d/CameraBackgroundBrush.h"
#include "2d/Grid.h"
#include "2d/Light.h"

// include
#include "base/Protocols.h"

// renderer
#include "renderer/CallbackCommand.h"
#include "renderer/CustomCommand.h"
#include "renderer/GroupCommand.h"
#include "renderer/Material.h"
#include "renderer/Pass.h"
#include "renderer/QuadCommand.h"
#include "renderer/RenderCommand.h"
#include "renderer/RenderCommandPool.h"
#include "renderer/RenderState.h"
#include "renderer/Renderer.h"
#include "renderer/Technique.h"
#include "renderer/Texture2D.h"
#include "renderer/TextureCube.h"
#include "renderer/TextureCache.h"
#include "renderer/TrianglesCommand.h"
#include "renderer/Shaders.h"

// physics
#include "physics/PhysicsBody.h"
#include "physics/PhysicsContact.h"
#include "physics/PhysicsJoint.h"
#include "physics/PhysicsShape.h"
#include "physics/PhysicsWorld.h"

// platform
#include "platform/Common.h"
#include "platform/Device.h"
#include "platform/FileUtils.h"
#include "platform/FileStream.h"
#include "platform/Image.h"
#include "platform/PlatformConfig.h"
#include "platform/PlatformMacros.h"
#include "platform/SAXParser.h"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS)
#    include "platform/ios/Application-ios.h"
#    include "platform/ios/GLViewImpl-ios.h"
#    include "platform/ios/StdC-ios.h"
#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_IOS

#if (AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID)
#    include "platform/android/Application-android.h"
#    include "platform/android/GLViewImpl-android.h"
#    include "platform/android/GL-android.h"
#    include "platform/android/StdC-android.h"
#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)
#    include "platform/win32/Application-win32.h"
#    include "platform/GLViewImpl.h"
#    include "platform/win32/GL-win32.h"
#    include "platform/win32/StdC-win32.h"
#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_WIN32

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WINRT)
#    include "platform/winrt/Application-winrt.h"
#    include "platform/winrt/GLViewImpl-winrt.h"
#    include "platform/winrt/GL-winrt.h"
#    include "platform/winrt/StdC-winrt.h"
#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_WINRT

#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC)
#    include "platform/GLViewImpl.h"
#    include "platform/mac/Application-mac.h"
#    include "platform/mac/StdC-mac.h"
#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_MAC

#if (AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#    include "platform/linux/Application-linux.h"
#    include "platform/GLViewImpl.h"
#    include "platform/linux/GL-linux.h"
#    include "platform/linux/StdC-linux.h"
#endif  // AX_TARGET_PLATFORM == AX_PLATFORM_LINUX

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WASM)
    #include "platform/wasm/Application-wasm.h"
    #include "platform/GLViewImpl.h"
    #include "platform/wasm/GL-wasm.h"
    #include "platform/wasm/StdC-wasm.h"
#endif // AX_TARGET_PLATFORM == AX_PLATFORM_WASM

// script_support
#include "base/ScriptSupport.h"

// sprite_nodes
#include "2d/Animation.h"
#include "2d/AnimationCache.h"
#include "2d/Sprite.h"
#include "2d/AnchoredSprite.h"
#include "2d/AutoPolygon.h"
#include "2d/SpriteBatchNode.h"
#include "2d/SpriteFrame.h"
#include "2d/SpriteFrameCache.h"

// text_input_node
#include "2d/TextFieldTTF.h"

// textures
#include "renderer/TextureAtlas.h"

// tilemap_parallax_nodes
#include "2d/ParallaxNode.h"
#include "2d/TMXObjectGroup.h"
#include "2d/TMXXMLParser.h"
#include "2d/TileMapAtlas.h"
#include "2d/FastTMXLayer.h"
#include "2d/FastTMXTiledMap.h"

// component
#include "2d/Component.h"
#include "2d/ComponentContainer.h"

// 3d
#include "3d/AABB.h"
#include "3d/Animate3D.h"
#include "3d/Animation3D.h"
#include "3d/AttachNode.h"
#include "3d/BillBoard.h"
#include "3d/Frustum.h"
#include "3d/Mesh.h"
#include "3d/MeshSkin.h"
#include "3d/MotionStreak3D.h"
#include "3d/MeshVertexIndexData.h"
#include "3d/OBB.h"
#include "3d/Plane.h"
#include "3d/Ray.h"
#include "3d/Skeleton3D.h"
#include "3d/Skybox.h"
#include "3d/MeshRenderer.h"
#include "3d/MeshMaterial.h"
#include "3d/Terrain.h"
#include "3d/VertexAttribBinding.h"

NS_AX_BEGIN

AX_DLL const char* axmolVersion();

NS_AX_END

#endif  // __AX_H__
