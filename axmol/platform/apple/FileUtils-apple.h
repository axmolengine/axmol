/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
 Copyright (c) 2011      Zynga Inc.
 Copyright (c) 2013-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/
#pragma once

#include <memory>
#include <string>
#include <vector>

#include "axmol/platform/FileUtils.h"
#include "axmol/platform/PlatformMacros.h"
#include "axmol/base/Types.h"

namespace ax
{

/**
 * @addtogroup platform
 * @{
 */

//! @brief  Helper class to handle file operations
class AX_DLL FileUtilsApple : public FileUtils
{
public:
    FileUtilsApple();
    virtual ~FileUtilsApple();

    bool init() override;

    std::string getWritablePath() const override;
    std::string getNativeWritableAbsolutePath() const override;
    virtual std::string getFullPathForFilenameWithinDirectory(std::string_view directory,
                                                              std::string_view filename) const override;

#if AX_FILEUTILS_APPLE_ENABLE_OBJC
    void setBundle(NSBundle* bundle);
#endif

    bool createDirectories(std::string_view path) const override;
    virtual std::string getPathForDirectory(std::string_view dir, std::string_view searchPath) const override;

private:
    bool isFileExistInternal(std::string_view filePath) const override;

    struct IMPL;
    std::unique_ptr<IMPL> pimpl_;
};

// end of platform group
/// @}

}  // namespace ax
