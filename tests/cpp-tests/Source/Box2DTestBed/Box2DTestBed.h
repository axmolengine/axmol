/****************************************************************************
 * Copyright (c) 2021 @aismann; Peter Eismann, Germany; dreifrankensoft

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

#ifndef _BOX2D_TESTBED_H_
#define _BOX2D_TESTBED_H_

#include "axmol.h"
#include "box2d/box2d.h"
#include "../BaseTest.h"
#include "Box2DTestDebugDrawNode.h"

class SampleEntry;
class Sample;

DEFINE_TEST_SUITE(Box2DTestBedTests);

class Box2DTestBed : public TestCase
{
public:
    static Box2DTestBed* create(int entryIndex);

    Box2DTestBed();
    virtual ~Box2DTestBed();

    void onEnter() override;
    void onExit() override;

    void onDrawImGui();

    void initPhysics();
    void update(float dt) override;

    bool initWithEntryIndex(int entryIndex);

    void onKeyPressed(ax::EventKeyboard::KeyCode code, ax::Event* event);
    void onKeyReleased(ax::EventKeyboard::KeyCode code, ax::Event* event);

    void onMouseDown(ax::Event* event);
    void onMouseUp(ax::Event* event);
    void onMouseMove(ax::Event* event);
    void onMouseScroll(ax::Event* event);

    void RestartSample();

    SampleEntry* m_entry{};
    Sample* m_sample{};
    int m_entryIndex{};

private:
    b2Vec2 _mouseDownPos{};
    ax::Vec2 _dragingStartPos;
    bool _draging{false};

    /*ax::EventListenerTouchOneByOne* _touchListener{};*/
    ax::EventListenerKeyboard* _keyboardListener{};
    ax::EventListenerMouse* _mouseListener{};

    // Debug stuff
    Box2DTestDebugDrawNode* _debugDraw{};
};

#endif
