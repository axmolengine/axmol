/****************************************************************************
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2022 Bytedance Inc.

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

#ifndef __tests__VideoPlayerTest__
#define __tests__VideoPlayerTest__

#include "../UIScene.h"

DEFINE_TEST_SUITE(VideoPlayerTests);

class VideoPlayerTest : public UIScene
{
public:
    CREATE_FUNC(VideoPlayerTest);

    virtual bool init() override;

    void menuCloseCallback(ax::Ref* sender);

    void menuRatioCallback(ax::Ref* sender);
    void menuResourceVideoCallback(ax::Ref* sender);
    void menuOnlineVideoCallback(ax::Ref* sender);

    void menuFullScreenCallback(ax::Ref* sender);
    void menuPauseCallback(ax::Ref* sender);
    void menuResumeCallback(ax::Ref* sender);
    void menuStopCallback(ax::Ref* sender);
    void menuHintCallback(ax::Ref* sender);
    void menuLoopCallback(ax::Ref* sender);

    void sliderCallback(ax::Ref* sender, ax::ui::Slider::EventType eventType);

    void videoEventCallback(ax::Ref* sender, ax::ui::VideoPlayer::EventType eventType);

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

    void menuCloseCallback(ax::Ref* sender);
    void switchStyleCallback(ax::Ref* sender);
    void switchUserInputCallback(ax::Ref* sender);

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
