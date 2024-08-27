#include "JoystickScene.h"

using namespace ax;

JoystickScene::~JoystickScene()
{
    AX_SAFE_RELEASE(_joystick);
}

void JoystickScene::continueInit()
{
    UIPackage::addPackage("UI/Joystick");

    _view = UIPackage::createObject("Joystick", "Main")->as<GComponent>();
    _groot->addChild(_view);

    _joystick = JoystickModule::create(_view);
    _joystick->retain();

    GObject* tf = _view->getChild("n9");

    _joystick->addEventListener(JoystickModule::MOVE, [tf](EventContext*context) {
        tf->setText(context->getDataValue().asString());
    });

    _joystick->addEventListener(JoystickModule::END, [tf](EventContext*context) {
        tf->setText("");
    });
}