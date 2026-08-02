/****************************************************************************
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

#ifndef _EFFEKSEERTEST_H_
#define _EFFEKSEERTEST_H_

#include "axmol/axmol.h"
#include "../BaseTest.h"
#include "EffekseerAxmol.h"

struct TestEffect
{
    const char* filename;
    const char* displayName;
    float magnification;
    bool hasTargetPosition = false;
    ax::Vec3 targetPosition;
};

class EffekseerTests : public TestSuite
{
public:
    EffekseerTests();

    virtual ~EffekseerTests();

private:
    std::vector<std::string> _searchPaths;
};

class EffekseerTest : public TestCase
{
public:
    CREATE_FUNC(EffekseerTest);

    EffekseerTest();
    virtual ~EffekseerTest();

    virtual bool init() override;
    virtual std::string title() const override;

    void update(float delta) override;
    void visit(const ax::SceneRenderState& state, const ax::Mat4& parentTransform, uint32_t parentFlags) override;

private:
    void switchEffect(int direction);
    void playCurrentEffect();
    void updateLabel();

    bool onPointerDown(ax::PointerEvent* ev);
    void onPointerMove(ax::PointerEvent* ev);
    void onPointerUp(ax::PointerEvent* ev);

    efk::EffectManager* _manager = nullptr;
    efk::EffectEmitter* _emitter = nullptr;

    ax::Label* _label     = nullptr;
    ax::Camera* _3dCamera = nullptr;

    float _angle      = 0.0f;
    int _currentIndex = 0;
};

#endif  // _EFFEKSEERTEST_H_
