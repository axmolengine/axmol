/****************************************************************************
 Copyright (c) 2015-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <stdint.h>
#include "axmol/network/IDownloaderImpl.h"

namespace ax
{

class Scheduler;

namespace network
{
class DownloadContextCURL;
class DownloaderHints;
class DownloaderCURL;

class DownloaderCURL : public IDownloaderImpl
{
public:
    DownloaderCURL(const DownloaderHints& hints);
    virtual ~DownloaderCURL();

    void startTask(std::shared_ptr<DownloadTask>& task) override;

protected:
    class Impl;
    std::shared_ptr<Impl> _impl;

    void _lazyScheduleUpdate();

    static void _updateTaskProgressInfo(DownloadTask& task, int64_t totalExpected = -1);

    // scheduler for update processing and finished task in main schedule
    void _onDownloadFinished(DownloadTask& task);

    // scheduler for update processing and finished task in main schedule
    void _onUpdate(float);
    std::string _schedulerKey;
    Scheduler* _scheduler = nullptr;
};

}  // namespace network
}  // namespace ax
