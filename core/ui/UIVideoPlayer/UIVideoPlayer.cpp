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

#include "ui/UIVideoPlayer/UIVideoPlayer.h"

#if defined(_WIN32)
#    include <unordered_map>
#    include <stdlib.h>
#    include <string>
#    include "base/CCDirector.h"
#    include "base/CCEventListenerKeyboard.h"
#    include "platform/CCFileUtils.h"
#    include "ui/UIHelper.h"
#    include "media/MediaEngine.h"
#    include "yasio/detail/byte_buffer.hpp"
#    include "ntcvt/ntcvt.hpp"
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

using namespace ax::ui;

namespace
{
struct PrivateVideoDescriptor
{
    MediaEngine* _vplayer = nullptr;
    Texture2D* _vtexture  = nullptr;
    Sprite* _vrender      = nullptr;

    yasio::byte_buffer _sampleBuffer;

    bool _scaleDirty = false;

    VideoExtent _vextent;
    VideoSampleFormat _sampleFormat = VideoSampleFormat::NONE;

    bool updateExtent(const VideoExtent& extent)
    {
        bool updated = !this->_vextent.equals(extent);
        if (updated)
            this->_vextent = extent;
        return updated;
    }

    bool updateSampleFormat(VideoSampleFormat sampleFormat)
    {
        bool updated = sampleFormat != this->_sampleFormat;
        if (updated)
            this->_sampleFormat = sampleFormat;
        return updated;
    }

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
                    const Vec2 originalScale{static_cast<float>(_vextent.cx) / _vtexture->getPixelsWide(),
                                             static_cast<float>(_vextent.cy) / _vtexture->getPixelsHigh()};

                    const auto aspectRatio =
                        (std::min)(viewSize.x / videoSize.x, viewSize.y / (videoSize.y * originalScale.y));

                    _vrender->setScale(originalScale.x * aspectRatio, originalScale.y * aspectRatio);
                }

                LayoutHelper::centerNode(_vrender);

