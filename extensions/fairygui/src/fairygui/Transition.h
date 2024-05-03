#ifndef __TRANSITION_H__
#define __TRANSITION_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;
class GComponent;
class TransitionItem;
class GTweener;
class ByteBuffer;

class Transition : public ax::Object
{
public:
    typedef std::function<void()> PlayCompleteCallback;
    typedef std::function<void()> TransitionHook;

    Transition(GComponent* owner);
    virtual ~Transition();

    GComponent* getOwner() const { return _owner; }
    bool isPlaying() const { return _playing; }

    void play(PlayCompleteCallback callback = nullptr);
    void play(int times, float delay, PlayCompleteCallback callback = nullptr);
    void play(int times, float delay, float startTime, float endTime, PlayCompleteCallback callback = nullptr);
    void playReverse(PlayCompleteCallback callback = nullptr);
    void playReverse(int times, float delay, PlayCompleteCallback callback = nullptr);
    void changePlayTimes(int value);
    void stop();
    void stop(bool setToComplete, bool processCallback);
    void setAutoPlay(bool autoPlay, int times, float delay);
    void setPaused(bool paused);

    void setValue(const std::string& label, const ax::ValueVector& values);
    void setHook(const std::string& label, TransitionHook callback);
    void clearHooks();
    void setTarget(const std::string& label, GObject* newTarget);
    void setDuration(const std::string& label, float value);
    float getLabelTime(const std::string& label) const;
    float getTimeScale() const { return _timeScale; }
    void setTimeScale(float value);

    void updateFromRelations(const std::string& targetId, float dx, float dy);
    void onOwnerAddedToStage();
    void onOwnerRemovedFromStage();

    void setup(ByteBuffer* buffer);

    std::string name;

private:
    void play(int times, float delay, float startTime, float endTime, PlayCompleteCallback onComplete, bool reverse);
    void stopItem(TransitionItem* item, bool setToComplete);
    void onDelayedPlay();
    void internalPlay();
    void playItem(TransitionItem* item);
    void skipAnimations();
    void onDelayedPlayItem(GTweener* tweener);
    void onTweenStart(GTweener* tweener);
    void onTweenUpdate(GTweener* tweener);
    void onTweenComplete(GTweener* tweener);
    void onPlayTransCompleted(TransitionItem* item);
    void callHook(TransitionItem* item, bool tweenEnd);
    void checkAllComplete();
    void applyValue(TransitionItem* item);
    void decodeValue(TransitionItem* item, ByteBuffer* buffer, void* value);

    GComponent* _owner;
    std::vector<TransitionItem*> _items;
    int _totalTimes;
    int _totalTasks;
    bool _playing;
    bool _paused;
    float _ownerBaseX;
    float _ownerBaseY;
    PlayCompleteCallback _onComplete;
    int _options;
    bool _reversed;
    float _totalDuration;
    bool _autoPlay;
    int _autoPlayTimes;
    float _autoPlayDelay;
    float _timeScale;
    float _startTime;
    float _endTime;
};

NS_FGUI_END

#endif
