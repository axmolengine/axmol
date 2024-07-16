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
#include "NewAudioEngineTest.h"
#include "ui/CocosGUI.h"

USING_NS_AX;
using namespace ax::ui;

AudioEngineTests::AudioEngineTests()
{
    ADD_TEST_CASE(AudioControlTest);
    ADD_TEST_CASE(AudioWavTest);
    ADD_TEST_CASE(AudioLoadTest);
    ADD_TEST_CASE(PlaySimultaneouslyTest);
    ADD_TEST_CASE(AudioProfileTest);
    ADD_TEST_CASE(InvalidAudioFileTest);
    ADD_TEST_CASE(LargeAudioFileTest);
    ADD_TEST_CASE(AudioPerformanceTest);
    ADD_TEST_CASE(AudioSmallFileTest);
    ADD_TEST_CASE(AudioSmallFile2Test);
    ADD_TEST_CASE(AudioSmallFile3Test);
    ADD_TEST_CASE(AudioPauseResumeAfterPlay);
    ADD_TEST_CASE(AudioPreloadSameFileMultipleTimes);
    ADD_TEST_CASE(AudioPlayFileInWritablePath);
    ADD_TEST_CASE(AudioIssue16938Test);
    ADD_TEST_CASE(AudioPlayInFinishedCB);
    ADD_TEST_CASE(AudioUncacheInFinishedCB);

    ADD_TEST_CASE(AudioIssue18597Test);
    ADD_TEST_CASE(AudioIssue11143Test);

    // FIXME: Please keep AudioSwitchStateTest to the last position since this test case doesn't work well on each
    // platforms.
    ADD_TEST_CASE(AudioSwitchStateTest);
}

namespace
{

class TextButton : public ax::Label
{
public:
    static TextButton* create(std::string_view text, const std::function<void(TextButton*)>& onTriggered)
    {
        auto ret = new TextButton();

        TTFConfig ttfconfig("fonts/arial.ttf", 25);
        if (ret->setTTFConfig(ttfconfig))
        {
            ret->setString(text);
            ret->_onTriggered = onTriggered;

            ret->autorelease();

            return ret;
        }

        delete ret;
        return nullptr;
    }

    void setEnabled(bool enabled)
    {
        _enabled = enabled;
        if (_enabled)
        {
            this->setColor(Color3B::WHITE);
        }
        else
        {
            this->setColor(Color3B::GRAY);
        }
    }

private:
    TextButton() : _onTriggered(nullptr), _enabled(true)
    {
        auto listener = EventListenerTouchOneByOne::create();
        listener->setSwallowTouches(true);

        listener->onTouchBegan     = AX_CALLBACK_2(TextButton::onTouchBegan, this);
        listener->onTouchEnded     = AX_CALLBACK_2(TextButton::onTouchEnded, this);
        listener->onTouchCancelled = AX_CALLBACK_2(TextButton::onTouchCancelled, this);

        _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
    }

    bool touchHits(Touch* touch)
    {
        auto hitPos = this->convertToNodeSpace(touch->getLocation());
        if (hitPos.x >= 0 && hitPos.y >= 0 && hitPos.x <= _contentSize.width && hitPos.y <= _contentSize.height)
        {
            return true;
        }
        return false;
    }

    bool onTouchBegan(Touch* touch, Event* event)
    {
        auto hits = touchHits(touch);
        if (hits)
        {
            scaleButtonTo(0.95f);
        }
        return hits;
    }

    void onTouchEnded(Touch* touch, Event* event)
    {
        if (_enabled)
        {
            auto hits = touchHits(touch);
            if (hits && _onTriggered)
            {
                _onTriggered(this);
            }
        }

        scaleButtonTo(1);
    }

    void onTouchCancelled(Touch* touch, Event* event) { scaleButtonTo(1); }

    void scaleButtonTo(float scale)
    {
        auto action = ScaleTo::create(0.05f, scale);
        action->setTag(10000);
        stopActionByTag(10000);
        runAction(action);
    }

    std::function<void(TextButton*)> _onTriggered;

    bool _enabled;
};

class SliderEx : public Slider
{
public:
    static SliderEx* create()
    {
        auto ret = new SliderEx();
        if (ret->init())
        {
            ret->loadBarTexture("cocosui/sliderTrack.png");
            ret->loadSlidBallTextures("cocosui/sliderThumb.png", "cocosui/sliderThumb.png", "");
            ret->loadProgressBarTexture("cocosui/sliderProgress.png");
            ret->setTouchEnabled(true);

            ret->autorelease();

            return ret;
        }
        AX_SAFE_DELETE(ret);
        return ret;
    }

    void setRatio(float ratio)
    {
        ratio = clampf(ratio, 0.0f, 1.0f);

        _ratio = ratio;
        setPercent(100 * _ratio);
    }

    float getRatio()
    {
        _ratio = 1.0f * _percent / _maxPercent;
        return _ratio;
    }

private:
    float _ratio;
};
}  // namespace

