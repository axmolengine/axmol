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

#include "ui/UIMediaPlayer.h"

// Now, common implementation based on redesigned MediaEngine is enable for windows and macOS
#if defined(AX_ENABLE_MEDIA)
#    include <unordered_map>
#    include <stdlib.h>
#    include <string>
#    include "base/Director.h"
#    include "base/EventListenerKeyboard.h"
#    include "platform/FileUtils.h"
#    include "ui/UIHelper.h"
#    include "media/MediaEngine.h"
#    include "ui/LayoutHelper.h"
#    include "UIButton.h"
#    include "UILayout.h"
#    include "yasio/byte_buffer.hpp"
//-----------------------------------------------------------------------------------------------------------

USING_NS_AX;

//-----------------------------------------------------------------------------------------------------------

#    define PS_SET_UNIFORM(ps, name, value)                         \
        do                                                          \
        {                                                           \
            decltype(value) __v = value;                            \
            auto __loc          = (ps) -> getUniformLocation(name); \
            (ps)->setUniform(__loc, &__v, sizeof(__v));             \
        } while (false)

#    define PS_SET_UNIFORM_R(ps, name, value)               \
        do                                                  \
        {                                                   \
            auto __loc = (ps) -> getUniformLocation(name);  \
            (ps)->setUniform(__loc, &value, sizeof(value)); \
        } while (false)

using namespace ax::ui;

namespace
{
struct PrivateVideoDescriptor
{
    MediaEngine* _engine        = nullptr;
    Texture2D* _vtexture        = nullptr;
    Texture2D* _vchromaTexture  = nullptr;
    Texture2D* _vchroma2Texture = nullptr;
    Sprite* _vrender            = nullptr;

    MEVideoPixelDesc _vpixelDesc;

    Vec2 _originalViewSize;

    bool _scaleDirty = false;

    void closePlayer()
    {
        if (_engine)
            _engine->close();
    }

    void rescaleTo(MediaPlayer* videoView)
    {
        auto& videoSize = _vrender->getContentSize();
        if (videoSize.x > 0 && videoSize.y > 0)
        {  // rescale video to view node
            auto viewSize = videoView->getContentSize();
            if (viewSize.x > 0 && viewSize.y > 0)
            {
                if (_vpixelDesc._rotation % 180 != 0)
                    std::swap(viewSize.x, viewSize.y);
                _vrender->setRotation(_vpixelDesc._rotation);

                if (!videoView->isKeepAspectRatioEnabled())
                {
                    const auto scale = Vec2(viewSize.x / videoSize.x, viewSize.y / videoSize.y);
                    _vrender->setScale(scale.x, scale.y);

                    auto* mediaController = videoView->getMediaController();
                    if (mediaController)
                    {
                        if (mediaController->getOrientation() == MediaController::Orientation::RotatedLeft)
                        {
                            mediaController->setContentSize(videoSize *
                                                            Vec2(viewSize.y / videoSize.x, viewSize.x / videoSize.y));
                            mediaController->setRotation(-90);
                        }
                        else if (mediaController->getOrientation() == MediaController::Orientation::RotatedRight)
                        {
                            mediaController->setContentSize(videoSize *
                                                            Vec2(viewSize.y / videoSize.x, viewSize.x / videoSize.y));
                            mediaController->setRotation(90);
                        }
                        else
                        {
                            mediaController->setContentSize(videoSize * scale);
                        }
                    }
                }
                else
                {
                    const auto aspectRatio = (std::min)(viewSize.x / videoSize.x, viewSize.y / (videoSize.y));

                    _vrender->setScale(aspectRatio);

                    auto* mediaController = videoView->getMediaController();
                    if (mediaController)
                    {
                        if (mediaController->getOrientation() == MediaController::Orientation::RotatedLeft)
                        {
                            mediaController->setContentSize(Vec2(videoSize.y * aspectRatio, videoSize.x * aspectRatio));
                            mediaController->setRotation(-90);
                        }
                        else if (mediaController->getOrientation() == MediaController::Orientation::RotatedRight)
                        {
                            mediaController->setContentSize(Vec2(videoSize.y * aspectRatio, videoSize.x * aspectRatio));
                            mediaController->setRotation(90);
                        }
                        else
                        {
                            mediaController->setContentSize(videoSize * aspectRatio);
                        }
                    }
                }

                LayoutHelper::centerNode(_vrender);

                _vrender->setVisible(true);
            }
            else
                _vrender->setVisible(false);
        }

        _scaleDirty = false;
    }

