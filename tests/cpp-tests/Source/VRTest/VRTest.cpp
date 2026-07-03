/****************************************************************************
 Copyright (c) 2012 cocos2d-x.org
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

#include "VRTest.h"
#include "axmol/vr/VRPreviewSceneCompositor.h"
#ifdef AX_ENABLE_OPENXR
#    include "axmol/vr/VRSceneCompositor.h"
#endif

using namespace ax;

VRTests::VRTests()
{
    ADD_TEST_CASE(VRTest1);
};

//------------------------------------------------------------------
//
// VRTest1
//
//------------------------------------------------------------------

VRTest1::VRTest1()
{
    auto size = Director::getInstance()->getVisibleSize();

    auto image = Sprite::create("Images/background.png");
    image->setPosition(size / 2);
    addChild(image);

    // Create RadioButtonGroup
    _radioButtonGroup = ui::RadioButtonGroup::create();
    addChild(_radioButtonGroup);

    // Create RadioButtons for three modes
    auto* normalBtn  = ui::RadioButton::create("cocosui/radio_button_off.png", "cocosui/radio_button_on.png");
    auto* previewBtn = ui::RadioButton::create("cocosui/radio_button_off.png", "cocosui/radio_button_on.png");
    auto* hmdBtn     = ui::RadioButton::create("cocosui/radio_button_off.png", "cocosui/radio_button_on.png");

    // Set positions
    float startX  = size.width * 0.25f;
    float spacing = size.width * 0.25f;
    float buttonY = size.height * 0.2f;

    normalBtn->setPosition(Vec2(startX, buttonY));
    previewBtn->setPosition(Vec2(startX + spacing, buttonY));
    hmdBtn->setPosition(Vec2(startX + spacing * 2, buttonY));

    // Scale buttons for better visibility
    normalBtn->setScale(1.5f);
    previewBtn->setScale(1.5f);
    hmdBtn->setScale(1.5f);

    // Add buttons to group and scene
    _radioButtonGroup->addRadioButton(normalBtn);
    _radioButtonGroup->addRadioButton(previewBtn);
    _radioButtonGroup->addRadioButton(hmdBtn);

    addChild(normalBtn);
    addChild(previewBtn);
    addChild(hmdBtn);

    // Add labels under each button
    auto* normalLabel = Label::createWithTTF("Normal", "fonts/arial.ttf", 12);
    normalLabel->setPosition(Vec2(startX, buttonY - 25));
    addChild(normalLabel);

    auto* previewLabel = Label::createWithTTF("VR Preview", "fonts/arial.ttf", 12);
    previewLabel->setPosition(Vec2(startX + spacing, buttonY - 25));
    addChild(previewLabel);

    auto* hmdLabel = Label::createWithTTF("VR HMD", "fonts/arial.ttf", 12);
    hmdLabel->setPosition(Vec2(startX + spacing * 2, buttonY - 25));
    addChild(hmdLabel);

    // Disable VR HMD button if OpenXR is not enabled
#ifndef AX_ENABLE_OPENXR
    hmdBtn->setEnabled(false);
    hmdBtn->setBright(false);
#endif

    // Set event listener
    _radioButtonGroup->addEventListener(AX_CALLBACK_3(VRTest1::onRadioGroupChanged, this));

    // Default to Normal mode
    _radioButtonGroup->setSelectedButton(0);
}

std::string VRTest1::title() const
{
    return "Testing VR scene composition";
}

std::string VRTest1::subtitle() const
{
    return "Select VR mode using radio buttons";
}

void VRTest1::onRadioGroupChanged(ui::RadioButton* sender, int index, ui::RadioButtonGroup::EventType type)
{
    auto director   = Director::getInstance();
    auto renderView = director->getRenderView();
    switch (index)
    {
    case 0:  // Normal
        renderView->setSceneCompositor(nullptr);
        break;
    case 1:  // VR Preview
    {
        auto vrPreview = std::make_unique<experimental::VRPreviewSceneCompositor>();
        renderView->setSceneCompositor(std::move(vrPreview));
        break;
    }
    case 2:  // VR HMD
#ifdef AX_ENABLE_OPENXR
        renderView->setSceneCompositor(std::make_unique<experimental::VRSceneCompositor>());
#endif
        break;
    }
}
