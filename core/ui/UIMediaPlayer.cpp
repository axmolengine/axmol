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

#include "ui/UIMediaPlayer.h"

#include "UILayout.h"


// Now, common implementation based on redesigned MediaEngine is enable for windows and macOS
#if defined(_WIN32) || defined(__APPLE__) || defined(__ANDROID__) || defined(AX_ENABLE_VLC_MEDIA)
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
#    include "yasio/byte_buffer.hpp"
//-----------------------------------------------------------------------------------------------------------

USING_NS_AX;

//-----------------------------------------------------------------------------------------------------------

#    define PS_SET_UNIFORM(ps, name, value)                       \
        do                                                        \
        {                                                         \
            decltype(value) __v = value;                          \
            auto __loc          = (ps)->getUniformLocation(name); \
            (ps)->setUniform(__loc, &__v, sizeof(__v));           \
        } while (false)

#    define PS_SET_UNIFORM_R(ps, name, value)               \
        do                                                  \
        {                                                   \
            auto __loc = (ps)->getUniformLocation(name);    \
            (ps)->setUniform(__loc, &value, sizeof(value)); \
        } while (false)

using namespace ax::ui;

namespace
{
struct PrivateVideoDescriptor
{
    MediaEngine* _engine      = nullptr;
    Texture2D* _vtexture       = nullptr;
    Texture2D* _vchromaTexture = nullptr;
    Sprite* _vrender           = nullptr;

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
                if(_vpixelDesc._rotation % 180 != 0)
                    std::swap(viewSize.x, viewSize.y);
                _vrender->setRotation(_vpixelDesc._rotation);

                if (!videoView->isKeepAspectRatioEnabled())
                {
                    _vrender->setScale(viewSize.x / videoSize.x, viewSize.y / videoSize.y);
                }
                else
                {
                    const auto aspectRatio = (std::min)(viewSize.x / videoSize.x, viewSize.y / (videoSize.y));

                    _vrender->setScale(aspectRatio);
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

const char* CIRCLE_IMAGE =
    "iVBORw0KGgoAAAANSUhEUgAAABAAAAAQCAYAAAAf8/"
    "9hAAAAB3RJTUUH6AQWDgQPSHxoUwAAAAlwSFlzAAAewQAAHsEBw2lUUwAAAARnQU1BAACxjwv8YQUAAACbSURBVHjaY2BAA////"
    "+cH4mogXgrEJ4B4LxDPBeICIGZnwAeACnyA+PJ/3ABkoDkuzbVA/PE/"
    "YfASiFPRNVsD8VsiNMPAPSCWgmlmBuKNJGiGgZkwA7zI0AwCX4FYDmRAFZkGgEAayIBFFBjQCzJgBQUGTIRFH0Ve8CVTMzwQKYtGihMSVZIyVTITWn"
    "au/A9JH4eA+DCUjTU7AwAEsTgY+YCLcQAAAABJRU5ErkJggg==";

const char* BODY_IMAGE_1_PIXEL_HEIGHT =
    "iVBORw0KGgoAAAANSUhEUgAAAAwAAAABCAMAAADdNb8LAAAAA1BMVEX///+nxBvIAAAACklEQVR4AWNABgAADQABYc2cpAAAAABJRU5ErkJggg==";

const char* CIRCLE_IMAGE_KEY              = "/__circle16x16Image";
const char* BODY_IMAGE_1_PIXEL_HEIGHT_KEY = "/__bodyImage";

constexpr auto TIMELINE_BAR_HEIGHT = 12.f;

}  // namespace

BasicMediaController::BasicMediaController(MediaPlayer* player)
    : MediaController(player)
{
}

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
    createControls();
}

void BasicMediaController::onPressStateChangedToPressed()
{
    _lastTouch = std::chrono::steady_clock::now();

    if (_controlPanel->getOpacity() == 255)
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
        }, 1.f, "__media_controller_fader"sv);
    }), nullptr));
}

