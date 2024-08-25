#include "GMovieClip.h"
#include "PackageItem.h"
#include "display/FUISprite.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

GMovieClip::GMovieClip()
    : _playAction(nullptr),
    _content(nullptr),
    _playing(true)
{
    _sizeImplType = 1;
    _touchDisabled = true;
}

GMovieClip::~GMovieClip()
{
    AX_SAFE_RELEASE(_playAction);
}

void GMovieClip::handleInit()
{
    _content = FUISprite::create();
    _content->retain();

    _playAction = ActionMovieClip::create(nullptr);
    _playAction->retain();

    _displayObject = _content;
}

void GMovieClip::setPlaySettings(int start, int end, int times, int endAt, std::function<void()> completeCallback)
{
    _playAction->setPlaySettings(start, end, times, endAt, completeCallback);
}

void GMovieClip::setPlaying(bool value)
{
    if (_playing != value)
    {
        _playing = value;
        if (_playing)
            _content->runAction(_playAction);
        else
            _content->stopAction(_playAction);
    }
}

int GMovieClip::getFrame() const
{
    return _playAction->getFrame();
}
void GMovieClip::setFrame(int value)
{
    _playAction->setFrame(value);
}

float GMovieClip::getTimeScale() const
{
    return _playAction->getTimeScale();
}
void GMovieClip::setTimeScale(float value)
{
    _playAction->setTimeScale(value);
}

void GMovieClip::advance(float time)
{
    _playAction->advance(time);
}

FlipType GMovieClip::getFlip() const
{
    if (_content->isFlippedX() && _content->isFlippedY())
        return FlipType::BOTH;
    else if (_content->isFlippedX())
        return FlipType::HORIZONTAL;
    else if (_content->isFlippedY())
        return FlipType::VERTICAL;
    else
        return FlipType::NONE;
}

void GMovieClip::setFlip(FlipType value)
{
    _content->setFlippedX(value == FlipType::HORIZONTAL || value == FlipType::BOTH);
    _content->setFlippedY(value == FlipType::VERTICAL || value == FlipType::BOTH);
}

void GMovieClip::setColor(const ax::Color3B& value)
{
    _content->setColor(value);
}

void GMovieClip::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

    ((FUISprite*)_content)->setGrayed(_finalGrayed);
}

ax::Value GMovieClip::getProp(ObjectPropID propId)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        return Value(ToolSet::colorToInt(getColor()));
    case ObjectPropID::Playing:
        return Value(isPlaying());
    case ObjectPropID::Frame:
        return Value(getFrame());
    case ObjectPropID::TimeScale:
        return Value(_playAction->getTimeScale());
    default:
        return GObject::getProp(propId);
    }
}

void GMovieClip::setProp(ObjectPropID propId, const ax::Value& value)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        setColor(ToolSet::intToColor(value.asUnsignedInt()));
        break;
    case ObjectPropID::Playing:
        setPlaying(value.asBool());
        break;
    case ObjectPropID::Frame:
        setFrame(value.asInt());
        break;
    case ObjectPropID::TimeScale:
        _playAction->setTimeScale(value.asFloat());
        break;
    case ObjectPropID::DeltaTime:
        _playAction->advance(value.asFloat());
        break;
    default:
        GObject::setProp(propId, value);
        break;
    }
}

void GMovieClip::constructFromResource()
{
    PackageItem* contentItem = _packageItem->getBranch();
    sourceSize.width = contentItem->width;
    sourceSize.height = contentItem->height;
    initSize = sourceSize;

    contentItem = contentItem->getHighResolution();
    contentItem->load();

    _playAction->setAnimation(contentItem->animation, contentItem->repeatDelay, contentItem->swing);
    _content->runAction(_playAction);

    setSize(sourceSize.width, sourceSize.height);
}

void GMovieClip::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 5);

    if (buffer->readBool())
        setColor((Color3B)buffer->readColor());
    setFlip((FlipType)buffer->readByte());
    setFrame(buffer->readInt());
    setPlaying(buffer->readBool());
}

ActionMovieClip::ActionMovieClip() : _animation(nullptr),
_frame(0),
_frameElapsed(0),
_repeatedCount(0),
_repeatDelay(0),
_start(0),
_end(0),
_times(0),
_endAt(0),
_status(0),
_displayFrame(-1),
_swing(false),
_reversed(false),
_timeScale(1)
{
}

ActionMovieClip::~ActionMovieClip()
{
    AX_SAFE_RELEASE(_animation);
}

ActionMovieClip* ActionMovieClip::create(ax::Animation* animation, float repeatDelay, bool swing)
{
    ActionMovieClip* action = new ActionMovieClip();
    action->setAnimation(animation, repeatDelay, swing);
    action->autorelease();
    return action;
}

bool ActionMovieClip::isDone() const
{
    return false;
}

