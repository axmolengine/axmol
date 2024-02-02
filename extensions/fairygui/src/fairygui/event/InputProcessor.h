#ifndef __INPUTPROCESSOR_H__
#define __INPUTPROCESSOR_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "UIEventDispatcher.h"
#include "InputEvent.h"

NS_FGUI_BEGIN

class GComponent;
class TouchInfo;

class InputProcessor
{
public:
    typedef std::function<void(int eventType)> CaptureEventCallback;

    InputEvent* getRecentInput() { return &_recentInput; }
    static bool isTouchOnUI();

    InputProcessor(GComponent* owner);
    ~InputProcessor();

    ax::Vec2 getTouchPosition(int touchId);

    void addTouchMonitor(int touchId, GObject* target);
    void removeTouchMonitor(GObject* target);

    void cancelClick(int touchId);
    void simulateClick(GObject* target, int touchId = -1);

    void setCaptureCallback(CaptureEventCallback value) { _captureCallback = value; }
    
    void disableDefaultTouchEvent();
    bool touchDown(ax::Touch *touch, ax::Event *event);
    void touchMove(ax::Touch *touch, ax::Event *event);
    void touchUp(ax::Touch *touch, ax::Event *event);
    
private:
    bool onTouchBegan(ax::Touch * touch, ax::Event *);
    void onTouchMoved(ax::Touch * touch, ax::Event *);
    void onTouchEnded(ax::Touch * touch, ax::Event *);
    void onTouchCancelled(ax::Touch * touch, ax::Event *);

    void onMouseDown(ax::EventMouse* event);
    void onMouseUp(ax::EventMouse* event);
    void onMouseMove(ax::EventMouse* event);
    void onMouseScroll(ax::EventMouse* event);

    void onKeyDown(ax::EventKeyboard::KeyCode keyCode, ax::Event*);
    void onKeyUp(ax::EventKeyboard::KeyCode keyCode, ax::Event*);

    TouchInfo* getTouch(int touchId, bool createIfNotExisits = true);
    void updateRecentInput(TouchInfo* touch, GObject* target);
    void handleRollOver(TouchInfo* touch, GObject* target);
    void setBegin(TouchInfo* touch, GObject* target);
    void setEnd(TouchInfo* touch, GObject* target);
    GObject* clickTest(TouchInfo* touch, GObject* target);

    ax::EventListenerTouchOneByOne* _touchListener;
    ax::EventListenerMouse* _mouseListener;
    ax::EventListenerKeyboard* _keyboardListener;
    std::vector<TouchInfo*> _touches;
    GComponent* _owner;
    CaptureEventCallback _captureCallback;
    InputEvent _recentInput;
    uint16_t _keyModifiers;

    static bool _touchOnUI;
    static unsigned int _touchOnUIFlagFrameId;
    static InputProcessor* _activeProcessor;

    friend class UIEventDispatcher;
};

NS_FGUI_END

#endif
