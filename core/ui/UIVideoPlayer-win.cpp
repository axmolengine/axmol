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

#include "ui/UIVideoPlayer.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#    include <unordered_map>
#    include <stdlib.h>
#    include <string>
#    include "base/CCDirector.h"
#    include "base/CCEventListenerKeyboard.h"
#    include "platform/CCFileUtils.h"
#    include "ui/UIHelper.h"
#    include "ui/MFMediaPlayer.h"
#    include "renderer/backend/ProgramCache.h"
#    include "yasio/detail/byte_buffer.hpp"
#    include "ntcvt/ntcvt.hpp"

//-----------------------------------------------------------------------------------------------------------

USING_NS_CC;

//-----------------------------------------------------------------------------------------------------------

using namespace cocos2d::ui;

namespace
{
struct PrivateVideoDescriptor
{
    MFMediaPlayer* _vplayer = nullptr;
    Texture2D* _vtexture    = nullptr;
    Sprite* _vrender        = nullptr;

    yasio::byte_buffer _frameData;
    std::recursive_mutex _frameDataMtx;
    bool _frameDataDirty = false;

    int _frameWidth  = 0;
    int _frameHeight = 0;
};
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
    
    float cy = v_texCoord.y + 0.01625; // why needs adjust?
    vec4 uvColor = texture2D(u_texture1, vec2(v_texCoord.x, cy));
    vec3 yuv = vec3(texture2D(u_texture, v_texCoord).r, uvColor.r - 0.5, uvColor.a - 0.5);

    vec3 rgb = mat3( 1.0,       1.0,       1.0,
                     0,        -0.39465,   2.03211,
                     1.13983,  -0.58060,   0 ) * yuv;

    gl_FragColor = v_fragmentColor * vec4(rgb, 1.0);
}
)"sv;
enum
{
    VIDEO_PROGRAM_ID = 0x00fe2bc98,
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
    auto pvd   = new PrivateVideoDescriptor{};
    _videoView = pvd;
#    if CC_VIDEOPLAYER_DEBUG_DRAW
    _debugDrawNode = DrawNode::create();
    addChild(_debugDrawNode);
#    endif

    // Initialize mediaPlayer backend
    auto hr = MFMediaPlayer::CreateInstance(Director::getInstance()->getOpenGLView()->getWin32Window(), &pvd->_vplayer);
    if (SUCCEEDED(hr))
    {
        /// create video render sprite
        pvd->_vrender     = new Sprite();
        auto programCache = backend::ProgramCache::getInstance();
        programCache->registerCustomProgramFactory(VIDEO_PROGRAM_ID, positionTextureColor_vert, std::string{NV12_FRAG});
        auto program = programCache->getCustomProgram(VIDEO_PROGRAM_ID);
        pvd->_vrender->setProgramState(new backend::ProgramState(program), false);
        this->addProtectedChild(pvd->_vrender);

        /// setup media session callbacks
        // Invoke at system media session thread
        pvd->_vplayer->SampleEvent = [=](uint8_t* sampleBuffer, size_t len) {
            std::lock_guard<std::recursive_mutex> lck(pvd->_frameDataMtx);
            pvd->_frameData.assign(sampleBuffer, sampleBuffer + len);
            pvd->_frameDataDirty = true;
        };
    }
    else
    {
        cocos2d::log("Create VideoPlayer backend failed, hr=%d", (int)hr);
    }
}

VideoPlayer::~VideoPlayer()
{
    auto pvd = (PrivateVideoDescriptor*)_videoView;

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
    _videoURL    = FileUtils::getInstance()->fullPathForFilename(fileName);
    _videoSource = VideoPlayer::Source::FILENAME;

    auto pvd = (PrivateVideoDescriptor*)_videoView;
    if (pvd)
        pvd->_vplayer->OpenURL(ntcvt::from_chars(_videoURL).c_str());
}

void VideoPlayer::setURL(std::string_view videoUrl)
{
    _videoURL    = videoUrl;
    _videoSource = VideoPlayer::Source::URL;
}

