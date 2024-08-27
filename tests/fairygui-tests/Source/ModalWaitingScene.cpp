#include "ModalWaitingScene.h"

using namespace ax;

ModalWaitingScene::ModalWaitingScene():_testWin(nullptr)
{
}

ModalWaitingScene::~ModalWaitingScene()
{
    AX_SAFE_RELEASE(_testWin);
}

void ModalWaitingScene::continueInit()
{
    UIPackage::addPackage("UI/ModalWaiting");
    UIConfig::globalModalWaiting = "ui://ModalWaiting/GlobalModalWaiting";
    UIConfig::windowModalWaiting = "ui://ModalWaiting/WindowModalWaiting";

    _view = UIPackage::createObject("ModalWaiting", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _testWin = Window::create();
    _testWin->retain();
    _testWin->setContentPane(UIPackage::createObject("ModalWaiting", "TestWin")->as<GComponent>());
    _testWin->getContentPane()->getChild("n1")->addClickListener([this](EventContext*)
    {
        _testWin->showModalWait();
        //simulate a asynchronous request
        scheduleOnce([this](float)
        {
            _testWin->closeModalWait();
        }, 3, "wait");
    });

    _view->getChild("n0")->addClickListener([this](EventContext*) {  _testWin->show(); });

    _groot->showModalWait();

    //simulate a asynchronous request

    scheduleOnce([this](float)
    {
        _groot->closeModalWait();
    }, 3, "wait");
}
