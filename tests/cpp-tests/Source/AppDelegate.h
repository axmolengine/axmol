/****************************************************************************
 Copyright (c) 2013      cocos2d-x.org
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef _APP_DELEGATE_H_
#define _APP_DELEGATE_H_

#include "axmol/platform/Application.h"
#include "axmol/base/Types.h"
#include <span>
#include <string>
#include <string_view>

namespace ax
{
class CommandLineArgs;
}

class TestController;
/**
@brief    The axmol Application.

Private inheritance here hides part of interface from Director.
*/
class AppDelegate : private ax::Application
{
public:
    AppDelegate();
    virtual ~AppDelegate();

    void applicationWillLaunch() override;

    /**
    @brief    Implement Director and ax::Scene* init code here.
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

    /**
    @brief Called when application screen size changed
    @since axmol-3.0
    */
    void applicationScreenSizeChanged(int newWidth, int newHeight) override;

private:
    TestController* _testController;
};

#endif  // _APP_DELEGATE_H_