    static void updateColorTransform(backend::ProgramState* ps, bool bFullColorRange)
    {
        // clang-format off
        // 1.16438356 ~= 255/219.0
        const Mat4 colorTransform = bFullColorRange ? Mat4{ // 709Scaled
            1.16438356f,   0.00000000f,    1.79265225f,     0.0f,
            1.16438356f,  -0.213237017f, - 0.533004045f,    0.0f,
            1.16438356f,   2.11241937f,    0.00000000f,     0.0f,
            0.0627451017f, 0.501960814f,   0.501960814f,    0.0f // YUVOffset8Bits: 16/255.0f, 128/255.0f, 128/255.0f
        } : Mat4 { // 709Unscaled
            1.000000f,  0.0000000f,       1.57472198f,      0.0f,
		    1.000000f, -0.187314089f,     -0.46820747f,     0.0f,
		    1.000000f,  1.85561536f,      0.0000000f,       0.0f,
		    0.0627451f, 0.5019608f,       0.50196081f,      0.0f
        };

        // clang-format on
        PS_SET_UNIFORM_R(ps, "colorTransform", colorTransform);
    }
};

std::unique_ptr<MediaEngineFactory> _meFactory = MediaEngineFactory::create();

const char* BODY_IMAGE_1_PIXEL_HEIGHT =
    "iVBORw0KGgoAAAANSUhEUgAAAAwAAAABCAMAAADdNb8LAAAAA1BMVEX///+nxBvIAAAACklEQVR4AWNABgAADQABYc2cpAAAAABJRU5ErkJggg==";

const char* BODY_IMAGE_1_PIXEL_HEIGHT_KEY = "/__bodyImage";

constexpr auto TIMELINE_BAR_HEIGHT = 12.f;

RefPtr<Texture2D> g_mediaControlsTexture = nullptr;

enum class MediaControlButtonId
{
    Play,
    Stop,
    Pause,
    EnterFullscreen,
    ExitFullscreen,
    TimelineSliderButton
};

std::map<MediaControlButtonId, Rect> g_mediaControlTextureRegions;

void createMediaControlTexture()
{
    if (g_mediaControlsTexture)
        return;

    constexpr auto panelW = 64.f;
    constexpr auto panelH = 64.f;
    constexpr auto iconW  = 32.f;
    constexpr auto iconH  = 32.f;
    constexpr auto gap    = 10.f;
    constexpr auto border = 2;

    auto* drawNode = DrawNode::create();

    auto DrawStop = [&](const Vec2& middle) -> void {
        auto s = Vec2(middle.x - iconW / 2.f, middle.y + iconH / 2.f);
        drawNode->drawSolidRect(s, s + Vec2(iconW, -iconH), Color4F::WHITE);
    };

    auto DrawPlay = [&](const Vec2& middle) -> void {
        auto p1 = Vec2(middle.x - iconW / 2.f, middle.y + iconH / 2.f);
        auto p2 = Vec2(middle.x + iconW / 2.f, middle.y);
        auto p3 = Vec2(middle.x - iconW / 2.f, middle.y - iconH / 2.f);

        drawNode->drawTriangle(p1, p2, p3, Color4B::WHITE);
    };

    auto DrawPause = [&](const Vec2& middle) -> void {
        auto start = Vec2(middle.x - 3, middle.y + iconH / 2.f);
        drawNode->drawSolidRect(start, start + Vec2(-6, -iconH), Color4B::WHITE);

        start = Vec2(middle.x + 3, middle.y + iconH / 2.f);
        drawNode->drawSolidRect(start, start + Vec2(6, -iconH), Color4B::WHITE);
    };

    auto DrawEnterFullscreen = [&](const Vec2& middle) -> void {
        auto topLeft     = Vec2(middle.x - panelW / 2.f + 6, middle.y + panelH / 2.f - 6);
        auto topRight    = Vec2(middle.x + panelW / 2.f - 6, middle.y + panelH / 2.f - 6);
        auto bottomLeft  = Vec2(middle.x - panelW / 2.f + 6, middle.y - panelH / 2.f + 6);
        auto bottomRight = Vec2(middle.x + panelW / 2.f - 6, middle.y - panelH / 2.f + 6);

        // Top left
        drawNode->drawSolidRect(topLeft, topLeft + Vec2(20, -6), Color4B::WHITE);
        drawNode->drawSolidRect(topLeft, topLeft + Vec2(6, -20), Color4B::WHITE);

        // Top right
        drawNode->drawSolidRect(topRight, topRight + Vec2(-20, -6), Color4B::WHITE);
        drawNode->drawSolidRect(topRight, topRight + Vec2(-6, -20), Color4B::WHITE);

        // Bottom left
        drawNode->drawSolidRect(bottomLeft, bottomLeft + Vec2(20, 6), Color4B::WHITE);
        drawNode->drawSolidRect(bottomLeft, bottomLeft + Vec2(6, 20), Color4B::WHITE);

        // Bottom right
        drawNode->drawSolidRect(bottomRight, bottomRight + Vec2(-20, 6), Color4B::WHITE);
        drawNode->drawSolidRect(bottomRight, bottomRight + Vec2(-6, 20), Color4B::WHITE);
    };

    auto DrawExitFullScreen = [&](const Vec2& middle) -> void {
        auto topLeft     = Vec2(middle.x - 4, middle.y + 4);
        auto topRight    = Vec2(middle.x + 4, middle.y + 4);
        auto bottomLeft  = Vec2(middle.x - 4, middle.y - 4);
        auto bottomRight = Vec2(middle.x + 4, middle.y - 4);

        // Top left
        drawNode->drawSolidRect(topLeft, topLeft + Vec2(-20, 6), Color4B::WHITE);
        drawNode->drawSolidRect(topLeft, topLeft + Vec2(-6, 20), Color4B::WHITE);

        // Top right
        drawNode->drawSolidRect(topRight, topRight + Vec2(20, 6), Color4B::WHITE);
        drawNode->drawSolidRect(topRight, topRight + Vec2(6, 20), Color4B::WHITE);

        // Bottom left
        drawNode->drawSolidRect(bottomLeft, bottomLeft + Vec2(-20, -6), Color4B::WHITE);
        drawNode->drawSolidRect(bottomLeft, bottomLeft + Vec2(-6, -20), Color4B::WHITE);

        // Bottom right
        drawNode->drawSolidRect(bottomRight, bottomRight + Vec2(20, -6), Color4B::WHITE);
        drawNode->drawSolidRect(bottomRight, bottomRight + Vec2(6, -20), Color4B::WHITE);
    };

    auto DrawSliderControlButton = [&](const Vec2& middle) -> void {
        drawNode->drawSolidCircle(middle, panelW / 2, 0, 180, Color4B::WHITE);
    };

    std::map<MediaControlButtonId, std::function<void(const Vec2&)>> items = {
        {MediaControlButtonId::Play, DrawPlay},
        {MediaControlButtonId::Stop, DrawStop},
        {MediaControlButtonId::Pause, DrawPause},
        {MediaControlButtonId::EnterFullscreen, DrawEnterFullscreen},
        {MediaControlButtonId::ExitFullscreen, DrawExitFullScreen},
        {MediaControlButtonId::TimelineSliderButton, DrawSliderControlButton}};

    auto numItems    = static_cast<int>(items.size());
    auto totalWidth  = utils::nextPOT(numItems * panelW + (numItems - 1) * gap + (border * 2));
    auto totalHeight = utils::nextPOT(border * 2 + panelH);
    auto imageSize   = Size(static_cast<float>(totalWidth), static_cast<float>(totalHeight));
    auto* node       = Node::create();
    node->setContentSize(imageSize);
    node->setIgnoreAnchorPointForPosition(false);
    node->setAnchorPoint(Vec2::ANCHOR_BOTTOM_LEFT);
    node->setPosition(0, 0);
    node->addChild(drawNode);

    auto* rt = RenderTexture::create(totalWidth, totalHeight, PixelFormat::RGBA8, true);
    rt->beginWithClear(0, 0, 0, 0);

    g_mediaControlTextureRegions.clear();

    int i = 0;
    for (auto&& item : items)
    {
        auto midPoint =
            Vec2(border + (i * panelW) + (i * gap) + (panelW / 2.f), imageSize.height - border - (panelH / 2.f));
        item.second(midPoint);

#    if defined(AX_USE_GL)
        g_mediaControlTextureRegions[item.first] =
            Rect(border + (panelW * i) + (gap * i), imageSize.height - border - panelH, panelW, panelH);
#    else  // For Metal renderer
        g_mediaControlTextureRegions[item.first] = Rect(border + (panelW * i) + (gap * i), border, panelW, panelH);
#    endif

        ++i;
    }

    node->visit();
    rt->end();
    Director::getInstance()->getRenderer()->render();

    g_mediaControlsTexture = rt->getSprite()->getTexture();
}

}  // namespace

