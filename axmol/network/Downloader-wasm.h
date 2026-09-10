/****************************************************************************
 Copyright (c) 2019-present Simdsoft Limited and other Axmol contributors (see AUTHORS.md).


 https://axmol.dev

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include "axmol/network/IDownloaderImpl.h"
#include "axmol/network/Downloader.h"
#include <emscripten/fetch.h>

namespace ax
{
class Scheduler;
}

namespace ax
{
namespace network
{
class DownloadContextEmscripten;

class DownloaderEmscripten : public IDownloaderImpl
{
public:
    DownloaderEmscripten(const DownloaderHints& hints);
    virtual ~DownloaderEmscripten();

    void startTask(std::shared_ptr<DownloadTask>& task) override;

protected:
    int _id;

    DownloaderHints hints;

    std::unordered_map<emscripten_fetch_t*, DownloadContextEmscripten*> _taskMap;

    static void onError(emscripten_fetch_t* fetch);

    static void onProgress(emscripten_fetch_t* fetch);

    static void onDataLoad(emscripten_fetch_t* fetch);

    static void onLoad(emscripten_fetch_t* fetch);

    static void updateTaskProgressInfo(DownloadTask& task, emscripten_fetch_t* fetch);
};

}  // namespace network
}  // namespace ax
