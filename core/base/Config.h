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

#include "platform/PlatformConfig.h"

/**
 * @file
 * cocos2d (cc) configuration file.
 */

/** @def AX_ENABLE_STACKABLE_ACTIONS
 * If enabled, actions that alter the position property (eg: MoveBy, JumpBy, BezierBy, etc..) will be stacked.
 * If you run 2 or more 'position' actions at the same time on a node, then end position will be the sum of all the
 * positions. If disabled, only the last run action will take effect. Enabled by default. Disable to be compatible with
 * v2.0 and older versions.
 * @since v2.1
 */
#ifndef AX_ENABLE_STACKABLE_ACTIONS
#    define AX_ENABLE_STACKABLE_ACTIONS 1
#endif

/** @def AX_ENABLE_GL_STATE_CACHE
 * If enabled, cocos2d will maintain an OpenGL state cache internally to avoid unnecessary switches.
 * In order to use them, you have to use the following functions, instead of the GL ones:
 *  - ccGLUseProgram() instead of glUseProgram().
 *  - GL::deleteProgram() instead of glDeleteProgram().
 *  - GL::blendFunc() instead of glBlendFunc().

 * If this functionality is disabled, then ccGLUseProgram(), GL::deleteProgram(), GL::blendFunc() will call the GL ones,
 without using the cache.

 * It is recommended to enable whenever possible to improve speed.
 * If you are migrating your code from GL ES 1.1, then keep it disabled. Once all your code works as expected, turn it
 on.

 * Enabled by default.

 * @since v2.0.0
 */
#ifndef AX_ENABLE_GL_STATE_CACHE
#    define AX_ENABLE_GL_STATE_CACHE 1
#endif

/** @def AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL
 * If enabled, the texture coordinates will be calculated by using this formula:
 * - texCoord.left = (rect.origin.x*2+1) / (texture.wide*2);
 * - texCoord.right = texCoord.left + (rect.size.width*2-2)/(texture.wide*2);

 * The same for bottom and top.

 * This formula prevents artifacts by using 99% of the texture.
 * The "correct" way to prevent artifacts is by using the spritesheet-artifact-fixer.py or a similar tool.

 * Affected nodes:
 * - Sprite / SpriteBatchNode and subclasses: LabelBMFont, TMXTiledMap.
 * - LabelAtlas.
 * - QuadParticleSystem.
 * - TileMap.

 * To enabled set it to 1. Disabled by default.

 * @since v0.99.5
 */
#ifndef AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL
#    define AX_FIX_ARTIFACTS_BY_STRECHING_TEXEL 0
#endif

/** @def AX_DIRECTOR_STATS_INTERVAL
 * Seconds between FPS updates.
 * 0.5 seconds, means that the FPS number will be updated every 0.5 seconds.
 * Having a bigger number means a more reliable FPS.

 * Default value: 0.5f
 */
#ifndef AX_DIRECTOR_STATS_INTERVAL
#    define AX_DIRECTOR_STATS_INTERVAL (0.5f)
#endif

/** @def AX_DIRECTOR_FPS_POSITION
 * Position of the FPS.

 * Default: 0,0 (bottom-left corner).
 */
#ifndef AX_DIRECTOR_FPS_POSITION
#    define AX_DIRECTOR_FPS_POSITION Vec2(0, 0)
#endif

/** @def AX_DIRECTOR_DISPATCH_FAST_EVENTS
 * If enabled, and only when it is used with FastDirector, the main loop will wait 0.04 seconds to
 * dispatch all the events, even if there are not events to dispatch.
 * If your game uses lot's of events (eg: touches) it might be a good idea to enable this feature.
 * Otherwise, it is safe to leave it disabled.

 * To enable set it to 1. Disabled by default.

 * @warning This feature is experimental.
 */
#ifndef AX_DIRECTOR_DISPATCH_FAST_EVENTS
#    define AX_DIRECTOR_DISPATCH_FAST_EVENTS 0
#endif

/** @def AX_DIRECTOR_MAC_USE_DISPLAY_LINK_THREAD
 * If enabled, cocos2d-mac will run on the Display Link thread. If disabled cocos2d-mac will run in its own thread.
 * If enabled, the images will be drawn at the "correct" time, but the events might not be very responsive.
 * If disabled, some frames might be skipped, but the events will be dispatched as they arrived.
 * To enable set it to a 1, to disable it set to 0. Enabled by default.
 * Only valid for cocos2d-mac. Not supported on cocos2d-ios.
 */
#ifndef AX_DIRECTOR_MAC_USE_DISPLAY_LINK_THREAD
#    define AX_DIRECTOR_MAC_USE_DISPLAY_LINK_THREAD 1
#endif