AudioEngineTestDemo::AudioEngineTestDemo() : _isDestroyed(std::make_shared<bool>(false)) {}

void AudioEngineTestDemo::onExit()
{
    *_isDestroyed = true;
    AudioEngine::uncacheAll();
    TestCase::onExit();
}

std::string AudioEngineTestDemo::title() const
{
    return "New Audio Engine Test";
}

void AudioEngineTestDemo::onBackCallback(ax::Object* sender)
{
    AudioEngine::end();
    TestCase::onBackCallback(sender);
}

// AudioControlTest
bool AudioControlTest::init()
{
    auto ret          = AudioEngineTestDemo::init();
    _audioID          = AudioEngine::INVALID_AUDIO_ID;
    _loopEnabled      = false;
    _volume           = 1.0f;
    _duration         = AudioEngine::TIME_UNKNOWN;
    _timeRatio        = 0.0f;
    _updateTimeSlider = true;
    _isStopped        = false;

    std::string fontFilePath = "fonts/arial.ttf";

    auto& layerSize = this->getContentSize();

    _playOverLabel = Label::createWithSystemFont("Play Over", "", 30);
    _playOverLabel->setPosition(Vec2(layerSize / 2) + Vec2(0, 30));
    _playOverLabel->setVisible(false);
    addChild(_playOverLabel, 99999);

    auto playItem = TextButton::create("play", [&](TextButton* button) {
        if (_audioID == AudioEngine::INVALID_AUDIO_ID)
        {
            _audioID = AudioEngine::play2d("background.mp3", _loopEnabled, _volume);

            if (_audioID != AudioEngine::INVALID_AUDIO_ID)
            {
                _isStopped = false;

                button->setEnabled(false);
                AudioEngine::setFinishCallback(_audioID, [&](int id, std::string_view filePath) {
                    AXLOGD("_audioID({}), _isStopped:({}), played over!!!", _audioID, _isStopped);

                    _playOverLabel->setVisible(true);

                    scheduleOnce([&](float dt) { _playOverLabel->setVisible(false); }, 2.0f, "hide_play_over_label");

                    assert(!_isStopped);  // Stop audio should not trigger finished callback
                    _audioID = AudioEngine::INVALID_AUDIO_ID;
                    ((TextButton*)_playItem)->setEnabled(true);

                    _timeRatio = 0.0f;
                    ((SliderEx*)_timeSlider)->setRatio(_timeRatio);
                });
            }
        }
    });
    _playItem     = playItem;
    playItem->setPosition(layerSize.width * 0.3f, layerSize.height * 0.8f);
    addChild(playItem);

    auto stopItem = TextButton::create("stop", [&](TextButton* button) {
        if (_audioID != AudioEngine::INVALID_AUDIO_ID)
        {
            _isStopped = true;
            AudioEngine::stop(_audioID);

            _audioID = AudioEngine::INVALID_AUDIO_ID;
            ((TextButton*)_playItem)->setEnabled(true);
        }
    });
    stopItem->setPosition(layerSize.width * 0.7f, layerSize.height * 0.8f);
    addChild(stopItem);

    auto pauseItem = TextButton::create("pause", [&](TextButton* button) {
        if (_audioID != AudioEngine::INVALID_AUDIO_ID)
        {
            AudioEngine::pause(_audioID);
        }
    });
    pauseItem->setPosition(layerSize.width * 0.3f, layerSize.height * 0.7f);
    addChild(pauseItem);

    auto resumeItem = TextButton::create("resume", [&](TextButton* button) {
        if (_audioID != AudioEngine::INVALID_AUDIO_ID)
        {
            AudioEngine::resume(_audioID);
        }
    });
    resumeItem->setPosition(layerSize.width * 0.7f, layerSize.height * 0.7f);
    addChild(resumeItem);

    auto loopItem = TextButton::create("enable-loop", [&](TextButton* button) {
        _loopEnabled = !_loopEnabled;

        if (_audioID != AudioEngine::INVALID_AUDIO_ID)
        {
            AudioEngine::setLoop(_audioID, _loopEnabled);
        }
        if (_loopEnabled)
        {
            button->setString("disable-loop");
        }
        else
        {
            button->setString("enable-loop");
        }
    });
    loopItem->setPosition(layerSize.width * 0.5f, layerSize.height * 0.5f);
    addChild(loopItem);

    auto volumeSlider = SliderEx::create();
    volumeSlider->setPercent(100);
    volumeSlider->addEventListener([&](Object* sender, Slider::EventType event) {
        SliderEx* slider = dynamic_cast<SliderEx*>(sender);
        _volume          = slider->getRatio();
        if (_audioID != AudioEngine::INVALID_AUDIO_ID)
        {
            AudioEngine::setVolume(_audioID, _volume);
        }
    });
    volumeSlider->setPosition(Vec2(layerSize.width * 0.5f, layerSize.height * 0.35f));
    addChild(volumeSlider);

    auto timeSlider = SliderEx::create();
    timeSlider->addEventListener([&](Object* sender, Slider::EventType event) {
        SliderEx* slider = dynamic_cast<SliderEx*>(sender);
        switch (event)
        {
        case Slider::EventType::ON_SLIDEBALL_DOWN:
            _updateTimeSlider = false;
            break;
        case Slider::EventType::ON_SLIDEBALL_UP:
            if (_audioID != AudioEngine::INVALID_AUDIO_ID && _duration != AudioEngine::TIME_UNKNOWN)
            {
                float ratio = (float)slider->getPercent() / 100;
                ratio       = clampf(ratio, 0.0f, 1.0f);
                AudioEngine::setCurrentTime(_audioID, _duration * ratio);
            }
        case Slider::EventType::ON_SLIDEBALL_CANCEL:
            _updateTimeSlider = true;
        case Slider::EventType::ON_PERCENTAGE_CHANGED:
        default:
            // ignore
            break;
        }
    });
    timeSlider->setPosition(Vec2(layerSize.width * 0.5f, layerSize.height * 0.25f));
    addChild(timeSlider);
    _timeSlider = timeSlider;

    auto& volumeSliderPos = volumeSlider->getPosition();
    auto& sliderSize      = volumeSlider->getContentSize();
    auto volumeLabel      = Label::createWithTTF("volume:  ", fontFilePath, 20);
    volumeLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
    volumeLabel->setPosition(volumeSliderPos.x - sliderSize.width / 2, volumeSliderPos.y);
    addChild(volumeLabel);

    auto& timeSliderPos = timeSlider->getPosition();
    auto timeLabel      = Label::createWithTTF("time:  ", fontFilePath, 20);
    timeLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_RIGHT);
    timeLabel->setPosition(timeSliderPos.x - sliderSize.width / 2, timeSliderPos.y);
    addChild(timeLabel);

    this->schedule(AX_CALLBACK_1(AudioControlTest::update, this), 0.1f, "update_key");

    return ret;
}

