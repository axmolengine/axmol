/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
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

#ifndef __PHYSICS_3D_VIEWER_H__
#define __PHYSICS_3D_VIEWER_H__

#include "math/Math.h"
#include "base/Object.h"
#include "base/Types.h"
#include "base/Config.h"
#include "renderer/CustomCommand.h"
#include "renderer/backend/ProgramState.h"
#include "renderer/backend/Types.h"

#if defined(AX_ENABLE_3D_PHYSICS)

#    if (AX_ENABLE_BULLET_INTEGRATION)
#        include "bullet/LinearMath/btIDebugDraw.h"

NS_AX_BEGIN

/**
 * @addtogroup _3d
 * @{
 */

class Renderer;

/** @brief Physics3DDebugDrawer: debug draw the physics object, used by Physics3DWorld */
class Physics3DDebugDrawer : public btIDebugDraw
{
public:
    Physics3DDebugDrawer();
    virtual ~Physics3DDebugDrawer();

    void draw(ax::Renderer* renderer);

    // override function
    virtual void drawLine(const btVector3& from, const btVector3& to, const btVector3& color) override;
    virtual void drawContactPoint(const btVector3& PointOnB,
                                  const btVector3& normalOnB,
                                  btScalar distance,
                                  int lifeTime,
                                  const btVector3& color) override;
    virtual void reportErrorWarning(const char* warningString) override;
    virtual void draw3dText(const btVector3& location, const char* textString) override;
    virtual void setDebugMode(int debugMode) override;
    virtual int getDebugMode() const override;

    void clear();

protected:
    void init();
    void onBeforeDraw();
    void onAfterDraw();

protected:
    std::vector<V3F_C4F> _buffer;
    ax::backend::UniformLocation _locMVP;

    ax::BlendFunc _blendFunc = BlendFunc::DISABLE;
    ax::CustomCommand _customCommand;
    ax::backend::ProgramState* _programState = nullptr;

    bool _dirty    = true;
    int _debugMode = DBG_DrawWireframe | DBG_DrawConstraints | DBG_DrawConstraintLimits;

private:
    bool _oldDepthTestEnabled = false;
};

// end of 3d group
/// @}

NS_AX_END

#    endif  // AX_ENABLE_BULLET_INTEGRATION

#endif  // defined(AX_ENABLE_3D_PHYSICS)

#endif  // __PHYSICS_3D_VIEWER_H__
