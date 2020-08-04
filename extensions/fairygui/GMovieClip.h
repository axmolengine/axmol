#ifndef __GMOVIECLIP_H__
#define __GMOVIECLIP_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"

NS_FGUI_BEGIN

class ActionMovieClip;

class GMovieClip : public GObject
{
public:
    GMovieClip();
    virtual ~GMovieClip();

    CREATE_FUNC(GMovieClip);

    bool isPlaying() const { return _playing; }
    void setPlaying(bool value);

    int getFrame() const;
    void setFrame(int value);

    float getTimeScale() const;
    void setTimeScale(float value);

    void advance(float time);

    FlipType getFlip() const;
    void setFlip(FlipType value);

    cocos2d::Color3B getColor() const { return _content->getColor(); }
    void setColor(const cocos2d::Color3B& value);

    //from start to end(-1 means ending) repeat times(0 means infinite loop) when all is over, stopping at endAt(-1 means same value of end)
    void setPlaySettings(int start = 0, int end = -1, int times = 0, int endAt = -1, std::function<void()> completeCallback = nullptr);

    virtual void constructFromResource() override;

    virtual cocos2d::Value getProp(ObjectPropID propId) override;
    virtual void setProp(ObjectPropID propId, const cocos2d::Value& value) override;

protected:
    virtual void handleInit() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;
    virtual void handleGrayedChanged() override;

private:
    cocos2d::Sprite* _content;
    ActionMovieClip* _playAction;
    bool _playing;
};

class ActionMovieClip : public cocos2d::Action
{
public:
    static ActionMovieClip* create(cocos2d::Animation *animation, float repeatDelay = 0, bool swing =false);
    ActionMovieClip();
    ~ActionMovieClip();

    virtual bool isDone() const override;
    virtual void step(float dt) override;
    virtual void startWithTarget(cocos2d::Node *target) override;
    virtual ActionMovieClip* reverse() const override;
    virtual ActionMovieClip* clone() const override;

    int getFrame() { return _frame; }
    void setFrame(int value);

    float getTimeScale() const { return _timeScale; }
    void setTimeScale(float value) { _timeScale = value; }

    void advance(float time);

    void setPlaySettings(int start, int end, int times, int endAt, std::function<void()> completeCallback = nullptr);
    void setAnimation(cocos2d::Animation *animation, float repeatDelay = 0, bool swing = false);

private:
    void drawFrame();

    cocos2d::Animation* _animation;
    int _frame;
    float _frameElapsed;
    int _repeatedCount;
    bool _reversed;
    float _timeScale;

    float _repeatDelay;
    bool _swing;
    std::function<void()> _completeCallback;
    int _displayFrame;

    int _start;
    int _end;
    int _times;
    int _endAt;
    int _status; //0-none, 1-next loop, 2-ending, 3-ended

    CC_DISALLOW_COPY_AND_ASSIGN(ActionMovieClip);
};

NS_FGUI_END

#endif