void BasicMediaController::setContentSize(const Vec2& contentSize)
{
    Widget::setContentSize(contentSize);
    updateControlsForContentSize(contentSize);
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
    if (!_mediaPlayer)
        return;

    bool restrictedSize     = false;
    const auto& contentSize = getContentSize();
    if (contentSize.width < 600 || contentSize.height < 400)
    {
        restrictedSize = true;
    }

    auto state = _mediaPlayer->getState();
    if (state == MediaPlayer::MediaState::LOADING ||
        state == MediaPlayer::MediaState::CLOSED ||
        state == MediaPlayer::MediaState::ERROR)
    {
        _playButton->setVisible(false);
        _pauseButton->setVisible(false);
        _stopButton->setVisible(false);
        _fastForwardButton->setVisible(false);
        _fastRewindButton->setVisible(false);
        _timelineTotal->setVisible(false);
    }
    else
    {
        _fastForwardButton->setVisible(!restrictedSize);
        _fastRewindButton->setVisible(!restrictedSize);
        _timelineTotal->setVisible(true);

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

void BasicMediaController::createControls()
{
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

    auto primaryButtonPanel = Widget::create();
    primaryButtonPanel->setContentSize(Vec2(150, 60));
    primaryButtonPanel->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    primaryButtonPanel->setPositionNormalized(Vec2(0.5f, 0.5f));
    primaryButtonPanel->setScale(1 / scale);
    _controlPanel->addProtectedChild(primaryButtonPanel);

    _playButton = Button::create("");
    _playButton->addClickEventListener([this](Ref* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _playRate = 1.f;
        _mediaPlayer->setPlayRate(_playRate);
        _mediaPlayer->play();
        updateControllerState();
    });
    _playButton->setSwallowTouches(false);
    _playButton->setTitleLabel(Label::createWithSystemFont("\xe2\x8f\xb5", "Helvetica", 64));
    _playButton->setPositionNormalized(Vec2(0.25f, 0.5f));
    _playButton->setCascadeOpacityEnabled(true);
    _playButton->setVisible(false);
    primaryButtonPanel->addProtectedChild(_playButton, 1, -1);

    _stopButton = Button::create("");
    _stopButton->addClickEventListener([this](Ref* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _playRate = 1.f;
        _mediaPlayer->setPlayRate(_playRate);
        _mediaPlayer->stop();
        updateControllerState();
    });
    _stopButton->setSwallowTouches(false);
    _stopButton->setTitleLabel(Label::createWithSystemFont("\xe2\x8f\xb9", "Helvetica", 64));
    _stopButton->setPositionNormalized(Vec2(0.75f, 0.5f));
    _stopButton->setCascadeOpacityEnabled(true);
    _stopButton->setVisible(false);
    primaryButtonPanel->addProtectedChild(_stopButton, 1, -1);

    _pauseButton = Button::create("");
    _pauseButton->addClickEventListener([this](Ref* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _playRate = 1.f;
        _mediaPlayer->setPlayRate(_playRate);
        _mediaPlayer->pause();
        updateControllerState();
    });
    _pauseButton->setSwallowTouches(false);
    _pauseButton->setTitleLabel(Label::createWithSystemFont("\xe2\x8f\xb8", "Helvetica", 64));
    _pauseButton->setPositionNormalized(Vec2(0.25f, 0.5f));
    _pauseButton->setCascadeOpacityEnabled(true);
    _pauseButton->setVisible(false);
    primaryButtonPanel->addProtectedChild(_pauseButton, 1, -1);

    _fastForwardButton = Button::create("");
    _fastForwardButton->addClickEventListener([this](Ref* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        if (_playRate >= 4.f)
            return;

        _playRate *= 2;
        _mediaPlayer->setPlayRate(_playRate);
    });
    _fastForwardButton->setSwallowTouches(false);
    _fastForwardButton->setTitleLabel(Label::createWithSystemFont("\xe2\x8f\xa9", "Helvetica", 56));
    _fastForwardButton->setCascadeOpacityEnabled(true);
    _fastForwardButton->setPositionNormalized(Vec2(0.75f, 0.5f));
    _fastForwardButton->setVisible(false);
    _controlPanel->addProtectedChild(_fastForwardButton, 1, -1);

    _fastRewindButton = Button::create("");
    _fastRewindButton->addClickEventListener([this](Ref* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        if (_playRate <= (1.f / 4.f))
            return;

        _playRate /= 2;
        _mediaPlayer->setPlayRate(_playRate);
    });
    _fastRewindButton->setSwallowTouches(false);
    _fastRewindButton->setTitleLabel(Label::createWithSystemFont("\xe2\x8f\xaa", "Helvetica", 56));
    _fastRewindButton->setCascadeOpacityEnabled(true);
    _fastRewindButton->setPositionNormalized(Vec2(0.25f, 0.5f));
    _fastRewindButton->setVisible(false);
    _controlPanel->addProtectedChild(_fastRewindButton, 1, -1);

    _timelineTotal = utils::createSpriteFromBase64Cached(BODY_IMAGE_1_PIXEL_HEIGHT, BODY_IMAGE_1_PIXEL_HEIGHT_KEY);
    _timelineTotal->setAnchorPoint(Vec2::ANCHOR_MIDDLE_BOTTOM);
    _timelineTotal->setStretchEnabled(true);
    _timelineTotal->setPositionNormalized(Vec2(0.5f, 0.1f));
    _timelineTotal->setColor(Color3B::GRAY);
    _timelineTotal->setVisible(false);
    _timelineTotal->setCascadeOpacityEnabled(true);
    _timelineTotal->setContentSize(Size(contentSize.width - 40, TIMELINE_BAR_HEIGHT / scale));
    _controlPanel->addProtectedChild(_timelineTotal, 1);

    _timelinePlayed = utils::createSpriteFromBase64Cached(BODY_IMAGE_1_PIXEL_HEIGHT, BODY_IMAGE_1_PIXEL_HEIGHT_KEY);
    _timelinePlayed->setAnchorPoint(Vec2::ANCHOR_MIDDLE_LEFT);
    _timelinePlayed->setStretchEnabled(true);
    _timelinePlayed->setPositionNormalized(Vec2(0.0f, 0.5f));
    _timelinePlayed->setColor(Color3B::WHITE);
    _timelinePlayed->setCascadeOpacityEnabled(true);
    _timelineTotal->addChild(_timelinePlayed, 5);

    _timelineSelector = utils::createSpriteFromBase64Cached(CIRCLE_IMAGE, CIRCLE_IMAGE_KEY);
    _timelineSelector->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
    _timelineSelector->setPositionNormalized(Vec2(1.f, 0.5f));
    _timelineSelector->setCascadeOpacityEnabled(true);
    _timelineSelector->setStretchEnabled(true);
    _timelineSelector->setContentSize(Size(TIMELINE_BAR_HEIGHT * 1.5f, TIMELINE_BAR_HEIGHT * 1.5f) / scale);
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
}

void BasicMediaController::updateControlsGlobalZ(float globalZOrder)
{
    _controlPanel->setGlobalZOrder(globalZOrder);
    _timelineTotal->setGlobalZOrder(globalZOrder);
    _timelinePlayed->setGlobalZOrder(globalZOrder);
    _timelineSelector->setGlobalZOrder(globalZOrder);
}

void BasicMediaController::updateControls()
{
    if (_mediaPlayer)
    {
        const auto currentTime = _mediaPlayer->getCurrentTime();
        const auto duration    = _mediaPlayer->getDuration();
        auto& totalSize        = _timelineTotal->getContentSize();
        _timelinePlayed->setContentSize(Size(totalSize.width * (currentTime / duration), totalSize.height));
    }
}

void BasicMediaController::updateControlsForContentSize(const Vec2& contentSize)
{
    auto scale = Director::getInstance()->getGLView()->getScaleY();

    _mediaOverlay->setContentSize(contentSize);
    _controlPanel->setContentSize(contentSize);

    _timelineTotal->setContentSize(Size(contentSize.width - 40, TIMELINE_BAR_HEIGHT / scale));
}


MediaPlayer::MediaPlayer()
    : _fullScreenDirty(false)
    , _fullScreenEnabled(false)
    , _keepAspectRatioEnabled(false)
    , _videoPlayerIndex(-1)
    , _eventCallback(nullptr)
    , _isPlaying(false)
    , _isLooping(false)
    , _isUserInputEnabled(true)
    , _styleType(StyleType::DEFAULT)
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
        pvd->_engine->setCallbacks([this, pvd](MEMediaEventType event) {
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
                onPlayEvent(pvd->_engine->isPlaybackEnded() ? (int) EventType::COMPLETED : (int) EventType::STOPPED);
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
        }, [this, pvd](const ax::MEVideoFrame& frame) {
            auto pixelFormat       = frame._vpd._PF;
            auto bPixelDescChnaged = !frame._vpd.equals(pvd->_vpixelDesc);
            if (bPixelDescChnaged)
            {
                pvd->_vpixelDesc = frame._vpd;

                AX_SAFE_RELEASE(pvd->_vtexture);
                pvd->_vtexture = new Texture2D();  // deault is Sampler Filter is: LINEAR

                AX_SAFE_RELEASE_NULL(pvd->_vchromaTexture);
                if (pixelFormat >= MEVideoPixelFormat::YUY2)
                {  // use separated texture we can set differrent sample filter
                    pvd->_vchromaTexture = new Texture2D();  // Sampler Filter: NEAREST
                    pvd->_vchromaTexture->setAliasTexParameters();
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

    delete pvd;
}

bool MediaPlayer::init()
{
    if (!Widget::init())
    {
        return false;
    }

    if (_controllerEnabled)
    {
        setMediaController(BasicMediaController::create(this));
    }

    return true;
}

void MediaPlayer::setFileName(std::string_view fileName)
{
    auto fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
    if (ax::path2uri(fullPath) != _videoURL)
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
    _isUserInputEnabled = enableInput;
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
    auto engine = pvd->_engine;
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
    auto videoContext = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);
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

void MediaPlayer::setMediaControllerEnabled(bool enable)
{
    _controllerEnabled = enable;
    if (_mediaController)
    {
        _mediaController->setEnabled(_controllerEnabled);
    }
    else if (_controllerEnabled)
    {
        setMediaController(BasicMediaController::create(this));
    }
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
        _mediaController->setEnabled(_controllerEnabled);
        addProtectedChild(_mediaController, 1);
    }
}

void MediaPlayer::setFullScreenEnabled(bool enabled)
{
    if (_fullScreenEnabled != enabled)
    {
        _fullScreenEnabled = enabled;

        auto pvd = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);
        Widget::setContentSize(enabled ? _director->getGLView()->getDesignResolutionSize()
                                       : pvd->_originalViewSize);
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
    return _isUserInputEnabled;
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
        _isUserInputEnabled     = videoPlayer->_isUserInputEnabled;
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
    if (!_controllerEnabled || !_mediaController)
    {
        return;
    }

    _mediaController->updateControllerState();
}

#endif
