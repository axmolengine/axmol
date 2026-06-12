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
#pragma once

#if defined(AX_ENABLE_VIDEO)

#    include "axmol/ui/Button.h"
#    include "axmol/ui/Widget.h"
#    include "axmol/ui/LayoutGroup.h"
#    include "axmol/2d/Sprite.h"
#    include <chrono>

#    if AX_VIDEOPLAYER_DEBUG_DRAW
#        include "axmol/2d/DrawNode.h"
#    endif

#    undef ERROR

/**
 * @addtogroup ui
 * @{
 */

namespace ax
{
namespace ui
{

class VideoPlayer;

class AX_GUI_DLL VideoController : public ax::ui::Widget
{
public:
    enum class Orientation
    {
        Default,
        RotatedLeft,
        RotatedRight,
    };

    explicit VideoController(VideoPlayer* player) : _videoPlayer(player) {}

    virtual void updateControllerState()            = 0;
    virtual void setTimelineBarHeight(float height) = 0;

    void setOrientation(Orientation orientation);
    Orientation getOrientation() const { return _orientation; }

protected:
    VideoPlayer* _videoPlayer = nullptr;
    Orientation _orientation  = Orientation::Default;
};

class VideoPlayerControl : public ax::ui::Button
{
public:
    static VideoPlayerControl* create(SpriteFrame* frame);

    VideoPlayerControl() = default;
    ~VideoPlayerControl() override;

    virtual bool init(SpriteFrame* frame);

    void onSizeChanged() override;
    Vec2 resolvePreferredSize(const Vec2& /*sizeHint*/) const override;
    Vec2 getNormalSize() const override;

    void onPressStateChangedToNormal() override;
    void onPressStateChangedToPressed() override;
    void onPressStateChangedToDisabled() override;

protected:
    Sprite* _overlay = nullptr;
};

class AX_GUI_DLL DefaultVideoController : public VideoController
{
public:
    explicit DefaultVideoController(VideoPlayer* player);

    static DefaultVideoController* create(VideoPlayer* mediaPlayer);

    bool init() override;
    void initRenderNode() override;

    void onPressStateChangedToPressed() override;
    void setContentSize(const Vec2& contentSize) override;
    void update(float delta) override;
    void onEnter() override;
    void setGlobalZOrder(float globalZOrder) override;

    void updateControllerState() override;
    void setTimelineBarHeight(float height) override;

    virtual void createControls();
    virtual void updateControlsGlobalZ(float globalZOrder);
    virtual void updateControls();
    virtual void updateControlsForContentSize(const Vec2& contentSize);

protected:
    Widget* _controlPanel = nullptr;

    VideoPlayerControl* _fullscreenEnterButton = nullptr;
    VideoPlayerControl* _fullscreenExitButton  = nullptr;
    VideoPlayerControl* _playButton            = nullptr;
    VideoPlayerControl* _stopButton            = nullptr;
    VideoPlayerControl* _pauseButton           = nullptr;

    Sprite* _timelineSelector   = nullptr;
    Sprite* _timelineTotal      = nullptr;
    Sprite* _timelinePlayed     = nullptr;
    LayoutGroup* _mediaOverlay  = nullptr;
    Widget* _primaryButtonPanel = nullptr;

    PointerEventListener* _timelineTouchListener = nullptr;
    float _playRate                              = 1.f;
    std::chrono::steady_clock::time_point _lastTouch;
    bool _controlsReady = false;
    float _timelineBarHeight;
};

/**
 * @class VideoPlayer
 * @brief Play a video file.
 *
 * @note VideoPlayer play a video file base on system widget.
 * It's mean VideoPlayer play a video file above all graphical elements of axmol.
 */
class AX_GUI_DLL VideoPlayer : public ax::ui::Widget
{
public:
    /**
     * VideoPlayer play event type.
     */
    enum class EventType
    {
        PLAYING = 0,
        PAUSED,
        STOPPED,
        COMPLETED,
        ERROR,
        FULLSCREEN_SWITCH
    };
    using VideoPlayerCallback = std::function<void(Object*, EventType)>;

    enum class State
    {
        CLOSED = 0,
        LOADING,
        PLAYING,
        PAUSED,
        STOPPED,
        FINISHED,
        ERROR
    };
    using MediaState = State;

    /**
     * Styles of how the the video player is presented
     * For now only used on iOS to use either MPMovieControlStyleEmbedded (DEFAULT) or
     * MPMovieControlStyleNone (NONE)
     */
    enum class StyleType
    {
        DEFAULT = 0,
        NONE
    };

    /**
     *Static create method for instancing a VideoPlayer.
     */
    CREATE_FUNC(VideoPlayer);

    bool init() override;

    /**
     * Sets a file path as a video source for VideoPlayer.
     */
    virtual void setFileName(std::string_view videoPath);

    /**
     * @brief Get the local video file name.
     *
     * @return The video file name.
     */
    virtual std::string_view getFileName() const { return _videoURL; }

