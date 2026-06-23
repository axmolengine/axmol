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

#include "axmol/ui/VideoPlayer.h"
#include "axmol/renderer/RenderTexturePass.h"

// Now, common implementation based on redesigned MediaEngine is enable for windows and macOS
#if defined(AX_ENABLE_VIDEO)
#    include <unordered_map>
#    include <stdlib.h>
#    include <string>
#    include "axmol/base/Director.h"
#    include "axmol/base/KeyboardEventListener.h"
#    include "axmol/platform/FileUtils.h"
#    include "axmol/ui/UIHelper.h"
#    include "axmol/media/MediaEngine.h"
#    include "axmol/ui/Button.h"
#    include "axmol/ui/LayoutGroup.h"
#    include "axmol/tlx/byte_buffer.hpp"
//-----------------------------------------------------------------------------------------------------------

using namespace ax;

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
struct PrivateVideoContext
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

    std::function<void(const MEVideoFrame& frame)> _renderFrameFunc;

    bool updatePixelDesc(const MEVideoFrame& frame)
    {
        auto& desc = frame._vpd;
        if (desc.equals(_vpixelDesc))
            return false;

        _vpixelDesc = desc;

        auto pixelFormat = desc._PF;

        AX_SAFE_RELEASE(_vtexture);
        AX_SAFE_RELEASE_NULL(_vchromaTexture);
        AX_SAFE_RELEASE_NULL(_vchroma2Texture);

        auto programManager = ProgramManager::getInstance();

        switch (pixelFormat)
        {
        case MEVideoPixelFormat::YUY2:
            // RG8
            _vtexture = new Texture2D();
            _vtexture->initWithSpec(
                {
                    .width       = static_cast<uint16_t>(desc._dim.x),
                    .height      = static_cast<uint16_t>(desc._dim.y),
                    .pixelFormat = PixelFormat::RG8,
                },
                Texture2D::DEFAULT_SLICE_DATA);

            // RGBA8
            _vchromaTexture = new Texture2D();
            _vchromaTexture->initWithSpec(
                {
                    .width       = static_cast<uint16_t>(desc._dim.x >> 1),
                    .height      = static_cast<uint16_t>(desc._dim.y),
                    .pixelFormat = PixelFormat::RGBA8,
                },
                Texture2D::DEFAULT_SLICE_DATA);
            _vchromaTexture->setAliasTexParameters();
            _vrender->setProgramState(rhi::ProgramType::VIDEO_TEXTURE_YUY2);

            _renderFrameFunc = [this](const MEVideoFrame& frame) {
                if (_vtexture && _vchromaTexture)
                {
                    auto& bufferDim = frame._vpd._dim;
                    _vtexture->updateSubData(frame._dataPointer, 0, 0, bufferDim.x, bufferDim.y);
                    _vchromaTexture->updateSubData(frame._dataPointer, 0, 0, bufferDim.x >> 1, bufferDim.y);
                }
            };
            break;
        case MEVideoPixelFormat::NV12:
            // R8
            _vtexture = new Texture2D();
            _vtexture->initWithSpec(
                {
                    .width       = static_cast<uint16_t>(desc._dim.x),
                    .height      = static_cast<uint16_t>(desc._dim.y),
                    .pixelFormat = PixelFormat::R8,
                },
                Texture2D::DEFAULT_SLICE_DATA);

            // RG8
            _vchromaTexture = new Texture2D();
            _vchromaTexture->initWithSpec(
                {
                    .width       = static_cast<uint16_t>(desc._dim.x >> 1),
                    .height      = static_cast<uint16_t>(desc._dim.y >> 1),
                    .pixelFormat = PixelFormat::RG8,
                },
                Texture2D::DEFAULT_SLICE_DATA);
            _vchromaTexture->setAliasTexParameters();
            _vrender->setProgramState(rhi::ProgramType::VIDEO_TEXTURE_NV12);

            // allocate tex storage
            _renderFrameFunc = [this](const MEVideoFrame& frame) {
                if (_vtexture && _vchromaTexture)
                {
                    auto& bufferDim = frame._vpd._dim;
                    _vtexture->updateSubData(frame._dataPointer, 0, 0, bufferDim.x, bufferDim.y);
                    _vchromaTexture->updateSubData(frame._cbcrDataPointer, 0, 0, bufferDim.x >> 1, bufferDim.y >> 1);
                }
            };
            break;
        case MEVideoPixelFormat::I420:
            // R8
            _vtexture = new Texture2D();
            _vtexture->initWithSpec(
                {
                    .width       = static_cast<uint16_t>(desc._dim.x),
                    .height      = static_cast<uint16_t>(desc._dim.y),
                    .pixelFormat = PixelFormat::R8,
                },
                Texture2D::DEFAULT_SLICE_DATA);
            // R8
            _vchromaTexture = new Texture2D();
            _vchromaTexture->initWithSpec(
                {
                    .width       = static_cast<uint16_t>(desc._dim.x >> 1),
                    .height      = static_cast<uint16_t>(desc._dim.y >> 1),
                    .pixelFormat = PixelFormat::R8,
                },
                Texture2D::DEFAULT_SLICE_DATA);
            _vchromaTexture->setAliasTexParameters();
            // R8
            _vchroma2Texture = new Texture2D();
            _vchroma2Texture->initWithSpec(
                {
                    .width       = static_cast<uint16_t>(desc._dim.x >> 1),
                    .height      = static_cast<uint16_t>(desc._dim.y >> 1),
                    .pixelFormat = PixelFormat::R8,
                },
                Texture2D::DEFAULT_SLICE_DATA);
            _vchroma2Texture->setAliasTexParameters();
            _vrender->setProgramState(rhi::ProgramType::VIDEO_TEXTURE_I420);

            _renderFrameFunc = [this](const MEVideoFrame& frame) {
                if (_vtexture && _vchromaTexture && _vchroma2Texture)
                {
                    auto& bufferDim              = frame._vpd._dim;
                    const auto chromaTexDataSize = (bufferDim.x * bufferDim.y) >> 2;
                    _vtexture->updateSubData(frame._dataPointer, 0, 0, bufferDim.x, bufferDim.y);
                    _vchromaTexture->updateSubData(frame._cbcrDataPointer, 0, 0, bufferDim.x >> 1, bufferDim.y >> 1);
                    _vchroma2Texture->updateSubData(frame._cbcrDataPointer + chromaTexDataSize, 0, 0, bufferDim.x >> 1,
                                                    bufferDim.y >> 1);
                }
            };
            break;
        case MEVideoPixelFormat::RGB32:
            // RGBA8
            _vtexture = new Texture2D();
            _vtexture->initWithSpec(
                {
                    .width       = static_cast<uint16_t>(desc._dim.x),
                    .height      = static_cast<uint16_t>(desc._dim.y),
                    .pixelFormat = PixelFormat::RGBA8,
                },
                Texture2D::DEFAULT_SLICE_DATA);
            _vrender->setProgramState(rhi::ProgramType::VIDEO_TEXTURE_RGB32);

            _renderFrameFunc = [this](const MEVideoFrame& frame) {
                if (_vtexture)
                {
                    auto& bufferDim = frame._vpd._dim;
                    _vtexture->updateSubData(frame._dataPointer, 0, 0, bufferDim.x, bufferDim.y);
                }
            };
            break;
        case MEVideoPixelFormat::BGR32:
            // BGRA8
            _vtexture = new Texture2D();
            _vtexture->initWithSpec(
                {
                    .width       = static_cast<uint16_t>(desc._dim.x),
                    .height      = static_cast<uint16_t>(desc._dim.y),
                    .pixelFormat = PixelFormat::BGRA8,
                },
                Texture2D::DEFAULT_SLICE_DATA);
            _vrender->setProgramState(rhi::ProgramType::VIDEO_TEXTURE_RGB32);
            _renderFrameFunc = [this](const MEVideoFrame& frame) {
                if (_vtexture)
                {
                    auto& bufferDim = frame._vpd._dim;
                    _vtexture->updateSubData(frame._dataPointer, 0, 0, bufferDim.x, bufferDim.y);
                }
            };
            break;
        }

        _vrender->setTexture(_vtexture);
        _vrender->setTextureRect(ax::Rect{Vec2::zero, Vec2{
                                                          frame._videoDim.x / AX_CONTENT_SCALE_FACTOR(),
                                                          frame._videoDim.y / AX_CONTENT_SCALE_FACTOR(),
                                                      }});

        if (pixelFormat >= MEVideoPixelFormat::YUY2)
        {
            auto ps = _vrender->getProgramState();
            PrivateVideoContext::updateColorTransform(ps, frame._vpd._fullRange);

            ps->setTexture(ps->getUniformLocation("u_tex1"), 1, _vchromaTexture->getRHITexture());

            if (pixelFormat == MEVideoPixelFormat::I420)
                ps->setTexture(ps->getUniformLocation("u_tex2"), 2, _vchroma2Texture->getRHITexture());
        }

        _scaleDirty = true;

        return true;
    }

    void renderFrame(const MEVideoFrame& frame)
    {
        if (_renderFrameFunc)
            _renderFrameFunc(const_cast<MEVideoFrame&>(frame));
    }

    void rescaleTo(VideoPlayer* videoView)
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

                if (!videoView->isKeepAspectRatio())
                {
                    const auto scale = Vec2(viewSize.x / videoSize.x, viewSize.y / videoSize.y);
                    _vrender->setScale(scale.x, scale.y);

                    auto* videoController = videoView->getVideoController();
                    if (videoController)
                    {
                        if (videoController->getOrientation() == VideoController::Orientation::RotatedLeft)
                        {
                            videoController->setContentSize(videoSize *
                                                            Vec2(viewSize.y / videoSize.x, viewSize.x / videoSize.y));
                            videoController->setRotation(-90);
                        }
                        else if (videoController->getOrientation() == VideoController::Orientation::RotatedRight)
                        {
                            videoController->setContentSize(videoSize *
                                                            Vec2(viewSize.y / videoSize.x, viewSize.x / videoSize.y));
                            videoController->setRotation(90);
                        }
                        else
                        {
                            videoController->setContentSize(videoSize * scale);
                        }
                    }
                }
                else
                {
                    const auto aspectRatio = (std::min)(viewSize.x / videoSize.x, viewSize.y / (videoSize.y));

                    _vrender->setScale(aspectRatio);

                    auto* videoController = videoView->getVideoController();
                    if (videoController)
                    {
                        if (videoController->getOrientation() == VideoController::Orientation::RotatedLeft)
                        {
                            videoController->setContentSize(Vec2(videoSize.y * aspectRatio, videoSize.x * aspectRatio));
                            videoController->setRotation(-90);
                        }
                        else if (videoController->getOrientation() == VideoController::Orientation::RotatedRight)
                        {
                            videoController->setContentSize(Vec2(videoSize.y * aspectRatio, videoSize.x * aspectRatio));
                            videoController->setRotation(90);
                        }
                        else
                        {
                            videoController->setContentSize(videoSize * aspectRatio);
                        }
                    }
                }

                ui::Helper::centerNode(_vrender);

                _vrender->setVisible(true);
            }
            else
                _vrender->setVisible(false);
        }

        _scaleDirty = false;
    }

    static void updateColorTransform(rhi::ProgramState* ps, bool bFullColorRange)
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

