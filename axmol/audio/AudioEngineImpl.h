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

#include <queue>
#include <memory>

#include "axmol/base/Object.h"
#include "axmol/audio/AudioMacros.h"
#include "axmol/audio/AudioEffects.h"
#include "axmol/audio/AudioCache.h"
#include "axmol/audio/AudioPlayer.h"
#include "axmol/math/Vec3.h"

namespace ax
{

class Scheduler;
class Director;
struct AudioEngineSettings;

class AX_DLL AudioEngineImpl : public ax::Object
{
public:
    AudioEngineImpl();
    ~AudioEngineImpl();

    bool init(const AudioEngineSettings& settings);
    AudioId play2d(std::string_view fileFullPath, bool loop, float volume, float time);
    AudioId play3d(std::string_view fileFullPath,
                   const Vec3& position,
                   float distanceScale,
                   bool loop,
                   float volume,
                   float time);
    void setVolume(AudioId audioID, float volume);
    void setPitch(AudioId audioID, float pitch);
    void setLoop(AudioId audioID, bool loop);
    bool pause(AudioId audioID);
    bool resume(AudioId audioID);
    void stop(AudioId audioID);
    void stopAll();
    float getDuration(AudioId audioID);
    float getCurrentTime(AudioId audioID);
    bool setCurrentTime(AudioId audioID, float time);
    void setFinishCallback(AudioId audioID, const std::function<void(AudioId, std::string_view)>& callback);
    void setPan(AudioId audioId, float value, float distance);
    float getPan(AudioId audioId);
    ax::Vec3 getSourcePosition(AudioId audioId);
    void setSourcePosition(AudioId audioId, const ax::Vec3& position);
    void setListenerPosition(const ax::Vec3& position);
    ax::Vec3 getListenerPosition();
    void setReverbProperties(AudioId audioId, const ReverbProperties* reverbProperties);

    void uncache(std::string_view filePath);
    void uncacheAll();
    AudioCache* preload(std::string_view filePath, std::function<void(bool)> callback);
    void update(float dt);

    bool setHRTFEnabled(bool enabled);

    void pauseDevice();
    void resumeDevice();
    void reopenDevice();

    static AudioEngineImpl* current;

private:
    bool isExtensionPresent(const char* extensionId);
    void checkExtensions();

    // query players state per frame and dispatch finish callback if possible
    void _updatePlayers(bool forStop);
    void _play2d(AudioCache* cache, AudioId audioID);
    void _play3d(AudioCache* cache, AudioId audioID);
    void _unscheduleUpdate();
    ALuint findValidSource();
#if defined(__APPLE__) && !AX_USE_ALSOFT
    static ALvoid onAlSourceNotification(ALuint sourceID, ALuint notificationID, ALvoid* userData);
    void weakupAudioPlayer(ALuint sourceID);
#endif
    Director* _director{nullptr};

    ALuint _alSources[MAX_AUDIOINSTANCES];

    // available sources
    std::queue<ALuint> _unusedSourcesPool;

    // filePath,bufferInfo
    tlx::string_map<std::unique_ptr<AudioCache>> _audioCaches;

    // audioID,AudioInfo
    std::unordered_map<AudioId, AudioPlayer*> _audioPlayers;
    std::recursive_mutex _threadMutex;

    // finish callbacks
    std::vector<std::function<void()>> _finishCallbacks;

    bool _scheduled{false};
    bool _stereoExtension{false};

    AudioId _currentAudioID{0};
    Scheduler* _scheduler{nullptr};

    ALCdevice* _device{nullptr};
    ALCcontext* _context{nullptr};
};

}  // namespace ax
