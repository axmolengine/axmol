/****************************************************************************
 Copyright (c) 2016 Chukong Technologies Inc.
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

#include <string>

#include "axmol/base/Types.h"
#include "axmol/renderer/Texture2D.h"

namespace ax
{

class Scene;
class Renderer;
class RenderView;

namespace experimental
{
class AX_DLL IVRHeadTracker
{
public:
    virtual ~IVRHeadTracker() {}

    // pose
    virtual Vec3 getLocalPosition() = 0;
    // rotation
    virtual Mat4 getLocalRotation() = 0;
};

class AX_DLL IVRRenderer
{
public:
    virtual ~IVRRenderer() {}
    virtual void init(RenderView* rv)                                        = 0;
    virtual void cleanup()                                                   = 0;
    virtual void onRenderViewResized(RenderView* rv)                         = 0;
    virtual void setScissorRect(float x, float y, float width, float height) = 0;
    virtual const ScissorRect& getScissorRect() const                        = 0;
    virtual void render(Scene* scene, Renderer* renderer)                    = 0;
    virtual IVRHeadTracker* getHeadTracker()                                 = 0;
};
}  // namespace experimental

}  // namespace ax