void AudioControlTest::update(float dt)
{
    if (_audioID != AudioEngine::INVALID_AUDIO_ID)
    {
        if (_duration == AudioEngine::TIME_UNKNOWN)
        {
            _duration = AudioEngine::getDuration(_audioID);
        }
        if (_duration != AudioEngine::TIME_UNKNOWN)
        {
            auto time  = AudioEngine::getCurrentTime(_audioID);
            _timeRatio = time / _duration;
            if (_updateTimeSlider)
            {
                ((SliderEx*)_timeSlider)->setRatio(_timeRatio);
            }
        }
    }
}

AudioControlTest::~AudioControlTest() {}

std::string AudioControlTest::title() const
{
    return "Audio control test";
}

bool AudioLoadTest::init()
{
    if (AudioEngineTestDemo::init())
    {
        auto& layerSize = this->getContentSize();

        auto stateLabel = Label::createWithTTF("status:", "fonts/arial.ttf", 30);
        stateLabel->setPosition(layerSize.width / 2, layerSize.height * 0.7f);
        addChild(stateLabel);

        auto preloadItem = TextButton::create("preload", [&, stateLabel](TextButton* button) {
            stateLabel->setString("status:loading...");
            auto isDestroyed = _isDestroyed;
            AudioEngine::preload("audio/SoundEffectsFX009/FX082.mp3", [isDestroyed, stateLabel](bool isSuccess) {
                if (*isDestroyed)
                {
                    AXLOGD("AudioLoadTest scene was destroyed, no need to set the label text.");
                    return;
                }

                if (isSuccess)
                {
                    stateLabel->setString("status:load success");
                }
                else
                {
                    stateLabel->setString("status:load fail");
                }
            });
        });
        preloadItem->setPosition(layerSize.width * 0.35f, layerSize.height * 0.5f);
        addChild(preloadItem);

        auto uncacheItem = TextButton::create("uncache", [&, stateLabel](TextButton* button) {
            stateLabel->setString("status:uncache");
            AudioEngine::uncache("audio/SoundEffectsFX009/FX082.mp3");
        });
        uncacheItem->setPosition(layerSize.width * 0.65f, layerSize.height * 0.5f);
        addChild(uncacheItem);

        return true;
    }

    return false;
}

std::string AudioLoadTest::title() const
{
    return "Audio preload/uncache test";
}

// AudioWavTest
AudioWavTest::~AudioWavTest() {}

std::string AudioWavTest::title() const
{
    return "Audio wav test";
}

