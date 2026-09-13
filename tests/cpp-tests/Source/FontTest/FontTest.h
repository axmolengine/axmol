/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef _FONT_TEST_H_
#define _FONT_TEST_H_

#include "axmol/axmol.h"
#include "../BaseTest.h"

DEFINE_TEST_SUITE(FontTests);

class FontTest : public TestCase
{
public:
    static FontTest* create(std::string_view fontFile)
    {
        auto ret = new FontTest;
        if (ret->init())
        {
            ret->showFont(fontFile);
            ret->autorelease();
        }
        else
        {
            delete ret;
            ret = nullptr;
        }

        return ret;
    }

    void showFont(std::string_view fontFile);

    virtual std::string title() const override;
};

class FontNoReplacementTest : public TestCase
{
public:
    static FontNoReplacementTest* create();
    ~FontNoReplacementTest();
    virtual void onEnter() override;
    virtual std::string title() const override;

protected:
    FontNoReplacementTest();
};

#endif  // _FONT_TEST_H_
