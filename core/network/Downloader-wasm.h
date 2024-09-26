/****************************************************************************
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).


 https://axmol.dev

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

#pragma once

#include "network/IDownloaderImpl.h"
#include "network/Downloader.h"
#include <emscripten/fetch.h>

namespace ax {
    class Scheduler;
}

namespace ax { namespace network
{
    class DownloadTaskEmscripten;

    class DownloaderEmscripten : public IDownloaderImpl
    {
    public:
        DownloaderEmscripten(const DownloaderHints& hints);
        virtual ~DownloaderEmscripten();

        // virtual IDownloadTask *createCoTask(std::shared_ptr<const DownloadTask>& task) override;
        virtual void startTask(std::shared_ptr<DownloadTask>& task) override;

    protected:
        int _id;

        DownloaderHints hints;

        std::unordered_map<unsigned int, DownloadTaskEmscripten*> _taskMap;

        static void onError(emscripten_fetch_t *fetch);

        static void onProgress(emscripten_fetch_t *fetch);

        static void onDataLoad(emscripten_fetch_t *fetch);

        static void onLoad(emscripten_fetch_t *fetch);
    };

}}  // namespace ax::network

