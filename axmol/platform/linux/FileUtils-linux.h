/****************************************************************************
Copyright (c) 2011      Laschweinski
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Simdsoft Limited.

https://axmol.dev/

SPDX-License-Identifier: MIT
****************************************************************************/
#pragma once

#include "axmol/platform/FileUtils.h"
#include "axmol/platform/PlatformMacros.h"
#include "axmol/base/Types.h"
#include <string>
#include <vector>

namespace ax
{

/**
 * @addtogroup platform
 * @{
 */

//! @brief  Helper class to handle file operations
class AX_DLL FileUtilsLinux : public FileUtils
{
    friend class FileUtils;

protected:
    FileUtilsLinux();

public:
    /* override functions */
    bool init() override;
    std::string getWritablePath() const override;
    std::string getNativeWritableAbsolutePath() const override;

private:
    bool isFileExistInternal(std::string_view strFilePath) const override;

    static std::string s_exeName;
};

// end of platform group
/// @}

}  // namespace ax
