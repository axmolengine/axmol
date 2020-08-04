#ifndef __EVENTCONTEXT_H__
#define __EVENTCONTEXT_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "InputEvent.h"

NS_FGUI_BEGIN

class GObject;
class InputProcessor;

class EventContext
{
public:
    EventContext();
    ~EventContext();

    int getType() const { return _type; }
    cocos2d::Ref* getSender() const { return _sender; }
    InputEvent* getInput() const { return _inputEvent; }
    void stopPropagation() { _isStopped = true; }
    void preventDefault() { _defaultPrevented = true; }
    bool isDefaultPrevented() { return _defaultPrevented; }
    void captureTouch() { _touchCapture = 1; }
    void uncaptureTouch() { _touchCapture = 2; }

    const cocos2d::Value& getDataValue() const { return _dataValue; }
    void* getData() const { return _data; }

private:
    cocos2d::Ref* _sender;
    InputEvent* _inputEvent;
    cocos2d::Value _dataValue;
    void* _data;
    bool _isStopped;
    bool _defaultPrevented;
    int _touchCapture;
    int _type;

    friend class UIEventDispatcher;
};

NS_FGUI_END

#endif
