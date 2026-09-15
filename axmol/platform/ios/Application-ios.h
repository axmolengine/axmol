/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

#include "axmol/platform/Common.h"
#include "axmol/platform/ApplicationCore.h"

namespace ax
{

class AX_DLL Application : public ApplicationCore
{
public:
    /**
     */
    Application();
    /**
     * @lua NA
     */
    virtual ~Application();

    int run() override;

    /**
     @brief    Callback by Director for limit FPS.
     @param interval    The time, expressed in seconds, between current frame and next.
     */
    void setAnimationInterval(float interval) override;

    /**
    @brief Get current language config
    @return Current language config
    */
    LanguageType getCurrentLanguage() override;

    /**
     @brief Get current language iso 639-1 code
     @return Current language iso 639-1 code
     */
    const char* getCurrentLanguageCode() override;

    /**
     @brief Get target platform
     */
    Platform getTargetPlatform() override;

    /**
     @brief Get application version.
     */
    std::string getVersion() override;

    /**
     @brief Open url in default browser
     @param String with url to open.
     @return true if the resource located by the URL was successfully opened; otherwise false.
     */
    bool openURL(std::string_view url) override;

private:
    void postBoundaryTaskSignal();
};

}  // namespace ax
