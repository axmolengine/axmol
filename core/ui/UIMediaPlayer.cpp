/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
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

#include "ui/UIMediaPlayer.h"

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
            auto& viewSize = videoView->getContentSize();
            if (viewSize.x > 0 && viewSize.y > 0)
            {
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
}  // namespace

static std::unique_ptr<MediaEngineFactory> _meFactory = MediaEngineFactory::create();

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
#if AX_GLES_PROFILE != 200
                pvd->_vtexture->updateWithData(frame._dataPointer, frame._dataLen, PixelFormat::RG8, PixelFormat::RG8,
                                               bufferDim.x, bufferDim.y, false, 0);
#else
                pvd->_vtexture->updateWithData(frame._dataPointer, frame._dataLen, PixelFormat::LA8, PixelFormat::LA8,
                                               bufferDim.x, bufferDim.y, false, 0);
#endif
                pvd->_vchromaTexture->updateWithData(frame._dataPointer, frame._dataLen, PixelFormat::RGBA8,
                                                     PixelFormat::RGBA8, bufferDim.x >> 1, bufferDim.y, false, 0);
                break;
            }
            case MEVideoPixelFormat::NV12:
            {
#    if AX_GLES_PROFILE != 200
                pvd->_vtexture->updateWithData(frame._dataPointer, bufferDim.x * bufferDim.y, PixelFormat::R8,
                                               PixelFormat::R8, bufferDim.x, bufferDim.y, false, 0);
                pvd->_vchromaTexture->updateWithData(frame._cbcrDataPointer, (bufferDim.x * bufferDim.y) >> 1,
                                                     PixelFormat::RG8, PixelFormat::RG8, bufferDim.x >> 1,
                                                     bufferDim.y >> 1, false, 0);
#else
                pvd->_vtexture->updateWithData(frame._dataPointer, bufferDim.x * bufferDim.y, PixelFormat::A8,
                                               PixelFormat::A8, bufferDim.x, bufferDim.y, false, 0);
                pvd->_vchromaTexture->updateWithData(frame._cbcrDataPointer, (bufferDim.x * bufferDim.y) >> 1,
                                                     PixelFormat::LA8, PixelFormat::LA8, bufferDim.x >> 1,
                                                     bufferDim.y >> 1, false, 0);
#endif
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
        ax::log("Create MediaPlayer backend failed");
    }
}

MediaPlayer::~MediaPlayer()
{
    auto pvd = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);

    removeAllProtectedChildren();

    if (pvd->_engine)
        _meFactory->destroyMediaEngine(pvd->_engine);

    AX_SAFE_RELEASE(pvd->_vrender);
    AX_SAFE_RELEASE(pvd->_vtexture);
    AX_SAFE_RELEASE(pvd->_vchromaTexture);

    delete pvd;
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
    reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_originalViewSize = contentSize;
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
                break;
            default:
                engine->play();
            }
        }
    }
}

void MediaPlayer::pause()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
            engine->pause();
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
        }
    }
}

void MediaPlayer::stop()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
            engine->stop();
    }
}

void MediaPlayer::seekTo(float sec)
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_engine;
        if (engine)
            engine->setCurrentTime(sec);
    }
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

#endif
