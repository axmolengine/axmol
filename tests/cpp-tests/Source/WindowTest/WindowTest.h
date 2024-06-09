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

#ifndef __WINDOWTEST_H__
#define __WINDOWTEST_H__

#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC || AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || \
     AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#    include "../BaseTest.h"

DEFINE_TEST_SUITE(WindowTests);

class WindowTest : public TestCase
{
public:
    virtual std::string title() const override;
};

class WindowTestWindowed1 : public WindowTest
{
public:
    CREATE_FUNC(WindowTestWindowed1);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class WindowTestWindowed2 : public WindowTest
{
public:
    CREATE_FUNC(WindowTestWindowed2);
    virtual void onEnter() override;
    virtual std::string subtitle() const override;
};

class WindowTestFullscreen1 : public WindowTest
{
public:
    CREATE_FUNC(WindowTestFullscreen1);
    bool init() override;
    virtual std::string subtitle() const override;
};

class WindowTestFullscreen2 : public WindowTest
{
public:
    CREATE_FUNC(WindowTestFullscreen2);
    bool init() override;
    virtual std::string subtitle() const override;
};
class WindowTestFullscreen3 : public WindowTest
{
public:
    CREATE_FUNC(WindowTestFullscreen3);
    bool init() override;
    virtual std::string subtitle() const override;
};

class WindowTestResizedAndPositioned : public WindowTest
{
public:
    CREATE_FUNC(WindowTestResizedAndPositioned);
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual std::string subtitle() const override;

    void onWindowPositioned(ax::EventCustom* e);
    void onWindowResized(ax::EventCustom* e);

private:
    ax::Label* label1;
    ax::Label* label2;
};

class WindowTestClose : public WindowTest
{
public:
    CREATE_FUNC(WindowTestClose);
    virtual void onEnter() override;
    virtual void onExit() override;
    virtual std::string subtitle() const override;

    void onWindowClose(ax::EventCustom* e);

private:
    ax::Label* label;
};

#endif /* __WINDOWTEST_H__ */

#endif