static const float ZOOM_ACTION_TIME_STEP = 0.05f;

void MediaController::setOrientation(Orientation orientation)
{
    _orientation = orientation;
}

MediaPlayerControl* MediaPlayerControl::create(SpriteFrame* frame)
{
    auto* widget = new MediaPlayerControl();
    if (widget->init(frame))
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

MediaPlayerControl::~MediaPlayerControl()
{
    AX_SAFE_RELEASE(_overlay);
}

bool MediaPlayerControl::init(SpriteFrame* frame)
{
    if (!Button::init(""))
    {
        return false;
    }

    if (frame)
    {
        _overlay = Sprite::createWithSpriteFrame(frame);
        AX_SAFE_RETAIN(_overlay);
        auto spriteSize = _overlay->getContentSize();
        setContentSize(spriteSize);
        _overlay->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        _overlay->setPosition(_contentSize.width * 0.5f, _contentSize.height * 0.5f);
        addProtectedChild(_overlay, -2, -1);

        if (!_ignoreSize && _customSize.equals(Vec2::ZERO))
        {
            _customSize = _overlay->getContentSize();
        }
        this->updateChildrenDisplayedRGBA();
        if (_unifySize)
        {
            if (!_scale9Enabled)
            {
                updateContentSizeWithTextureSize(spriteSize);
            }
        }
        else
        {
            updateContentSizeWithTextureSize(spriteSize);
        }
    }

    return true;
}

void MediaPlayerControl::onSizeChanged()
{
    Button::onSizeChanged();
    if (_overlay)
    {
        _overlay->setPosition(_contentSize.width * 0.5f, _contentSize.height * 0.5f);
    }
}

Vec2 MediaPlayerControl::getVirtualRendererSize() const
{
    if (_unifySize)
    {
        return this->getNormalSize();
    }

    if (nullptr != _overlay)
    {
        Vec2 overlaySize = _overlay->getContentSize();
        if (!_normalTextureLoaded)
        {
            return overlaySize;
        }
    }
    return _normalTextureSize;
}

Vec2 MediaPlayerControl::getNormalSize() const
{
    if (_overlay)
    {
        return _overlay->getContentSize();
    }

    return Button::getNormalSize();
}

void MediaPlayerControl::onPressStateChangedToNormal()
{
    Button::onPressStateChangedToNormal();

    if (nullptr != _overlay)
    {
        _overlay->stopAllActions();
        if (_unifySize)
        {
            Action* zoomTitleAction = ScaleTo::create(ZOOM_ACTION_TIME_STEP, 1.0f, 1.0f);
            _overlay->runAction(zoomTitleAction);
        }
        else
        {
            _overlay->setScaleX(1.0f);
            _overlay->setScaleY(1.0f);
        }
    }
}

void MediaPlayerControl::onPressStateChangedToPressed()
{
    Button::onPressStateChangedToPressed();
    if (nullptr != _overlay)
    {
        _overlay->stopAllActions();
        Action* zoomTitleAction = ScaleTo::create(ZOOM_ACTION_TIME_STEP, 1.0f + _zoomScale, 1.0f + _zoomScale);
        _overlay->runAction(zoomTitleAction);
    }
}

void MediaPlayerControl::onPressStateChangedToDisabled()
{
    Button::onPressStateChangedToDisabled();
    if (nullptr != _overlay)
    {
        _overlay->setScale(1.0);
    }
}

BasicMediaController::BasicMediaController(MediaPlayer* player)
    : MediaController(player), _timelineBarHeight(TIMELINE_BAR_HEIGHT)
{}

BasicMediaController* BasicMediaController::create(MediaPlayer* mediaPlayer)
{
    auto* widget = new BasicMediaController(mediaPlayer);
    if (widget->init())
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

bool BasicMediaController::init()
{
    if (!Widget::init())
    {
        return false;
    }

    setTouchEnabled(true);
    setCascadeOpacityEnabled(false);
    updateControllerState();
    if (_mediaPlayer)
    {
        setContentSize(_mediaPlayer->getContentSize());
    }
    return true;
}

void BasicMediaController::initRenderer()
{
    Widget::initRenderer();

    // scheduleOnce is used to create the controls on the next update
    // loop. This is a work-around for a RenderTexture issue
    // when being created such places as a button click event handler
    // on Apple platforms/Metal renderer backend
    scheduleOnce(
        [this](float) {
        createControls();
        updateControlsForContentSize(_mediaPlayer->getContentSize());
        updateControllerState();
    },
        0.f, "__create_video_controls"sv);
}

void BasicMediaController::onPressStateChangedToPressed()
{
    _lastTouch = std::chrono::steady_clock::now();

    if (!_controlsReady || _controlPanel->getOpacity() == 255)
    {
        return;
    }

    updateControllerState();

    _mediaOverlay->runAction(Sequence::create(FadeTo::create(0.5f, 150), nullptr));
    _controlPanel->runAction(Sequence::create(FadeIn::create(0.5f), CallFunc::create([this] {
        if (_controlPanel->isScheduled("__media_controller_fader"sv))
            return;

        _controlPanel->schedule(
            [this](float) {
            auto now       = std::chrono::steady_clock::now();
            auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastTouch);
            if (deltaTime > std::chrono::milliseconds{2500})
            {
                _controlPanel->unschedule("__media_controller_fader"sv);
                _controlPanel->runAction(Sequence::create(FadeOut::create(0.5f), nullptr));
                _mediaOverlay->runAction(Sequence::create(FadeOut::create(0.5f), nullptr));
            }
        },
            1.f, "__media_controller_fader"sv);
    }),
                                              nullptr));
}

void BasicMediaController::setContentSize(const Vec2& contentSize)
{
    Widget::setContentSize(contentSize);
    updateControlsForContentSize(contentSize);
    updateControllerState();
}

void BasicMediaController::update(float delta)
{
    Widget::update(delta);
    updateControls();
}

void BasicMediaController::onEnter()
{
    Widget::onEnter();
    scheduleUpdate();
}

void BasicMediaController::setGlobalZOrder(float globalZOrder)
{
    Widget::setGlobalZOrder(globalZOrder);
    updateControlsGlobalZ(globalZOrder);
}

void BasicMediaController::updateControllerState()
{
    if (!_mediaPlayer || !_controlsReady)
        return;

    auto state = _mediaPlayer->getState();
    if (state == MediaPlayer::MediaState::LOADING || state == MediaPlayer::MediaState::CLOSED ||
        state == MediaPlayer::MediaState::ERROR)
    {
        _playButton->setVisible(false);
        _pauseButton->setVisible(false);
        _stopButton->setVisible(false);
        _timelineTotal->setVisible(false);
        _fullScreenExitButton->setVisible(false);
        _fullScreenEnterButton->setVisible(false);
    }
    else
    {
        _timelineTotal->setVisible(true);
        _fullScreenExitButton->setVisible(_mediaPlayer->isFullScreenEnabled());
        _fullScreenEnterButton->setVisible(!_mediaPlayer->isFullScreenEnabled());

        switch (state)
        {
        case MediaPlayer::MediaState::PLAYING:
            _playButton->setVisible(false);
            _pauseButton->setVisible(true);
            _stopButton->setVisible(true);
            break;
        case MediaPlayer::MediaState::PAUSED:
            _playButton->setVisible(true);
            _pauseButton->setVisible(false);
            _stopButton->setVisible(true);
            break;
        case MediaPlayer::MediaState::STOPPED:
        case MediaPlayer::MediaState::FINISHED:
            _playButton->setVisible(true);
            _pauseButton->setVisible(false);
            _stopButton->setVisible(false);
            break;
        default:;
        }
    }
}

void BasicMediaController::setTimelineBarHeight(float height)
{
    _timelineBarHeight = height;
    if (_timelineBarHeight < TIMELINE_BAR_HEIGHT)
        _timelineBarHeight = TIMELINE_BAR_HEIGHT;

    updateControlsForContentSize(getContentSize());
}

void BasicMediaController::createControls()
{
    createMediaControlTexture();

    // Check if controls are already created
    if (_controlsReady)
    {
        return;
    }

    const auto& contentSize = getContentSize();
    auto scale              = Director::getInstance()->getGLView()->getScaleY();

    _mediaOverlay = Layout::create();
    _mediaOverlay->setBackGroundColor(Color3B::BLACK);
    _mediaOverlay->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    _mediaOverlay->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _mediaOverlay->setPositionNormalized(Vec2(0.5f, 0.5f));
    _mediaOverlay->setContentSize(contentSize);
    _mediaOverlay->setOpacity(0);
    addProtectedChild(_mediaOverlay, 1);

    _controlPanel = Widget::create();
    _controlPanel->setContentSize(contentSize);
    _controlPanel->setPositionNormalized(Vec2(0.5f, 0.5f));
    _controlPanel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _controlPanel->setCascadeOpacityEnabled(true);
    _controlPanel->setOpacity(0);
    addProtectedChild(_controlPanel, 10);

    _primaryButtonPanel = Widget::create();
    _primaryButtonPanel->setContentSize(Vec2(150, 60));
    _primaryButtonPanel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _primaryButtonPanel->setPositionNormalized(Vec2(0.5f, 0.5f));
    _primaryButtonPanel->setScale(1 / scale);
    _controlPanel->addProtectedChild(_primaryButtonPanel);

    _playButton = MediaPlayerControl::create(SpriteFrame::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[MediaControlButtonId::Play]));
    _playButton->addClickEventListener([this](Object* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _playRate = 1.f;
        _mediaPlayer->setPlayRate(_playRate);
        _mediaPlayer->play();
        updateControllerState();
    });
    _playButton->setSwallowTouches(false);
    _playButton->setPositionNormalized(Vec2(0.25f, 0.5f));
    _playButton->setCascadeOpacityEnabled(true);
    _playButton->setVisible(false);
    _primaryButtonPanel->addProtectedChild(_playButton, 1, -1);

    _stopButton = MediaPlayerControl::create(SpriteFrame::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[MediaControlButtonId::Stop]));
    _stopButton->addClickEventListener([this](Object* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _playRate = 1.f;
        _mediaPlayer->setPlayRate(_playRate);
        _mediaPlayer->stop();
        updateControllerState();
    });
    _stopButton->setSwallowTouches(false);
    _stopButton->setPositionNormalized(Vec2(0.75f, 0.5f));
    _stopButton->setCascadeOpacityEnabled(true);
    _stopButton->setVisible(false);
    _primaryButtonPanel->addProtectedChild(_stopButton, 1, -1);

    _pauseButton = MediaPlayerControl::create(SpriteFrame::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[MediaControlButtonId::Pause]));
    _pauseButton->addClickEventListener([this](Object* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _playRate = 1.f;
        _mediaPlayer->setPlayRate(_playRate);
        _mediaPlayer->pause();
        updateControllerState();
    });
    _pauseButton->setSwallowTouches(false);
    _pauseButton->setPositionNormalized(Vec2(0.25f, 0.5f));
    _pauseButton->setCascadeOpacityEnabled(true);
    _pauseButton->setVisible(false);
    _primaryButtonPanel->addProtectedChild(_pauseButton, 1, -1);

    _fullScreenEnterButton = MediaPlayerControl::create(SpriteFrame::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[MediaControlButtonId::EnterFullscreen]));
    _fullScreenEnterButton->addClickEventListener([this](Object* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _mediaPlayer->setFullScreenEnabled(true);
        updateControllerState();
    });
    _fullScreenEnterButton->setSwallowTouches(false);
    _fullScreenEnterButton->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    _fullScreenEnterButton->setPositionNormalized(Vec2(0.03f, 0.97f));
    _fullScreenEnterButton->setCascadeOpacityEnabled(true);
    _fullScreenEnterButton->setVisible(false);
    _fullScreenEnterButton->setScale(1 / scale);
    _controlPanel->addProtectedChild(_fullScreenEnterButton, 1, -1);

    _fullScreenExitButton = MediaPlayerControl::create(SpriteFrame::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[MediaControlButtonId::ExitFullscreen]));
    _fullScreenExitButton->addClickEventListener([this](Object* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _mediaPlayer->setFullScreenEnabled(false);
        updateControllerState();
    });
    _fullScreenExitButton->setSwallowTouches(false);
    _fullScreenExitButton->setAnchorPoint(Vec2::ANCHOR_TOP_LEFT);
    _fullScreenExitButton->setPositionNormalized(Vec2(0.03f, 0.97f));
    _fullScreenExitButton->setCascadeOpacityEnabled(true);
    _fullScreenExitButton->setVisible(false);
    _fullScreenExitButton->setScale(1 / scale);
    _controlPanel->addProtectedChild(_fullScreenExitButton, 1, -1);

    _timelineTotal = utils::createSpriteFromBase64Cached(BODY_IMAGE_1_PIXEL_HEIGHT, BODY_IMAGE_1_PIXEL_HEIGHT_KEY);
    _timelineTotal->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    _timelineTotal->setStretchEnabled(true);
    _timelineTotal->setPositionNormalized(Vec2(0.5f, 0.1f));
    _timelineTotal->setColor(Color3B::GRAY);
    _timelineTotal->setVisible(false);
    _timelineTotal->setCascadeOpacityEnabled(true);
    _timelineTotal->setContentSize(Size(contentSize.width - 40, _timelineBarHeight / scale));
    _controlPanel->addProtectedChild(_timelineTotal, 1);

    _timelinePlayed = utils::createSpriteFromBase64Cached(BODY_IMAGE_1_PIXEL_HEIGHT, BODY_IMAGE_1_PIXEL_HEIGHT_KEY);
    _timelinePlayed->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _timelinePlayed->setStretchEnabled(true);
    _timelinePlayed->setPositionNormalized(Vec2(0.0f, 0.5f));
    _timelinePlayed->setColor(Color3B::WHITE);
    _timelinePlayed->setCascadeOpacityEnabled(true);
    _timelineTotal->addChild(_timelinePlayed, 5);

    _timelineSelector = Sprite::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[MediaControlButtonId::TimelineSliderButton]);
    _timelineSelector->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _timelineSelector->setPositionNormalized(Vec2(1.f, 0.5f));
    _timelineSelector->setCascadeOpacityEnabled(true);
    _timelineSelector->setStretchEnabled(true);
    _timelineSelector->setContentSize(Size(_timelineBarHeight, _timelineBarHeight) * 1.5f / scale);
    _timelineSelector->setVisible(false);
    _timelinePlayed->addChild(_timelineSelector, 10);

    _timelineTouchListener = EventListenerTouchOneByOne::create();
    _timelineTouchListener->setSwallowTouches(true);
    _timelineTouchListener->onTouchBegan = [this](ax::Touch* touch, ax::Event* event) -> bool {
        auto target               = event->getCurrentTarget();
        const auto locationInNode = target->convertToNodeSpace(touch->getLocation());
        const auto& size          = target->getContentSize();
        const auto rect           = ax::Rect(0, 0, size.width, size.height);

        if (rect.containsPoint(locationInNode))
        {
            auto percent  = locationInNode.x / rect.size.x;
            auto duration = _mediaPlayer->getDuration();
            auto newTime  = percent * duration;
            _mediaPlayer->seekTo(newTime);
            _timelineSelector->setVisible(true);
            return true;
        }

        return false;
    };
    _timelineTouchListener->onTouchMoved = [this](Touch* touch, Event* event) {
        auto target               = event->getCurrentTarget();
        const auto locationInNode = target->convertToNodeSpace(touch->getLocation());
        const auto& size          = target->getContentSize();
        const auto rect           = ax::Rect(0, 0, size.width, size.height);

        if (rect.containsPoint(locationInNode))
        {
            auto percent  = locationInNode.x / rect.size.x;
            auto duration = _mediaPlayer->getDuration();
            auto newTime  = percent * duration;
            _mediaPlayer->seekTo(newTime);
        }
    };
    _timelineTouchListener->onTouchEnded = [this](Touch* touch, Event* event) { _timelineSelector->setVisible(false); };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(_timelineTouchListener, _timelineTotal);

    _controlsReady = true;
}