bool AudioWavTest::init()
{
    if (AudioEngineTestDemo::init())
    {
        // list wav files, the wav samples download from wav wiki
        _wavFiles = FileUtils::getInstance()->listFiles("wav-samples");

        auto& layerSize = this->getContentSize();

        _stateLabel = Label::createWithTTF("wav file:", "fonts/arial.ttf", 30);
        _stateLabel->setPosition(layerSize.width / 2, layerSize.height * 0.7f);
        addChild(_stateLabel);

        auto playPrev = TextButton::create("Play Prev", [=](TextButton* button) {
            if (_curIndex > 0)
            {
                AudioEngine::stop(_audioID);
                _audioID = AudioEngine::play2d(_wavFiles[--_curIndex]);
                _stateLabel->setString(fmt::format("[index: {}] {}", _curIndex,
                                                           FileUtils::getFileShortName(_wavFiles[_curIndex])));
            }
        });
        playPrev->setPosition(layerSize.width * 0.35f, layerSize.height * 0.5f);
        addChild(playPrev);

        auto playNext = TextButton::create("Play Next", [=](TextButton* button) {
            if (_curIndex != -1 && _curIndex < (_wavFiles.size() - 1))
            {
                AudioEngine::stop(_audioID);
                _audioID = AudioEngine::play2d(_wavFiles[++_curIndex]);
                _stateLabel->setString(fmt::format("[index: {}] {}", _curIndex,
                                                           FileUtils::getFileShortName(_wavFiles[_curIndex])));
            }
        });
        playNext->setPosition(layerSize.width * 0.65f, layerSize.height * 0.5f);
        addChild(playNext);

        return true;
    }

    return false;
}

void AudioWavTest::onEnter()
{
    AudioEngineTestDemo::onEnter();

    if (!_wavFiles.empty())
    {
        _curIndex = 0;
        _audioID  = AudioEngine::play2d(_wavFiles[_curIndex]);
        _stateLabel->setString(fmt::format("[index: {}] {}", _curIndex,
                                                   FileUtils::getFileShortName(_wavFiles[_curIndex])));
    }
}

// PlaySimultaneouslyTest
bool PlaySimultaneouslyTest::init()
{
    auto ret = AudioEngineTestDemo::init();

    char text[36];
    int tmp = 81;
    for (int index = 0; index < TEST_COUNT; ++index)
    {
        sprintf(text, "audio/SoundEffectsFX009/FX0%d.mp3", tmp + index);
        _files[index] = text;
    }
    _playingcount = 0;

    auto playItem = TextButton::create("play-simultaneously", [&](TextButton* button) {
        int audioId;
        _playingcount = 0;
        button->setEnabled(false);
        auto startTime = utils::gettime();
        for (int index = 0; index < TEST_COUNT; ++index)
        {
            audioId = AudioEngine::play2d(_files[index]);
            if (audioId != AudioEngine::INVALID_AUDIO_ID)
            {
                _playingcount += 1;

                AudioEngine::setFinishCallback(audioId, [&](int id, std::string_view filePath) {
                    _playingcount -= 1;
                    if (_playingcount <= 0)
                    {
                        ((TextButton*)_playItem)->setEnabled(true);
                    }
                });
            }
            else
            {
                AXLOGD("{},{},Fail to play file:{}", __FILE__, __LINE__, _files[index]);
            }
        }
        AXLOGD("diff time:{}", utils::gettime() - startTime);
    });
    playItem->setPositionNormalized(Vec2(0.5f, 0.5f));
    this->addChild(playItem);
    _playItem = playItem;

    return ret;
}

PlaySimultaneouslyTest::~PlaySimultaneouslyTest() {}

std::string PlaySimultaneouslyTest::title() const
{
    return "Simultaneously play multiple audio";
}

// AudioProfileTest
bool AudioProfileTest::init()
{
    auto ret = AudioEngineTestDemo::init();

    char text[30];
    _files[0] = "background.mp3";
#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC
    _files[1] = "background.caf";
#else
    _files[1] = "background.ogg";
#endif

    std::string fontFilePath = "fonts/arial.ttf";
    _minDelay                = 1.0f;
    _time                    = 0.0f;

    _audioProfile.name         = "AudioProfileTest";
    _audioProfile.maxInstances = 3;
    _audioProfile.minDelay     = 1.0;

    Vec2 pos(0.5f, 0.7f);
    for (int index = 0; index < FILE_COUNT; ++index)
    {
        sprintf(text, "play %s", _files[index].c_str());

        auto playItem = TextButton::create(text, [&](TextButton* button) {
            int index = button->getTag();
            auto id   = AudioEngine::play2d(_files[index], false, 1.0f, &_audioProfile);
            if (id != AudioEngine::INVALID_AUDIO_ID)
            {
                _time = _minDelay;
                _audioCount += 1;
                char show[30];
                sprintf(show, "audio count:%d", _audioCount);
                _showLabel->setString(show);

                AudioEngine::setFinishCallback(id, [&](int id, std::string_view filePath) {
                    _audioCount -= 1;
                    char show[30];
                    sprintf(show, "audio count:%d", _audioCount);
                    _showLabel->setString(show);
                });
            }
        });
        playItem->setTag(index);
        playItem->setPositionNormalized(pos);
        this->addChild(playItem);
        pos.y -= 0.15f;
    }

    Vec2 origin = Director::getInstance()->getVisibleOrigin();
    Size size   = Director::getInstance()->getVisibleSize();

    auto profileInfoLabel =
        Label::createWithTTF("AudioProfile Info:\n    max instance:3  \n    minimum delay:1.0", fontFilePath, 12);
    profileInfoLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    profileInfoLabel->setPosition(Vec2(origin.x, origin.y + size.height * 0.65f));
    addChild(profileInfoLabel);

    _audioCount = 0;
    _showLabel  = Label::createWithTTF("audio count:0", fontFilePath, 12);
    _showLabel->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _showLabel->setPosition(Vec2(origin.x, origin.y + size.height * 0.5f));
    addChild(_showLabel);

    auto timeSlider = SliderEx::create();
    timeSlider->setEnabled(false);
    timeSlider->setPositionNormalized(pos);
    addChild(timeSlider);
    _timeSlider = timeSlider;

    this->schedule(AX_CALLBACK_1(AudioProfileTest::update, this), 0.05f, "update_key");

    return ret;
}

