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

#include "platform/PlatformConfig.h"
#include "platform/PlatformMacros.h"
#include "audio/AudioMacros.h"
#include <functional>
#include <list>
#include <string>
#include <unordered_map>

#ifdef ERROR
#    undef ERROR
#endif  // ERROR

/**
 * @addtogroup audio
 * @{
 */

namespace ax
{

/**
 * @struct AudioPlayerSettings
 *
 * @brief
 * @js NA
 */
struct AX_DLL AudioPlayerSettings
{
    bool loop = false; // Whether audio instance loop or not.
    float volume = 1.0f; // Volume value (range from 0.0 to 1.0).
    float time = 0.0f; // The initial time offset when play audio
};

/**
 * @class AudioProfile
 *
 * @brief
 * @js NA
 */
class AX_DLL AudioProfile
{
public:
    // Profile name can't be empty.
    std::string name;
    // The maximum number of simultaneous audio instance.
    unsigned int maxInstances;

    /* Minimum delay in between sounds */
    double minDelay;

    /**
     * Default constructor
     *
     * @lua new
     */
    AudioProfile() : maxInstances(0), minDelay(0.0) {}
};

class AudioEngineImpl;

/**
 * @class AudioEngine
 *
 * @brief Offers a interface to play audio.
 *
 * @note Make sure to call AudioEngine::end() when the audio engine is not needed anymore to release resources.
 * @js NA
 */

class AX_DLL AudioEngine
{
public:
    /** AudioState enum,all possible states of an audio instance.*/
    enum class AudioState
    {
        ERROR = -1,
        INITIALIZING,
        PLAYING,
        PAUSED
    };

    static const int INVALID_AUDIO_ID;

    static const float TIME_UNKNOWN;

    static bool lazyInit();

    /**
     * Release objects relating to AudioEngine.
     *
     * @warning It must be called before the application exit.
     * @lua endToLua
     */
    static void end();

    /**
     * Gets the default profile of audio instances.
     *
     * @return The default profile of audio instances.
     */
    static AudioProfile* getDefaultProfile();

    /**
     * Play 2d sound.
     *
     * @param filePath The path of an audio file.
     * @param loop Whether audio instance loop or not.
     * @param volume Volume value (range from 0.0 to 1.0).
     * @param profile A profile for audio instance. When profile is not specified, default profile will be used.
     * @return An audio ID. It allows you to dynamically change the behavior of an audio instance on the fly.
     *
     * @see `AudioProfile`
     */
    static AUDIO_ID play2d(std::string_view filePath,
                           bool loop                   = false,
                           float volume                = 1.0f,
                           const AudioProfile* profile = nullptr);

     /**
     * Play 2d sound.
     *
     * @param filePath The path of an audio file.
     * @param settings The player settings for audio.
     * @param profile A profile for audio instance. When profile is not specified, default profile will be used.
     * @return An audio ID. It allows you to dynamically change the behavior of an audio instance on the fly.
     *
     * @see `AudioProfile`, `AudioPlayerSettings`
     */
    static AUDIO_ID play2d(std::string_view filePath,
                           const AudioPlayerSettings& settings,
                           const AudioProfile* profile = nullptr);
    /**
     * Sets whether an audio instance loop or not.
     *
     * @param audioID An audioID returned by the play2d function.
     * @param loop Whether audio instance loop or not.
     */
    static void setLoop(AUDIO_ID audioID, bool loop);

    /**
     * Checks whether an audio instance is loop.
     *
     * @param audioID An audioID returned by the play2d function.
     * @return Whether or not an audio instance is loop.
     */
    static bool isLoop(AUDIO_ID audioID);

    /**
     * Sets volume for an audio instance.
     *
     * @param audioID An audioID returned by the play2d function.
     * @param volume Volume value (range from 0.0 to 1.0).
     */
    static void setVolume(AUDIO_ID audioID, float volume);

    /**
     * Gets the volume value of an audio instance.
     *
     * @param audioID An audioID returned by the play2d function.
     * @return Volume value (range from 0.0 to 1.0).
     */
    static float getVolume(AUDIO_ID audioID);

    /**
     * Pause an audio instance.
     *
     * @param audioID An audioID returned by the play2d function.
     */
    static void pause(AUDIO_ID audioID);

    /** Pause all playing audio instances. */
    static void pauseAll();

    /**
     * Resume an audio instance.
     *
     * @param audioID An audioID returned by the play2d function.
     */
    static void resume(AUDIO_ID audioID);

    /** Resume all suspended audio instances. */
    static void resumeAll();

    /**
     * Stop an audio instance.
     *
     * @param audioID An audioID returned by the play2d function.
     */
    static void stop(AUDIO_ID audioID);

