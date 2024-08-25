#include "Transition.h"
#include "GComponent.h"
#include "GRoot.h"
#include "tween/GPath.h"
#include "tween/GTween.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;
using namespace std;

const int OPTION_IGNORE_DISPLAY_CONTROLLER = 1;
const int OPTION_AUTO_STOP_DISABLED = 2;
const int OPTION_AUTO_STOP_AT_END = 4;

class TValueBase
{
};

class TValue_Visible : public TValueBase
{
public:
    bool visible;
};

class TValue_Animation : public TValueBase
{
public:
    int frame;
    bool playing;
    bool flag;
};

class TValue_Sound : public TValueBase
{
public:
    std::string sound;
    float volume;
};

class TValue_Transition : public TValueBase
{
public:
    std::string transName;
    int playTimes;
    Transition* trans;
    float stopTime;
};

class TValue_Shake : public TValueBase
{
public:
    float amplitude;
    float duration;
    ax::Vec2 lastOffset;
    ax::Vec2 offset;
};

class TValue_Text : public TValueBase
{
public:
    std::string text;
};

class TValue : public TValueBase
{
public:
    float f1;
    float f2;
    float f3;
    float f4;
    bool b1;
    bool b2;
    bool b3;

    TValue();
    ax::Vec2 getVec2() const;
    void setVec2(const ax::Vec2& value);
    ax::Vec4 getVec4() const;
    void setVec4(const ax::Vec4& value);
    ax::Color4B getColor() const;
    void setColor(const ax::Color4B& value);
};

TValue::TValue()
{
    f1 = f2 = f3 = f4 = 0;
    b1 = b2 = true;
    b3 = false;
}

ax::Vec2 TValue::getVec2() const
{
    return ax::Vec2(f1, f2);
}

void TValue::setVec2(const ax::Vec2& value)
{
    f1 = value.x;
    f2 = value.y;
}

ax::Vec4 TValue::getVec4() const
{
    return ax::Vec4(f1, f2, f3, f4);
}

void TValue::setVec4(const ax::Vec4& value)
{
    f1 = value.x;
    f2 = value.y;
    f3 = value.z;
    f4 = value.w;
}

ax::Color4B TValue::getColor() const
{
    return ax::Color4B(f1, f2, f3, f4);
}

void TValue::setColor(const ax::Color4B& value)
{
    f1 = value.r;
    f2 = value.g;
    f3 = value.b;
    f4 = value.a;
}

class TweenConfig
{
public:
    float duration;
    EaseType easeType;
    int repeat;
    bool yoyo;

    TValue* startValue;
    TValue* endValue;
    GPath* path;

    string endLabel;
    Transition::TransitionHook endHook;

    TweenConfig();
    ~TweenConfig();
};

TweenConfig::TweenConfig()
    : startValue(nullptr),
    endValue(nullptr),
    path(nullptr)
{
    easeType = EaseType::QuadOut;
    startValue = new TValue();
    endValue = new TValue();
}

TweenConfig::~TweenConfig()
{
    AX_SAFE_DELETE(path);
    AX_SAFE_DELETE(startValue);
    AX_SAFE_DELETE(endValue);
}

class TransitionItem
{
public:
    float time;
    string targetId;
    TransitionActionType type;
    TweenConfig* tweenConfig;
    string label;
    TValueBase* value;
    Transition::TransitionHook hook;

    //running properties
    GTweener* tweener;
    GObject* target;
    uint32_t displayLockToken;

    TransitionItem(TransitionActionType aType);
    ~TransitionItem();
};

TransitionItem::TransitionItem(TransitionActionType aType)
    : time(0),
    type(aType),
    hook(nullptr),
    tweenConfig(nullptr),
    tweener(nullptr),
    target(nullptr),
    displayLockToken(0)
{
    switch (type)
    {
    case TransitionActionType::XY:
    case TransitionActionType::Size:
    case TransitionActionType::Scale:
    case TransitionActionType::Pivot:
    case TransitionActionType::Skew:
    case TransitionActionType::Alpha:
    case TransitionActionType::Rotation:
    case TransitionActionType::Color:
    case TransitionActionType::ColorFilter:
        value = new TValue();
        break;

    case TransitionActionType::Animation:
        value = new TValue_Animation();
        break;

    case TransitionActionType::Shake:
        value = new TValue_Shake();
        break;

    case TransitionActionType::Sound:
        value = new TValue_Sound();
        break;

    case TransitionActionType::Transition:
        value = new TValue_Transition();
        break;

    case TransitionActionType::Visible:
        value = new TValue_Visible();
        break;

    case TransitionActionType::Text:
    case TransitionActionType::Icon:
        value = new TValue_Text();
        break;

    default:
        value = nullptr;
        break;
    }
}

TransitionItem::~TransitionItem()
{
    if (tweener != nullptr)
    {
        tweener->kill();
        tweener = nullptr;
    }

    target = nullptr;
    hook = nullptr;
    if (tweenConfig != nullptr)
    {
        tweenConfig->endHook = nullptr;
        delete tweenConfig;
    }

    AX_SAFE_DELETE(value);
}