void BasicMediaController::updateControlsGlobalZ(float globalZOrder)
{
    if (!_mediaPlayer || !_controlsReady)
        return;

    _controlPanel->setGlobalZOrder(globalZOrder);
    _timelineTotal->setGlobalZOrder(globalZOrder);
    _timelinePlayed->setGlobalZOrder(globalZOrder);
    _timelineSelector->setGlobalZOrder(globalZOrder);
}

void BasicMediaController::updateControls()
{
    if (_mediaPlayer && _controlsReady)
    {
        const auto currentTime = _mediaPlayer->getCurrentTime();
        const auto duration    = _mediaPlayer->getDuration();
        auto& totalSize        = _timelineTotal->getContentSize();
        _timelinePlayed->setContentSize(Size(totalSize.width * (currentTime / duration), totalSize.height));
    }
}

void BasicMediaController::updateControlsForContentSize(const Vec2& contentSize)
{
    if (!_controlsReady)
        return;

    _mediaOverlay->setContentSize(contentSize);
    _controlPanel->setContentSize(contentSize);

    auto scale = Director::getInstance()->getGLView()->getScaleY();
    _primaryButtonPanel->setScale(1 / scale);
    _timelineTotal->setContentSize(Size(contentSize.width - 40, _timelineBarHeight / scale));
    _timelineSelector->setContentSize(Size(_timelineBarHeight, _timelineBarHeight) * 1.5f / scale);
    _fullScreenEnterButton->setScale(1 / scale);
    _fullScreenExitButton->setScale(1 / scale);

    _fullScreenEnterButton->setPositionNormalized(Vec2());
    _fullScreenEnterButton->setPositionNormalized(Vec2(0.03f, 0.97f));
    _fullScreenExitButton->setPositionNormalized(Vec2());
    _fullScreenExitButton->setPositionNormalized(Vec2(0.03f, 0.97f));
}

