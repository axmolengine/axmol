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

#include "ui/UIVideoPlayer.h"

// Now, common implementation based on redesigned MediaEngine is enable for windows and macOS
#if defined(_WIN32) || AX_TARGET_PLATFORM == AX_PLATFORM_MAC || AX_TARGET_PLATFORM == AX_TARGET_OS_TVOS
#    include <unordered_map>
#    include <stdlib.h>
#    include <string>
#    include "base/CCDirector.h"
#    include "base/CCEventListenerKeyboard.h"
#    include "platform/CCFileUtils.h"
#    include "ui/UIHelper.h"
#    include "media/MediaEngine.h"
#    include "yasio/detail/byte_buffer.hpp"
#    include "ui/LayoutHelper.h"
#    include "yasio/detail/sz.hpp"
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
    MediaEngine* _vplayer      = nullptr;
    Texture2D* _vtexture       = nullptr;
    Texture2D* _vchromaTexture = nullptr;
    Sprite* _vrender           = nullptr;

    MEVideoPixelDesc _vpixelDesc;

    Vec2 _originalViewSize;

    bool _scaleDirty = false;

    void closePlayer()
    {
        if (_vplayer)
            _vplayer->Close();
    }

    void rescaleTo(VideoPlayer* videoView)
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

static std::unique_ptr<MediaEngineFactory> _meFactory = CreatePlatformMediaEngineFactory();

VideoPlayer::VideoPlayer()
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
    pvd->_vplayer = _meFactory->CreateMediaEngine();
    if (pvd->_vplayer)
    {
        /// create video render sprite
        pvd->_vrender = new Sprite();
        pvd->_vrender->init();
        pvd->_vrender->setAutoUpdatePS(false);
        this->addProtectedChild(pvd->_vrender);
        /// setup media event callback
        pvd->_vplayer->SetMediaEventCallback([=](MEMediaEventType event) {
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
                onPlayEvent((int)EventType::STOPPED);
                break;

            /* Raised by a media source when a presentation ends. This event signals that all streams in the
            presentation are complete. The Media Session forwards this event to the application. */
            // case MEEndOfPresentation:
            //    onPlayEvent((int)EventType::COMPLETED);
            //    break;

            /* Raised by the Media Session when it has finished playing the last presentation in the playback queue.
             * We send complete event at this case
             */
            case MEMediaEventType::Completed:
                onPlayEvent((int)EventType::COMPLETED);
                break;
            case MEMediaEventType::Error:
                onPlayEvent((int)EventType::ERROR);
                break;
            }
        });
    }
    else
    {
        ax::log("Create VideoPlayer backend failed");
    }
}

VideoPlayer::~VideoPlayer()
{
    auto pvd = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);

    removeAllProtectedChildren();

    if (pvd->_vplayer)
        _meFactory->DestroyMediaEngine(pvd->_vplayer);

    AX_SAFE_RELEASE(pvd->_vrender);
    AX_SAFE_RELEASE(pvd->_vtexture);
    AX_SAFE_RELEASE(pvd->_vchromaTexture);

    delete pvd;
}

void VideoPlayer::setFileName(std::string_view fileName)
{
    auto fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
    fullPath.insert(fullPath.begin(), FILE_URL_SCHEME.begin(), FILE_URL_SCHEME.end());
    if (fullPath != _videoURL)
    {
        reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->closePlayer();
        _videoURL = std::move(fullPath);
    }
    _videoSource = VideoPlayer::Source::FILENAME;
}

void VideoPlayer::setURL(std::string_view videoUrl)
{
    if (_videoURL != videoUrl)
    {
        reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->closePlayer();
        _videoURL = videoUrl;
    }
    _videoSource = VideoPlayer::Source::URL;
}

void VideoPlayer::setLooping(bool looping)
{
    _isLooping = looping;

    auto pvd = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);
    if (pvd->_vplayer)
        pvd->_vplayer->SetLoop(looping);
}

void VideoPlayer::setUserInputEnabled(bool enableInput)
{
    _isUserInputEnabled = enableInput;
}

void VideoPlayer::setStyle(StyleType style)
{
    _styleType = style;
}

