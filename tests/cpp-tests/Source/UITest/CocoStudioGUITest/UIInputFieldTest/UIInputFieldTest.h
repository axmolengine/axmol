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

#ifndef __TestCpp__UIInputFieldTest__
#define __TestCpp__UIInputFieldTest__

#include "../UIScene.h"

DEFINE_TEST_SUITE(UIInputFieldTests);

class UIInputFieldTest : public UIScene
{
public:
    CREATE_FUNC(UIInputFieldTest);

    UIInputFieldTest();
    ~UIInputFieldTest();
    virtual bool init() override;

protected:
    ax::ui::Text* _displayValueLabel;
};

class UIInputFieldTest_MaxLength : public UIScene
{
public:
    CREATE_FUNC(UIInputFieldTest_MaxLength);

    UIInputFieldTest_MaxLength();
    ~UIInputFieldTest_MaxLength();
    virtual bool init() override;

protected:
    ax::ui::Text* _displayValueLabel;
};

class UIInputFieldTest_Password : public UIScene
{
public:
    CREATE_FUNC(UIInputFieldTest_Password);

    UIInputFieldTest_Password();
    ~UIInputFieldTest_Password();
    virtual bool init() override;

protected:
    ax::ui::Text* _displayValueLabel;
};

class UIInputFieldTest_LineWrap : public UIScene
{
public:
    CREATE_FUNC(UIInputFieldTest_LineWrap);

    UIInputFieldTest_LineWrap();
    ~UIInputFieldTest_LineWrap();
    virtual bool init() override;

protected:
    ax::ui::Text* _displayValueLabel;
};

class UIInputFieldTest_TrueTypeFont : public UIScene
{
public:
    CREATE_FUNC(UIInputFieldTest_TrueTypeFont);

    UIInputFieldTest_TrueTypeFont();
    ~UIInputFieldTest_TrueTypeFont();
    virtual bool init() override;

protected:
    ax::ui::Text* _displayValueLabel;
};

class UIInputFieldTest_BMFont : public UIScene
{
public:
    CREATE_FUNC(UIInputFieldTest_BMFont);

    UIInputFieldTest_BMFont();
    ~UIInputFieldTest_BMFont();
    virtual bool init() override;

protected:
    ax::ui::Text* _displayValueLabel;
};

class UIInputFieldTest_PlaceHolderColor : public UIScene
{
public:
    CREATE_FUNC(UIInputFieldTest_PlaceHolderColor);

    UIInputFieldTest_PlaceHolderColor();
    ~UIInputFieldTest_PlaceHolderColor();
    virtual bool init() override;

protected:
    ax::ui::Text* _displayValueLabel;
};
#endif /* defined(__TestCpp__UIInputFieldTest__) */
