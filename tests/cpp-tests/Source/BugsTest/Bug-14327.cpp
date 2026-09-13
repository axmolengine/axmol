/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#include "Bug-14327.h"

#include "axmol/tlx/format.hpp"

#if (AX_TARGET_PLATFORM == AX_PLATFORM_WIN32)

using namespace ax;

bool Bug14327Layer::init()
{
    if (BugsTestBase::init())
    {
        auto renderView    = Director::getInstance()->getRenderView();
        auto visibleOrigin = renderView->getVisibleOrigin();
        auto visibleSize   = renderView->getVisibleSize();

        auto pBg = Sprite::create("Images/HelloWorld.png");
        pBg->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 2));
        addChild(pBg);

        _removeTime = time(nullptr) + 20;

        _TTFShowTime = Label::createWithSystemFont("Edit control will be removed after 00:20!", "Arial", 20);
        _TTFShowTime->setPosition(
            Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height - 60));
        this->addChild(_TTFShowTime);

        auto editBoxSize = Size(visibleSize.width - 100, visibleSize.height * 0.1);

        std::string pNormalSprite = "extensions/green_edit.png";
        _edit = ui::EditBox::create(editBoxSize + Size(0, 20), ui::Scale9Sprite::create(pNormalSprite));
        _edit->setPosition(Vec2(visibleOrigin.x + visibleSize.width / 2, visibleOrigin.y + visibleSize.height / 2));
        _edit->setFontColor(Color32::red);
        _edit->setReturnType(ui::EditBox::KeyboardReturnType::DONE);
        _edit->setDelegate(this);
        this->addChild(_edit);

        this->scheduleUpdate();
        return true;
    }

    return false;
}

void Bug14327Layer::update(float dt)
{
    int32_t delta = _removeTime - time(nullptr);
    if (delta > 0)
    {
        ldiv_t ret = ldiv(delta, 60L);
        char buf[100];
        auto infoStr =
            fmt::format_to_z(buf, "{}{:02d}:{:02d}", "Edit control will be removed after ", ret.quot, ret.rem);
        _TTFShowTime->setString(infoStr);
    }
    else
    {
        _edit->removeFromParent();
        _edit = nullptr;
        _TTFShowTime->setString("Edit control has been removed!\nIt should not crash.");
        this->unscheduleUpdate();
    }
}

void Bug14327Layer::editBoxEditingDidBegin(ax::ui::EditBox* editBox)
{
    AXLOGD("editBox {} DidBegin !", fmt::ptr(editBox));
}

void Bug14327Layer::editBoxEditingDidEndWithAction(ax::ui::EditBox* editBox,
                                                   ax::ui::EditBoxDelegate::EditBoxEndAction EditBoxEndAction)
{
    AXLOGD("editBox {} DidEnd !", fmt::ptr(editBox));
}

void Bug14327Layer::editBoxTextChanged(ax::ui::EditBox* editBox, std::string_view text)
{
    AXLOGD("editBox {} TextChanged, text: {} ", fmt::ptr(editBox), text);
}

void Bug14327Layer::editBoxReturn(ui::EditBox* editBox)
{
    AXLOGD("editBox {} was returned !", fmt::ptr(editBox));
}

#endif
