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

#include "platform/PlatformConfig.h"

#ifndef __NEWAUDIOENGINE_TEST_H_
#    define __NEWAUDIOENGINE_TEST_H_

#    include "axmol.h"
#    include "../BaseTest.h"

#    include "audio/AudioEngine.h"

DEFINE_TEST_SUITE(AudioEngineTests);

class AudioEngineTestDemo : public TestCase
{
public:
    AudioEngineTestDemo();

    std::string title() const override;
    void onExit() override;

    void onBackCallback(ax::Object* sender) override;

protected:
    std::shared_ptr<bool> _isDestroyed;
};

class AudioControlTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioControlTest);

    virtual ~AudioControlTest();

    bool init() override;

    void update(float dt) override;

    std::string title() const override;

private:
    int _audioID;
    bool _loopEnabled;
    float _volume;
    float _duration;
    float _timeRatio;

    void* _playItem;
    void* _timeSlider;
    bool _updateTimeSlider;
    bool _isStopped;
    ax::Label* _playOverLabel;
};

class AudioOpusTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioOpusTest);

    virtual ~AudioOpusTest();

    bool init() override;

    void onEnter() override;

    std::string title() const override;

private:
    ax::Label* _stateLabel = nullptr;
    int _audioID           = -1;
};

class AudioWavTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioWavTest);

    virtual ~AudioWavTest();

    bool init() override;

    void onEnter() override;

    std::string title() const override;

private:
    int _audioID = -1;

    int _curIndex = -1;
    std::vector<std::string> _wavFiles;

    ax::Label* _stateLabel = nullptr;
};

class PlaySimultaneouslyTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(PlaySimultaneouslyTest);

    virtual ~PlaySimultaneouslyTest();

    bool init() override;

    std::string title() const override;

private:
    static const int TEST_COUNT = 10;
    std::string _files[TEST_COUNT];

    void* _playItem;
    int _playingcount;
};

class AudioProfileTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioProfileTest);

    virtual ~AudioProfileTest();

    bool init() override;

    std::string title() const override;
    std::string subtitle() const override;

    void update(float dt) override;

private:
    static const int FILE_COUNT = 2;
    std::string _files[FILE_COUNT];
    ax::AudioProfile _audioProfile;

    int _audioCount;
    ax::Label* _showLabel;
    float _time;
    float _minDelay;
    void* _timeSlider;
};

class InvalidAudioFileTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(InvalidAudioFileTest);

    virtual ~InvalidAudioFileTest();

    bool init() override;

    std::string title() const override;
    std::string subtitle() const override;

private:
};

class LargeAudioFileTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(LargeAudioFileTest);

    virtual ~LargeAudioFileTest();

    bool init() override;

    std::string title() const override;

private:
};

class AudioLoadTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioLoadTest);

    bool init() override;

    std::string title() const override;
};

class AudioIssue18597Test : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioIssue18597Test);

    float _time;

    bool init() override;

    std::string title() const override;
    std::string subtitle() const override;
};

class AudioIssue11143Test : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioIssue11143Test);

    bool init() override;

    std::string title() const override;
    std::string subtitle() const override;
};

class AudioPerformanceTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioPerformanceTest);

    bool init() override;

    std::string title() const override;
    std::string subtitle() const override;
};

class AudioSwitchStateTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioSwitchStateTest);

    void onEnter() override;
    std::string title() const override;
    std::string subtitle() const override;
};

class AudioSmallFileTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioSmallFileTest);

    void onEnter() override;

    std::string title() const override;
    std::string subtitle() const override;
};

class AudioSmallFile2Test : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioSmallFile2Test);

    void onEnter() override;

    std::string title() const override;
    std::string subtitle() const override;
};

class AudioSmallFile3Test : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioSmallFile3Test);

    void onEnter() override;

    std::string title() const override;
    std::string subtitle() const override;
};

class AudioPauseResumeAfterPlay : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioPauseResumeAfterPlay);

    void onEnter() override;

    std::string title() const override;
    std::string subtitle() const override;
};

class AudioIssue16938Test : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioIssue16938Test);

    void onEnter() override;

    std::string title() const override;
    std::string subtitle() const override;
};

class AudioIssue3317Test : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioIssue3317Test);

    void onEnter() override;

    std::string title() const override;
    std::string subtitle() const override;

private:
    int _audioID = ax::AudioEngine::INVALID_AUDIO_ID;
    int _state   = 0;
};

class AudioPreloadSameFileMultipleTimes : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioPreloadSameFileMultipleTimes);

    void onEnter() override;

    std::string title() const override;
    std::string subtitle() const override;
};

class AudioPlayFileInWritablePath : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioPlayFileInWritablePath);

    void onEnter() override;
    void onExit() override;

    std::string title() const override;
    std::string subtitle() const override;

private:
    std::vector<std::string> _oldSearchPaths;
};

class AudioPlayInFinishedCB : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioPlayInFinishedCB);

    void onEnter() override;
    void onExit() override;

    std::string title() const override;
    std::string subtitle() const override;

private:
    void doPlay(std::string_view filename);
    void playMusic(std::string_view filename);
    std::list<std::string> _playList;
};

class AudioUncacheInFinishedCB : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioUncacheInFinishedCB);

    void onEnter() override;
    void onExit() override;

    std::string title() const override;
    std::string subtitle() const override;

private:
};

class AudioPanningTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioPanningTest);

    ~AudioPanningTest() override;

    bool init() override;
    void update(float dt) override;
    std::string title() const override;

private:
    int _audioID;
    bool _loopEnabled;
    float _volume;
    float _pan;
    float _duration;
    float _timeRatio;

    void* _playItem;
    void* _timeSlider;
    bool _updateTimeSlider;
    bool _isStopped;
    ax::Label* _playOverLabel;
};

#    if AX_USE_ALSOFT
class AudioReverbTest : public AudioEngineTestDemo
{
public:
    CREATE_FUNC(AudioReverbTest);

    ~AudioReverbTest() override;

    bool init() override;
    void update(float dt) override;
    std::string title() const override;

private:
    int _audioID;
    bool _loopEnabled;
    float _volume;
    float _duration;
    float _timeRatio;

    void* _playItem;
    void* _timeSlider;
    bool _updateTimeSlider;
    bool _isStopped;
    ax::Label* _playOverLabel;
    std::map<std::string, ax::ReverbProperties> _reverbSettingsMap;
    std::map<std::string, ax::ReverbProperties>::const_iterator _currentReverbItr;
};
#    endif

#endif /* defined(__NEWAUDIOENGINE_TEST_H_) */
