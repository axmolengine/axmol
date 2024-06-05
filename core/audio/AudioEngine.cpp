/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

 https://axmolengine.github.io/

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

#include "platform/PlatformConfig.h"

#include "audio/AudioEngine.h"
#include <condition_variable>
#include <queue>
#include "platform/FileUtils.h"
#include "base/Utils.h"

#include "audio/AudioEngineImpl.h"

#define TIME_DELAY_PRECISION 0.0001

#ifdef ERROR
#    undef ERROR
#endif  // ERROR

NS_AX_BEGIN

const int AudioEngine::INVALID_AUDIO_ID = -1;
const float AudioEngine::TIME_UNKNOWN   = -1.0f;

// audio file path,audio IDs
hlookup::string_map<std::list<AUDIO_ID>> AudioEngine::_audioPathIDMap;
// profileName,ProfileHelper
hlookup::string_map<AudioEngine::ProfileHelper> AudioEngine::_audioPathProfileHelperMap;
unsigned int AudioEngine::_maxInstances                        = MAX_AUDIOINSTANCES;
AudioEngine::ProfileHelper* AudioEngine::_defaultProfileHelper = nullptr;
std::unordered_map<AUDIO_ID, AudioEngine::AudioInfo> AudioEngine::_audioIDInfoMap;
AudioEngineImpl* AudioEngine::_audioEngineImpl = nullptr;

bool AudioEngine::_isEnabled                                  = true;

AudioEngine::AudioInfo::AudioInfo()
    : profileHelper(nullptr), volume(1.0f), loop(false), duration(TIME_UNKNOWN), state(AudioState::INITIALIZING)
{}

AudioEngine::AudioInfo::~AudioInfo() {}

void AudioEngine::end()
{
    // make sure everythings cleanup before delete audio engine
    // fix #127
    uncacheAll();

    delete _audioEngineImpl;
    _audioEngineImpl = nullptr;

    delete _defaultProfileHelper;
    _defaultProfileHelper = nullptr;
}

bool AudioEngine::lazyInit()
{
    if (_audioEngineImpl == nullptr)
    {
        _audioEngineImpl = new AudioEngineImpl();
        if (!_audioEngineImpl->init())
        {
            delete _audioEngineImpl;
            _audioEngineImpl = nullptr;
            return false;
        }
    }

    return true;
}

AUDIO_ID AudioEngine::play2d(std::string_view filePath, bool loop, float volume, const AudioProfile* profile)
{
    return play2d(filePath, ax::AudioPlayerSettings{loop, volume, 0.0f});
}

AUDIO_ID AudioEngine::play2d(std::string_view filePath, const AudioPlayerSettings& settings, const AudioProfile* profile)
{
    AUDIO_ID ret = AudioEngine::INVALID_AUDIO_ID;

    do
    {
        if (!isEnabled())
        {
            break;
        }

        if (!lazyInit())
        {
            break;
        }

        if (!FileUtils::getInstance()->isFileExist(filePath))
        {
            break;
        }

        auto profileHelper = _defaultProfileHelper;
        if (profile && profile != &profileHelper->profile)
        {
            AX_ASSERT(!profile->name.empty());
            profileHelper          = &_audioPathProfileHelperMap[profile->name];
            profileHelper->profile = *profile;
        }

        if (_audioIDInfoMap.size() >= _maxInstances)
        {
            AXLOGE("Fail to play {} cause by limited max instance of AudioEngine", filePath);
            break;
        }
        if (profileHelper)
        {
            if (profileHelper->profile.maxInstances != 0 &&
                profileHelper->audioIDs.size() >= profileHelper->profile.maxInstances)
            {
                AXLOGE("Fail to play {} cause by limited max instance of AudioProfile", filePath);
                break;
            }
            if (profileHelper->profile.minDelay > TIME_DELAY_PRECISION)
            {
                auto currTime = utils::gettime();
                if (profileHelper->lastPlayTime > TIME_DELAY_PRECISION &&
                    currTime - profileHelper->lastPlayTime <= profileHelper->profile.minDelay)
                {
                    AXLOGE("Fail to play {} cause by limited minimum delay", filePath);
                    break;
                }
            }
        }

        float volume = settings.volume;
        if (volume < 0.0f)
        {
            volume = 0.0f;
        }
        else if (volume > 1.0f)
        {
            volume = 1.0f;
        }

        ret = _audioEngineImpl->play2d(filePath, settings.loop, volume, settings.time);
        if (ret != INVALID_AUDIO_ID)
        {
            _audioPathIDMap[filePath.data()].emplace_back(ret);
            auto it = _audioPathIDMap.find(filePath);

            auto& audioRef    = _audioIDInfoMap[ret];
            audioRef.volume   = volume;
            audioRef.loop     = settings.loop;
            audioRef.filePath = it->first;

            if (profileHelper)
            {
                profileHelper->lastPlayTime = utils::gettime();
                profileHelper->audioIDs.emplace_back(ret);
            }
            audioRef.profileHelper = profileHelper;
        }
    } while (0);

    return ret;
}