void AudioProfileTest::update(float dt)
{
    if (_time > 0.0f)
    {
        _time -= dt;
        ((SliderEx*)_timeSlider)->setRatio(_time / _minDelay);
    }
}

AudioProfileTest::~AudioProfileTest() {}

std::string AudioProfileTest::title() const
{
    return "AudioProfileTest";
}

std::string AudioProfileTest::subtitle() const
{
    return "See the console.";
}

// InvalidAudioFileTest
bool InvalidAudioFileTest::init()
{
    auto ret = AudioEngineTestDemo::init();

    auto playItem = TextButton::create("play unsupported media type", [&](TextButton* button) {
#if AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_MAC
        AudioEngine::play2d("background.ogg");
#elif AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || AX_TARGET_PLATFORM == AX_PLATFORM_WIN32
        AudioEngine::play2d("background.caf");
#endif
    });
    playItem->setPositionNormalized(Vec2(0.5f, 0.6f));
    this->addChild(playItem);

    auto playItem2 = TextButton::create("play not-existent file",
                                        [&](TextButton* button) { AudioEngine::play2d("not-existent file.mp3"); });
    playItem2->setPositionNormalized(Vec2(0.5f, 0.4f));
    this->addChild(playItem2);

    return ret;
}

InvalidAudioFileTest::~InvalidAudioFileTest() {}

std::string InvalidAudioFileTest::title() const
{
    return "Test invalid audio file";
}

std::string InvalidAudioFileTest::subtitle() const
{
    return "Not crash,please see the console.";
}

// LargeAudioFileTest
bool LargeAudioFileTest::init()
{
    auto ret = AudioEngineTestDemo::init();

    auto playItem = TextButton::create("play large audio file",
                                       [&](TextButton* button) { AudioEngine::play2d("audio/LuckyDay.mp3"); });
    playItem->setPositionNormalized(Vec2::ANCHOR_MIDDLE);
    this->addChild(playItem);

    return ret;
}

LargeAudioFileTest::~LargeAudioFileTest() {}

std::string LargeAudioFileTest::title() const
{
    return "Test large audio file";
}

bool AudioIssue18597Test::init()
{
    if (AudioEngineTestDemo::init())
    {
        auto& layerSize = this->getContentSize();

        // test case for https://github.com/cocos2d/cocos2d-x/issues/18597
        this->schedule(
            [=](float dt) {
                AXLOGD("issues 18597 audio crash test");
                for (int i = 0; i < 2; ++i)
                {
                    auto id = AudioEngine::play2d("audio/MUS_BGM_Battle_Round1_v1.caf", true, 1.0f);
                    this->runAction(Sequence::create(DelayTime::create(8.0f),
                                                     CallFunc::create([=]() { AudioEngine::stop(id); }), nullptr));
                }
            },
            2.0, 10000, 0.0, "audio test");
        // add label to show the side effect of "UnqueueBuffers Before alSourceStop"
        _time          = 0.0;
        auto labelTime = Label::createWithBMFont("fonts/bitmapFontTest2.fnt", "time: ");
        labelTime->setPosition(layerSize.width * 0.5f, layerSize.height * 0.5f);
        labelTime->setTag(999);
        this->addChild(labelTime);
        // update label quickly
        this->schedule(
            [=](float dt) {
                _time += dt;
                char timeString[20] = {0};
                sprintf(timeString, "Time %2.2f", _time);
                dynamic_cast<Label*>(this->getChildByTag(999))->setString(timeString);
            },
            0.05, 1000000, 0, "update label quickly");

        return true;
    }

    return false;
}

std::string AudioIssue18597Test::title() const
{
    return "Test for issue 18597";
}

std::string AudioIssue18597Test::subtitle() const
{
    return "no crash for more than 10 minutes";
}