MediaPlayer::MediaPlayer()
{
    auto pvd      = new PrivateVideoDescriptor{};
    _videoContext = pvd;
#    if AX_VIDEOPLAYER_DEBUG_DRAW
    _debugDrawNode = DrawNode::create();
    addChild(_debugDrawNode);
#    endif

    // Initialize mediaPlayer backend
    pvd->_engine = _meFactory->createMediaEngine();
    if (pvd->_engine)
    {
        /// create video render sprite
        pvd->_vrender = new Sprite();
        pvd->_vrender->init();
        pvd->_vrender->setAutoUpdatePS(false);
        this->addProtectedChild(pvd->_vrender);
        /// setup media event callback
        pvd->_engine->setCallbacks(
            [this, pvd](MEMediaEventType event) {
            switch (event)
            {
            case MEMediaEventType::Playing:
                if (!isPlaying())
                    onPlayEvent((int)EventType::PLAYING);
                break;

            case MEMediaEventType::Paused:
                onPlayEvent((int)EventType::PAUSED);
                break;

            case MEMediaEventType::Stopped:
                onPlayEvent(pvd->_engine->isPlaybackEnded() ? (int)EventType::COMPLETED : (int)EventType::STOPPED);
                break;

            /* Raised by a media source when a presentation ends. This event signals that all streams in the
            presentation are complete. The Media Session forwards this event to the application. */
            // case MEEndOfPresentation:
            //    onPlayEvent((int)EventType::COMPLETED);
            //    break;

            /* Raised by the Media Session when it has finished playing the last presentation in the playback queue.
             * We send complete event at this case
             */
            // case MEMediaEventType::Stopped:
            //     onPlayEvent((int)EventType::COMPLETED);
            //     break;
            case MEMediaEventType::Error:
                onPlayEvent((int)EventType::ERROR);
                break;
            }
        },
            [this, pvd](const ax::MEVideoFrame& frame) {
            auto pixelFormat       = frame._vpd._PF;
            auto bPixelDescChnaged = !frame._vpd.equals(pvd->_vpixelDesc);
            if (bPixelDescChnaged)
            {
                pvd->_vpixelDesc = frame._vpd;

                AX_SAFE_RELEASE(pvd->_vtexture);
                pvd->_vtexture = new Texture2D();  // deault is Sampler Filter is: LINEAR

                AX_SAFE_RELEASE_NULL(pvd->_vchromaTexture);
                AX_SAFE_RELEASE_NULL(pvd->_vchroma2Texture);
                if (pixelFormat >= MEVideoPixelFormat::YUY2)
                {  // use separated texture we can set differrent sample filter
                    pvd->_vchromaTexture = new Texture2D();  // Sampler Filter: NEAREST
                    pvd->_vchromaTexture->setAliasTexParameters();

                    if (pixelFormat == MEVideoPixelFormat::I420)
                    {
                        pvd->_vchroma2Texture = new Texture2D();  // Sampler Filter: NEAREST
                        pvd->_vchroma2Texture->setAliasTexParameters();
                    }
                }

                auto programManager = ProgramManager::getInstance();

                switch (pixelFormat)
                {
                case MEVideoPixelFormat::YUY2:
                    pvd->_vrender->setProgramState(backend::ProgramType::VIDEO_TEXTURE_YUY2);
                    break;
                case MEVideoPixelFormat::NV12:
                    pvd->_vrender->setProgramState(backend::ProgramType::VIDEO_TEXTURE_NV12);
                    break;
                case MEVideoPixelFormat::I420:
                    pvd->_vrender->setProgramState(backend::ProgramType::VIDEO_TEXTURE_I420);
                    break;
                default:
                    pvd->_vrender->setProgramState(backend::ProgramType::VIDEO_TEXTURE_RGB32);
                }
            }

            auto& bufferDim = frame._vpd._dim;

            switch (pixelFormat)
            {
            case MEVideoPixelFormat::YUY2:
            {
                pvd->_vtexture->updateWithData(frame._dataPointer, frame._dataLen, PixelFormat::RG8, PixelFormat::RG8,
                                               bufferDim.x, bufferDim.y, false, 0);
                pvd->_vchromaTexture->updateWithData(frame._dataPointer, frame._dataLen, PixelFormat::RGBA8,
                                                     PixelFormat::RGBA8, bufferDim.x >> 1, bufferDim.y, false, 0);
                break;
            }
            case MEVideoPixelFormat::NV12:
            {
                pvd->_vtexture->updateWithData(frame._dataPointer, bufferDim.x * bufferDim.y, PixelFormat::R8,
                                               PixelFormat::R8, bufferDim.x, bufferDim.y, false, 0);
                pvd->_vchromaTexture->updateWithData(frame._cbcrDataPointer, (bufferDim.x * bufferDim.y) >> 1,
                                                     PixelFormat::RG8, PixelFormat::RG8, bufferDim.x >> 1,
                                                     bufferDim.y >> 1, false, 0);
                break;
            }
            case MEVideoPixelFormat::I420:
            {
                pvd->_vtexture->updateWithData(frame._dataPointer, bufferDim.x * bufferDim.y, PixelFormat::R8,
                                               PixelFormat::R8, bufferDim.x, bufferDim.y, false, 0);
                const auto chromaTexDataSize = (bufferDim.x * bufferDim.y) >> 2;
                pvd->_vchromaTexture->updateWithData(frame._cbcrDataPointer, chromaTexDataSize, PixelFormat::R8,
                                                     PixelFormat::R8, bufferDim.x >> 1, bufferDim.y >> 1, false, 0);
                pvd->_vchroma2Texture->updateWithData(frame._cbcrDataPointer + chromaTexDataSize, chromaTexDataSize,
                                                      PixelFormat::R8, PixelFormat::R8, bufferDim.x >> 1,
                                                      bufferDim.y >> 1, false, 0);
                break;
            }
            case MEVideoPixelFormat::RGB32:
                pvd->_vtexture->updateWithData(frame._dataPointer, frame._dataLen, PixelFormat::RGBA8,
                                               PixelFormat::RGBA8, bufferDim.x, bufferDim.y, false, 0);
                break;
            case MEVideoPixelFormat::BGR32:
                pvd->_vtexture->updateWithData(frame._dataPointer, frame._dataLen, PixelFormat::BGRA8,
                                               PixelFormat::BGRA8, bufferDim.x, bufferDim.y, false, 0);
                break;
            default:;
            }
            if (bPixelDescChnaged)
            {
                pvd->_vrender->setTexture(pvd->_vtexture);
                pvd->_vrender->setTextureRect(ax::Rect{Vec2::ZERO, Vec2{
                                                                       frame._videoDim.x / AX_CONTENT_SCALE_FACTOR(),
                                                                       frame._videoDim.y / AX_CONTENT_SCALE_FACTOR(),
                                                                   }});

                if (pixelFormat >= MEVideoPixelFormat::YUY2)
                {
                    auto ps = pvd->_vrender->getProgramState();
                    PrivateVideoDescriptor::updateColorTransform(ps, frame._vpd._fullRange);

                    ps->setTexture(ps->getUniformLocation("u_tex1"), 1, pvd->_vchromaTexture->getBackendTexture());

                    if (pixelFormat == MEVideoPixelFormat::I420)
                        ps->setTexture(ps->getUniformLocation("u_tex2"), 2, pvd->_vchroma2Texture->getBackendTexture());
                }

                pvd->_scaleDirty = true;
            }
        });
    }
    else
    {
        AXLOGE("Create MediaPlayer backend failed");
    }
}

