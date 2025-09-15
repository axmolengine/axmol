/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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

#pragma once

#include <string>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>

#include "axmol/audio/AudioMacros.h"
#include "axmol/platform/PlatformMacros.h"
#include "axmol/audio/alconfig.h"
#include "axmol/math/Vec3.h"

namespace ax
{

class AudioCache;
class AudioEngineImpl;

class AX_DLL AudioPlayer
{
    friend class AudioEngineImpl;

public:
    AudioPlayer();
    ~AudioPlayer();

    void destroy();

    // queue buffer related stuff
    bool setTime(float time);
    float getTime() { return _currTime; }
    bool setLoop(bool loop);

    bool isFinished() const;

protected:
    void setCache(AudioCache* cache);
    void rotateBufferThread(int offsetFrame);
    bool play2d();
    bool play3d();
#if defined(__APPLE__)
    void wakeupRotateThread();
#endif

    AudioCache* _audioCache{nullptr};

    float _volume{1.0f};
    float _pitch{0.0f};
    bool _loop{false};

    bool _isDestroyed{false};
    bool _removeByAudioEngine{false};
    bool _ready{false};
    ALuint _alSource{0};

    // play by circular buffer
    float _currTime{0.0f};

    ALuint _bufferIds[QUEUEBUFFER_NUM] = {0};

    unsigned int _id;

    float _pan{};
    float _distanceScale;
    Vec3 _sourcePosition;

    bool _streamingSource{false};
    bool _timeDirty{false};
    bool _isRotateThreadExited{false};
#if defined(__APPLE__)
    std::atomic_bool _needWakeupRotateThread;
#endif

    std::thread* _rotateBufferThread{nullptr};
    std::condition_variable _sleepCondition;
    std::mutex _sleepMutex;

    std::mutex _play2dMutex;
    std::function<void(AUDIO_ID, std::string_view)> _finishCallbak;

    friend class AudioEngineImpl;
};

}  // namespace ax