bool AudioIssue11143Test::init()
{
    if (AudioEngineTestDemo::init())
    {
        auto& layerSize = this->getContentSize();

        auto playItem = TextButton::create("play", [](TextButton* button) {
            AudioEngine::play2d("audio/SoundEffectsFX009/FX082.mp3", true);
            AudioEngine::stopAll();

            auto audioId  = AudioEngine::play2d("audio/SoundEffectsFX009/FX082.mp3", true);
            char key[100] = {0};
            sprintf(key, "play another sound %d", audioId);
            button->scheduleOnce(
                [audioId](float dt) {
                    AudioEngine::stop(audioId);
                    AudioEngine::play2d("audio/SoundEffectsFX009/FX083.mp3");
                },
                0.3f, key);
        });
        playItem->setPosition(layerSize.width * 0.5f, layerSize.height * 0.5f);
        addChild(playItem);

        return true;
    }

    return false;
}

std::string AudioIssue11143Test::title() const
{
    return "Test for issue 11143";
}

std::string AudioIssue11143Test::subtitle() const
{
    return "2 seconds after first sound play,you should hear another sound.";
}

// Enable profiles for this file
#undef AX_PROFILER_DISPLAY_TIMERS
#define AX_PROFILER_DISPLAY_TIMERS() Profiler::getInstance()->displayTimers()
#undef AX_PROFILER_PURGE_ALL
#define AX_PROFILER_PURGE_ALL() Profiler::getInstance()->releaseAllTimers()

#undef AX_PROFILER_START
#define AX_PROFILER_START(__name__) ProfilingBeginTimingBlock(__name__)
#undef AX_PROFILER_STOP
#define AX_PROFILER_STOP(__name__) ProfilingEndTimingBlock(__name__)
#undef AX_PROFILER_RESET
#define AX_PROFILER_RESET(__name__) ProfilingResetTimingBlock(__name__)

#undef AX_PROFILER_START_CATEGORY
#define AX_PROFILER_START_CATEGORY(__cat__, __name__) \
    do                                                \
    {                                                 \
        if (__cat__)                                  \
            ProfilingBeginTimingBlock(__name__);      \
    } while (0)
#undef AX_PROFILER_STOP_CATEGORY
#define AX_PROFILER_STOP_CATEGORY(__cat__, __name__) \
    do                                               \
    {                                                \
        if (__cat__)                                 \
            ProfilingEndTimingBlock(__name__);       \
    } while (0)
#undef AX_PROFILER_RESET_CATEGORY
#define AX_PROFILER_RESET_CATEGORY(__cat__, __name__) \
    do                                                \
    {                                                 \
        if (__cat__)                                  \
            ProfilingResetTimingBlock(__name__);      \
    } while (0)

#undef AX_PROFILER_START_INSTANCE
#define AX_PROFILER_START_INSTANCE(__id__, __name__)                                                      \
    do                                                                                                    \
    {                                                                                                     \
        ProfilingBeginTimingBlock(String::createWithFormat("%08X - %s", __id__, __name__)->getCString()); \
    } while (0)
#undef AX_PROFILER_STOP_INSTANCE
#define AX_PROFILER_STOP_INSTANCE(__id__, __name__)                                                     \
    do                                                                                                  \
    {                                                                                                   \
        ProfilingEndTimingBlock(String::createWithFormat("%08X - %s", __id__, __name__)->getCString()); \
    } while (0)
#undef AX_PROFILER_RESET_INSTANCE
#define AX_PROFILER_RESET_INSTANCE(__id__, __name__)                                                      \
    do                                                                                                    \
    {                                                                                                     \
        ProfilingResetTimingBlock(String::createWithFormat("%08X - %s", __id__, __name__)->getCString()); \
    } while (0)

bool AudioPerformanceTest::init()
{
    if (AudioEngineTestDemo::init())
    {
        std::vector<std::string> audioFiles = {
            "audio/SoundEffectsFX009/FX081.mp3", "audio/SoundEffectsFX009/FX082.mp3",
            "audio/SoundEffectsFX009/FX083.mp3", "audio/SoundEffectsFX009/FX084.mp3",
            "audio/SoundEffectsFX009/FX085.mp3", "audio/SoundEffectsFX009/FX086.mp3",
            "audio/SoundEffectsFX009/FX087.mp3", "audio/SoundEffectsFX009/FX088.mp3",
            "audio/SoundEffectsFX009/FX089.mp3", "audio/SoundEffectsFX009/FX090.mp3"};

        for (const auto& audioFile : audioFiles)
        {
            AudioEngine::preload(audioFile);
        }

        auto& layerSize = this->getContentSize();

        auto playItem = TextButton::create("Start Test", [this, audioFiles](TextButton* button) {
            button->setEnabled(false);
            static_cast<TextButton*>(getChildByName("DisplayButton"))->setEnabled(true);

            unschedule("test");
            schedule(
                [audioFiles](float dt) {
                    int index = ax::random(0, (int)(audioFiles.size() - 1));
                    AX_PROFILER_START("play2d");
                    AudioEngine::play2d(audioFiles[index]);
                    AX_PROFILER_STOP("play2d");
                },
                0.25f, "test");
        });
        playItem->setPosition(layerSize.width * 0.5f, layerSize.height * 2 / 3);
        playItem->setName("PlayButton");
        addChild(playItem);

        auto displayItem = TextButton::create("Display Result", [this, playItem](TextButton* button) {
            unschedule("test");
            AudioEngine::stopAll();
            AX_PROFILER_DISPLAY_TIMERS();
            playItem->setEnabled(true);
            button->setEnabled(false);
        });
        displayItem->setEnabled(false);
        displayItem->setPosition(layerSize.width * 0.5f, layerSize.height / 3);
        displayItem->setName("DisplayButton");
        addChild(displayItem);

        return true;
    }

    return false;
}

