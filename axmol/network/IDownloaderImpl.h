/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).

 https://axmol.dev/

 SPDX-License-Identifier: MIT
****************************************************************************/

#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include "axmol/base/Logging.h"

namespace ax
{

namespace network
{
class DownloadTask;

class AX_DLL IDownloadContext
{
public:
    virtual ~IDownloadContext() {}
    virtual void cancel() {}
};

class IDownloaderImpl
{
public:
    virtual ~IDownloaderImpl() {}

    std::function<void(const DownloadTask& task)> onTaskProgress;

    std::function<void(const DownloadTask& task,
                       int errorCode,
                       int errorCodeInternal,
                       std::string_view errorStr,
                       std::vector<unsigned char>& data)>
        onTaskFinish;

    virtual void startTask(std::shared_ptr<DownloadTask>& task) = 0;
};

}  // namespace network
}  // namespace ax