    /**
     * Sets a URL as a video source for VideoPlayer.
     */
    virtual void setURL(std::string_view _videoURL);

    /**
     * @brief Get the URL of remoting video source.
     *
     * @return A remoting URL address.
     */
    virtual std::string_view getURL() const { return _videoURL; }

    /**
     * @brief Set if playback is done in loop mode
     *
     * @param looping the video will or not automatically restart at the end
     */
    virtual void setLooping(bool looping);

    /**
     * Set if the player will enable user input for basic pause and resume of video
     *
     * @param enableInput If true, input will be handled for basic functionality (pause/resume)
     */
    virtual void setUserInputEnabled(bool enableInput);

    /**
     * Set the style of the player
     *
     * @param style The corresponding style
     */
    virtual void setStyle(StyleType style);

    /**
     * Sets play rate, currently, only supported by windows
     *
     * @param fRate   the rate for play, default 1.0
     */
    virtual void setPlayRate(float fRate);

    /**
     * Starts playback.
     */
    virtual void play();

    /**
     * Pauses actions and playback.
     */
    void pause() override;

    /**
     * Resumes actions and playback.
     */
    void resume() override;

    /**
     * @brief Pauses playback
     *
     */
    void pausePlayback();

    /**
     * @brief Resumes playback
     *
     */
    void resumePlayback();

    /**
     * Stops playback.
     */
    virtual void stop();

    /**
     * Seeks to specified time position.
     *
     * @param sec   The offset in seconds from the start to seek to.
     */
    virtual void seekTo(float sec);

    /**
     * Gets the current video position.
     *
     * @return float The current position in seconds
     */
    virtual float getCurrentTime();

    /**
     * Gets total video duration
     *
     * @return float The duration in seconds
     */
    virtual float getDuration();

    /**
     * Checks whether the VideoPlayer is playing.
     *
     * @return True if currently playing, false otherwise.
     */
    virtual bool isPlaying() const;

    /**
     * Checks whether the VideoPlayer is set with looping mode.
     *
     * @return true if the videoplayer is set to loop, false otherwise.
     */
    virtual bool isLooping() const;

    /**
     * Checks whether the VideoPlayer is set to listen user input to resume and pause the video
     *
     * @return true if the videoplayer user input is set, false otherwise.
     */
    virtual bool isUserInputEnabled() const;

    /**
     * Causes the video player to keep aspect ratio or no when displaying the video.
     *
     * @param enable    Specify true to keep aspect ratio or false to scale the video until
     * both dimensions fit the visible bounds of the view exactly.
     */
    virtual void setKeepAspectRatio(bool enable);

    /**
     * Indicates whether the video player keep aspect ratio when displaying the video.
     */
    virtual bool isKeepAspectRatio() const { return _keepAspectRatio; }

    /**
     * Causes the video player to enter or exit full-screen mode.
     *
     * @param fullscreen    Specify true to enter full-screen mode or false to exit full-screen mode.
     */
    virtual void setFullscreen(bool fullscreen);

    /**
     * Indicates whether the video player is in full-screen mode.
     *
     * @return True if the video player is in full-screen mode, false otherwise.
     */
    virtual bool isFullscreen() const;

    /**
     * @brief A function which will be called when video is playing.
     *
     * @param event @see VideoPlayer::EventType.
     */
    virtual void onPlayEvent(EventType eventType);

    /**
     * Register a callback to be invoked when the video state is updated.
     */
    virtual void addEventListener(const VideoPlayerCallback& callback);

    void setVisible(bool visible) override;
    void draw(Renderer* renderer, const Mat4& transform, uint32_t flags) override;
    void onEnter() override;
    void onExit() override;

    void setContentSize(const Size& contentSize) override;

    /**
     * @brief Get current state of the video
     *
     * @return State
     */
    State getState() const;

    Node* getRenderNode() override;

    void setVideoController(VideoController* controller);
    VideoController* getVideoController() const { return _videoController; }

    VideoPlayer();
    ~VideoPlayer() override;

protected:
    ax::ui::Widget* createCloneInstance() override;
    void copySpecialProperties(Widget* model) override;
    virtual void updateVideoController();
    void postEvent(EventType eventType);

#    if AX_VIDEOPLAYER_DEBUG_DRAW
    DrawNode* _debugDrawNode;
#    endif

    enum class Source
    {
        FILENAME = 0,
        URL
    };

    bool _isPlaying        = false;
    bool _isLooping        = false;
    bool _fullscreenDirty  = false;
    bool _fullscreen       = false;
    bool _keepAspectRatio  = false;
    bool _userInputEnabled = false;

    StyleType _styleType = StyleType::DEFAULT;

    std::string _videoURL;
    Source _videoSource;

    void* _videoContext = nullptr;

    VideoController* _videoController = nullptr;
    VideoPlayerCallback _eventCallback;
};
using MediaPlayer = VideoPlayer;
}  // namespace ui

}  // namespace ax
// end group
/// @}
#endif
