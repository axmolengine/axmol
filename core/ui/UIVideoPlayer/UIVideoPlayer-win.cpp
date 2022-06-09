/****************************************************************************
 Copyright (c) 2014-2016 Chukong Technologies Inc.
 Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2022 Bytedance Inc.

 https://adxeproject.github.io/

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

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#    include <unordered_map>
#    include <stdlib.h>
#    include <string>
#    include "base/CCDirector.h"
#    include "base/CCEventListenerKeyboard.h"
#    include "platform/CCFileUtils.h"
#    include "ui/UIHelper.h"
#    include "ui/UIVideoPlayer/MFMediaPlayer.h"
#    include "renderer/backend/ProgramCache.h"
#    include "yasio/detail/byte_buffer.hpp"
#    include "ntcvt/ntcvt.hpp"
#    include "ui/LayoutHelper.h"
#    include "yasio/detail/sz.hpp"
//-----------------------------------------------------------------------------------------------------------

USING_NS_CC;

//-----------------------------------------------------------------------------------------------------------

#    define PS_SET_UNIFORM(ps, name, value)                       \
        do                                                        \
        {                                                         \
            decltype(value) __v = value;                          \
            auto __loc          = (ps)->getUniformLocation(name); \
            (ps)->setUniform(__loc, &__v, sizeof(__v));           \
        } while (false)

using namespace cocos2d::ui;

namespace
{
/*
* refer to: https://github.com/doyoulikerock/D3D11NV12Rendering/blob/master/D3D11NV12Rendering/PixelShader.hlsl
// Converting 8-bit YUV to RGB888
static const float3x3 YUVtoRGBCoeffMatrix =
{
    1.164383,  1.164383, 1.164383,
    0.000000, -0.391762, 2.017232,
    1.596027, -0.812968, 0.000000
};
*/

// refer to:
// https://docs.microsoft.com/en-us/windows/win32/medfound/recommended-8-bit-yuv-formats-for-video-rendering#nv12
std::string_view NV12_FRAG = R"(
#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform sampler2D u_texture; // Y sample
uniform sampler2D u_texture1; // UV sample
uniform vec2 uv_scale;
uniform float out_w;

const mat3 YUVtoRGBCoeff = mat3(
    1.16438356, 1.16438356,  1.16438356,
    0.00000000, -0.213237017, 2.11241937,
    1.79265225, -0.533004045, 0.00000000
);

const vec3 YUVOffsets = vec3(0.0627451017, 0.501960814, 0.501960814);

vec3 YuvToRgb(vec3 YUV)
{
    YUV -= YUVOffsets;
    return YUVtoRGBCoeff * YUV;
}

void main()
{
    vec3 YUV;

    /* For dual sampler */
    //vec2 tXY = v_texCoord;
    //YUV.x = texture2D(u_texture, tXY).x;
    //tXY.y += 0.015625; // why needs adjust 1.0/64 ?
    //YUV.yz = texture2D(u_texture1, tXY).xw;

    /* For single sampler */
    vec2 tXY = v_texCoord * uv_scale;
    YUV.x = texture2D(u_texture, tXY).x;
    
    tXY.y *= 0.5;
    tXY.y += 2.0 / 3.0;
    
    float UVOffs = floor(v_texCoord.x * out_w / 2.0) * 2;
    float UPos = ((UVOffs * uv_scale.x) + 0.5) / out_w;
    float VPos = ((UVOffs * uv_scale.x) + 1.5) / out_w;
    
    YUV.y = texture2D(u_texture, vec2(UPos, tXY.y)).x;
    YUV.z = texture2D(u_texture, vec2(VPos, tXY.y)).x;

    /* Convert YUV to RGB */
    vec4 OutColor;
    OutColor.xyz = YuvToRgb(YUV);
    OutColor.w = 1.0;

    gl_FragColor = v_fragmentColor * OutColor;
}
)"sv;

// refer to:
// https://docs.microsoft.com/en-us/windows/win32/medfound/recommended-8-bit-yuv-formats-for-video-rendering#yuy2
std::string_view YUY2_FRAG = R"(

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform sampler2D u_texture; // Y sample
uniform sampler2D u_texture1; // UV sample
uniform vec2 uv_scale;
uniform float out_w; // texture width

