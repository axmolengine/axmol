/****************************************************************************
 Copyright (c) 2013 cocos2d-x.org
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

#ifndef _BILLBOARD_TEST_H_
#define _BILLBOARD_TEST_H_

#include "../BaseTest.h"
#include <string>

NS_AX_BEGIN
class BillBoard;
class Camera;
NS_AX_END  // namespace ax

class BillBoardRotationTest : public TestCase
{
public:
    CREATE_FUNC(BillBoardRotationTest);
    BillBoardRotationTest();
    virtual ~BillBoardRotationTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;

protected:
};

class BillBoardTest : public TestCase
{
public:
    virtual ~BillBoardTest();
    CREATE_FUNC(BillBoardTest);
    BillBoardTest();
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
    virtual void update(float dt) override;
    void addNewBillBoardWithCoords(ax::Vec3 p);
    void addNewAniBillBoardWithCoords(ax::Vec3 p);
    void rotateCameraCallback(ax::Object* sender, float value);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);

    void menuCallback_orientedPoint(ax::Object* sender);
    void menuCallback_orientedPlane(ax::Object* sender);

protected:
    ax::Camera* _camera;
    ax::Layer* _layerBillBoard;

    std::vector<ax::BillBoard*> _billboards;
};

DEFINE_TEST_SUITE(BillBoardTests);

#endif