    /** Stop all audio instances. */
    static void stopAll();

    /**
     * Sets the current playback position of an audio instance.
     *
     * @param audioID   An audioID returned by the play2d function.
     * @param sec       The offset in seconds from the start to seek to.
     * @return
     */
    static bool setCurrentTime(AUDIO_ID audioID, float sec);

    /**
     * Gets the current playback position of an audio instance.
     *
     * @param audioID An audioID returned by the play2d function.
     * @return The current playback position of an audio instance.
     */
    static float getCurrentTime(AUDIO_ID audioID);

    /**
     * Gets the duration of an audio instance.
     *
     * @param audioID An audioID returned by the play2d function.
     * @return The duration of an audio instance.
     */
    static float getDuration(AUDIO_ID audioID);

    /**
     * Returns the state of an audio instance.
     *
     * @param audioID An audioID returned by the play2d function.
     * @return The status of an audio instance.
     */
    static AudioState getState(AUDIO_ID audioID);

    /**
     * Register a callback to be invoked when an audio instance has completed playing.
     *
     * @param audioID An audioID returned by the play2d function.
     * @param callback
     */
    static void setFinishCallback(AUDIO_ID audioID, const std::function<void(AUDIO_ID, std::string_view)>& callback);

    /**
     * Gets the maximum number of simultaneous audio instance of AudioEngine.
     */
    static int getMaxAudioInstance() { return _maxInstances; }

    /**
     * Sets the maximum number of simultaneous audio instance for AudioEngine.
     *
     * @param maxInstances The maximum number of simultaneous audio instance.
     */
    static bool setMaxAudioInstance(int maxInstances);

    /**
     * Uncache the audio data from internal buffer.
     * AudioEngine cache audio data on ios,mac, and win32 platform.
     *
     * @warning This can lead to stop related audio first.
     * @param filePath Audio file path.
     */
    static void uncache(std::string_view filePath);

    /**
     * Uncache all audio data from internal buffer.
     *
     * @warning All audio will be stopped first.
     */
    static void uncacheAll();

    /**
     * Gets the audio profile by id of audio instance.
     *
     * @param audioID An audioID returned by the play2d function.
     * @return The audio profile.
     */
    static AudioProfile* getProfile(AUDIO_ID audioID);

    /**
     * Gets an audio profile by name.
     *
     * @param profileName A name of audio profile.
     * @return The audio profile.
     */
    static AudioProfile* getProfile(std::string_view profileName);

    /**
     * Preload audio file.
     * @param filePath The file path of an audio.
     */
    static void preload(std::string_view filePath) { preload(filePath, nullptr); }

    /**
     * Preload audio file.
     * @param filePath The file path of an audio.
     * @param callback A callback which will be called after loading is finished.
     */
    static void preload(std::string_view filePath, std::function<void(bool isSuccess)> callback);

    /**
     * Gets playing audio count.
     */
    static int getPlayingAudioCount();

    /**
     * Whether to enable playing audios
     * @note If it's disabled, current playing audios will be stopped and the later 'preload', 'play2d' methods will
     * take no effects.
     */
    static void setEnabled(bool isEnabled);
    /**
     * Check whether AudioEngine is enabled.
     */
    static bool isEnabled();

protected:
    static void addTask(const std::function<void()>& task);
    static void remove(AUDIO_ID audioID);

    struct ProfileHelper
    {
        AudioProfile profile;

        std::list<int> audioIDs;

        double lastPlayTime;

        ProfileHelper() : lastPlayTime(0.0) {}
    };

    struct AudioInfo
    {
        std::string filePath;
        ProfileHelper* profileHelper;

        float volume;
        bool loop;
        float duration;
        AudioState state;

        AudioInfo();
        ~AudioInfo();

    private:
        AudioInfo(const AudioInfo& info);
        AudioInfo(AudioInfo&& info);
        AudioInfo& operator=(const AudioInfo& info);
        AudioInfo& operator=(AudioInfo&& info);
    };

    // audioID,audioAttribute
    static std::unordered_map<AUDIO_ID, AudioInfo> _audioIDInfoMap;

    // audio file path,audio IDs
    static hlookup::string_map<std::list<AUDIO_ID>> _audioPathIDMap;

    // profileName,ProfileHelper
    static hlookup::string_map<ProfileHelper> _audioPathProfileHelperMap;

    static unsigned int _maxInstances;

    static ProfileHelper* _defaultProfileHelper;

    static AudioEngineImpl* _audioEngineImpl;

    static bool _isEnabled;

    friend class AudioEngineImpl;
};

}

// end group
/// @}
