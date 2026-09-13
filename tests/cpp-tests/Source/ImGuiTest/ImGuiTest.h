/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef _IMGUIEXT_TEST_H_
#define _IMGUIEXT_TEST_H_

#include "axmol/axmol.h"
#include "../BaseTest.h"

#if AX_ENABLE_EXT_IMGUI

DEFINE_TEST_SUITE(ImGuiTests);

class ImGuiTest : public TestCase
{
public:
    CREATE_FUNC(ImGuiTest);

    std::string title() const override { return "ImGui Test"; }

    void onEnter() override;
    void onDrawImGui();
    void onExit() override;
};

#endif

#endif  // _CURL_TEST_H_
