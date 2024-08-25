#include "DemoScene.h"
#include "MenuScene.h"

using namespace ax;

bool DemoScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    _groot = GRoot::create(this);
    _groot->retain();

    continueInit();

    //add a closebutton to scene
    GObject* closeButton = UIPackage::createObject("MainMenu", "CloseButton");
    closeButton->setPosition(_groot->getWidth() - closeButton->getWidth() - 10, _groot->getHeight() - closeButton->getHeight() - 10);
    closeButton->addRelation(_groot, RelationType::Right_Right);
    closeButton->addRelation(_groot, RelationType::Bottom_Bottom);
    closeButton->setSortingOrder(100000);
    closeButton->addClickListener(AX_CALLBACK_1(DemoScene::onClose, this));
    _groot->addChild(closeButton);

    return true;
}

void DemoScene::continueInit()
{
}

void DemoScene::onClose(EventContext* context)
{
    if (!dynamic_cast<MenuScene*>(this))
    {
        Director::getInstance()->replaceScene(MenuScene::create());
    }
    else
    {
        //Close the axmol game scene and quit the application
        Director::getInstance()->end();

        /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use Director::getInstance()->end() and exit(0) as given above,instead trigger a custom event created in RootViewController.mm as below*/

        //EventCustom customEndEvent("game_scene_close_event");
        //_eventDispatcher->dispatchEvent(&customEndEvent);
    }
}

DemoScene::DemoScene() :
    _groot(nullptr)
{
}

DemoScene::~DemoScene()
{
    AX_SAFE_RELEASE(_groot);
}