MediaPlayer::~MediaPlayer()
{
    auto pvd = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);

    removeAllProtectedChildren();

    AX_SAFE_RELEASE_NULL(_mediaController);

    if (pvd->_engine)
        _meFactory->destroyMediaEngine(pvd->_engine);

    AX_SAFE_RELEASE(pvd->_vrender);
    AX_SAFE_RELEASE(pvd->_vtexture);
    AX_SAFE_RELEASE(pvd->_vchromaTexture);
    AX_SAFE_RELEASE(pvd->_vchroma2Texture);

    if (g_mediaControlsTexture && g_mediaControlsTexture->getReferenceCount() == 1)
    {
        g_mediaControlsTexture = nullptr;
    }

    delete pvd;
}

bool MediaPlayer::init()
{
    if (!Widget::init())
    {
        return false;
    }

    if (_userInputEnabled)
    {
        setMediaController(BasicMediaController::create(this));
    }

    return true;
}

void MediaPlayer::setFileName(std::string_view fileName)
{
    auto fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
    if (ax::utils::filePathToUrl(std::forward<std::string>(fullPath)) != _videoURL)
    {
        reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->closePlayer();
        _videoURL = std::move(fullPath);
    }
    _videoSource = MediaPlayer::Source::FILENAME;
}

void MediaPlayer::setURL(std::string_view videoUrl)
{
    if (_videoURL != videoUrl)
    {
        reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->closePlayer();
        _videoURL = videoUrl;
    }
    _videoSource = MediaPlayer::Source::URL;
}