/** @def AX_NODE_RENDER_SUBPIXEL
 * If enabled, the Node objects (Sprite, Label,etc) will be able to render in subpixels.
 * If disabled, integer pixels will be used.
 * To enable set it to 1. Enabled by default.
 */
#ifndef AX_NODE_RENDER_SUBPIXEL
#    define AX_NODE_RENDER_SUBPIXEL 1
#endif

/** @def AX_SPRITEBATCHNODE_RENDER_SUBPIXEL
 * If enabled, the Sprite objects rendered with SpriteBatchNode will be able to render in subpixels.
 * If disabled, integer pixels will be used.
 * To enable set it to 1. Enabled by default.
 */
#ifndef AX_SPRITEBATCHNODE_RENDER_SUBPIXEL
#    define AX_SPRITEBATCHNODE_RENDER_SUBPIXEL 1
#endif

/** @def AX_TEXTURE_ATLAS_USE_VAO
 * By default, TextureAtlas (used by many cocos2d classes) will use VAO (Vertex Array Objects).
 * Apple recommends its usage but they might consume a lot of memory, specially if you use many of them.
 * So for certain cases, where you might need hundreds of VAO objects, it might be a good idea to disable it.
 * To disable it set it to 0. Enabled by default.
 * If a device doesn't support VAO though it claims to support should add exceptions list here.
 */
#ifndef AX_TEXTURE_ATLAS_USE_VAO
#    define AX_TEXTURE_ATLAS_USE_VAO 1
#endif

/** @def AX_USE_LA88_LABELS
 * If enabled, it will use LA88 (Luminance Alpha 16-bit textures) for LabelTTF objects.
 * If it is disabled, it will use A8 (Alpha 8-bit textures).
 * LA88 textures are 6% faster than A8 textures, but they will consume 2x memory.
 * This feature is enabled by default.
 * @since v0.99.5
 */
#ifndef AX_USE_LA88_LABELS
#    define AX_USE_LA88_LABELS 1
#endif

/** @def AX_SPRITE_DEBUG_DRAW
 * If enabled, all subclasses of Sprite will draw a bounding box.
 * Useful for debugging purposes only. It is recommended to leave it disabled.
 * To enable set it to a value different than 0. Disabled by default:
 * 0 -- disabled
 * 1 -- draw bounding box
 * 2 -- draw texture box
 */
#ifndef AX_SPRITE_DEBUG_DRAW
#    define AX_SPRITE_DEBUG_DRAW 0
#endif

/** @def AX_LABEL_DEBUG_DRAW
 * If enabled, all subclasses of Label will draw a bounding box.
 * Useful for debugging purposes only. It is recommended to leave it disabled.
 * To enable set it to a value different than 0. Disabled by default:
 * 0 -- disabled
 * 1 -- draw bounding box
 */
#ifndef AX_LABEL_DEBUG_DRAW
#    define AX_LABEL_DEBUG_DRAW 0
#endif

/** @def AX_SPRITEBATCHNODE_DEBUG_DRAW
 * If enabled, all subclasses of Sprite that are rendered using an SpriteBatchNode draw a bounding box.
 * Useful for debugging purposes only. It is recommended to leave it disabled.
 * To enable set it to a value different than 0. Disabled by default.
 */
#ifndef AX_SPRITEBATCHNODE_DEBUG_DRAW
#    define AX_SPRITEBATCHNODE_DEBUG_DRAW 0
#endif

/** @def AX_LABELBMFONT_DEBUG_DRAW
 * If enabled, all subclasses of LabelBMFont will draw a bounding box.
 * Useful for debugging purposes only. It is recommended to leave it disabled.
 * To enable set it to a value different than 0. Disabled by default.
 */
#ifndef AX_LABELBMFONT_DEBUG_DRAW
#    define AX_LABELBMFONT_DEBUG_DRAW 0
#endif

/** @def AX_LABELATLAS_DEBUG_DRAW
 * If enabled, all subclasses of LabeltAtlas will draw a bounding box
 * Useful for debugging purposes only. It is recommended to leave it disabled.
 * To enable set it to a value different than 0. Disabled by default.
 */
#ifndef AX_LABELATLAS_DEBUG_DRAW
#    define AX_LABELATLAS_DEBUG_DRAW 0
#endif

/** @def AX_NODE_DEBUG_VERIFY_EVENT_LISTENERS
 * If enabled (in conjunction with assertion macros) will verify on Node destruction that the node being destroyed has
 * no event listeners still associated with it in the event dispatcher. This can be used to track down problems where
 * the event dispatch system has dangling pointers to destroyed nodes. Note: event listener verification will always be
 * disabled in builds where assertions are disabled regardless of this setting.
 */
