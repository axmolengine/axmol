/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/axmol.h"

/**
@brief    The axmol Application.

Private inheritance here hides part of interface from Director.
*/
class AppDelegate : private ax::Application
{
public:
    AppDelegate();
    ~AppDelegate() override;

    void applicationWillLaunch() override;

    /**
    @brief    Implement Director and Scene init code here.
    @return true    Initialize success, app continue.
    @return false   Initialize failed, app terminate.
    */
    bool applicationDidFinishLaunching() override;

    /**
    @brief  Called when the application moves to the background
    */
    void applicationDidEnterBackground() override;

    /**
    @brief  Called when the application reenters the foreground
    */
    void applicationWillEnterForeground() override;

    /**
    @brief Called when application will quit
    @since axmol-2.10.0
    */
    void applicationWillQuit() override;
};