void VideoPlayer::setLooping(bool looping)
{
    _isLooping = looping;

    auto pvd = (PrivateVideoDescriptor*)_videoView;
    if (pvd && pvd->_vplayer)
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

    auto pvd = (PrivateVideoDescriptor*)_videoView;  //
    if (pvd && pvd->_frameDataDirty)
    {
        pvd->_frameDataDirty = false;

        auto& vrender = pvd->_vrender;

        std::lock_guard<std::recursive_mutex> lck(pvd->_frameDataMtx);
        uint8_t* nv12Data  = pvd->_frameData.data();
        size_t nv12DataLen = pvd->_frameData.size();
        auto w = pvd->_frameWidth = pvd->_vplayer->GetVideoWidth();
        auto h = pvd->_frameHeight = pvd->_vplayer->GetVideoHeight();

        // refer to: https://www.cnblogs.com/nanqiang/p/10224867.html
        bool needsInit = !pvd->_vtexture;
        if (!pvd->_vtexture)
            pvd->_vtexture = new Texture2D();

        size_t ySampleSize = w * h;
        pvd->_vtexture->updateWithData(nv12Data, ySampleSize, PixelFormat::L8, PixelFormat::L8, w, h, false, 0);
        pvd->_vtexture->updateWithData(nv12Data + ySampleSize, nv12DataLen - ySampleSize, PixelFormat::LA8,
                                       PixelFormat::LA8, w >> 1, h >> 1, false, 1);

        if (needsInit)
            pvd->_vrender->initWithTexture(pvd->_vtexture);
    }
    if (flags & FLAGS_TRANSFORM_DIRTY)
    {
        auto uiRect = cocos2d::ui::Helper::convertBoundingBoxToScreen(this);
        /*JniHelper::callStaticVoidMethod(videoHelperClassName, "setVideoRect", _videoPlayerIndex, (int)uiRect.origin.x,
                                        (int)uiRect.origin.y, (int)uiRect.size.width, (int)uiRect.size.height);*/
    }

#    if CC_VIDEOPLAYER_DEBUG_DRAW
    _debugDrawNode->clear();
    auto size         = getContentSize();
    Point vertices[4] = {Point::ZERO, Point(size.width, 0), Point(size.width, size.height), Point(0, size.height)};
    _debugdrawNode->drawPoly(vertices, 4, true, Color4F(1.0, 1.0, 1.0, 1.0));
#    endif
}

void VideoPlayer::setFullScreenEnabled(bool enabled)
{
    if (_fullScreenEnabled != enabled)
    {
        _fullScreenEnabled = enabled;

        auto frameSize = Director::getInstance()->getOpenGLView()->getFrameSize();
        // JniHelper::callStaticVoidMethod(videoHelperClassName, "setFullScreenEnabled", _videoPlayerIndex, enabled,
        //                                (int)frameSize.width, (int)frameSize.height);
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
        _keepAspectRatioEnabled = enable;
        // JniHelper::callStaticVoidMethod(videoHelperClassName, "setVideoKeepRatioEnabled", _videoPlayerIndex, enable);
    }
}

#    if CC_VIDEOPLAYER_DEBUG_DRAW
void VideoPlayer::drawDebugData()
{
    Director* director = Director::getInstance();
    CCASSERT(nullptr != director, "Director is null when setting matrix stack");

    director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

    auto size = getContentSize();

    Point vertices[4] = {Point::ZERO, Point(size.width, 0), Point(size.width, size.height), Point(0, size.height)};

    DrawPrimitives::drawPoly(vertices, 4, true);

    director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}
#    endif

void VideoPlayer::play()
{
    if (!_videoURL.empty())
    {
        // JniHelper::callStaticVoidMethod(videoHelperClassName, "startVideo", _videoPlayerIndex);
    }
}

void VideoPlayer::pause()
{
    if (!_videoURL.empty())
    {
        // JniHelper::callStaticVoidMethod(videoHelperClassName, "pauseVideo", _videoPlayerIndex);
    }
}

void VideoPlayer::resume()
{
    if (!_videoURL.empty())
    {
        // JniHelper::callStaticVoidMethod(videoHelperClassName, "resumeVideo", _videoPlayerIndex);
    }
}

void VideoPlayer::stop()
{
    if (!_videoURL.empty())
    {
        // JniHelper::callStaticVoidMethod(videoHelperClassName, "stopVideo", _videoPlayerIndex);
    }
}

void VideoPlayer::seekTo(float sec)
{
    if (!_videoURL.empty())
    {
        // JniHelper::callStaticVoidMethod(videoHelperClassName, "seekVideoTo", _videoPlayerIndex, int(sec * 1000));
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

    if (!visible || isRunning())
    {
        // JniHelper::callStaticVoidMethod(videoHelperClassName, "setVideoVisible", _videoPlayerIndex, visible);
    }
}

void VideoPlayer::onEnter()
{
    Widget::onEnter();
    if (isVisible() && !_videoURL.empty())
    {
        // JniHelper::callStaticVoidMethod(videoHelperClassName, "setVideoVisible", _videoPlayerIndex, true);
    }
}

void VideoPlayer::onExit()
{
    Widget::onExit();
    // JniHelper::callStaticVoidMethod(videoHelperClassName, "setVideoVisible", _videoPlayerIndex, false);
}

void VideoPlayer::addEventListener(const VideoPlayer::ccVideoPlayerCallback& callback)
{
    _eventCallback = callback;
}

void VideoPlayer::onPlayEvent(int event)
{
    if (event == (int)VideoPlayer::EventType::PLAYING)
    {
        _isPlaying = true;
    }
    else
    {
        _isPlaying = false;
    }

    if (_eventCallback)
    {
        _eventCallback(this, (VideoPlayer::EventType)event);
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
        _videoView              = videoPlayer->_videoView;
    }
}

#endif
