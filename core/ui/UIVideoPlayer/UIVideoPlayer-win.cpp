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

void main()
{
    // refer to: 
    // a. https://gist.github.com/crearo/0d50442145b63c6c288d1c1675909990
    // b. https://github.com/tqk2811/TqkLibrary.Media.VideoPlayer/blob/38a2dce908215045cc27cffb741a6e4b8492c9cd/TqkLibrary.Media.VideoPlayer.OpenGl/Renders/NV12Render.cs#L14
    // c. https://www.cnblogs.com/nanqiang/p/10224867.html
    
    float cy = v_texCoord.y + 0.01625; // why needs adjust?
    vec4 uvColor = texture2D(u_texture1, vec2(v_texCoord.x, cy));
    vec3 yuv = vec3(texture2D(u_texture, v_texCoord).r, uvColor.r - 0.5, uvColor.a - 0.5);

    vec3 rgb = mat3( 1.0,       1.0,       1.0,
                     0,        -0.39465,   2.03211,
                     1.13983,  -0.58060,   0 ) * yuv;

    gl_FragColor = v_fragmentColor * vec4(rgb, 1.0);
}
)"sv;

std::string_view YUY2_FRAG = R"(
// refer to: https://github.com/TheRealNox/glsl-shaders/blob/master/glsl/colourConverter.f.glsl

#ifdef GL_ES
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
#else
varying vec4 v_fragmentColor;
varying vec2 v_texCoord;
#endif

uniform sampler2D u_texture; // Y sample
uniform float tex_w; // texture width
uniform float tex_h; // texture height

vec4 inYUY2(vec4 tempyuv, float isOdd)
{
	if (isOdd > 0.0)
		return vec4(tempyuv.b, tempyuv.g, tempyuv.a, 255.0);
	else
		return vec4(tempyuv.r, tempyuv.g, tempyuv.a, 255.0);
}

vec4 limitedYCbCrToComputerRGBNormalized(vec4 yuv)
{
	vec4 rgb = vec4(0.0);
	float scale = 1.0 / 256.0;
	
	yuv = yuv * 255.0;
	
	yuv.r -= 16.0;
	yuv.g -= 128.0;
	yuv.b -= 128.0;
	
	rgb.r = scale * ((298.082 * yuv.r) + (458.942 * yuv.b));
	rgb.g = scale * ((298.082 * yuv.r) + (-54.592 * yuv.g) + (-136.425 * yuv.b));
	rgb.b = scale * ((298.082 * yuv.r) + (540.775 * yuv.g));
	
	rgb.a = 255.0;
	
	rgb = rgb / 255.0;
	
	return rgb;
}

vec4 convertLimitedYUY2toComputerRGB()
{
	vec4 tempyuv = vec4(0.0);
	vec2 textureRealSize = vec2(tex_w, tex_h);
	
	vec2 pixelPos = vec2(textureRealSize.x * v_texCoord.x, textureRealSize.y * v_texCoord.y);
	
	float isOdd = floor(mod(pixelPos.x, 2.0));
	
	vec2 packedCoor = vec2(v_texCoord.x/2.0, v_texCoord.y);
	
	tempyuv = inYUY2(texture2D(u_texture, packedCoor), isOdd);
	
	return limitedYCbCrToComputerRGBNormalized(tempyuv);
}

vec4 fullYCbCrToComputerRGBNormalized(vec4 yuv)
{
	vec4 rgb = vec4(0.0);
	float scale = 1.0 / 256.0;
	
	yuv = yuv * 255.0;
	
	yuv.g -= 128.0;
	yuv.b -= 128.0;
	
	rgb.r = scale * ((256.0 * yuv.r) + (403.1488 * yuv.b));
	rgb.g = scale * ((256.0 * yuv.r) + (-47.954944 * yuv.g) + (-119.839744 * yuv.b));
	rgb.b = scale * ((256.0 * yuv.r) + (475.0336 * yuv.g));
	
	rgb.a = 255.0;
	
	rgb = rgb / 255.0;
	
	return rgb;
}

vec4 convertFullYUY2toComputerRGB()
{
	vec4 tempyuv = vec4(0.0);
	// vec2 textureRealSize = textureSize(u_texture, 0);
    vec2 textureRealSize = vec2(tex_w, tex_h);
	
	vec2 pixelPos = vec2(textureRealSize.x * v_texCoord.x, textureRealSize.y * v_texCoord.y);
	
	float isOdd = floor(mod(pixelPos.x, 2.0));
	
	vec2 packedCoor = vec2(v_texCoord.x/2.0, v_texCoord.y);
	
	tempyuv = inYUY2(texture2D(u_texture, packedCoor), isOdd);
	
	return fullYCbCrToComputerRGBNormalized(tempyuv);
}

void main()
{
  vec4 color = convertFullYUY2toComputerRGB();
  gl_FragColor = v_fragmentColor * vec4(color.rgb, 1.0);
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
                    const Vec2 originalScale{1.0f, _sampleFormat == VideoSampleFormat::YUY2 ? 2.0f : 1.0f};

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
            auto w = pvd->_videoWidth = pvd->_vplayer->GetVideoWidth();
            auto h = pvd->_videoHeight = pvd->_vplayer->GetVideoHeight();

            bool needsInit = !pvd->_vtexture;
            if (!pvd->_vtexture)
            {
                pvd->_vtexture    = new Texture2D();
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
                        std::string{pvd->_sampleFormat == VideoSampleFormat::YUY2 ? YUY2_FRAG : NV12_FRAG});
                    auto program = programCache->getCustomProgram(VIDEO_PROGRAM_ID);
                    pvd->_vrender->setProgramState(new backend::ProgramState(program), false);
                    break;
                }
                default:;
                }
            }

            switch (pvd->_sampleFormat)
            {
            case VideoSampleFormat::YUY2:
                pvd->_vtexture->updateWithData(sampleData, sampleDataLen, PixelFormat::RGBA8, PixelFormat::RGBA8, w,
                                               h / 2, false, 0);
                break;
            case VideoSampleFormat::NV12:
            {
                const size_t ySampleSize = w * h;
                pvd->_vtexture->updateWithData(sampleData, ySampleSize, PixelFormat::L8, PixelFormat::L8, w, h, false,
                                               0);
                pvd->_vtexture->updateWithData(sampleData + ySampleSize, sampleDataLen - ySampleSize, PixelFormat::LA8,
                                               PixelFormat::LA8, w >> 1, h >> 1, false, 1);
                break;
            }
            case VideoSampleFormat::RGB32:
                pvd->_vtexture->updateWithData(sampleData, sampleDataLen, PixelFormat::RGBA8, PixelFormat::RGBA8, w, h,
                                               false, 0);
                break;
            default:;
            }
            if (needsInit)
            {
                pvd->_vrender->initWithTexture(pvd->_vtexture);

                if (pvd->_sampleFormat == VideoSampleFormat::YUY2)
                {
                    auto ps = pvd->_vrender->getProgramState();
                    PS_SET_UNIFORM(ps, "tex_w", (float)w);
                    PS_SET_UNIFORM(ps, "tex_h", (float)h);
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