static constexpr std::string_view BODY_IMAGE_1_PIXEL_HEIGHT =
    "iVBORw0KGgoAAAANSUhEUgAAAAwAAAABCAMAAADdNb8LAAAAA1BMVEX///+nxBvIAAAACklEQVR4AWNABgAADQABYc2cpAAAAABJRU5ErkJggg=="sv;

static constexpr std::string_view BODY_IMAGE_1_PIXEL_HEIGHT_KEY = "/__bodyImage"sv;

constexpr auto TIMELINE_BAR_HEIGHT = 12.f;

RefPtr<Texture2D> g_mediaControlsTexture = nullptr;

enum class VideoControlButtonId
{
    Play,
    Stop,
    Pause,
    EnterFullscreen,
    ExitFullscreen,
    TimelineSliderButton
};

std::map<VideoControlButtonId, Rect> g_mediaControlTextureRegions;

static void createVideoControlTexture()
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
        drawNode->drawSolidRect(s, s + Vec2(iconW, -iconH), Color::white);
    };

    auto DrawPlay = [&](const Vec2& middle) -> void {
        auto p1 = Vec2(middle.x - iconW / 2.f, middle.y + iconH / 2.f);
        auto p2 = Vec2(middle.x + iconW / 2.f, middle.y);
        auto p3 = Vec2(middle.x - iconW / 2.f, middle.y - iconH / 2.f);

        drawNode->drawTriangle(p1, p2, p3, Color::white);
    };

    auto DrawPause = [&](const Vec2& middle) -> void {
        auto start = Vec2(middle.x - 3, middle.y + iconH / 2.f);
        drawNode->drawSolidRect(start, start + Vec2(-6, -iconH), Color::white);

        start = Vec2(middle.x + 3, middle.y + iconH / 2.f);
        drawNode->drawSolidRect(start, start + Vec2(6, -iconH), Color::white);
    };

    auto DrawEnterFullscreen = [&](const Vec2& middle) -> void {
        auto topLeft     = Vec2(middle.x - panelW / 2.f + 6, middle.y + panelH / 2.f - 6);
        auto topRight    = Vec2(middle.x + panelW / 2.f - 6, middle.y + panelH / 2.f - 6);
        auto bottomLeft  = Vec2(middle.x - panelW / 2.f + 6, middle.y - panelH / 2.f + 6);
        auto bottomRight = Vec2(middle.x + panelW / 2.f - 6, middle.y - panelH / 2.f + 6);

        // Top left
        drawNode->drawSolidRect(topLeft, topLeft + Vec2(20, -6), Color::white);
        drawNode->drawSolidRect(topLeft, topLeft + Vec2(6, -20), Color::white);

        // Top right
        drawNode->drawSolidRect(topRight, topRight + Vec2(-20, -6), Color::white);
        drawNode->drawSolidRect(topRight, topRight + Vec2(-6, -20), Color::white);

        // Bottom left
        drawNode->drawSolidRect(bottomLeft, bottomLeft + Vec2(20, 6), Color::white);
        drawNode->drawSolidRect(bottomLeft, bottomLeft + Vec2(6, 20), Color::white);

        // Bottom right
        drawNode->drawSolidRect(bottomRight, bottomRight + Vec2(-20, 6), Color::white);
        drawNode->drawSolidRect(bottomRight, bottomRight + Vec2(-6, 20), Color::white);
    };

    auto DrawExitFullScreen = [&](const Vec2& middle) -> void {
        auto topLeft     = Vec2(middle.x - 4, middle.y + 4);
        auto topRight    = Vec2(middle.x + 4, middle.y + 4);
        auto bottomLeft  = Vec2(middle.x - 4, middle.y - 4);
        auto bottomRight = Vec2(middle.x + 4, middle.y - 4);

        // Top left
        drawNode->drawSolidRect(topLeft, topLeft + Vec2(-20, 6), Color::white);
        drawNode->drawSolidRect(topLeft, topLeft + Vec2(-6, 20), Color::white);

        // Top right
        drawNode->drawSolidRect(topRight, topRight + Vec2(20, 6), Color::white);
        drawNode->drawSolidRect(topRight, topRight + Vec2(6, 20), Color::white);

        // Bottom left
        drawNode->drawSolidRect(bottomLeft, bottomLeft + Vec2(-20, -6), Color::white);
        drawNode->drawSolidRect(bottomLeft, bottomLeft + Vec2(-6, -20), Color::white);

        // Bottom right
        drawNode->drawSolidRect(bottomRight, bottomRight + Vec2(20, -6), Color::white);
        drawNode->drawSolidRect(bottomRight, bottomRight + Vec2(6, -20), Color::white);
    };

    auto DrawSliderControlButton = [&](const Vec2& middle) -> void {
        drawNode->drawSolidCircle(middle, panelW / 2, 0, 180, Color::white);
    };

    std::map<VideoControlButtonId, std::function<void(const Vec2&)>> items = {
        {VideoControlButtonId::Play, DrawPlay},
        {VideoControlButtonId::Stop, DrawStop},
        {VideoControlButtonId::Pause, DrawPause},
        {VideoControlButtonId::EnterFullscreen, DrawEnterFullscreen},
        {VideoControlButtonId::ExitFullscreen, DrawExitFullScreen},
        {VideoControlButtonId::TimelineSliderButton, DrawSliderControlButton}};

    auto numItems    = static_cast<int>(items.size());
    auto totalWidth  = utils::nextPOT(numItems * panelW + (numItems - 1) * gap + (border * 2));
    auto totalHeight = utils::nextPOT(border * 2 + panelH);
    auto imageSize   = Size(static_cast<float>(totalWidth), static_cast<float>(totalHeight));
    auto* node       = Node::create();
    node->setContentSize(imageSize);
    node->setIgnoreAnchorPointForPosition(false);
    node->setAnchorPoint(Anchors::bottomLeft);
    node->setPosition(0, 0);
    node->addChild(drawNode);

    auto* rt = RenderTexture::createForCanvas(Vec2(totalWidth, totalHeight), PixelFormat::RGBA8, PixelFormat::D24S8);

    {
        auto scope = RefPtr<RenderTexturePass>(RenderTexturePass::obtain(rt), tlx::adopt_object);
        scope->begin();
        scope->clear(ClearFlag::COLOR, {.color = Color(0, 0, 0, 0)});

        g_mediaControlTextureRegions.clear();

        int i = 0;
        for (auto&& item : items)
        {
            auto midPoint =
                Vec2(border + (i * panelW) + (i * gap) + (panelW / 2.f), imageSize.height - border - (panelH / 2.f));
            item.second(midPoint);
            g_mediaControlTextureRegions[item.first] = Rect(border + (panelW * i) + (gap * i), border, panelW, panelH);
            ++i;
        }

        node->visit();
        scope->end();
    }
    Director::getInstance()->getRenderer()->render();

    g_mediaControlsTexture = rt;
}

}  // namespace

