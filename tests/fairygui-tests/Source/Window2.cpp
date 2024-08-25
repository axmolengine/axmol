#include "Window2.h"

using namespace ax;

void Window2::onInit()
{
    setContentPane(UIPackage::createObject("Basics", "WindowB")->as<GComponent>());
    center();
}

void  Window2::doShowAnimation()
{
    setScale(0.1f, 0.1f);
    setPivot(0.5f, 0.5f);

    GTween::to(getScale(), Vec2::ONE, 0.3f)->setTarget(this, TweenPropType::Scale)->onComplete(AX_CALLBACK_0(Window2::onShown, this));
}

void  Window2::doHideAnimation()
{
    GTween::to(getScale(), Vec2(0.1f, 0.1f), 0.3f)->setTarget(this, TweenPropType::Scale)->onComplete(AX_CALLBACK_0(Window2::hideImmediately, this));
}

void Window2::onShown()
{
    _contentPane->getTransition("t1")->play();
}

void Window2::onHide()
{
    _contentPane->getTransition("t1")->stop();
}