#ifndef __BASICS_SCENE_H__
#define __BASICS_SCENE_H__

#include "cocos2d.h"
#include "DemoScene.h"

USING_NS_FGUI;

class Window1;
class Window2;

class BasicsScene : public DemoScene
{
public:
    BasicsScene();
    ~BasicsScene();

    // implement the "static create()" method manually
    CREATE_FUNC(BasicsScene);

protected:
    virtual void continueInit() override;

private:
    void runDemo(EventContext* context);
    void onClickBack(EventContext* context);
    void onClickMenu(EventContext* context);

    void playDragDrop();
    void playWindow();
    void playDepth();
    void playText();
    void playPopup();
    void playProgress();
    void onPlayProgress(float dt);

    GComponent* _view;
    GObject* _backBtn;
    GComponent* _demoContainer;
    GController* _cc;
    Window1* _winA;
    Window2* _winB;
    PopupMenu* _pm;
    GComponent* _popupCom;

    cocos2d::Map<std::string, GComponent*> _demoObjects;

};

#endif // __BASIC_SCENE_H__