std::string AudioPerformanceTest::title() const
{
    return "Test Performance of AudioEngine::play2d, audio is played 1 time per second";
}

std::string AudioPerformanceTest::subtitle() const
{
    return "Please see console for the result";
}

/////////////////////////////////////////////////////////////////////////

void AudioSwitchStateTest::onEnter()
{
    AudioEngineTestDemo::onEnter();

    //
    // !!!Notes:
    // a. No special reason, but we must play the tests audio at onEnter for this test suite only.
    // b. The latst audio test scene will stop all audio players,
    //   so if play any sound at init funciton, you can't herer
    // c. Please refer to AudioEngineTestDemo::onExit
    //
    AudioEngine::preload("audio/SoundEffectsFX009/FX081.mp3");
    AudioEngine::play2d("audio/SoundEffectsFX009/FX082.mp3");
    AudioEngine::play2d("audio/LuckyDay.mp3");
}

std::string AudioSwitchStateTest::title() const
{
    return "play, preload, stop switch test";
}

std::string AudioSwitchStateTest::subtitle() const
{
    return "Should not crash. No sound is ok";
}

/////////////////////////////////////////////////////////////////////////

void AudioSmallFileTest::onEnter()
{
    AudioEngineTestDemo::onEnter();

    //
    // !!!Notes:
    // a. No special reason, but we must play the tests audio at onEnter for this test suite only.
    // b. The latst audio test scene will stop all audio players,
    //   so if play any sound at init funciton, you can't hear it
    // c. Please refer to AudioEngineTestDemo::onExit
    //
    AudioEngine::play2d("audio/SmallFile.mp3");
}

std::string AudioSmallFileTest::title() const
{
    return "Playing small mp3 file";
}

std::string AudioSmallFileTest::subtitle() const
{
    return "Should not crash";
}

/////////////////////////////////////////////////////////////////////////
void AudioSmallFile2Test::onEnter()
{
    AudioEngineTestDemo::onEnter();

    //
    // !!!Notes:
    // a. No special reason, but we must play the tests audio at onEnter for this test suite only.
    // b. The latst audio test scene will stop all audio players,
    //   so if play any sound at init funciton, you can't hear it
    // c. Please refer to AudioEngineTestDemo::onExit
    //
    AudioEngine::play2d("audio/SmallFile2.mp3");
}

std::string AudioSmallFile2Test::title() const
{
    return "Play small mp3 file 2";
}

std::string AudioSmallFile2Test::subtitle() const
{
    return "Should not crash and should not have rasp!";
}

/////////////////////////////////////////////////////////////////////////
void AudioSmallFile3Test::onEnter()
{
    AudioEngineTestDemo::onEnter();

    //
    // !!!Notes:
    // a. No special reason, but we must play the tests audio at onEnter for this test suite only.
    // b. The latst audio test scene will stop all audio players,
    //   so if play any sound at init funciton, you can't hear it
    // c. Please refer to AudioEngineTestDemo::onExit
    //
    AudioEngine::play2d("audio/SmallFile3.mp3");
}

std::string AudioSmallFile3Test::title() const
{
    return "Play small mp3 file 3";
}

std::string AudioSmallFile3Test::subtitle() const
{
    return "Should not crash!";
}

/////////////////////////////////////////////////////////////////////////
void AudioPauseResumeAfterPlay::onEnter()
{
    AudioEngineTestDemo::onEnter();

    int audioId = AudioEngine::play2d("audio/SoundEffectsFX009/FX082.mp3");
    AudioEngine::pause(audioId);
    AudioEngine::resume(audioId);

    for (int i = 0; i < 10; ++i)
    {
        AudioEngine::pause(audioId);
        AudioEngine::resume(audioId);
    }
}

std::string AudioPauseResumeAfterPlay::title() const
{
    return "pause & resume right after play2d";
}

std::string AudioPauseResumeAfterPlay::subtitle() const
{
    return "Should not crash";
}