                _vrender->setVisible(true);
            }
            else
                _vrender->setVisible(false);
        }

        _scaleDirty = false;
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
        pvd->_vplayer->SetMediaEventCallback([=](MediaEventType event) {
            switch (event)
            {
            case MediaEventType::PLAYING:
                if (!isPlaying())
                    onPlayEvent((int)EventType::PLAYING);
                break;

            case MediaEventType::PAUSED:
                onPlayEvent((int)EventType::PAUSED);
                break;

            case MediaEventType::STOPPED:
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
            case MediaEventType::COMPLETED:
                onPlayEvent((int)EventType::COMPLETED);
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
    auto pvd = (PrivateVideoDescriptor*)_videoContext;

    removeAllProtectedChildren();

    if (pvd->_vplayer)
        _meFactory->DestroyMediaEngine(pvd->_vplayer);

    if (pvd->_vrender)
        pvd->_vrender->release();
    if (pvd->_vtexture)
        pvd->_vtexture->release();

    delete pvd;
}

void VideoPlayer::setFileName(std::string_view fileName)
{
    auto fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
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

    auto pvd = (PrivateVideoDescriptor*)_videoContext;
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

    auto pvd     = (PrivateVideoDescriptor*)_videoContext;  //
    auto vrender = pvd->_vrender;
    auto vplayer = pvd->_vplayer;
    if (!vrender || !vplayer)
        return;

    if (vrender->isVisible() && isPlaying() && vplayer->GetLastVideoFrame(pvd->_sampleBuffer))
    {
        bool extentChanged       = pvd->updateExtent(vplayer->GetVideoExtent());
        bool sampleFormatChanged = pvd->updateSampleFormat(vplayer->GetVideoSampleFormat());

        auto sampleFormat = pvd->_sampleFormat;

        uint8_t* sampleData  = pvd->_sampleBuffer.data();
        size_t sampleDataLen = pvd->_sampleBuffer.size();

        auto rWidth  = pvd->_vextent.cx;
        auto rHeight = pvd->_vextent.cy;

        Vec2 uvScale{1.0f, 1.0f};

        bool needsRecreateTexture = extentChanged || sampleFormatChanged;
        if (needsRecreateTexture)
        {
            if (pvd->_vtexture)
                pvd->_vtexture->release();
            pvd->_vtexture = new Texture2D();

            auto programManager = ProgramManager::getInstance();

            switch (sampleFormat)
            {
            case VideoSampleFormat::YUY2:
                pvd->_vrender->setProgramState(backend::ProgramType::VIDEO_TEXTURE_YUY2);
                break;
            case VideoSampleFormat::NV12:
                pvd->_vrender->setProgramState(backend::ProgramType::VIDEO_TEXTURE_NV12);
                break;
            case VideoSampleFormat::BGR32:
                pvd->_vrender->setProgramState(backend::ProgramType::VIDEO_TEXTURE_BGR32);
                break;
            default:
                pvd->_vrender->setProgramState(backend::ProgramType::VIDEO_TEXTURE_RGB32);
            }
        }

        switch (sampleFormat)
        {
        case VideoSampleFormat::NV12:
        {
            /* For single sampler */
            int texelWidth  = YASIO_SZ_ALIGN(rWidth, 16);
            int texelHeight = pvd->_vplayer->IsH264() ? YASIO_SZ_ALIGN(rHeight, 16) * 3 / 2 : rHeight * 3 / 2;
            uvScale.x       = rWidth / (float)texelWidth;
            uvScale.y       = rHeight / (float)texelHeight;
            pvd->_vtexture->updateWithData(sampleData, sampleDataLen, PixelFormat::L8, PixelFormat::L8, texelWidth,
                                           texelHeight, false);

            /* For dual sampler */
            // const int ySampleSize = rWidth * rHeight;
            // pvd->_vtexture->updateWithData(sampleData, ySampleSize, PixelFormat::L8, PixelFormat::L8, rWidth,
            //                                rHeight, false, 0);
            // pvd->_vtexture->updateWithData(sampleData + ySampleSize, sampleDataLen - ySampleSize, PixelFormat::LA8,
            //                               PixelFormat::LA8, rWidth >> 1, rHeight >> 1, false, 1);
            break;
        }
        case VideoSampleFormat::YUY2:
        {
            int texelWidth = pvd->_vplayer->IsH264() ? (YASIO_SZ_ALIGN(rWidth, 16)) : (rWidth);
            uvScale.x      = (float)rWidth / texelWidth;

            /* For single sampler */
            // pvd->_vtexture->updateWithData(sampleData, sampleDataLen, PixelFormat::RGBA8, PixelFormat::RGBA8,
            //                               texelWidth >> 1, rHeight, false, 0);

            /* For dual sampler */
            pvd->_vtexture->updateWithData(sampleData, sampleDataLen, PixelFormat::LA8, PixelFormat::LA8, texelWidth,
                                           rHeight, false, 0);
            pvd->_vtexture->updateWithData(sampleData, sampleDataLen, PixelFormat::RGBA8, PixelFormat::RGBA8,
                                           texelWidth >> 1, rHeight, false, 1);
            break;
        }
        case VideoSampleFormat::RGB32:
        case VideoSampleFormat::BGR32:
            pvd->_vtexture->updateWithData(sampleData, sampleDataLen, PixelFormat::RGBA8, PixelFormat::RGBA8, rWidth,
                                           rHeight, false, 0);
            break;
        default:;
        }
        if (needsRecreateTexture)
        {
            pvd->_vrender->setTexture(pvd->_vtexture);
            pvd->_vrender->setTextureRect(ax::Rect{Vec2::ZERO, pvd->_vtexture->getContentSize()});

            if (sampleFormat == VideoSampleFormat::NV12 || sampleFormat == VideoSampleFormat::YUY2)
            {
                auto ps = pvd->_vrender->getProgramState();
                PS_SET_UNIFORM(ps, "out_w", (float)rWidth);
                PS_SET_UNIFORM(ps, "uv_scale", uvScale);
            }

            pvd->_scaleDirty = true;
        }
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
    // reinterpret_cast<PrivateVideoDescriptor*>(_videoContext)->_originalViewSize = contentSize;
}

void VideoPlayer::setFullScreenEnabled(bool enabled)
{
    if (_fullScreenEnabled != enabled)
    {
        _fullScreenEnabled = enabled;

        auto pvd = reinterpret_cast<PrivateVideoDescriptor*>(_videoContext);
        // Widget::setContentSize(enabled ? _director->getOpenGLView()->getFrameSize() : pvd->_originalViewSize);
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
            case MediaState::Closed:
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
            case MediaState::Stopped:
            case MediaState::Paused:
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