void MediaPlayer::setLooping(bool looping)
{
    _isLooping = looping;

    auto pvd = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);
    if (pvd->_engine)
        pvd->_engine->setLoop(looping);
}

void MediaPlayer::setUserInputEnabled(bool enableInput)
{
    _userInputEnabled = enableInput;
    if (_mediaController)
    {
        _mediaController->setEnabled(_userInputEnabled);
    }
    else if (_userInputEnabled)
    {
        setMediaController(BasicMediaController::create(this));
    }
}

void MediaPlayer::setStyle(StyleType style)
{
    _styleType = style;
}

void MediaPlayer::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    ax::ui::Widget::draw(renderer, transform, flags);

    auto pvd     = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);
    auto vrender = pvd->_vrender;
    auto engine  = pvd->_engine;
    if (!vrender || !engine)
        return;

    if (vrender->isVisible() && isPlaying())
    {  // render 1 video sample if avaiable
        engine->transferVideoFrame();
    }
    if (pvd->_scaleDirty || (flags & FLAGS_TRANSFORM_DIRTY))
        pvd->rescaleTo(this);

#    if AX_VIDEOPLAYER_DEBUG_DRAW
    _debugDrawNode->clear();
    auto size         = getContentSize();
    Point vertices[4] = {Point::ZERO, Point(size.width, 0), Point(size.width, size.height), Point(0, size.height)};
    _debugDrawNode->drawPoly(vertices, 4, true, Color4F(1.0, 1.0, 1.0, 1.0));