static const float ZOOM_ACTION_TIME_STEP = 0.05f;

void VideoController::setOrientation(Orientation orientation)
{
    _orientation = orientation;
}

VideoPlayerControl* VideoPlayerControl::create(SpriteFrame* frame)
{
    auto* widget = new VideoPlayerControl();
    if (widget->init(frame))
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

VideoPlayerControl::~VideoPlayerControl()
{
    AX_SAFE_RELEASE(_overlay);
}

bool VideoPlayerControl::init(SpriteFrame* frame)
{
    if (!Button::init(""))
    {
        return false;
    }

    if (frame)
    {
        _overlay = Sprite::createWithSpriteFrame(frame);
        AX_SAFE_RETAIN(_overlay);
        _overlay->setAnchorPoint(Anchors::center);
        _overlay->setPosition(_contentSize.width * 0.5f, _contentSize.height * 0.5f);
        addProtectedChild(_overlay, -2, -1);

        if (!_autoSize && _customSize.equals(Vec2::zero))
        {
            _customSize = _overlay->getContentSize();
        }
        this->updateChildrenDisplayedRGBA();
        updateContentSize();
    }

    return true;
}

void VideoPlayerControl::onSizeChanged()
{
    Button::onSizeChanged();
    if (_overlay)
    {
        _overlay->setPosition(_contentSize.width * 0.5f, _contentSize.height * 0.5f);
    }
}

Vec2 VideoPlayerControl::resolvePreferredSize(const Vec2& /*sizeHint*/) const
{
    if (!_autoSize)
    {
        return this->getNormalSize();
    }

    if (_overlay)
    {
        Vec2 overlaySize = _overlay->getContentSize();
        if (!_normalTextureLoaded)
        {
            return overlaySize;
        }
    }
    return _normalTextureSize;
}

Vec2 VideoPlayerControl::getNormalSize() const
{
    if (_overlay)
    {
        return _overlay->getContentSize();
    }

    return Button::getNormalSize();
}

void VideoPlayerControl::onPressStateChangedToNormal()
{
    Button::onPressStateChangedToNormal();

    if (nullptr != _overlay)
    {
        _overlay->stopAllActions();
        if (!_autoSize)
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

void VideoPlayerControl::onPressStateChangedToPressed()
{
    Button::onPressStateChangedToPressed();
    if (nullptr != _overlay)
    {
        _overlay->stopAllActions();
        Action* zoomTitleAction = ScaleTo::create(ZOOM_ACTION_TIME_STEP, 1.0f + _zoomScale, 1.0f + _zoomScale);
        _overlay->runAction(zoomTitleAction);
    }
}

void VideoPlayerControl::onPressStateChangedToDisabled()
{
    Button::onPressStateChangedToDisabled();
    if (nullptr != _overlay)
    {
        _overlay->setScale(1.0);
    }
}

DefaultVideoController::DefaultVideoController(VideoPlayer* player)
    : VideoController(player), _timelineBarHeight(TIMELINE_BAR_HEIGHT)
{}

DefaultVideoController* DefaultVideoController::create(VideoPlayer* videoPlayer)
{
    auto* widget = new DefaultVideoController(videoPlayer);
    if (widget->init())
    {
        widget->autorelease();
        return widget;
    }
    AX_SAFE_DELETE(widget);
    return nullptr;
}

bool DefaultVideoController::init()
{
    if (!Widget::init())
    {
        return false;
    }

    setPointerEnabled(true);
    setCascadeOpacityEnabled(false);
    updateControllerState();
    if (_videoPlayer)
    {
        setContentSize(_videoPlayer->getContentSize());
    }
    return true;
}

void DefaultVideoController::initRenderNode()
{
    Widget::initRenderNode();

    // scheduleOnce is used to create the controls on the next update
    // loop. This is a work-around for a RenderTexture issue
    // when being created such places as a button click event handler
    // on Apple platforms/Metal renderer backend
    scheduleOnce([this](float) {
        createControls();
        updateControlsForContentSize(_videoPlayer->getContentSize());
        updateControllerState();
    }, 0.f, "__create_video_controls"sv);
}

void DefaultVideoController::onPressStateChangedToPressed()
{
    _lastTouch = std::chrono::steady_clock::now();

    if (!_controlsReady || _controlPanel->getOpacity() == 255)
    {
        return;
    }

    updateControllerState();

    _mediaOverlay->runAction(Sequence::create(FadeTo::create(0.5f, 150), nullptr));
    _controlPanel->runAction(Sequence::create(FadeIn::create(0.5f), CallFunc::create([this] {
        if (_controlPanel->isScheduled("__video_controller_fader"sv))
            return;

        _controlPanel->schedule([this](float) {
            auto now       = std::chrono::steady_clock::now();
            auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(now - _lastTouch);
            if (deltaTime > std::chrono::milliseconds{2500})
            {
                _controlPanel->unschedule("__video_controller_fader"sv);
                _controlPanel->runAction(Sequence::create(FadeOut::create(0.5f), nullptr));
                _mediaOverlay->runAction(Sequence::create(FadeOut::create(0.5f), nullptr));
            }
        }, 1.f, "__video_controller_fader"sv);
    }),
                                              nullptr));
}

void DefaultVideoController::setContentSize(const Vec2& contentSize)
{
    Widget::setContentSize(contentSize);
    updateControlsForContentSize(contentSize);
    updateControllerState();
}

void DefaultVideoController::update(float delta)
{
    Widget::update(delta);
    updateControls();
}

void DefaultVideoController::onEnter()
{
    Widget::onEnter();
    scheduleUpdate();
}

void DefaultVideoController::setGlobalZOrder(float globalZOrder)
{
    Widget::setGlobalZOrder(globalZOrder);
    updateControlsGlobalZ(globalZOrder);
}

void DefaultVideoController::updateControllerState()
{
    if (!_videoPlayer || !_controlsReady)
        return;

    auto state = _videoPlayer->getState();
    if (state == VideoPlayer::State::LOADING || state == VideoPlayer::State::CLOSED ||
        state == VideoPlayer::State::ERROR)
    {
        _playButton->setVisible(false);
        _pauseButton->setVisible(false);
        _stopButton->setVisible(false);
        _timelineTotal->setVisible(false);
        _fullscreenExitButton->setVisible(false);
        _fullscreenEnterButton->setVisible(false);
    }
    else
    {
        _timelineTotal->setVisible(true);
        _fullscreenExitButton->setVisible(_videoPlayer->isFullscreen());
        _fullscreenEnterButton->setVisible(!_videoPlayer->isFullscreen());

        switch (state)
        {
        case VideoPlayer::State::PLAYING:
            _playButton->setVisible(false);
            _pauseButton->setVisible(true);
            _stopButton->setVisible(true);
            break;
        case VideoPlayer::State::PAUSED:
            _playButton->setVisible(true);
            _pauseButton->setVisible(false);
            _stopButton->setVisible(true);
            break;
        case VideoPlayer::State::STOPPED:
        case VideoPlayer::State::FINISHED:
            _playButton->setVisible(true);
            _pauseButton->setVisible(false);
            _stopButton->setVisible(false);
            break;
        default:;
        }
    }
}

void DefaultVideoController::setTimelineBarHeight(float height)
{
    _timelineBarHeight = height;
    if (_timelineBarHeight < TIMELINE_BAR_HEIGHT)
        _timelineBarHeight = TIMELINE_BAR_HEIGHT;

    updateControlsForContentSize(getContentSize());
}

void DefaultVideoController::createControls()
{
    createVideoControlTexture();

    // Check if controls are already created
    if (_controlsReady)
    {
        return;
    }

    const auto& contentSize = getContentSize();
    auto scale              = Director::getInstance()->getRenderView()->getScaleY();

    _mediaOverlay = Layout::create();
    _mediaOverlay->setBackGroundColor(Color32::black);
    _mediaOverlay->setBackGroundColorType(Layout::BackGroundColorType::SOLID);
    _mediaOverlay->setAnchorPoint(Anchors::center);
    _mediaOverlay->setPositionNormalized(Vec2(0.5f, 0.5f));
    _mediaOverlay->setContentSize(contentSize);
    _mediaOverlay->setOpacity(0);
    addProtectedChild(_mediaOverlay, 1);

    _controlPanel = Widget::create();
    _controlPanel->setContentSize(contentSize);
    _controlPanel->setPositionNormalized(Vec2(0.5f, 0.5f));
    _controlPanel->setAnchorPoint(Anchors::center);
    _controlPanel->setCascadeOpacityEnabled(true);
    _controlPanel->setOpacity(0);
    addProtectedChild(_controlPanel, 10);

    _primaryButtonPanel = Widget::create();
    _primaryButtonPanel->setContentSize(Vec2(150, 60));
    _primaryButtonPanel->setAnchorPoint(Anchors::center);
    _primaryButtonPanel->setPositionNormalized(Vec2(0.5f, 0.5f));
    _primaryButtonPanel->setScale(1 / scale);
    _controlPanel->addProtectedChild(_primaryButtonPanel);

    _playButton = VideoPlayerControl::create(SpriteFrame::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[VideoControlButtonId::Play]));
    _playButton->addClickEventListener([this](Object* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _playRate = 1.f;
        _videoPlayer->setPlayRate(_playRate);
        _videoPlayer->play();
        updateControllerState();
    });
    _playButton->setPositionNormalized(Vec2(0.25f, 0.5f));
    _playButton->setCascadeOpacityEnabled(true);
    _playButton->setVisible(false);
    _primaryButtonPanel->addProtectedChild(_playButton, 1, -1);

    _stopButton = VideoPlayerControl::create(SpriteFrame::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[VideoControlButtonId::Stop]));
    _stopButton->addClickEventListener([this](Object* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _playRate = 1.f;
        _videoPlayer->setPlayRate(_playRate);
        _videoPlayer->stop();
        updateControllerState();
    });
    _stopButton->setPositionNormalized(Vec2(0.75f, 0.5f));
    _stopButton->setCascadeOpacityEnabled(true);
    _stopButton->setVisible(false);
    _primaryButtonPanel->addProtectedChild(_stopButton, 1, -1);

    _pauseButton = VideoPlayerControl::create(SpriteFrame::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[VideoControlButtonId::Pause]));
    _pauseButton->addClickEventListener([this](Object* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _playRate = 1.f;
        _videoPlayer->setPlayRate(_playRate);
        _videoPlayer->pause();
        updateControllerState();
    });
    _pauseButton->setPositionNormalized(Vec2(0.25f, 0.5f));
    _pauseButton->setCascadeOpacityEnabled(true);
    _pauseButton->setVisible(false);
    _primaryButtonPanel->addProtectedChild(_pauseButton, 1, -1);

    _fullscreenEnterButton = VideoPlayerControl::create(SpriteFrame::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[VideoControlButtonId::EnterFullscreen]));
    _fullscreenEnterButton->addClickEventListener([this](Object* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _videoPlayer->setFullscreen(true);
        updateControllerState();
    });
    _fullscreenEnterButton->setAnchorPoint(Anchors::topLeft);
    _fullscreenEnterButton->setPositionNormalized(Vec2(0.03f, 0.97f));
    _fullscreenEnterButton->setCascadeOpacityEnabled(true);
    _fullscreenEnterButton->setVisible(false);
    _fullscreenEnterButton->setScale(1 / scale);
    _controlPanel->addProtectedChild(_fullscreenEnterButton, 1, -1);

    _fullscreenExitButton = VideoPlayerControl::create(SpriteFrame::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[VideoControlButtonId::ExitFullscreen]));
    _fullscreenExitButton->addClickEventListener([this](Object* ref) {
        if (_controlPanel->getOpacity() <= 50)
            return;
        _videoPlayer->setFullscreen(false);
        updateControllerState();
    });
    _fullscreenExitButton->setAnchorPoint(Anchors::topLeft);
    _fullscreenExitButton->setPositionNormalized(Vec2(0.03f, 0.97f));
    _fullscreenExitButton->setCascadeOpacityEnabled(true);
    _fullscreenExitButton->setVisible(false);
    _fullscreenExitButton->setScale(1 / scale);
    _controlPanel->addProtectedChild(_fullscreenExitButton, 1, 1033);

    _timelineTotal = utils::createSpriteFromBase64Cached(BODY_IMAGE_1_PIXEL_HEIGHT, BODY_IMAGE_1_PIXEL_HEIGHT_KEY);
    _timelineTotal->setAnchorPoint(Anchors::bottomCenter);
    _timelineTotal->setStretchEnabled(true);
    _timelineTotal->setPositionNormalized(Vec2(0.5f, 0.1f));
    _timelineTotal->setColor(Color32::gray);
    _timelineTotal->setVisible(false);
    _timelineTotal->setCascadeOpacityEnabled(true);
    _timelineTotal->setContentSize(Size(contentSize.width - 40, _timelineBarHeight / scale));
    _controlPanel->addProtectedChild(_timelineTotal, 1);

    _timelinePlayed = utils::createSpriteFromBase64Cached(BODY_IMAGE_1_PIXEL_HEIGHT, BODY_IMAGE_1_PIXEL_HEIGHT_KEY);
    _timelinePlayed->setAnchorPoint(Anchors::leftCenter);
    _timelinePlayed->setStretchEnabled(true);
    _timelinePlayed->setPositionNormalized(Vec2(0.0f, 0.5f));
    _timelinePlayed->setColor(Color32::white);
    _timelinePlayed->setCascadeOpacityEnabled(true);
    _timelineTotal->addChild(_timelinePlayed, 5);

    _timelineSelector = Sprite::createWithTexture(
        g_mediaControlsTexture, g_mediaControlTextureRegions[VideoControlButtonId::TimelineSliderButton]);
    _timelineSelector->setAnchorPoint(Anchors::center);
    _timelineSelector->setPositionNormalized(Vec2(1.f, 0.5f));
    _timelineSelector->setCascadeOpacityEnabled(true);
    _timelineSelector->setStretchEnabled(true);
    _timelineSelector->setContentSize(Size(_timelineBarHeight, _timelineBarHeight) * 1.5f / scale);
    _timelineSelector->setVisible(false);
    _timelinePlayed->addChild(_timelineSelector, 10);

    _timelineTouchListener                = PointerEventListener::create();
    _timelineTouchListener->onPointerDown = [this](PointerEvent* event) -> bool {
        auto target               = event->getCurrentTarget();
        const auto locationInNode = target->convertToNodeSpace(event->getLocation());
        const auto& size          = target->getContentSize();
        const auto rect           = ax::Rect(0, 0, size.width, size.height);

        if (rect.containsPoint(locationInNode))
        {
            auto percent  = locationInNode.x / rect.size.x;
            auto duration = _videoPlayer->getDuration();
            auto newTime  = percent * duration;
            _videoPlayer->seekTo(newTime);
            _timelineSelector->setVisible(true);
            return true;
        }

        return false;
    };
    _timelineTouchListener->onPointerMove = [this](PointerEvent* event) {
        auto target               = event->getCurrentTarget();
        const auto locationInNode = target->convertToNodeSpace(event->getLocation());
        const auto& size          = target->getContentSize();
        const auto rect           = ax::Rect(0, 0, size.width, size.height);

        if (rect.containsPoint(locationInNode))
        {
            auto percent  = locationInNode.x / rect.size.x;
            auto duration = _videoPlayer->getDuration();
            auto newTime  = percent * duration;
            _videoPlayer->seekTo(newTime);
        }
        return true;
    };
    _timelineTouchListener->onPointerUp = [this](PointerEvent* /*event*/) { _timelineSelector->setVisible(false); };
    getEventDispatcher()->addEventListenerWithSceneGraphPriority(_timelineTouchListener, _timelineTotal);

    _controlsReady = true;
}

