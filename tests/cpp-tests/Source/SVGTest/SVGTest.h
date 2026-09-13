/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef _SVG_TEST_H_
#define _SVG_TEST_H_

#include "axmol/axmol.h"
#include "../BaseTest.h"
#include <string>

DEFINE_TEST_SUITE(SVGTests);

class SVG : public TestCase
{
public:
    CREATE_FUNC(SVG);
    SVG();
    void addNewSpriteWithCoords(ax::Vec2 p);
    virtual bool onPointerDown(ax::PointerEvent* event);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

class SVG1 : public TestCase
{
public:
    CREATE_FUNC(SVG1);
    SVG1();
    void addNewSpriteWithCoords(ax::Vec2 p);
    virtual bool onPointerDown(ax::PointerEvent* event);
    virtual std::string title() const override;
    virtual std::string subtitle() const override;
};

#endif