const mat3 YUVtoRGBCoeff = mat3(
    1.16438356, 1.16438356,  1.16438356,
    0.00000000, -0.213237017, 2.11241937,
    1.79265225, -0.533004045, 0.00000000
);

const vec3 YUVOffsets = vec3(0.0627451017, 0.501960814, 0.501960814);

vec3 YuvToRgb(vec3 YUV)
{
    YUV -= YUVOffsets;
    return YUVtoRGBCoeff * YUV;
}

void main()
{
    vec2 tXY = v_texCoord * uv_scale;

    vec3 YUV;
    
    /* For dual sampler */
    YUV.yz = texture2D(u_texture1, tXY).yw;
    YUV.x = texture2D(u_texture, tXY).x;
	
    /* For single sampler */
    //YUV.yz = texture2D(u_texture, tXY).yw;
    //
    //vec4 YUY2P = texture2D(u_texture, tXY);
    //float Pos = v_texCoord.x * out_w;
    //YUV.x = floor(mod(Pos, 2.0)) == 0.0 ? YUY2P.z : YUY2P.x;

    /* Convert YUV to RGB */
    vec4 OutColor;
    OutColor.xyz = YuvToRgb(YUV);
    OutColor.w = 1.0;

    gl_FragColor = v_fragmentColor * OutColor;
}
)"sv;

enum
{
    VIDEO_PROGRAM_ID = 0x0fe2bc98,
};
enum class VideoSampleFormat
{
    UNKNOWN,
    RGB32,
    YUY2,
    NV12,
};
struct PrivateVideoDescriptor
{
    MFMediaPlayer* _vplayer = nullptr;
    Texture2D* _vtexture    = nullptr;
    Sprite* _vrender        = nullptr;

    VideoSampleFormat _sampleFormat = VideoSampleFormat::UNKNOWN;
    yasio::byte_buffer _sampleBuffer;
    std::recursive_mutex _sampleBufferMtx;
    bool _sampleDirty = false;

    bool _scaleDirty = false;

    int _videoWidth  = 0;
    int _videoHeight = 0;

    Vec2 _originalViewSize;

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
                    const Vec2 originalScale{static_cast<float>(_videoWidth) / _vtexture->getPixelsWide(),
                                             static_cast<float>(_videoHeight) / _vtexture->getPixelsHigh()};

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
#    if CC_VIDEOPLAYER_DEBUG_DRAW
    _debugDrawNode = DrawNode::create();
    addChild(_debugDrawNode);
#    endif

    // Initialize mediaPlayer backend
    auto hr = MFMediaPlayer::CreateInstance(&pvd->_vplayer);
    if (SUCCEEDED(hr))
    {
        /// create video render sprite
        pvd->_vrender = new Sprite();
        this->addProtectedChild(pvd->_vrender);
        /// setup media session callbacks
        // Invoke at system media session thread
        pvd->_vplayer->SampleEvent = [=](uint8_t* sampleBuffer, size_t len) {
            std::lock_guard<std::recursive_mutex> lck(pvd->_sampleBufferMtx);
            pvd->_sampleBuffer.assign(sampleBuffer, sampleBuffer + len);
            pvd->_sampleDirty = true;
        };
        pvd->_vplayer->SessionEvent = [=](int event) {
            switch (event)
            {
            case MESessionStarted:
                if (!isPlaying())
                    onPlayEvent((int)EventType::PLAYING);
                break;

            case MESessionPaused:
                onPlayEvent((int)EventType::PAUSED);
                break;

            case MESessionStopped:
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
            case MESessionEnded:
                onPlayEvent((int)EventType::COMPLETED);
                break;
            }
        };
    }
    else
    {
        cocos2d::log("Create VideoPlayer backend failed, hr=%d", (int)hr);
    }
}

