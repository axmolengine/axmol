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
#ifndef __AUDIO_ENGINE_IMPL_H_
#    define __AUDIO_ENGINE_IMPL_H_

#    include "platform/PlatformConfig.h"

#    include <unordered_map>
#    include <queue>

#    include "base/Object.h"
#    include "audio/AudioMacros.h"
#    include "audio/AudioCache.h"
#    include "audio/AudioPlayer.h"

namespace ax
{

class Scheduler;

class AX_DLL AudioEngineImpl : public ax::Object
{
public:
    AudioEngineImpl();
    ~AudioEngineImpl();

    bool init();
    AUDIO_ID play2d(std::string_view fileFullPath, bool loop, float volume, float time);
    void setVolume(AUDIO_ID audioID, float volume);
    void setLoop(AUDIO_ID audioID, bool loop);
    bool pause(AUDIO_ID audioID);
    bool resume(AUDIO_ID audioID);
    void stop(AUDIO_ID audioID);
    void stopAll();
    float getDuration(AUDIO_ID audioID);
    float getCurrentTime(AUDIO_ID audioID);
    bool setCurrentTime(AUDIO_ID audioID, float time);
    void setFinishCallback(AUDIO_ID audioID, const std::function<void(AUDIO_ID, std::string_view)>& callback);

    void uncache(std::string_view filePath);
    void uncacheAll();
    AudioCache* preload(std::string_view filePath, std::function<void(bool)> callback);
    void update(float dt);

private:
    // query players state per frame and dispatch finish callback if possible
    void _updatePlayers(bool forStop);
    void _play2d(AudioCache* cache, AUDIO_ID audioID);
    void _unscheduleUpdate();
    ALuint findValidSource();
#if defined(__APPLE__) && !AX_USE_ALSOFT
    static ALvoid myAlSourceNotificationCallback(ALuint sid, ALuint notificationID, ALvoid* userData);
#endif
    ALuint _alSources[MAX_AUDIOINSTANCES];

    // available sources
    std::queue<ALuint> _unusedSourcesPool;

    // filePath,bufferInfo
    hlookup::string_map<std::unique_ptr<AudioCache>> _audioCaches;

    // audioID,AudioInfo
    std::unordered_map<AUDIO_ID, AudioPlayer*> _audioPlayers;
    std::recursive_mutex _threadMutex;

    // finish callbacks
    std::vector<std::function<void()>> _finishCallbacks;

    bool _scheduled;

    AUDIO_ID _currentAudioID;
    Scheduler* _scheduler;
};

}
#endif  // __AUDIO_ENGINE_INL_H_