void ActionMovieClip::step(float dt)
{
    if (_animation == nullptr)
        return;

    auto frames = _animation->getFrames();
    int frameCount = (int)frames.size();
    if (frameCount == 0 || _status == 3)
        return;

    if (_timeScale != 1)
        dt *= _timeScale;

    _frameElapsed += dt;
    float tt = (frames.at(_frame)->getDelayUnits() + ((_frame == 0 && _repeatedCount > 0) ? _repeatDelay : 0)) * _animation->getDelayPerUnit();
    if (_frameElapsed < tt)
        return;

    _frameElapsed -= tt;
    if (_frameElapsed > _animation->getDelayPerUnit())
        _frameElapsed = _animation->getDelayPerUnit();

    if (_swing)
    {
        if (_reversed)
        {
            _frame--;
            if (_frame <= 0)
            {
                _frame = 0;
                _repeatedCount++;
                _reversed = !_reversed;
            }
        }
        else
        {
            _frame++;
            if (_frame > frameCount - 1)
            {
                _frame = MAX(0, frameCount - 2);
                _repeatedCount++;
                _reversed = !_reversed;
            }
        }
    }
    else
    {
        _frame++;
        if (_frame > frameCount - 1)
        {
            _frame = 0;
            _repeatedCount++;
        }
    }

    if (_status == 1) //new loop
    {
        _frame = _start;
        _frameElapsed = 0;
        _status = 0;
    }
    else if (_status == 2) //ending
    {
        _frame = _endAt;
        _frameElapsed = 0;
        _status = 3; //ended

        if (_completeCallback)
            _completeCallback();
    }
    else
    {
        if (_frame == _end)
        {
            if (_times > 0)
            {
                _times--;
                if (_times == 0)
                    _status = 2; //ending
                else
                    _status = 1; //new loop
            }
            else if (_start != 0)
                _status = 1; //new loop
        }
    }

    drawFrame();
}

void ActionMovieClip::startWithTarget(Node* target)
{
    Action::startWithTarget(target);

    drawFrame();
}

ActionMovieClip* ActionMovieClip::reverse() const
{
    AX_ASSERT(0);
    return nullptr;
}

ActionMovieClip* ActionMovieClip::clone() const
{
    return ActionMovieClip::create(_animation->clone(), _repeatDelay, _swing);
}

void ActionMovieClip::setFrame(int value)
{
    if (_animation == nullptr)
        return;

    auto frames = _animation->getFrames();
    int frameCount = (int)frames.size();

    if (value >= frameCount)
        value = frameCount - 1;

    _frame = value;
    _frameElapsed = 0;
    _displayFrame = -1;
    drawFrame();
}

void ActionMovieClip::advance(float time)
{
    if (_animation == nullptr)
        return;

    auto frames = _animation->getFrames();
    int frameCount = (int)frames.size();
    if (frameCount == 0)
        return;

    int beginFrame = _frame;
    bool beginReversed = _reversed;
    float backupTime = time;
    while (true)
    {
        float tt = (frames.at(_frame)->getDelayUnits() + ((_frame == 0 && _repeatedCount > 0) ? _repeatDelay : 0)) * _animation->getDelayPerUnit();
        if (time < tt)
        {
            _frameElapsed = 0;
            break;
        }

        time -= tt;

        if (_swing)
        {
            if (_reversed)
            {
                _frame--;
                if (_frame <= 0)
                {
                    _frame = 0;
                    _repeatedCount++;
                    _reversed = !_reversed;
                }
            }
            else
            {
                _frame++;
                if (_frame > frameCount - 1)
                {
                    _frame = MAX(0, frameCount - 2);
                    _repeatedCount++;
                    _reversed = !_reversed;
                }
            }
        }
        else
        {
            _frame++;
            if (_frame > frameCount - 1)
            {
                _frame = 0;
                _repeatedCount++;
            }
        }

        if (_frame == beginFrame && _reversed == beginReversed)
        {
            float roundTime = backupTime - time;
            time -= (int)floor(time / roundTime) * roundTime;
        }
    }
}

void ActionMovieClip::setPlaySettings(int start, int end, int times, int endAt, std::function<void()> completeCallback)
{
    auto frames = _animation->getFrames();
    int frameCount = (int)frames.size();

    _start = start;
    _end = end;
    if (_end == -1 || _end > frameCount - 1)
        _end = frameCount - 1;
    _times = times;
    _endAt = endAt;
    if (_endAt == -1)
        _endAt = _end;
    _status = 0;
    _completeCallback = completeCallback;

    setFrame(start);
}

void ActionMovieClip::setAnimation(ax::Animation* animation, float repeatDelay, bool swing)
{
    if (_animation != animation)
    {
        AX_SAFE_RETAIN(animation);
        AX_SAFE_RELEASE(_animation);
        _animation = animation;
    }

    if (_animation == nullptr)
        return;

    _repeatDelay = repeatDelay;
    _swing = swing;
    _completeCallback = nullptr;

    auto frames = _animation->getFrames();
    int frameCount = (int)frames.size();
    if (_end == -1 || _end > frameCount - 1)
        _end = frameCount - 1;
    if (_endAt == -1 || _endAt > frameCount - 1)
        _endAt = frameCount - 1;

    if (_frame < 0 || _frame > frameCount - 1)
        _frame = frameCount - 1;

    _displayFrame = -1;
    _frameElapsed = 0;
    _repeatedCount = 0;
    _reversed = false;
}

void ActionMovieClip::drawFrame()
{
    if (_target == nullptr || _animation == nullptr)
        return;

    auto frames = _animation->getFrames();
    int frameCount = (int)frames.size();

    if (_frame == _displayFrame || frameCount == 0)
        return;

    _displayFrame = _frame;
    AnimationFrame* frame = frames.at(_frame);
    //auto blend = static_cast<Sprite*>(_target)->getBlendFunc();
    static_cast<Sprite*>(_target)->setSpriteFrame(frame->getSpriteFrame());
    //static_cast<Sprite*>(_target)->setBlendFunc(blend);
}

NS_FGUI_END