VideoPlayer::~VideoPlayer()
{
    auto pvd = (PrivateVideoDescriptor*)_videoContext;

    removeAllProtectedChildren();

    if (pvd->_vplayer)
    {
        pvd->_vplayer->Shutdown();
        pvd->_vplayer->Release();
    }
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
        pvd->_vplayer->SetLooping(looping);
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
    cocos2d::ui::Widget::draw(renderer, transform, flags);

    auto pvd     = (PrivateVideoDescriptor*)_videoContext;  //
    auto vrender = pvd->_vrender;
    if (!vrender)
        return;
    if (pvd->_sampleDirty)
    {
        pvd->_sampleDirty = false;
        if (vrender->isVisible())
        {
            std::lock_guard<std::recursive_mutex> lck(pvd->_sampleBufferMtx);
            uint8_t* sampleData  = pvd->_sampleBuffer.data();
            size_t sampleDataLen = pvd->_sampleBuffer.size();
            auto rWidth = pvd->_videoWidth = pvd->_vplayer->GetVideoWidth();
            auto rHeight = pvd->_videoHeight = pvd->_vplayer->GetVideoHeight();

            Vec2 uvScale{1.0f, 1.0f};

            bool needsInit = !pvd->_vtexture;
            if (!pvd->_vtexture)
            {
                pvd->_vtexture = new Texture2D();

                auto programCache = backend::ProgramCache::getInstance();

                auto& sampleOutFormat = pvd->_vplayer->GetVideoOutputFormat();

                if (sampleOutFormat == MFVideoFormat_YUY2)
                    pvd->_sampleFormat = VideoSampleFormat::YUY2;
                else if (sampleOutFormat == MFVideoFormat_NV12)
                    pvd->_sampleFormat = VideoSampleFormat::NV12;
                else if (sampleOutFormat == MFVideoFormat_RGB32)
                    pvd->_sampleFormat = VideoSampleFormat::RGB32;

                switch (pvd->_sampleFormat)
                {
                case VideoSampleFormat::YUY2:
                case VideoSampleFormat::NV12:
                {
                    programCache->registerCustomProgramFactory(
                        VIDEO_PROGRAM_ID, positionTextureColor_vert,
                        std::string{pvd->_sampleFormat == VideoSampleFormat::NV12 ? NV12_FRAG : YUY2_FRAG});
                    auto program = programCache->getCustomProgram(VIDEO_PROGRAM_ID);
                    pvd->_vrender->setProgramState(new backend::ProgramState(program), false);
                    break;
                }
                default:;
                }
            }

            switch (pvd->_sampleFormat)
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
                pvd->_vtexture->updateWithData(sampleData, sampleDataLen, PixelFormat::LA8, PixelFormat::LA8,
                                               texelWidth, rHeight, false, 0);
                pvd->_vtexture->updateWithData(sampleData, sampleDataLen, PixelFormat::RGBA8, PixelFormat::RGBA8,
                                               texelWidth >> 1, rHeight, false, 1);
                break;
            }
            case VideoSampleFormat::RGB32:
                pvd->_vtexture->updateWithData(sampleData, sampleDataLen, PixelFormat::RGBA8, PixelFormat::RGBA8,
                                               rWidth, rHeight, false, 0);
                break;
            default:;
            }
            if (needsInit)
            {
                pvd->_vrender->initWithTexture(pvd->_vtexture);

                if (pvd->_sampleFormat == VideoSampleFormat::NV12 || pvd->_sampleFormat == VideoSampleFormat::YUY2)
                {
                    auto ps = pvd->_vrender->getProgramState();
                    PS_SET_UNIFORM(ps, "out_w", (float)rWidth);
                    PS_SET_UNIFORM(ps, "uv_scale", uvScale);
                }

                pvd->_scaleDirty = true;
            }
        }
    }
    if (pvd->_scaleDirty || (flags & FLAGS_TRANSFORM_DIRTY))
        pvd->rescaleTo(this);

#    if CC_VIDEOPLAYER_DEBUG_DRAW
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
        Widget::setContentSize(enabled ? _director->getOpenGLView()->getFrameSize() : pvd->_originalViewSize);
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
            case MFPlayerState::Closed:
            case MFPlayerState::Ready:
                vplayer->SetPlayOnOpen(TRUE);
                vplayer->OpenURL(ntcvt::from_chars(_videoURL).c_str());
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
            case MFPlayerState::Stopped:
            case MFPlayerState::Paused:
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
            vplayer->SetPosition(static_cast<MFTIME>((std::nano::den / 100) * sec));
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
    cocos2d::ui::Widget::setVisible(visible);
}

void VideoPlayer::onEnter()
{
    Widget::onEnter();
}

void VideoPlayer::onExit()
{
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
        _director->getScheduler()->performFunctionInCocosThread(
            std::bind(_eventCallback, this, (VideoPlayer::EventType)event));
    }
}

cocos2d::ui::Widget* VideoPlayer::createCloneInstance()
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
