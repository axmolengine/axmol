#ifndef __INPUTPROCESSOR_H__
#define __INPUTPROCESSOR_H__

#include "FairyGUIMacros.h"
#include "axmol/cocos2d.h"
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

    ax::Vec2 getTouchPosition(intptr_t touchId);

    void addTouchMonitor(int touchId, GObject* target);
    void removeTouchMonitor(GObject* target);

    void cancelClick(int touchId);
    void simulateClick(GObject* target, int touchId = -1);

    void setCaptureCallback(CaptureEventCallback value) { _captureCallback = value; }
    
    void disableDefaultTouchEvent();
    bool pointerDown(ax::PointerEvent* event);
    void pointerMove(ax::PointerEvent* event);
    void pointerUp(ax::PointerEvent* event);
    
private:
    bool onPointerDown(ax::PointerEvent* event);
    void onPointerMove(ax::PointerEvent* event);
    void onPointerUp(ax::PointerEvent* event);
    void onPointerCancel(ax::PointerEvent* event);

    bool onPointerScroll(ax::PointerEvent* event);

    void onKeyDown(ax::KeyboardEvent*);
    void onKeyUp(ax::KeyboardEvent*);

    TouchInfo* getTouch(int touchId, bool createIfNotExisits = true);
    void updateRecentInput(TouchInfo* touch, GObject* target);
    void handleRollOver(TouchInfo* touch, GObject* target);
    void setBegin(TouchInfo* touch, GObject* target);
    void setEnd(TouchInfo* touch, GObject* target);
    GObject* clickTest(TouchInfo* touch, GObject* target);

    ax::PointerEventListener* _pointerListener;
    ax::KeyboardEventListener* _keyboardListener;
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
