/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

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

#ifndef __AXMOL_EXT_H__
#define __AXMOL_EXT_H__

#include "ExtensionMacros.h"

#include "GUI/src/GUI/ScrollView/ScrollView.h"
#include "GUI/src/GUI/ScrollView/TableView.h"

// Physics integration
#include "physics-nodes/src/physics-nodes/PhysicsDebugNode.h"
#include "physics-nodes/src/physics-nodes/PhysicsDebugNodeBox2D.h"
#include "physics-nodes/src/physics-nodes/PhysicsDebugNodeChipmunk2D.h"
#include "physics-nodes/src/physics-nodes/PhysicsSprite.h"
#include "physics-nodes/src/physics-nodes/PhysicsSpriteBox2D.h"
#include "physics-nodes/src/physics-nodes/PhysicsSpriteChipmunk2D.h"

#include "assets-manager/src/assets-manager/AssetsManager.h"
#include "assets-manager/src/assets-manager/AssetsManagerEx.h"
#include "assets-manager/src/assets-manager/EventAssetsManagerEx.h"
#include "assets-manager/src/assets-manager/EventListenerAssetsManagerEx.h"
#include "assets-manager/src/assets-manager/Manifest.h"

// Particle System, include Particle Universe Particle System
#include "Particle3D/src/Particle3D/Particle3DRender.h"
#include "Particle3D/src/Particle3D/ParticleSystem3D.h"
#include "Particle3D/src/Particle3D/PU/PUParticleSystem3D.h"

#endif /* __AXMOL_EXT_H__ */