void DefaultVideoController::updateControlsGlobalZ(float globalZOrder)
{
    if (!_videoPlayer || !_controlsReady)
        return;

    _controlPanel->setGlobalZOrder(globalZOrder);
    _timelineTotal->setGlobalZOrder(globalZOrder);
    _timelinePlayed->setGlobalZOrder(globalZOrder);
    _timelineSelector->setGlobalZOrder(globalZOrder);
}

void DefaultVideoController::updateControls()
{
    if (_videoPlayer && _controlsReady)
    {
        const auto currentTime = _videoPlayer->getCurrentTime();
        const auto duration    = _videoPlayer->getDuration();
        auto& totalSize        = _timelineTotal->getContentSize();
        _timelinePlayed->setContentSize(Size(totalSize.width * (currentTime / duration), totalSize.height));
    }
}

void DefaultVideoController::updateControlsForContentSize(const Vec2& contentSize)
{
    if (!_controlsReady)
        return;

    _mediaOverlay->setContentSize(contentSize);
    _controlPanel->setContentSize(contentSize);

    auto scale = Director::getInstance()->getRenderView()->getScaleY();
    _primaryButtonPanel->setScale(1 / scale);
    _timelineTotal->setContentSize(Size(contentSize.width - 40, _timelineBarHeight / scale));
    _timelineSelector->setContentSize(Size(_timelineBarHeight, _timelineBarHeight) * 1.5f / scale);
    _fullscreenEnterButton->setScale(1 / scale);
    _fullscreenExitButton->setScale(1 / scale);

    _fullscreenEnterButton->setPositionNormalized(Vec2());
    _fullscreenEnterButton->setPositionNormalized(Vec2(0.03f, 0.97f));
    _fullscreenExitButton->setPositionNormalized(Vec2());
    _fullscreenExitButton->setPositionNormalized(Vec2(0.03f, 0.97f));
}

