/****************************************************************************
 Copyright (c) 2010 cocos2d-x.org
 Copyright (c) Microsoft Open Technologies, Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 
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
#ifndef __AX_FILEUTILS_WINRT_H__
#define __AX_FILEUTILS_WINRT_H__

#include <string>
#include <vector>

#include "platform/FileUtils.h"
#include "platform/PlatformMacros.h"
#include "base/Types.h"

NS_AX_BEGIN

/**
 * @addtogroup platform
 * @{
 */

//! @brief  Helper class to handle file operations
class AX_DLL FileUtilsWinRT : public FileUtils
{
    friend class FileUtils;

protected:
    FileUtilsWinRT();

public:
    /* override functions */
    bool init();
    virtual std::string getWritablePath() const;
    virtual bool isAbsolutePath(std::string_view strPath) const override;
    virtual std::string getPathForFilename(std::string_view filename,
                                           std::string_view resolutionDirectory) const override;
    virtual std::string getFullPathForFilenameWithinDirectory(std::string_view strDirectory,
                                                              std::string_view strFilename) const override;
    //virtual std::string getSuitableFOpen(const std::string_view& filenameUtf8) const override;
    virtual int64_t getFileSize(std::string_view filepath) const override;
    static std::string getAppPath();

    std::string getNativeWritableAbsolutePath() const override;

private:
    virtual bool isFileExistInternal(std::string_view strFilePath) const override;

    /**
     *  Renames a file under the given directory.
     *
     *  @param path     The parent directory path of the file, it must be an absolute path.
     *  @param oldname  The current name of the file.
     *  @param name     The new name of the file.
     *  @return True if the file have been renamed successfully, false if not.
     */
    virtual bool renameFile(std::string_view path,
                            std::string_view oldname, std::string_view name) const override;

    /**
     *  Renames a file.
     *
     *  @param oldfullpath  The current name of the file, including path and name.
     *  @param newfullpath  The new name of the file, including path and name
     *  @return True if the file have been renamed successfully, false if not.
     */
    virtual bool renameFile(std::string_view oldfullpath, std::string_view newfullpath) const override;

    /**
     *  Checks whether a directory exists without considering search paths and resolution orders.
     *  @param dirPath The directory (with absolute path) to look up for
     *  @return Returns true if the directory found at the given absolute path, otherwise returns false
     */
    virtual bool isDirectoryExistInternal(std::string_view dirPath) const override;

    /**
     *  Removes a file.
     *
     *  @param filepath The full path of the file, it must be an absolute path.
     *  @return True if the file have been removed successfully, false if not.
     */
    virtual bool removeFile(std::string_view filepath) const override;

    /**
     *  Creates a directory.
     *
     *  @param dirPath The path of the directory, it must be an absolute path.
     *  @return True if the directory have been created successfully, false if not.
     */
    virtual bool createDirectories(std::string_view dirPath) const override;

    /**
     *  Removes a directory.
     *
     *  @param dirPath  The full path of the directory, it must be an absolute path.
     *  @return True if the directory have been removed successfully, false if not.
     */
    virtual bool removeDirectory(std::string_view dirPath) const override;
};

// end of platform group
/// @}

NS_AX_END

#endif  // __AX_FILEUTILS_WINRT_H__