Transition::Transition(GComponent* owner)
    : _owner(owner),
    _totalTimes(0),
    _totalTasks(0),
    _playing(false),
    _paused(false),
    _ownerBaseX(0),
    _ownerBaseY(0),
    _onComplete(nullptr),
    _options(0),
    _reversed(false),
    _totalDuration(0),
    _autoPlay(false),
    _autoPlayDelay(0),
    _timeScale(1),
    _startTime(0),
    _endTime(0)
{
}

Transition::~Transition()
{
    if (_playing)
        GTween::kill(this); //delay start

    for (auto& item : _items)
        delete item;

    _playing = false;
    _onComplete = nullptr;
}

void Transition::play(PlayCompleteCallback callback)
{
    play(1, 0, 0, -1, callback, false);
}

void Transition::play(int times, float delay, PlayCompleteCallback callback)
{
    play(times, delay, 0, -1, callback, false);
}

void Transition::play(int times, float delay, float startTime, float endTime, PlayCompleteCallback callback)
{
    play(times, delay, startTime, endTime, callback, false);
}

void Transition::playReverse(PlayCompleteCallback callback)
{
    play(1, 0, 0, -1, callback, true);
}

void Transition::playReverse(int times, float delay, PlayCompleteCallback callback)
{
    play(times, delay, 0, -1, callback, true);
}

void Transition::play(int times, float delay, float startTime, float endTime, PlayCompleteCallback onComplete, bool reverse)
{
    stop(true, true);

    _totalTimes = times;
    _reversed = reverse;
    _startTime = startTime;
    _endTime = endTime;
    _playing = true;
    _paused = false;
    _onComplete = onComplete;

    int cnt = (int)_items.size();
    for (int i = 0; i < cnt; i++)
    {
        TransitionItem* item = _items[i];
        if (item->target == nullptr)
        {
            if (!item->targetId.empty())
                item->target = _owner->getChildById(item->targetId);
            else
                item->target = _owner;
        }
        else if (item->target != _owner && item->target->getParent() != _owner) //maybe removed
            item->target = nullptr;

        if (item->target != nullptr && item->type == TransitionActionType::Transition)
        {
            TValue_Transition* value = (TValue_Transition*)item->value;
            Transition* trans = dynamic_cast<GComponent*>(item->target)->getTransition(value->transName);
            if (trans == this)
                trans = nullptr;
            if (trans != nullptr)
            {
                if (value->playTimes == 0) //stop
                {
                    int j;
                    for (j = i - 1; j >= 0; j--)
                    {
                        TransitionItem* item2 = _items[j];
                        if (item2->type == TransitionActionType::Transition)
                        {
                            TValue_Transition* value2 = (TValue_Transition*)item2->value;
                            if (value2->trans == trans)
                            {
                                value2->stopTime = item->time - item2->time;
                                break;
                            }
                        }
                    }
                    if (j < 0)
                        value->stopTime = 0;
                    else
                        trans = nullptr; //no need to handle stop anymore
                }
                else
                    value->stopTime = -1;
            }
            value->trans = trans;
        }
    }

    if (delay == 0)
        onDelayedPlay();
    else
        GTween::delayedCall(delay)->setTarget(this)->onComplete(AX_CALLBACK_0(Transition::onDelayedPlay, this));
}

void Transition::changePlayTimes(int value)
{
    _totalTimes = value;
}

void Transition::setAutoPlay(bool autoPlay, int times, float delay)
{
    if (_autoPlay != autoPlay)
    {
        _autoPlay = autoPlay;
        _autoPlayTimes = times;
        _autoPlayDelay = delay;
        if (_autoPlay)
        {
            if (_owner->onStage())
                play(_autoPlayTimes, _autoPlayDelay, nullptr);
        }
        else
        {
            if (!_owner->onStage())
                stop(false, true);
        }
    }
}

void Transition::stop()
{
    stop(true, false);
}

void Transition::stop(bool setToComplete, bool processCallback)
{
    if (!_playing)
        return;

    _playing = false;
    _totalTasks = 0;
    _totalTimes = 0;
    PlayCompleteCallback func = _onComplete;
    _onComplete = nullptr;

    int cnt = (int)_items.size();
    if (_reversed)
    {
        for (int i = cnt - 1; i >= 0; i--)
        {
            TransitionItem* item = _items[i];
            if (item->target == nullptr)
                continue;

            stopItem(item, setToComplete);
        }
    }
    else
    {
        for (int i = 0; i < cnt; i++)
        {
            TransitionItem* item = _items[i];
            if (item->target == nullptr)
                continue;

            stopItem(item, setToComplete);
        }
    }
    if (processCallback && func != nullptr)
        func();
}