VideoPlayer::VideoPlayer()
{
    auto pvd      = new PrivateVideoContext{};
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
                    onPlayEvent(EventType::PLAYING);
                break;

            case MEMediaEventType::Paused:
                onPlayEvent(EventType::PAUSED);
                break;

            case MEMediaEventType::Stopped:
                onPlayEvent(pvd->_engine->isPlaybackEnded() ? EventType::COMPLETED : EventType::STOPPED);
                break;

            /* Raised by a media source when a presentation ends. This event signals that all streams in the
            presentation are complete. The Media Session forwards this event to the application. */
            // case MEEndOfPresentation:
            //    onPlayEvent(EventType::COMPLETED);
            //    break;

            /* Raised by the Media Session when it has finished playing the last presentation in the playback queue.
             * We send complete event at this case
             */
            // case MEMediaEventType::Stopped:
            //     onPlayEvent(EventType::COMPLETED);
            //     break;
            case MEMediaEventType::Error:
                onPlayEvent(EventType::ERROR);
                break;
            }
        }, [this, pvd](const ax::MEVideoFrame& frame) {
            pvd->updatePixelDesc(frame);
            pvd->renderFrame(frame);
        });
    }
    else
    {
        AXLOGE("Create VideoPlayer backend failed");
    }
}

VideoPlayer::~VideoPlayer()
{
    auto pvd = reinterpret_cast<PrivateVideoContext*>(_videoContext);

    removeAllProtectedChildren();

    AX_SAFE_RELEASE_NULL(_videoController);

    if (pvd->_engine)
    {
        pvd->closePlayer();
        _meFactory->destroyMediaEngine(pvd->_engine);
    }
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

bool VideoPlayer::init()
{
    if (!Widget::init())
    {
        return false;
    }

    if (_userInputEnabled)
    {
        setVideoController(DefaultVideoController::create(this));
    }

    return true;
}

void VideoPlayer::setFileName(std::string_view fileName)
{
    auto fullPath = FileUtils::getInstance()->fullPathForFilename(fileName);
    if (ax::utils::filePathToUrl(std::forward<std::string>(fullPath)) != _videoURL)
    {
        reinterpret_cast<PrivateVideoContext*>(_videoContext)->closePlayer();
        _videoURL = std::move(fullPath);
    }
    _videoSource = VideoPlayer::Source::FILENAME;
}

void VideoPlayer::setURL(std::string_view videoUrl)
{
    if (_videoURL != videoUrl)
    {
        reinterpret_cast<PrivateVideoContext*>(_videoContext)->closePlayer();
        _videoURL = videoUrl;
    }
    _videoSource = VideoPlayer::Source::URL;
}

void VideoPlayer::setLooping(bool looping)
{
    _isLooping = looping;

    auto pvd = reinterpret_cast<PrivateVideoContext*>(_videoContext);
    if (pvd->_engine)
        pvd->_engine->setLoop(looping);
}

void VideoPlayer::setUserInputEnabled(bool enableInput)
{
    _userInputEnabled = enableInput;
    if (_videoController)
    {
        _videoController->setEnabled(_userInputEnabled);
    }
    else if (_userInputEnabled)
    {
        setVideoController(DefaultVideoController::create(this));
    }
}

void VideoPlayer::setStyle(StyleType style)
{
    _styleType = style;
}

Node* VideoPlayer::getRenderNode()
{
    auto pvd = reinterpret_cast<PrivateVideoContext*>(_videoContext);
    return pvd->_vrender;
}

void VideoPlayer::draw(Renderer* renderer, const Mat4& transform, uint32_t flags)
{
    ax::ui::Widget::draw(renderer, transform, flags);

    auto pvd     = reinterpret_cast<PrivateVideoContext*>(_videoContext);
    auto vrender = pvd->_vrender;
    auto engine  = pvd->_engine;
    if (!vrender || !engine)
        return;

    if (isPlaying())
    {  // render 1 video sample if avaiable
        engine->transferVideoFrame();
    }
    if (pvd->_scaleDirty || (flags & FLAGS_TRANSFORM_DIRTY))
        pvd->rescaleTo(this);

#    if AX_VIDEOPLAYER_DEBUG_DRAW
    _debugDrawNode->clear();
    auto size         = getContentSize();
    Point vertices[4] = {Point::zero, Point(size.width, 0), Point(size.width, size.height), Point(0, size.height)};
    _debugDrawNode->drawPoly(vertices, 4, true, Color32::white);
#    endif
}

void VideoPlayer::setContentSize(const Size& contentSize)
{
    Widget::setContentSize(contentSize);
    auto videoContext               = reinterpret_cast<PrivateVideoContext*>(_videoContext);
    videoContext->_originalViewSize = contentSize;
    if (_videoController)
        _videoController->setContentSize(contentSize);
}

VideoPlayer::State VideoPlayer::getState() const
{
    if (_videoURL.empty())
        return State::CLOSED;

    auto engine = reinterpret_cast<PrivateVideoContext*>(_videoContext)->_engine;
    if (engine)
    {
        switch (engine->getState())
        {
        case MEMediaState::Closed:
            return State::CLOSED;
        case MEMediaState::Preparing:
            return State::LOADING;
        case MEMediaState::Playing:
            return State::PLAYING;
        case MEMediaState::Paused:
            return State::PAUSED;
        case MEMediaState::Stopped:
            return State::STOPPED;
        case MEMediaState::Error:
            return State::ERROR;
        }
    }

    return State::CLOSED;
}

void VideoPlayer::setVideoController(VideoController* controller)
{
    if (_videoController)
    {
        removeProtectedChild(_videoController, true);
        AX_SAFE_RELEASE(_videoController);
    }
    _videoController = controller;
    if (_videoController)
    {
        AX_SAFE_RETAIN(_videoController);
        _videoController->setPositionNormalized(Vec2(0.5f, 0.5f));
        _videoController->setAnchorPoint(Anchors::center);
        _videoController->setEnabled(_userInputEnabled);
        addProtectedChild(_videoController, 1);
    }
}

void VideoPlayer::setFullscreen(bool enabled)
{
    if (_fullscreen != enabled)
    {
        _fullscreen = enabled;

        auto pvd               = reinterpret_cast<PrivateVideoContext*>(_videoContext);
        const auto contentSize = enabled ? _director->getCanvasSize() : pvd->_originalViewSize;

        // Don't invoke this->setContentSize to avoid overwriting original view size.
        Widget::setContentSize(contentSize);

        if (_videoController)
            _videoController->setContentSize(contentSize);

        postEvent(EventType::FULLSCREEN_SWITCH);
    }
}

bool VideoPlayer::isFullscreen() const
{
    return _fullscreen;
}

void VideoPlayer::setKeepAspectRatio(bool enable)
{
    if (_keepAspectRatio != enable)
    {
        _keepAspectRatio                                                   = enable;
        reinterpret_cast<PrivateVideoContext*>(_videoContext)->_scaleDirty = true;
    }
}

void VideoPlayer::setPlayRate(float fRate)
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoContext*>(_videoContext)->_engine;
        if (engine)
            engine->setRate(fRate);
    }
}

