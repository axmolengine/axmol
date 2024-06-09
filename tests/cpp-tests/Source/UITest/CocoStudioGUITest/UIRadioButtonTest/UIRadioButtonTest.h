/****************************************************************************
 Copyright (c) 2015 cocos2d-x.org
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

#ifndef __TestCpp__UIRadioButtonTest__
#define __TestCpp__UIRadioButtonTest__

#include "../UIScene.h"

DEFINE_TEST_SUITE(UIRadioButtonTests);

class UIRadioButtonTest : public UIScene
{
public:
    CREATE_FUNC(UIRadioButtonTest);

    UIRadioButtonTest();
    ~UIRadioButtonTest();

    virtual bool init() override;

    void addRadioButton(Object* sender);
    void deleteRadioButton(Object* sender);

private:
    ax::ui::RadioButtonGroup* _radioButtonGroup;
    ax::ui::Text* _allowNoSelectionText;
};

class UIRadioButtonTwoGroupsTest : public UIScene
{
public:
    CREATE_FUNC(UIRadioButtonTwoGroupsTest);

    UIRadioButtonTwoGroupsTest();
    ~UIRadioButtonTwoGroupsTest();

    virtual bool init() override;
    void onChangedRadioButtonGroup1(ax::ui::RadioButton* radioButton,
                                    int index,
                                    ax::ui::RadioButtonGroup::EventType type);
    void onChangedRadioButtonGroup2(ax::ui::RadioButton* radioButton,
                                    int index,
                                    ax::ui::RadioButtonGroup::EventType type);
    void onChangedRadioButtonSelect(ax::ui::RadioButton* radioButton, ax::ui::RadioButton::EventType type);
    void clearRadioButtonGroup(Object* sender);

protected:
    void addLog(std::string_view log);

    ax::ui::RadioButtonGroup* _radioButtonGroups[2];
    ax::ui::Text* _groupEventLabel;
    ax::ui::Text* _buttonEventLabel;
    ax::ui::Text* _logConsole;
    int _numberOfLogLines;
};

class UIRadioButtonTabTest : public UIScene
{
public:
    CREATE_FUNC(UIRadioButtonTabTest);
    virtual bool init() override;
};

#endif /* defined(__TestCpp__UIRadioButtonTest__) */