#ifndef AX_NODE_DEBUG_VERIFY_EVENT_LISTENERS
#    define AX_NODE_DEBUG_VERIFY_EVENT_LISTENERS 0
#endif

/** @def AX_ENABLE_PROFILERS
 * If enabled, will activate various profilers within cocos2d. This statistical data will be output to the console
 * once per second showing average time (in milliseconds) required to execute the specific routine(s).
 * Useful for debugging purposes only. It is recommended to leave it disabled.
 * To enable set it to a value different than 0. Disabled by default.
 */
#ifndef AX_ENABLE_PROFILERS
#    define AX_ENABLE_PROFILERS 0
#endif

/** Enable Lua engine debug log. */
#ifndef AX_LUA_ENGINE_DEBUG
#    define AX_LUA_ENGINE_DEBUG 0
#endif

/** Use physics integration API. */
//  It works with:
// Chipmunk2D or Box2D
#if defined(AX_ENABLE_PHYSICS)
/** Use Chipmunk2D physics 2d engine on physics integration API. */
#    ifndef AX_ENABLE_CHIPMUNK_INTEGRATION
#        define AX_ENABLE_CHIPMUNK_INTEGRATION 0
#    endif
/** or use Box2D physics 2d engine on physics integration API. */
#    ifndef AX_ENABLE_BOX2D_INTEGRATION
#        define AX_ENABLE_BOX2D_INTEGRATION 1
#    endif
#endif  // defined(AX_ENABLE_PHYSICS)

#if defined(AX_ENABLE_3D_PHYSICS)
/** Use bullet physics engine. */
#    ifndef AX_ENABLE_BULLET_INTEGRATION
#        define AX_ENABLE_BULLET_INTEGRATION 1
#    endif
#endif

/** Use culling or not. */
#ifndef AX_USE_CULLING
#    define AX_USE_CULLING 1
#endif

/** Support PNG or not. If your application don't use png format picture, you can undefine this macro to save package
 * size.
 */
#ifndef AX_USE_PNG
#    define AX_USE_PNG 1
#endif  // AX_USE_PNG

/** Support JPEG or not. If your application don't use jpeg format picture, you can undefine this macro to save package
 * size.
 */
#ifndef AX_USE_JPEG
#    define AX_USE_JPEG 1
#endif  // AX_USE_JPEG

/** Support webp or not. If your application don't use webp format picture, you can undefine this macro to save package
 * size.
 */
#ifndef AX_USE_WEBP
#    define AX_USE_WEBP 1
#endif  // AX_USE_WEBP

/** Enable Lua Script binding */
#ifndef AX_ENABLE_SCRIPT_BINDING
#    define AX_ENABLE_SCRIPT_BINDING 1
#endif

/** When AX_ENABLE_SCRIPT_BINDING and AX_ENABLE_GC_FOR_NATIVE_OBJECTS are both 1
 * then the Garbage collector will release the native objects, only when the JS/Lua objects
 * are collected.
 * The benefit is that users don't need to retain/release the JS/Lua objects manually.
 * Disabled by default.
 */
#ifdef AX_ENABLE_SCRIPT_BINDING
#    ifndef AX_ENABLE_GC_FOR_NATIVE_OBJECTS
#        define AX_ENABLE_GC_FOR_NATIVE_OBJECTS 0
#    endif
#endif

#ifndef AX_FILEUTILS_APPLE_ENABLE_OBJC
#    define AX_FILEUTILS_APPLE_ENABLE_OBJC 1
#endif

/** @def AX_ENABLE_PREMULTIPLIED_ALPHA
 * If enabled, all textures will be preprocessed to multiply its rgb components
 * by its alpha component.
 */
#ifndef AX_ENABLE_PREMULTIPLIED_ALPHA
#    define AX_ENABLE_PREMULTIPLIED_ALPHA 1
#endif

/** @def AX_STRIP_FPS
 * Whether to strip FPS related data and functions, such as ax_fps_images_png
 */
#ifndef AX_STRIP_FPS
#    define AX_STRIP_FPS 0
#endif

#ifndef AX_REDUCE_PAUSED_CPU_USAGE
#    define AX_REDUCE_PAUSED_CPU_USAGE 0
#endif

#ifndef AX_META_TEXTURES
#    define AX_META_TEXTURES 2
#endif

/// @name namespace ax
/// @{
#ifdef __cplusplus
#    define USING_NS_AX using namespace ax
#else
#    define USING_NS_AX
#endif

namespace ax
{}
namespace axmol = ax;
//  end of namespace group
/// @}