void VideoPlayer::play()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoContext*>(_videoContext)->_engine;
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
            updateVideoController();
        }
    }
}

void VideoPlayer::pause()
{
    Widget::pause();
    pausePlayback();
}

void VideoPlayer::resume()
{
    resumePlayback();
    Widget::resume();
}

void VideoPlayer::pausePlayback()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoContext*>(_videoContext)->_engine;
        if (engine)
        {
            engine->pause();
            updateVideoController();
        }
    }
}

void VideoPlayer::resumePlayback()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoContext*>(_videoContext)->_engine;
        if (engine)
        {
            switch (engine->getState())
            {
            case MEMediaState::Stopped:
            case MEMediaState::Paused:
                engine->play();
            }

            updateVideoController();
        }
    }
}

void VideoPlayer::stop()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoContext*>(_videoContext)->_engine;
        if (engine)
        {
            engine->stop();
            updateVideoController();
        }
    }
}

void VideoPlayer::seekTo(float sec)
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoContext*>(_videoContext)->_engine;
        if (engine)
        {
            engine->setCurrentTime(sec);
            updateVideoController();
        }
    }
}

float VideoPlayer::getCurrentTime()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoContext*>(_videoContext)->_engine;
        if (engine)
        {
            return static_cast<float>(engine->getCurrentTime());
        }
    }

    return 0.f;
}