/////////////////////////////////////////////////////////////////////////
void AudioIssue16938Test::onEnter()
{
    AudioEngineTestDemo::onEnter();

    AudioEngine::play2d("audio/EntireFramesTest.mp3");
}

std::string AudioIssue16938Test::title() const
{
    return "Issue 16938 Test";
}

std::string AudioIssue16938Test::subtitle() const
{
    return "Should heard the entire audio frames";
}

/////////////////////////////////////////////////////////////////////////
void AudioPreloadSameFileMultipleTimes::onEnter()
{
    AudioEngineTestDemo::onEnter();

    for (int i = 0; i < 10; ++i)
    {
        AudioEngine::preload("audio/SoundEffectsFX009/FX082.mp3", [i](bool isSucceed) {
            AXLOGD("111: {} preload {}", i, isSucceed ? "succeed" : "failed");
            AudioEngine::preload("audio/SoundEffectsFX009/FX082.mp3", [i](bool isSucceed) {
                AXLOGD("222: {} preload {}", i, isSucceed ? "succeed" : "failed");
                AudioEngine::preload("audio/SoundEffectsFX009/FX082.mp3", [i](bool isSucceed) {
                    AXLOGD("333: {} preload {}", i, isSucceed ? "succeed" : "failed");
                });
            });
        });
    }
}

std::string AudioPreloadSameFileMultipleTimes::title() const
{
    return "Preload same file multiple times";
}

std::string AudioPreloadSameFileMultipleTimes::subtitle() const
{
    return "Should not crash";
}

void AudioPlayFileInWritablePath::onEnter()
{
    AudioEngineTestDemo::onEnter();

    auto fileUtils           = FileUtils::getInstance();
    std::string writablePath = fileUtils->getWritablePath();
    std::string musicFile    = "background.mp3";
    std::string saveFilePath = writablePath + "background_in_writable_dir.mp3";

    _oldSearchPaths = fileUtils->getOriginalSearchPaths();
    fileUtils->addSearchPath(writablePath, true);

    if (!fileUtils->isFileExist(saveFilePath))
    {
        Data data = fileUtils->getDataFromFile(musicFile);
        auto fp  = fopen(saveFilePath.c_str(), "wb");
        if (fp != nullptr)
        {
            fwrite(data.getBytes(), data.getSize(), 1, fp);
            fclose(fp);
        }
    }

    AudioEngine::play2d(saveFilePath);
}

void AudioPlayFileInWritablePath::onExit()
{
    AudioEngineTestDemo::onExit();

    FileUtils::getInstance()->setSearchPaths(_oldSearchPaths);
}

std::string AudioPlayFileInWritablePath::title() const
{
    return "Play audio in writable path";
}

std::string AudioPlayFileInWritablePath::subtitle() const
{
    return "Could play audio";
}

//
void AudioPlayInFinishedCB::onEnter()
{
    AudioEngineTestDemo::onEnter();

    auto item = MenuItemFont::create("Play 3 files one by one", [this](Object* sender) {
        playMusic("background.mp3");
        playMusic("background.mp3");
        playMusic("background.mp3");
    });

    item->setPosition(VisibleRect::center());

    auto menu = Menu::create(item, nullptr);
    menu->setPosition(Vec2::ANCHOR_BOTTOM_LEFT);
    addChild(menu);
}

void AudioPlayInFinishedCB::onExit()
{
    AudioEngineTestDemo::onExit();
}

std::string AudioPlayInFinishedCB::title() const
{
    return "Click menu item to play 3 audio files";
}

std::string AudioPlayInFinishedCB::subtitle() const
{
    return "After played over, click again, should also hear 3 audios";
}

void AudioPlayInFinishedCB::doPlay(std::string_view filename)
{
    int playID = AudioEngine::play2d(filename, false, 1);
    AudioEngine::setFinishCallback(playID, [this](int finishID, std::string_view file) {
        _playList.pop_front();
        AXLOGD("finish music {}", file);
        if (!_playList.empty())
        {
            std::string_view name = _playList.front();
            doPlay(name);
        }
    });
}

void AudioPlayInFinishedCB::playMusic(std::string_view filename)
{
    _playList.emplace_back(std::string{filename});
    if (_playList.size() == 1)
    {
        doPlay(filename);
    }
}

//
void AudioUncacheInFinishedCB::onEnter()
{
    AudioEngineTestDemo::onEnter();

    int id = AudioEngine::play2d("background.mp3");
    AudioEngine::setFinishCallback(id, [](int i, std::string_view str) { AudioEngine::uncacheAll(); });
}

void AudioUncacheInFinishedCB::onExit()
{
    AudioEngineTestDemo::onExit();
}

std::string AudioUncacheInFinishedCB::title() const
{
    return "UncacheAll in finshed callback";
}

std::string AudioUncacheInFinishedCB::subtitle() const
{
    return "Should not crash";
}
