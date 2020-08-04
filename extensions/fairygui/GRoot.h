#ifndef __GROOT_H__
#define __GROOT_H__

#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "GGraph.h"
#include "Window.h"
#include "cocos2d.h"
#include "event/InputProcessor.h"

NS_FGUI_BEGIN

class WeakPtr;

class GRoot : public GComponent
{
public:
    GRoot();
    virtual ~GRoot();

    static GRoot* create(cocos2d::Scene* scene, int zOrder = 1000);
    static GRoot* getInstance() { return _inst; }

    void showWindow(Window* win);
    void hideWindow(Window* win);
    void hideWindowImmediately(Window* win);
    void bringToFront(Window* win);
    void showModalWait();
    void closeModalWait();
    void closeAllExceptModals();
    void closeAllWindows();
    Window* getTopWindow();

    GObject* getModalWaitingPane();
    GGraph* getModalLayer();
    bool hasModalWindow();
    bool isModalWaiting();

    InputProcessor* getInputProcessor() const { return _inputProcessor; }
    cocos2d::Vec2 getTouchPosition(int touchId);
    GObject* getTouchTarget();
    
    cocos2d::Vec2 worldToRoot(const cocos2d::Vec2 &pt);
    cocos2d::Vec2 rootToWorld(const cocos2d::Vec2 &pt);

    void showPopup(GObject* popup);
    void showPopup(GObject* popup, GObject* target, PopupDirection dir);
    void togglePopup(GObject* popup);
    void togglePopup(GObject* popup, GObject* target, PopupDirection dir);
    void hidePopup();
    void hidePopup(GObject* popup);
    bool hasAnyPopup();
    cocos2d::Vec2 getPoupPosition(GObject* popup, GObject* target, PopupDirection dir);

    void showTooltips(const std::string& msg);
    void showTooltipsWin(GObject* tooltipWin);
    void hideTooltips();

    void playSound(const std::string& url, float volumeScale = 1);
    bool isSoundEnabled() const { return _soundEnabled; }
    void setSoundEnabled(bool value);
    float getSoundVolumeScale() const { return _soundVolumeScale; }
    void setSoundVolumeScale(float value);

    static int contentScaleLevel;

protected:
    virtual void handlePositionChanged() override;
    virtual void onEnter() override;
    virtual void onExit() override;

private:
    bool initWithScene(cocos2d::Scene* scene, int zOrder);
    void onWindowSizeChanged();
    void createModalLayer();
    void adjustModalLayer();
    void closePopup(GObject* target);
    void checkPopups();
    void onTouchEvent(int eventType);
    void updateContentScaleLevel();

    CALL_LATER_FUNC(GRoot, doShowTooltipsWin);

    cocos2d::EventListener* _windowSizeListener;
    InputProcessor* _inputProcessor;

    GGraph* _modalLayer;
    GObject* _modalWaitPane;
    std::vector<WeakPtr> _popupStack;
    std::vector<WeakPtr> _justClosedPopups;
    GObject* _tooltipWin;
    GObject* _defaultTooltipWin;

    static bool _soundEnabled;
    static float _soundVolumeScale;

    static GRoot* _inst;
};

NS_FGUI_END

#endif