void Transition::stopItem(TransitionItem* item, bool setToComplete)
{
    if (item->displayLockToken != 0)
    {
        item->target->releaseDisplayLock(item->displayLockToken);
        item->displayLockToken = 0;
    }

    if (item->tweener != nullptr)
    {
        item->tweener->kill(setToComplete);
        item->tweener = nullptr;

        if (item->type == TransitionActionType::Shake && !setToComplete)
        {
            item->target->_gearLocked = true;
            item->target->setPosition(item->target->getX() - ((TValue_Shake*)item->value)->lastOffset.x, item->target->getY() - ((TValue_Shake*)item->value)->lastOffset.y);
            item->target->_gearLocked = false;
        }
    }
}

void Transition::setPaused(bool paused)
{
    if (!_playing || _paused == paused)
        return;

    _paused = paused;
    GTweener* tweener = GTween::getTween(this);
    if (tweener != nullptr)
        tweener->setPaused(paused);

    for (auto& item : _items)
    {
        if (item->target == nullptr)
            continue;

        if (item->type == TransitionActionType::Transition)
        {
            if (((TValue_Transition*)item->value)->trans != nullptr)
                ((TValue_Transition*)item->value)->trans->setPaused(paused);
        }
        else if (item->type == TransitionActionType::Animation)
        {
            if (paused)
            {
                ((TValue_Animation*)item->value)->flag = item->target->getProp(ObjectPropID::Playing).asBool();
                item->target->setProp(ObjectPropID::Playing, Value(false));
            }
            else
                item->target->setProp(ObjectPropID::Playing, Value(((TValue_Animation*)item->value)->flag));
        }

        if (item->tweener != nullptr)
            item->tweener->setPaused(paused);
    }
}

