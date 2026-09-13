/****************************************************************************
Copyright (c) 2011      Laschweinski
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

#include "axmol/platform/Common.h"
#include "axmol/platform/ApplicationCore.h"
#include <string>
#include <chrono>

namespace ax
{
class Rect;

class Application : public ApplicationCore
{
public:
    /**
     */
    Application();
    /**
     * @lua NA
     */
    virtual ~Application();

    /**
     @brief Callback by Director for limit FPS.
     @param interval    The time, which expressed in second in second, between current frame and next.
     */
    void setAnimationInterval(float interval) override;

    int run() override;

    /* override functions */
    LanguageType getCurrentLanguage() override;

    /**
    @brief Get current language iso 639-1 code
    @return Current language iso 639-1 code
    */
    const char* getCurrentLanguageCode() override;

    /**
    @brief Get application version
    */
    std::string getVersion() override;

    /**
     @brief Open url in default browser
     @param String with url to open.
     @return true if the resource located by the URL was successfully opened; otherwise false.
     */
    bool openURL(std::string_view url) override;

    /**
     @brief Get target platform
     */
    Platform getTargetPlatform() override;

protected:
    std::chrono::nanoseconds _animationInterval;  // nano seconds
    std::string _resourceRootPath;
};

}  // namespace ax
