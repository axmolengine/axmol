/****************************************************************************
 Copyright (c) 2013-2016 Chukong Technologies Inc.
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

#ifndef __cocos2d_tests__UIEditBoxTest__
#define __cocos2d_tests__UIEditBoxTest__

#include "UIScene.h"

DEFINE_TEST_SUITE(UIEditBoxTests);

class UIEditBoxTest : public UIScene, public ax::ui::EditBoxDelegate
{
public:
    CREATE_FUNC(UIEditBoxTest);

    virtual bool init() override;

    virtual void editBoxEditingDidBegin(ax::ui::EditBox* editBox) override;
    virtual void editBoxEditingDidEndWithAction(ax::ui::EditBox* editBox,
                                                ax::ui::EditBoxDelegate::EditBoxEndAction action) override;
    virtual void editBoxTextChanged(ax::ui::EditBox* editBox, std::string_view text) override;
    virtual void editBoxReturn(ax::ui::EditBox* editBox) override;

protected:
    ax::Label* _TTFShowEditReturn;
    ax::ui::EditBox* _editName;
    ax::ui::EditBox* _editPassword;
    ax::ui::EditBox* _editEmail;
    ax::Node* _editEmailParent;
};

class UIEditBoxTestToggleVisibility : public UIScene, public ax::ui::EditBoxDelegate
{
public:
    CREATE_FUNC(UIEditBoxTestToggleVisibility);

    virtual bool init() override;

    virtual void editBoxEditingDidBegin(ax::ui::EditBox* editBox) override;
    virtual void editBoxTextChanged(ax::ui::EditBox* editBox, std::string_view text) override;
    virtual void editBoxReturn(ax::ui::EditBox* editBox) override;

protected:
    ax::Label* _TTFShowEditReturn;
    ax::ui::EditBox* _editName;
    ax::ui::EditBox* _editPassword;
    ax::ui::EditBox* _editEmail;
    ax::Node* _editEmailParent;
};

class UIEditBoxTestTextHorizontalAlignment : public UIScene
{
public:
    CREATE_FUNC(UIEditBoxTestTextHorizontalAlignment);

    virtual bool init() override;
};

class UIEditBoxTestPressedAndDisabled : public UIScene
{
public:
    CREATE_FUNC(UIEditBoxTestPressedAndDisabled);
    virtual bool init() override;
};

#endif /* defined(__cocos2d_tests__UIEditBoxTest__) */