#    endif
}

void MediaPlayer::setContentSize(const Size& contentSize)
{
    Widget::setContentSize(contentSize);
    auto videoContext               = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);
    videoContext->_originalViewSize = contentSize;
    if (_mediaController)
    {
        _mediaController->setContentSize(contentSize);
    }
}

MediaPlayer::MediaState MediaPlayer::getState() const
{
    if (_videoURL.empty())
        return MediaState::CLOSED;

    auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
    if (engine)
    {
        switch (engine->getState())
        {
        case MEMediaState::Closed:
            return MediaState::CLOSED;
        case MEMediaState::Preparing:
            return MediaState::LOADING;
        case MEMediaState::Playing:
            return MediaState::PLAYING;
        case MEMediaState::Paused:
            return MediaState::PAUSED;
        case MEMediaState::Stopped:
            return MediaState::STOPPED;
        case MEMediaState::Error:
            return MediaState::ERROR;
        }
    }

    return MediaState::CLOSED;
}

void MediaPlayer::setMediaController(MediaController* controller)
{
    if (_mediaController)
    {
        removeProtectedChild(_mediaController, true);
        AX_SAFE_RELEASE(_mediaController);
    }
    _mediaController = controller;
    if (_mediaController)
    {
        AX_SAFE_RETAIN(_mediaController);
        _mediaController->setPositionNormalized(Vec2(0.5f, 0.5f));
        _mediaController->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
        _mediaController->setEnabled(_userInputEnabled);
        addProtectedChild(_mediaController, 1);
    }
}

void MediaPlayer::setFullScreenEnabled(bool enabled)
{
    if (_fullScreenEnabled != enabled)
    {
        _fullScreenEnabled = enabled;

        auto pvd               = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);
        const auto contentSize = enabled ? _director->getGLView()->getDesignResolutionSize() : pvd->_originalViewSize;
        Widget::setContentSize(contentSize);

        sendEvent((int)EventType::FULLSCREEN_SWITCH);
    }
}

bool MediaPlayer::isFullScreenEnabled() const
{
    return _fullScreenEnabled;
}

void MediaPlayer::setKeepAspectRatioEnabled(bool enable)
{
    if (_keepAspectRatioEnabled != enable)
    {
        _keepAspectRatioEnabled                                               = enable;
        reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_scaleDirty = true;
    }
}

void MediaPlayer::setPlayRate(float fRate)
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
            engine->setRate(fRate);
    }
}

void MediaPlayer::play()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
        {
            switch (engine->getState())
            {
            case MEMediaState::Closed:
                engine->setAutoPlay(true);
                engine->open(_videoURL);
                break;
            default:
                engine->play();
            }
            updateMediaController();
        }
    }
}

void MediaPlayer::pause()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
        {
            engine->pause();
            updateMediaController();
        }
    }
}

void MediaPlayer::resume()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
        {
            switch (engine->getState())
            {
            case MEMediaState::Stopped:
            case MEMediaState::Paused:
                engine->play();
            }

            updateMediaController();
        }
    }
}

void MediaPlayer::stop()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
        {
            engine->stop();
            updateMediaController();
        }
    }
}

void MediaPlayer::seekTo(float sec)
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
        {
            engine->setCurrentTime(sec);
            updateMediaController();
        }
    }
}

float MediaPlayer::getCurrentTime()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
        {
            return static_cast<float>(engine->getCurrentTime());
        }
    }

    return 0.f;
}

float MediaPlayer::getDuration()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
        {
            return static_cast<float>(engine->getDuration());
        }
    }

    return 0.f;
}

bool MediaPlayer::isPlaying() const
{
    return _isPlaying;
}

bool MediaPlayer::isLooping() const
{
    return _isLooping;
}

bool MediaPlayer::isUserInputEnabled() const
{
    return _userInputEnabled;
}

void MediaPlayer::setVisible(bool visible)
{
    ax::ui::Widget::setVisible(visible);
}

void MediaPlayer::onEnter()
{
    Widget::onEnter();
}

void MediaPlayer::onExit()
{
    _eventCallback = nullptr;
    Widget::onExit();
}

void MediaPlayer::addEventListener(const MediaPlayer::ccVideoPlayerCallback& callback)
{
    _eventCallback = callback;
}

void MediaPlayer::onPlayEvent(int event)
{
    _isPlaying = (event == (int)MediaPlayer::EventType::PLAYING);

    sendEvent(event);
}

void MediaPlayer::sendEvent(int event)
{
    if (_eventCallback)
    {
        _director->getScheduler()->runOnAxmolThread(std::bind(_eventCallback, this, (MediaPlayer::EventType)event));
    }
}

ax::ui::Widget* MediaPlayer::createCloneInstance()
{
    return MediaPlayer::create();
}

void MediaPlayer::copySpecialProperties(Widget* widget)
{
    MediaPlayer* videoPlayer = dynamic_cast<MediaPlayer*>(widget);
    if (videoPlayer)
    {
        _isPlaying              = videoPlayer->_isPlaying;
        _isLooping              = videoPlayer->_isLooping;
        _userInputEnabled       = videoPlayer->_userInputEnabled;
        _styleType              = videoPlayer->_styleType;
        _fullScreenEnabled      = videoPlayer->_fullScreenEnabled;
        _fullScreenDirty        = videoPlayer->_fullScreenDirty;
        _videoURL               = videoPlayer->_videoURL;
        _keepAspectRatioEnabled = videoPlayer->_keepAspectRatioEnabled;
        _videoSource            = videoPlayer->_videoSource;
        _videoPlayerIndex       = videoPlayer->_videoPlayerIndex;
        _eventCallback          = videoPlayer->_eventCallback;
    }
}

void MediaPlayer::updateMediaController()
{
    if (!_userInputEnabled || !_mediaController)
    {
        return;
    }

    _mediaController->updateControllerState();
}

#endif