float VideoPlayer::getDuration()
{
    if (!_videoURL.empty())
    {
        auto engine = reinterpret_cast<PrivateVideoContext*>(_videoContext)->_engine;
        if (engine)
        {
            return static_cast<float>(engine->getDuration());
        }
    }

    return 0.f;
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
    return _userInputEnabled;
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

void VideoPlayer::addEventListener(const VideoPlayerCallback& callback)
{
    _eventCallback = callback;
}

void VideoPlayer::onPlayEvent(EventType eventType)
{
    _isPlaying = (eventType == EventType::PLAYING);

    postEvent(eventType);
}

void VideoPlayer::postEvent(EventType eventType)
{
    if (_eventCallback)
    {
        RefPtr<VideoPlayer> guard(this);
        _director->postTask([guard, eventType]() {
            if (guard->_eventCallback)
                guard->_eventCallback(guard, eventType);
        });
    }
}

ax::ui::Widget* VideoPlayer::createCloneInstance()
{
    return VideoPlayer::create();
}

void VideoPlayer::copySpecialProperties(Widget* widget)
{
    VideoPlayer* player = dynamic_cast<VideoPlayer*>(widget);
    if (player)
    {
        _isPlaying        = player->_isPlaying;
        _isLooping        = player->_isLooping;
        _userInputEnabled = player->_userInputEnabled;
        _styleType        = player->_styleType;
        _fullscreen       = player->_fullscreen;
        _fullscreenDirty  = player->_fullscreenDirty;
        _videoURL         = player->_videoURL;
        _keepAspectRatio  = player->_keepAspectRatio;
        _videoSource      = player->_videoSource;
        _eventCallback    = player->_eventCallback;
    }
}

void VideoPlayer::updateVideoController()
{
    if (!_userInputEnabled || !_videoController)
    {
        return;
    }

    _videoController->updateControllerState();
}

#endif