void VideoPlayer::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    ax::ui::Widget::draw(renderer, transform, flags);

    auto pvd     = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);
    auto vrender = pvd->_vrender;
    auto vplayer = pvd->_vplayer;
    if (!vrender || !vplayer)
        return;

    if (vrender->isVisible() && isPlaying())
    {  // render 1 video sample if avaiable

        vplayer->TransferVideoFrame([this, pvd](const ax::MEVideoFrame& frame){
            auto pixelFormat = frame._vpd._PF;
            auto bPixelDescChnaged         = !frame._vpd.equals(pvd->_vpixelDesc);
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
                case MEVideoPixelFormat::BGR32:
                    pvd->_vrender->setProgramState(backend::ProgramType::VIDEO_TEXTURE_BGR32);
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
                pvd->_vtexture->updateWithData(frame._dataPointer, frame._dataLen, PixelFormat::LA8,
                                               PixelFormat::LA8,
                                               bufferDim.x, bufferDim.y, false, 0);
                pvd->_vchromaTexture->updateWithData(frame._dataPointer, frame._dataLen, PixelFormat::RGBA8,
                                                     PixelFormat::RGBA8,
                                                     bufferDim.x >> 1, bufferDim.y, false, 0);
                break;
            }
            case MEVideoPixelFormat::NV12:
            {
                pvd->_vtexture->updateWithData(frame._dataPointer, bufferDim.x * bufferDim.y, PixelFormat::A8,
                                               PixelFormat::A8, bufferDim.x, bufferDim.y, false, 0);
                pvd->_vchromaTexture->updateWithData(frame._cbcrDataPointer, (bufferDim.x * bufferDim.y) >> 1,
                                                     PixelFormat::RG8,
                                                     PixelFormat::RG8, bufferDim.x >> 1, bufferDim.y >> 1, false, 0);
                break;
            }
            case MEVideoPixelFormat::RGB32:
            case MEVideoPixelFormat::BGR32:
                pvd->_vtexture->updateWithData(frame._dataPointer, frame._dataLen, PixelFormat::RGBA8,
                                               PixelFormat::RGBA8, bufferDim.x, bufferDim.y, false, 0);
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
    if (pvd->_scaleDirty || (flags & FLAGS_TRANSFORM_DIRTY))
        pvd->rescaleTo(this);

#    if AX_VIDEOPLAYER_DEBUG_DRAW
    _debugDrawNode->clear();
    auto size         = getContentSize();
    Point vertices[4] = {Point::ZERO, Point(size.width, 0), Point(size.width, size.height), Point(0, size.height)};
    _debugDrawNode->drawPoly(vertices, 4, true, Color4F(1.0, 1.0, 1.0, 1.0));
#    endif
}

void VideoPlayer::setContentSize(const Size& contentSize)
{
    Widget::setContentSize(contentSize);
    reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_originalViewSize = contentSize;
}

void VideoPlayer::setFullScreenEnabled(bool enabled)
{
    if (_fullScreenEnabled != enabled)
    {
        _fullScreenEnabled = enabled;

        auto pvd = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);
        Widget::setContentSize(enabled ? _director->getOpenGLView()->getDesignResolutionSize()
                                       : pvd->_originalViewSize);
    }
}

bool VideoPlayer::isFullScreenEnabled() const
{
    return _fullScreenEnabled;
}

void VideoPlayer::setKeepAspectRatioEnabled(bool enable)
{
    if (_keepAspectRatioEnabled != enable)
    {
        _keepAspectRatioEnabled                                               = enable;
        reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_scaleDirty = true;
    }
}

void VideoPlayer::setPlayRate(float fRate)
{
    if (!_videoURL.empty())
    {
        auto vplayer = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_vplayer;
        if (vplayer)
            vplayer->SetRate(fRate);
    }
}

void VideoPlayer::play()
{
    if (!_videoURL.empty())
    {
        auto vplayer = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_vplayer;
        if (vplayer)
        {
            switch (vplayer->GetState())
            {
            case MEMediaState::Closed:
                vplayer->SetAutoPlay(true);
                vplayer->Open(_videoURL);
                break;
            default:
                vplayer->Play();
            }
        }
    }
}

void VideoPlayer::pause()
{
    if (!_videoURL.empty())
    {
        auto vplayer = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_vplayer;
        if (vplayer)
            vplayer->Pause();
    }
}

void VideoPlayer::resume()
{
    if (!_videoURL.empty())
    {
        auto vplayer = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_vplayer;
        if (vplayer)
        {
            switch (vplayer->GetState())
            {
            case MEMediaState::Stopped:
            case MEMediaState::Paused:
                vplayer->Play();
            }
        }
    }
}

void VideoPlayer::stop()
{
    if (!_videoURL.empty())
    {
        auto vplayer = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_vplayer;
        if (vplayer)
            vplayer->Stop();
    }
}

void VideoPlayer::seekTo(float sec)
{
    if (!_videoURL.empty())
    {
        auto vplayer = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_vplayer;
        if (vplayer)
            vplayer->SetCurrentTime(sec);
    }
}

bool VideoPlayer::isPlaying() const
{
    return _isPlaying;
}

bool VideoPlayer::isLooping() const
{
    return _isLooping;
}

bool VideoPlayer::isUserInputEnabled() const
{
    return _isUserInputEnabled;
}

void VideoPlayer::setVisible(bool visible)
{
    ax::ui::Widget::setVisible(visible);
}

void VideoPlayer::onEnter()
{
    Widget::onEnter();
}

void VideoPlayer::onExit()
{
    _eventCallback = nullptr;
    Widget::onExit();
}

void VideoPlayer::addEventListener(const VideoPlayer::ccVideoPlayerCallback& callback)
{
    _eventCallback = callback;
}

void VideoPlayer::onPlayEvent(int event)
{
    _isPlaying = (event == (int)VideoPlayer::EventType::PLAYING);

    if (_eventCallback)
    {
        _director->getScheduler()->runOnAxmolThread(std::bind(_eventCallback, this, (VideoPlayer::EventType)event));
    }
}

ax::ui::Widget* VideoPlayer::createCloneInstance()
{
    return VideoPlayer::create();
}

void VideoPlayer::copySpecialProperties(Widget* widget)
{
    VideoPlayer* videoPlayer = dynamic_cast<VideoPlayer*>(widget);
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