void AudioEngine::setLoop(AUDIO_ID audioID, bool loop)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end() && it->second.loop != loop)
    {
        _audioEngineImpl->setLoop(audioID, loop);
        it->second.loop = loop;
    }
}

void AudioEngine::setVolume(AUDIO_ID audioID, float volume)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end())
    {
        if (volume < 0.0f)
        {
            volume = 0.0f;
        }
        else if (volume > 1.0f)
        {
            volume = 1.0f;
        }

        if (it->second.volume != volume)
        {
            _audioEngineImpl->setVolume(audioID, volume);
            it->second.volume = volume;
        }
    }
}

void AudioEngine::pause(AUDIO_ID audioID)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end() && it->second.state == AudioState::PLAYING)
    {
        _audioEngineImpl->pause(audioID);
        it->second.state = AudioState::PAUSED;
    }
}

void AudioEngine::pauseAll()
{
    auto itEnd = _audioIDInfoMap.end();
    for (auto it = _audioIDInfoMap.begin(); it != itEnd; ++it)
    {
        if (it->second.state == AudioState::PLAYING)
        {
            _audioEngineImpl->pause(it->first);
            it->second.state = AudioState::PAUSED;
        }
    }
}

void AudioEngine::resume(AUDIO_ID audioID)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end() && it->second.state == AudioState::PAUSED)
    {
        _audioEngineImpl->resume(audioID);
        it->second.state = AudioState::PLAYING;
    }
}

void AudioEngine::resumeAll()
{
    auto itEnd = _audioIDInfoMap.end();
    for (auto it = _audioIDInfoMap.begin(); it != itEnd; ++it)
    {
        if (it->second.state == AudioState::PAUSED)
        {
            _audioEngineImpl->resume(it->first);
            it->second.state = AudioState::PLAYING;
        }
    }
}

void AudioEngine::stop(AUDIO_ID audioID)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end())
    {
        _audioEngineImpl->stop(audioID);

        remove(audioID);
    }
}

void AudioEngine::remove(AUDIO_ID audioID)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end())
    {
        if (it->second.profileHelper)
        {
            it->second.profileHelper->audioIDs.remove(audioID);
        }
        _audioPathIDMap[it->second.filePath].remove(audioID);
        _audioIDInfoMap.erase(audioID);
    }
}

void AudioEngine::stopAll()
{
    if (!_audioEngineImpl)
    {
        return;
    }
    _audioEngineImpl->stopAll();
    auto itEnd = _audioIDInfoMap.end();
    for (auto it = _audioIDInfoMap.begin(); it != itEnd; ++it)
    {
        if (it->second.profileHelper)
        {
            it->second.profileHelper->audioIDs.remove(it->first);
        }
    }
    _audioPathIDMap.clear();
    _audioIDInfoMap.clear();
}

void AudioEngine::uncache(std::string_view filePath)
{
    if (!_audioEngineImpl)
    {
        return;
    }
    auto audioIDsIter = _audioPathIDMap.find(filePath);
    if (audioIDsIter != _audioPathIDMap.end())
    {
        //@Note: For safely iterating elements from the audioID list, we need to copy the list
        // since 'AudioEngine::remove' may be invoked in '_audioEngineImpl->stop' synchronously.
        // If this happens, it will break the iteration, and crash will appear on some devices.
        std::list<AUDIO_ID> copiedIDs(audioIDsIter->second);

        for (AUDIO_ID audioID : copiedIDs)
        {
            _audioEngineImpl->stop(audioID);

            auto itInfo = _audioIDInfoMap.find(audioID);
            if (itInfo != _audioIDInfoMap.end())
            {
                if (itInfo->second.profileHelper)
                {
                    itInfo->second.profileHelper->audioIDs.remove(audioID);
                }
                _audioIDInfoMap.erase(audioID);
            }
        }
        _audioPathIDMap.erase(filePath);
    }

    if (_audioEngineImpl)
    {
        _audioEngineImpl->uncache(filePath);
    }
}

