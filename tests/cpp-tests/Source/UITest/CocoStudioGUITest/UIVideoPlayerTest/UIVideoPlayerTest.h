/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2019-present Simdsoft Limited.

 https://axmol.dev/

 SPDX-License-Identifier: MIT
 ****************************************************************************/

#ifndef __tests__VideoPlayerTest__
#define __tests__VideoPlayerTest__

#include "../UIScene.h"

DEFINE_TEST_SUITE(VideoPlayerTests);

class VideoPlayerTest : public UIScene
{
public:
    CREATE_FUNC(VideoPlayerTest);

    virtual bool init() override;

    void menuCloseCallback(ax::Object* sender);

    void menuRatioCallback(ax::Object* sender);
    void menuResourceVideoCallback(ax::Object* sender);
    void menuOnlineVideoCallback(ax::Object* sender);

    void menuFullScreenCallback(ax::Object* sender);
    void menuPauseCallback(ax::Object* sender);
    void menuResumeCallback(ax::Object* sender);
    void menuStopCallback(ax::Object* sender);
    void menuHintCallback(ax::Object* sender);
    void menuLoopCallback(ax::Object* sender);

    void sliderCallback(ax::Object* sender, ax::ui::Slider::EventType eventType);

    void videoEventCallback(ax::Object* sender, ax::ui::VideoPlayer::EventType eventType);

    void onEnter() override;
    void onExit() override;

private:
    void createVideo();
    void createSlider();

    ax::ui::VideoPlayer* _videoPlayer = nullptr;
    ax::Label* _videoStateLabel;
    ax::Label* _loopStatusLabel;
    ax::Rect _visibleRect;
};

class SimpleVideoPlayerTest : public UIScene
{
public:
    CREATE_FUNC(SimpleVideoPlayerTest);

    SimpleVideoPlayerTest();

    virtual bool init() override;

    void menuCloseCallback(ax::Object* sender);
    void switchStyleCallback(ax::Object* sender);
    void switchUserInputCallback(ax::Object* sender);

    void onEnter() override;
    void onExit() override;

private:
    void createVideo();

    ax::Rect _visibleRect;
    ax::ui::VideoPlayer* _videoPlayer = nullptr;

    ax::MenuItemFont* _switchUserInputEnabled;
    ax::MenuItemFont* _switchStyle;

    bool _userInputEnabled;
    ax::ui::VideoPlayer::StyleType _style;

    void updateButtonsTexts();
};

#endif  // __tests__VideoPlayerTest__
