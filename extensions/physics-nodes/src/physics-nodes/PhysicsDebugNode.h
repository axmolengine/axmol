/*
 * Copyright (c) 2019 Erin Catto
 * Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 */

#ifndef __PHYSICSNODES_DEBUGNODE_BOX2D_H__
#define __PHYSICSNODES_DEBUGNODE_BOX2D_H__

#include "extensions/ExtensionMacros.h"
#include "extensions/ExtensionExport.h"
#include "2d/DrawNode.h"
#include "box2d/box2d.h"
#include "axmol.h"

NS_AX_EXT_BEGIN

// This class implements debug drawing callbacks that are invoked inside b2World::Step.
class AX_EX_DLL PhysicsDebugNode : public DrawNode
{
public:
    PhysicsDebugNode();
    virtual bool initWithWorld(b2WorldId worldId);

    void setAutoDraw(bool bval) { _autoDraw = bval; }
    bool isAutoDraw() const { return _autoDraw; }

    // control border thinkness
    void setThinkness(float fval) { _thinkness = fval; }
    float getThinkness() const { return _thinkness; }

    void setWorldOffset(const Vec2& offset) { _worldOffset = offset; }
    const Vec2& getWorldOffset() const { return _worldOffset; }

    void setPTMRatio(float ratio) { _ratio = ratio; }
    float getPTMRatio() const { return _ratio; }

    // Overrides
    virtual void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;

    b2DebugDraw& getB2DebugDraw() { return _debugDraw; }

protected:
    b2WorldId _world{};
    b2DebugDraw _debugDraw{b2DefaultDebugDraw()};
    bool _autoDraw{true};
    float _thinkness{0.5f};

    ax::Vec2 _worldOffset{};
    float _ratio{1.0f};
};

NS_AX_EXT_END

#endif  //__PHYSICSNODES_DEBUGNODE_BOX2D_H__
