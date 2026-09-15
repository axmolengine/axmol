/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#pragma once

#include <string>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <atomic>

#include "axmol/audio/AudioMacros.h"
#include "axmol/audio/AudioEffects.h"
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

    void stop();

    // queue buffer related stuff
    bool setTime(float time);
    float getTime() { return _currTime; }
    bool setLoop(bool loop);

    bool isFinished() const;

    void setReverbProperties(const ReverbProperties* reverbProperties);

protected:
    void setCache(AudioCache* cache);
    void rotateBufferThread(int offsetFrame);
    bool play2d();
    bool play3d();
#if defined(__APPLE__)
    void wakeupRotateThread();
#endif
    void clearEffects();

    AudioCache* _audioCache{nullptr};

    float _volume{1.0f};
    float _pitch{0.0f};
    bool _loop{false};

    bool _stopping{false};
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

#if AX_USE_ALSOFT
    ReverbProperties _reverbProperties;
#endif
    uint32_t _reverbSlot{};
    uint32_t _reverbEffect{};

    std::thread* _rotateBufferThread{nullptr};
    std::condition_variable _sleepCondition;
    std::mutex _sleepMutex;

    std::mutex _play2dMutex;
    std::function<void(AudioId, std::string_view)> _finishCallbak;

    friend class AudioEngineImpl;
};

}  // namespace ax