void Transition::setValue(const std::string& label, const ValueVector& values)
{
    for (auto& item : _items)
    {
        void* value;
        if (item->label == label)
        {
            if (item->tweenConfig != nullptr)
                value = item->tweenConfig->startValue;
            else
                value = item->value;
        }
        else if (item->tweenConfig != nullptr && item->tweenConfig->endLabel == label)
        {
            value = item->tweenConfig->endValue;
        }
        else
            continue;

        switch (item->type)
        {
        case TransitionActionType::XY:
        case TransitionActionType::Size:
        case TransitionActionType::Pivot:
        case TransitionActionType::Scale:
        case TransitionActionType::Skew:
        {
            TValue* tvalue = (TValue*)value;
            tvalue->b1 = true;
            tvalue->b2 = true;
            tvalue->f1 = values[0].asFloat();
            tvalue->f2 = values[1].asFloat();
            break;
        }

        case TransitionActionType::Alpha:
        case TransitionActionType::Rotation:
            ((TValue*)value)->f1 = values[0].asFloat();
            ;
            break;

        case TransitionActionType::Color:
        {
            uint32_t v = values[0].asUnsignedInt();
            ((TValue*)value)->setColor(Color4B((v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF, (v >> 24) & 0xFF));
            break;
        }

        case TransitionActionType::Animation:
        {
            TValue_Animation* tvalue = (TValue_Animation*)value;
            tvalue->frame = values[0].asInt();
            if (values.size() > 1)
                tvalue->playing = values[1].asBool();
            break;
        }

        case TransitionActionType::Visible:
            ((TValue_Visible*)value)->visible = values[0].asBool();
            break;

        case TransitionActionType::Sound:
        {
            TValue_Sound* tvalue = (TValue_Sound*)value;
            tvalue->sound = values[0].asString();
            if (values.size() > 1)
                tvalue->volume = values[1].asFloat();
            break;
        }

        case TransitionActionType::Transition:
        {
            TValue_Transition* tvalue = (TValue_Transition*)value;
            tvalue->transName = values[0].asString();
            if (values.size() > 1)
                tvalue->playTimes = values[1].asInt();
            break;
        }

        case TransitionActionType::Shake:
        {
            ((TValue_Shake*)value)->amplitude = values[0].asFloat();
            if (values.size() > 1)
                ((TValue_Shake*)value)->duration = values[1].asFloat();
            break;
        }

        case TransitionActionType::ColorFilter:
        {
            TValue* tvalue = (TValue*)value;
            tvalue->f1 = values[0].asFloat();
            tvalue->f2 = values[1].asFloat();
            tvalue->f3 = values[2].asFloat();
            tvalue->f4 = values[3].asFloat();
            break;
        }

        case TransitionActionType::Text:
        case TransitionActionType::Icon:
            ((TValue_Text*)value)->text = values[0].asString();
            break;
        default:
            break;
        }
    }
}

void Transition::setHook(const std::string& label, TransitionHook callback)
{
    for (auto& item : _items)
    {
        if (item->label == label)
        {
            item->hook = callback;
            break;
        }
        else if (item->tweenConfig != nullptr && item->tweenConfig->endLabel == label)
        {
            item->tweenConfig->endHook = callback;
            break;
        }
    }
}

void Transition::clearHooks()
{
    for (auto& item : _items)
    {
        item->hook = nullptr;
        if (item->tweenConfig != nullptr)
            item->tweenConfig->endHook = nullptr;
    }
}

void Transition::setTarget(const std::string& label, GObject* newTarget)
{
    for (auto& item : _items)
    {
        if (item->label == label)
        {

            item->targetId = newTarget->id;
            item->target = nullptr;
        }
    }
}

void Transition::setDuration(const std::string& label, float value)
{
    for (auto& item : _items)
    {
        if (item->tweenConfig != nullptr && item->label == label)
            item->tweenConfig->duration = value;
    }
}

float Transition::getLabelTime(const string& label) const
{
    for (auto& item : _items)
    {
        if (item->label == label)
            return item->time;
        else if (item->tweenConfig != nullptr && item->label == label)
            return item->time + item->tweenConfig->duration;
    }

    return NAN;
}

void Transition::setTimeScale(float value)
{
    if (_timeScale != value)
    {
        _timeScale = value;

        for (auto& item : _items)
        {
            if (item->tweener != nullptr)
                item->tweener->setTimeScale(value);
            else if (item->type == TransitionActionType::Transition)
            {
                if (((TValue_Transition*)item->value)->trans != nullptr)
                    ((TValue_Transition*)item->value)->trans->setTimeScale(value);
            }
            else if (item->type == TransitionActionType::Animation)
            {
                if (item->target != nullptr)
                    item->target->setProp(ObjectPropID::TimeScale, Value(value));
            }
        }
    }
}

void Transition::updateFromRelations(const std::string& targetId, float dx, float dy)
{
    int cnt = (int)_items.size();
    if (cnt == 0)
        return;

    for (auto& item : _items)
    {
        if (item->type == TransitionActionType::XY && item->targetId == targetId)
        {
            if (item->tweenConfig != nullptr)
            {
                if(!item->tweenConfig->startValue->b3){
                    item->tweenConfig->startValue->f1 += dx;
                    item->tweenConfig->startValue->f2 += dy;
                    item->tweenConfig->endValue->f1 += dx;
                    item->tweenConfig->endValue->f2 += dy;
                }
            }
            else
            {
                if(!((TValue*)item->value)->b3){
                    ((TValue*)item->value)->f1 += dx;
                    ((TValue*)item->value)->f2 += dy;
                }
            }
        }
    }
}

void Transition::onOwnerAddedToStage()
{
    if (_autoPlay && !_playing)
        play(_autoPlayTimes, _autoPlayDelay, nullptr);
}

void Transition::onOwnerRemovedFromStage()
{
    if ((_options & OPTION_AUTO_STOP_DISABLED) == 0)
        stop((_options & OPTION_AUTO_STOP_AT_END) != 0 ? true : false, false);
}

void Transition::onDelayedPlay()
{
    internalPlay();

    _playing = _totalTasks > 0;
    if (_playing)
    {
        if ((_options & OPTION_IGNORE_DISPLAY_CONTROLLER) != 0)
        {
            for (auto& item : _items)
            {
                if (item->target != nullptr && item->target != _owner)
                    item->displayLockToken = item->target->addDisplayLock();
            }
        }
    }
    else if (_onComplete != nullptr)
    {
        PlayCompleteCallback func = _onComplete;
        _onComplete = nullptr;
        func();
    }
}

void Transition::internalPlay()
{
    _ownerBaseX = _owner->getX();
    _ownerBaseY = _owner->getY();

    _totalTasks = 0;

    bool needSkipAnimations = false;
    int cnt = (int)_items.size();
    if (!_reversed)
    {
        for (int i = 0; i < cnt; i++)
        {
            TransitionItem* item = _items[i];
            if (item->target == nullptr)
                continue;

            if (item->type == TransitionActionType::Animation && _startTime != 0 && item->time <= _startTime)
            {
                needSkipAnimations = true;
                ((TValue_Animation*)item->value)->flag = false;
            }
            else
                playItem(item);
        }
    }
    else
    {
        for (int i = cnt - 1; i >= 0; i--)
        {
            TransitionItem* item = _items[i];
            if (item->target == nullptr)
                continue;

            playItem(item);
        }
    }

    if (needSkipAnimations)
        skipAnimations();
}

void Transition::playItem(TransitionItem* item)
{
    float time;
    if (item->tweenConfig != nullptr)
    {
        if (_reversed)
            time = (_totalDuration - item->time - item->tweenConfig->duration);
        else
            time = item->time;

        if (_endTime == -1 || time <= _endTime)
        {
            TValue* startValue;
            TValue* endValue;

            if (_reversed)
            {
                startValue = item->tweenConfig->endValue;
                endValue = item->tweenConfig->startValue;
            }
            else
            {
                startValue = item->tweenConfig->startValue;
                endValue = item->tweenConfig->endValue;
            }

            ((TValue*)item->value)->b1 = startValue->b1 || endValue->b1;
            ((TValue*)item->value)->b2 = startValue->b2 || endValue->b2;

            switch (item->type)
            {
            case TransitionActionType::XY:
            case TransitionActionType::Size:
            case TransitionActionType::Scale:
            case TransitionActionType::Skew:
                item->tweener = GTween::to(startValue->getVec2(), endValue->getVec2(), item->tweenConfig->duration);
                break;

            case TransitionActionType::Alpha:
            case TransitionActionType::Rotation:
                item->tweener = GTween::to(startValue->f1, endValue->f1, item->tweenConfig->duration);
                break;

            case TransitionActionType::Color:
                item->tweener = GTween::to(startValue->getColor(), endValue->getColor(), item->tweenConfig->duration);
                break;

            case TransitionActionType::ColorFilter:
                item->tweener = GTween::to(startValue->getVec4(), endValue->getVec4(), item->tweenConfig->duration);
                break;
            default:
                break;
            }

            item->tweener->setDelay(time)
                ->setEase(item->tweenConfig->easeType)
                ->setRepeat(item->tweenConfig->repeat, item->tweenConfig->yoyo)
                ->setTimeScale(_timeScale)
                ->setTargetAny(item)
                ->onStart(AX_CALLBACK_1(Transition::onTweenStart, this))
                ->onUpdate(AX_CALLBACK_1(Transition::onTweenUpdate, this))
                ->onComplete1(AX_CALLBACK_1(Transition::onTweenComplete, this));

            if (_endTime >= 0)
                item->tweener->setBreakpoint(_endTime - time);

            _totalTasks++;
        }
    }
    else if (item->type == TransitionActionType::Shake)
    {
        TValue_Shake* value = (TValue_Shake*)item->value;

        if (_reversed)
            time = (_totalDuration - item->time - value->duration);
        else
            time = item->time;

        if (_endTime == -1 || time <= _endTime)
        {
            value->lastOffset.setZero();
            value->offset.setZero();
            item->tweener = GTween::shake(Vec2::ZERO, value->amplitude, value->duration)
                ->setDelay(time)
                ->setTimeScale(_timeScale)
                ->setTargetAny(item)
                ->onStart(AX_CALLBACK_1(Transition::onTweenStart, this))
                ->onUpdate(AX_CALLBACK_1(Transition::onTweenUpdate, this))
                ->onComplete1(AX_CALLBACK_1(Transition::onTweenComplete, this));

            if (_endTime >= 0)
                item->tweener->setBreakpoint(_endTime - item->time);

            _totalTasks++;
        }
    }
    else
    {
        if (_reversed)
            time = (_totalDuration - item->time);
        else
            time = item->time;

        if (time <= _startTime)
        {
            applyValue(item);
            callHook(item, false);
        }
        else if (_endTime == -1 || time <= _endTime)
        {
            _totalTasks++;
            item->tweener = GTween::delayedCall(time)
                ->setTimeScale(_timeScale)
                ->setTargetAny(item)
                ->onComplete1(AX_CALLBACK_1(Transition::onDelayedPlayItem, this));
        }
    }

    if (item->tweener != nullptr)
        item->tweener->seek(_startTime);
}

void Transition::skipAnimations()
{
    int frame;
    float playStartTime;
    float playTotalTime;
    GObject* target;
    TValue_Animation* value;
    TransitionItem* item;

    int cnt = (int)_items.size();
    for (int i = 0; i < cnt; i++)
    {
        item = _items[i];
        if (item->type != TransitionActionType::Animation || item->time > _startTime)
            continue;

        value = (TValue_Animation*)item->value;
        if (value->flag)
            continue;

        target = item->target;
        frame = target->getProp(ObjectPropID::Frame).asInt();
        playStartTime = target->getProp(ObjectPropID::Playing).asBool() ? 0 : -1;
        playTotalTime = 0;

        for (int j = i; j < cnt; j++)
        {
            item = _items[j];
            if (item->type != TransitionActionType::Animation || item->target != target || item->time > _startTime)
                continue;

            value = (TValue_Animation*)item->value;
            value->flag = true;

            if (value->frame != -1)
            {
                frame = value->frame;
                if (value->playing)
                    playStartTime = item->time;
                else
                    playStartTime = -1;
                playTotalTime = 0;
            }
            else
            {
                if (value->playing)
                {
                    if (playStartTime < 0)
                        playStartTime = item->time;
                }
                else
                {
                    if (playStartTime >= 0)
                        playTotalTime += (item->time - playStartTime);
                    playStartTime = -1;
                }
            }

            callHook(item, false);
        }

        if (playStartTime >= 0)
            playTotalTime += (_startTime - playStartTime);

        target->setProp(ObjectPropID::Playing, Value(playStartTime >= 0));
        target->setProp(ObjectPropID::Frame, Value(frame));
        if (playTotalTime > 0)
            target->setProp(ObjectPropID::DeltaTime, Value(playTotalTime));
    }
}

void Transition::onDelayedPlayItem(GTweener* tweener)
{
    TransitionItem* item = (TransitionItem*)tweener->getTarget();
    item->tweener = nullptr;
    _totalTasks--;

    applyValue(item);
    callHook(item, false);

    checkAllComplete();
}

void Transition::onTweenStart(GTweener* tweener)
{
    TransitionItem* item = (TransitionItem*)tweener->getTarget();

    if (item->type == TransitionActionType::XY || item->type == TransitionActionType::Size)
    {
        TValue* startValue;
        TValue* endValue;

        if (_reversed)
        {
            startValue = item->tweenConfig->endValue;
            endValue = item->tweenConfig->startValue;
        }
        else
        {
            startValue = item->tweenConfig->startValue;
            endValue = item->tweenConfig->endValue;
        }

        if (item->type == TransitionActionType::XY)
        {
            if (item->target != _owner)
            {
                if (!startValue->b1)
                    tweener->startValue.x = item->target->getX();
                else if (startValue->b3) //percent
                    tweener->startValue.x = startValue->f1 * _owner->getWidth();

                if (!startValue->b2)
                    tweener->startValue.y = item->target->getY();
                else if (startValue->b3) //percent
                    tweener->startValue.y = startValue->f2 * _owner->getHeight();

                if (!endValue->b1)
                    tweener->endValue.x = tweener->startValue.x;
                else if (endValue->b3)
                    tweener->endValue.x = endValue->f1 * _owner->getWidth();

                if (!endValue->b2)
                    tweener->endValue.y = tweener->startValue.y;
                else if (endValue->b3)
                    tweener->endValue.y = endValue->f2 * _owner->getHeight();
            }
            else
            {
                if (!startValue->b1)
                    tweener->startValue.x = item->target->getX() - _ownerBaseX;
                if (!startValue->b2)
                    tweener->startValue.y = item->target->getY() - _ownerBaseY;

                if (!endValue->b1)
                    tweener->endValue.x = tweener->startValue.x;
                if (!endValue->b2)
                    tweener->endValue.y = tweener->startValue.y;
            }
        }
        else
        {
            if (!startValue->b1)
                tweener->startValue.x = item->target->getWidth();
            if (!startValue->b2)
                tweener->startValue.y = item->target->getHeight();

            if (!endValue->b1)
                tweener->endValue.x = tweener->startValue.x;
            if (!endValue->b2)
                tweener->endValue.y = tweener->startValue.y;
        }

        if (item->tweenConfig->path != nullptr)
        {
            ((TValue*)item->value)->b1 = ((TValue*)item->value)->b2 = true;
            tweener->setPath(item->tweenConfig->path);
        }
    }

    callHook(item, false);
}

void Transition::onTweenUpdate(GTweener* tweener)
{
    TransitionItem* item = (TransitionItem*)tweener->getTarget();
    switch (item->type)
    {
    case TransitionActionType::XY:
    case TransitionActionType::Size:
    case TransitionActionType::Scale:
    case TransitionActionType::Skew:
        if (item->tweenConfig->path != nullptr)
            ((TValue*)item->value)->setVec2(tweener->value.getVec2() + tweener->startValue.getVec2());
        else
            ((TValue*)item->value)->setVec2(tweener->value.getVec2());
        break;

    case TransitionActionType::Alpha:
    case TransitionActionType::Rotation:
        ((TValue*)item->value)->f1 = tweener->value.x;
        break;

    case TransitionActionType::Color:
        ((TValue*)item->value)->setColor(tweener->value.getColor());
        break;

    case TransitionActionType::ColorFilter:
        ((TValue*)item->value)->setVec4(tweener->value.getVec4());
        break;

    case TransitionActionType::Shake:
        ((TValue_Shake*)item->value)->offset = tweener->deltaValue.getVec2();
        break;
    default:
        break;
    }
    applyValue(item);
}

void Transition::onTweenComplete(GTweener* tweener)
{
    TransitionItem* item = (TransitionItem*)tweener->getTarget();
    item->tweener = nullptr;
    _totalTasks--;

    if (tweener->allCompleted())
        callHook(item, true);

    checkAllComplete();
}

void Transition::onPlayTransCompleted(TransitionItem* item)
{
    _totalTasks--;

    checkAllComplete();
}

void Transition::callHook(TransitionItem* item, bool tweenEnd)
{
    if (tweenEnd)
    {
        if (item->tweenConfig != nullptr && item->tweenConfig->endHook != nullptr)
            item->tweenConfig->endHook();
    }
    else
    {
        if (item->time >= _startTime && item->hook != nullptr)
            item->hook();
    }
}

void Transition::checkAllComplete()
{
    if (_playing && _totalTasks == 0)
    {
        if (_totalTimes < 0)
        {
            internalPlay();
        }
        else
        {
            _totalTimes--;
            if (_totalTimes > 0)
                internalPlay();
            else
            {
                _playing = false;

                for (auto& item : _items)
                {
                    if (item->target != nullptr && item->displayLockToken != 0)
                    {
                        item->target->releaseDisplayLock(item->displayLockToken);
                        item->displayLockToken = 0;
                    }
                }

                if (_onComplete != nullptr)
                {
                    PlayCompleteCallback func = _onComplete;
                    _onComplete = nullptr;
                    func();
                }
            }
        }
    }
}

void Transition::applyValue(TransitionItem* item)
{
    item->target->_gearLocked = true;

    switch (item->type)
    {
    case TransitionActionType::XY:
    {
        TValue* value = (TValue*)item->value;
        if (item->target == _owner)
        {
            if (value->b1 && value->b2)
                item->target->setPosition(value->f1 + _ownerBaseX, value->f2 + _ownerBaseY);
            else if (value->b1)
                item->target->setX(value->f1 + _ownerBaseX);
            else
                item->target->setY(value->f2 + _ownerBaseY);
        }
        else
        {
            if (value->b3) //position in percent
            {
                if (value->b1 && value->b2)
                    item->target->setPosition(value->f1 * _owner->getWidth(), value->f2 * _owner->getHeight());
                else if (value->b1)
                    item->target->setX(value->f1 * _owner->getWidth());
                else if (value->b2)
                    item->target->setY(value->f2 * _owner->getHeight());
            }
            else
            {
                if (value->b1 && value->b2)
                    item->target->setPosition(value->f1, value->f2);
                else if (value->b1)
                    item->target->setX(value->f1);
                else if (value->b2)
                    item->target->setY(value->f2);
            }
        }
    }
    break;

    case TransitionActionType::Size:
    {
        TValue* value = (TValue*)item->value;
        if (!value->b1)
            value->f1 = item->target->getWidth();
        if (!value->b2)
            value->f2 = item->target->getHeight();
        item->target->setSize(value->f1, value->f2);
    }
    break;

    case TransitionActionType::Pivot:
        item->target->setPivot(((TValue*)item->value)->f1, ((TValue*)item->value)->f2, item->target->isPivotAsAnchor());
        break;

    case TransitionActionType::Alpha:
        item->target->setAlpha(((TValue*)item->value)->f1);
        break;

    case TransitionActionType::Rotation:
        item->target->setRotation(((TValue*)item->value)->f1);
        break;

    case TransitionActionType::Scale:
        item->target->setScale(((TValue*)item->value)->f1, ((TValue*)item->value)->f2);
        break;

    case TransitionActionType::Skew:
        item->target->setSkewX(((TValue*)item->value)->f1);
        item->target->setSkewY(((TValue*)item->value)->f2);
        break;

    case TransitionActionType::Color:
        item->target->setProp(ObjectPropID::Color, Value(ToolSet::colorToInt((Color3B)((TValue*)item->value)->getColor())));
        break;

    case TransitionActionType::Animation:
    {
        TValue_Animation* value = (TValue_Animation*)item->value;
        if (value->frame >= 0)
            item->target->setProp(ObjectPropID::Frame, Value(value->frame));
        item->target->setProp(ObjectPropID::Playing, Value(value->playing));
        item->target->setProp(ObjectPropID::TimeScale, Value(_timeScale));
        break;
    }

    case TransitionActionType::Visible:
        item->target->setVisible(((TValue_Visible*)item->value)->visible);
        break;

    case TransitionActionType::Shake:
    {
        TValue_Shake* value = (TValue_Shake*)item->value;
        item->target->setPosition(item->target->getX() - value->lastOffset.x + value->offset.x, item->target->getY() - value->lastOffset.y + value->offset.y);
        value->lastOffset = value->offset;
        break;
    }

    case TransitionActionType::Transition:
        if (_playing)
        {
            TValue_Transition* value = (TValue_Transition*)item->value;
            if (value->trans != nullptr)
            {
                _totalTasks++;

                float startTime = _startTime > item->time ? (_startTime - item->time) : 0;
                float endTime = _endTime >= 0 ? (_endTime - item->time) : -1;
                if (value->stopTime >= 0 && (endTime < 0 || endTime > value->stopTime))
                    endTime = value->stopTime;
                value->trans->setTimeScale(_timeScale);
                value->trans->play(value->playTimes, 0, startTime, endTime, [this, item]() { onPlayTransCompleted(item); }, _reversed);
            }
        }
        break;

    case TransitionActionType::Sound:
        if (_playing && item->time >= _startTime)
        {
            TValue_Sound* value = (TValue_Sound*)item->value;
            if (!value->sound.empty())
                UIRoot->playSound(value->sound, value->volume);
            break;
        }

    case TransitionActionType::ColorFilter:
        break;

    case TransitionActionType::Text:
        item->target->setText(((TValue_Text*)item->value)->text);
        break;

    case TransitionActionType::Icon:
        item->target->setIcon(((TValue_Text*)item->value)->text);
        break;
    default:
        break;
    }

    item->target->_gearLocked = false;
}

static std::vector<GPathPoint> helperPoints;

void Transition::setup(ByteBuffer* buffer)
{
    name = buffer->readS();
    _options = buffer->readInt();
    _autoPlay = buffer->readBool();
    _autoPlayTimes = buffer->readInt();
    _autoPlayDelay = buffer->readFloat();

    int cnt = buffer->readShort();
    for (int i = 0; i < cnt; i++)
    {
        int dataLen = buffer->readUshort();
        int curPos = buffer->getPos();

        buffer->seek(curPos, 0);

        TransitionItem* item = new TransitionItem((TransitionActionType)buffer->readByte());
        _items.push_back(item);

        item->time = buffer->readFloat();
        int targetId = buffer->readShort();
        if (targetId < 0)
            item->targetId = STD_STRING_EMPTY;
        else
            item->targetId = _owner->getChildAt(targetId)->id;
        item->label = buffer->readS();

        if (buffer->readBool())
        {
            buffer->seek(curPos, 1);

            item->tweenConfig = new TweenConfig();
            item->tweenConfig->duration = buffer->readFloat();
            if (item->time + item->tweenConfig->duration > _totalDuration)
                _totalDuration = item->time + item->tweenConfig->duration;
            item->tweenConfig->easeType = (EaseType)buffer->readByte();
            item->tweenConfig->repeat = buffer->readInt();
            item->tweenConfig->yoyo = buffer->readBool();
            item->tweenConfig->endLabel = buffer->readS();

            buffer->seek(curPos, 2);

            decodeValue(item, buffer, item->tweenConfig->startValue);

            buffer->seek(curPos, 3);

            decodeValue(item, buffer, item->tweenConfig->endValue);

            if (buffer->version >= 2)
            {
                int pathLen = buffer->readInt();
                if (pathLen > 0)
                {
                    item->tweenConfig->path = new GPath();
                    std::vector<GPathPoint>& pts = helperPoints;
                    pts.clear();

                    Vec3 v0, v1, v2;

                    for (int j = 0; j < pathLen; j++)
                    {
                        GPathPoint::CurveType curveType = (GPathPoint::CurveType)buffer->readByte();
                        switch (curveType)
                        {
                        case GPathPoint::CurveType::Bezier:
                            v0.x = buffer->readFloat();
                            v0.y = buffer->readFloat();
                            v1.x = buffer->readFloat();
                            v1.y = buffer->readFloat();
                            pts.push_back(GPathPoint(v0, v1));
                            break;

                        case GPathPoint::CurveType::CubicBezier:
                            v0.x = buffer->readFloat();
                            v0.y = buffer->readFloat();
                            v1.x = buffer->readFloat();
                            v1.y = buffer->readFloat();
                            v2.x = buffer->readFloat();
                            v2.y = buffer->readFloat();
                            pts.push_back(GPathPoint(v0, v1, v2));
                            break;

                        default:
                            v0.x = buffer->readFloat();
                            v0.y = buffer->readFloat();
                            pts.push_back(GPathPoint(v0, curveType));
                            break;
                        }
                    }

                    item->tweenConfig->path->create(pts.data(), (int)pts.size());
                }
            }
        }
        else
        {
            if (item->time > _totalDuration)
                _totalDuration = item->time;

            buffer->seek(curPos, 2);

            decodeValue(item, buffer, item->value);
        }

        buffer->setPos(curPos + dataLen);
    }
}

void Transition::decodeValue(TransitionItem* item, ByteBuffer* buffer, void* value)
{
    switch (item->type)
    {
    case TransitionActionType::XY:
    case TransitionActionType::Size:
    case TransitionActionType::Pivot:
    case TransitionActionType::Skew:
    {
        TValue* tvalue = (TValue*)value;
        tvalue->b1 = buffer->readBool();
        tvalue->b2 = buffer->readBool();
        tvalue->f1 = buffer->readFloat();
        tvalue->f2 = buffer->readFloat();

        if (buffer->version >= 2 && item->type == TransitionActionType::XY)
            tvalue->b3 = buffer->readBool(); //percent
        break;
    }

    case TransitionActionType::Alpha:
    case TransitionActionType::Rotation:
        ((TValue*)value)->f1 = buffer->readFloat();
        break;

    case TransitionActionType::Scale:
    {
        ((TValue*)value)->f1 = buffer->readFloat();
        ((TValue*)value)->f2 = buffer->readFloat();
        break;
    }

    case TransitionActionType::Color:
        ((TValue*)value)->setColor(buffer->readColor());
        break;

    case TransitionActionType::Animation:
    {
        ((TValue_Animation*)value)->playing = buffer->readBool();
        ((TValue_Animation*)value)->frame = buffer->readInt();
        break;
    }

    case TransitionActionType::Visible:
        ((TValue_Visible*)value)->visible = buffer->readBool();
        break;

    case TransitionActionType::Sound:
    {
        ((TValue_Sound*)value)->sound = buffer->readS();
        ((TValue_Sound*)value)->volume = buffer->readFloat();
        break;
    }

    case TransitionActionType::Transition:
    {
        ((TValue_Transition*)value)->transName = buffer->readS();
        ((TValue_Transition*)value)->playTimes = buffer->readInt();
        break;
    }

    case TransitionActionType::Shake:
    {
        ((TValue_Shake*)value)->amplitude = buffer->readFloat();
        ((TValue_Shake*)value)->duration = buffer->readFloat();
        break;
    }

    case TransitionActionType::ColorFilter:
    {
        TValue* tvalue = (TValue*)value;
        tvalue->f1 = buffer->readFloat();
        tvalue->f2 = buffer->readFloat();
        tvalue->f3 = buffer->readFloat();
        tvalue->f4 = buffer->readFloat();
        break;
    }

    case TransitionActionType::Text:
    case TransitionActionType::Icon:
        ((TValue_Text*)value)->text = buffer->readS();
        break;

    default:
        break;
    }
}

NS_FGUI_END
