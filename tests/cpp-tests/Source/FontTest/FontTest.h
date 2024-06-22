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

#ifndef _FONT_TEST_H_
#define _FONT_TEST_H_

#include "axmol.h"
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