void AudioEngine::uncacheAll()
{
    if (!_audioEngineImpl)
    {
        return;
    }
    stopAll();
    _audioEngineImpl->uncacheAll();
}

float AudioEngine::getDuration(AUDIO_ID audioID)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end() && it->second.state != AudioState::INITIALIZING)
    {
        if (it->second.duration == TIME_UNKNOWN)
        {
            it->second.duration = _audioEngineImpl->getDuration(audioID);
        }
        return it->second.duration;
    }

    return TIME_UNKNOWN;
}

bool AudioEngine::setCurrentTime(AUDIO_ID audioID, float time)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end() && it->second.state != AudioState::INITIALIZING)
    {
        return _audioEngineImpl->setCurrentTime(audioID, time);
    }

    return false;
}

float AudioEngine::getCurrentTime(AUDIO_ID audioID)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end() && it->second.state != AudioState::INITIALIZING)
    {
        return _audioEngineImpl->getCurrentTime(audioID);
    }
    return 0.0f;
}

void AudioEngine::setFinishCallback(AUDIO_ID audioID, const std::function<void(AUDIO_ID, std::string_view)>& callback)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end())
    {
        _audioEngineImpl->setFinishCallback(audioID, callback);
    }
}

bool AudioEngine::setMaxAudioInstance(int maxInstances)
{
    if (maxInstances > 0 && maxInstances <= MAX_AUDIOINSTANCES)
    {
        _maxInstances = maxInstances;
        return true;
    }

    return false;
}

bool AudioEngine::isLoop(AUDIO_ID audioID)
{
    auto tmpIterator = _audioIDInfoMap.find(audioID);
    if (tmpIterator != _audioIDInfoMap.end())
    {
        return tmpIterator->second.loop;
    }

    AXLOGW("AudioEngine::isLoop-->The audio instance {} is non-existent", audioID);
    return false;
}

float AudioEngine::getVolume(AUDIO_ID audioID)
{
    auto tmpIterator = _audioIDInfoMap.find(audioID);
    if (tmpIterator != _audioIDInfoMap.end())
    {
        return tmpIterator->second.volume;
    }

    AXLOGW("AudioEngine::getVolume-->The audio instance {} is non-existent", audioID);
    return 0.0f;
}

AudioEngine::AudioState AudioEngine::getState(AUDIO_ID audioID)
{
    auto tmpIterator = _audioIDInfoMap.find(audioID);
    if (tmpIterator != _audioIDInfoMap.end())
    {
        return tmpIterator->second.state;
    }

    return AudioState::ERROR;
}

AudioProfile* AudioEngine::getProfile(AUDIO_ID audioID)
{
    auto it = _audioIDInfoMap.find(audioID);
    if (it != _audioIDInfoMap.end())
    {
        return &it->second.profileHelper->profile;
    }

    return nullptr;
}

AudioProfile* AudioEngine::getDefaultProfile()
{
    if (_defaultProfileHelper == nullptr)
    {
        _defaultProfileHelper = new ProfileHelper();
    }

    return &_defaultProfileHelper->profile;
}

AudioProfile* AudioEngine::getProfile(std::string_view name)
{
    auto it = _audioPathProfileHelperMap.find(name);
    if (it != _audioPathProfileHelperMap.end())
    {
        return &it->second.profile;
    }
    else
    {
        return nullptr;
    }
}

void AudioEngine::preload(std::string_view filePath, std::function<void(bool isSuccess)> callback)
{
    if (!isEnabled())
    {
        callback(false);
        return;
    }

    lazyInit();

    if (_audioEngineImpl)
    {
        if (!FileUtils::getInstance()->isFileExist(filePath))
        {
            if (callback)
            {
                callback(false);
            }
            return;
        }

        _audioEngineImpl->preload(filePath, callback);
    }
}

void AudioEngine::addTask(const std::function<void()>& task)
{
    lazyInit();
    Director::getInstance()->getJobSystem()->enqueue(task);
}

int AudioEngine::getPlayingAudioCount()
{
    return static_cast<int>(_audioIDInfoMap.size());
}

void AudioEngine::setEnabled(bool isEnabled)
{
    if (_isEnabled != isEnabled)
    {
        _isEnabled = isEnabled;

        if (!_isEnabled)
        {
            stopAll();
        }
    }
}

bool AudioEngine::isEnabled()
{
    return _isEnabled;
}
NS_AX_END
#undef LOG_TAG
