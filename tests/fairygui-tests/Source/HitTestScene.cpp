#include "HitTestScene.h"

using namespace ax;

void HitTestScene::continueInit()
{
    UIPackage::addPackage("UI/HitTest");

    _view = UIPackage::createObject("HitTest", "Main")->as<GComponent>();
    _groot->addChild(_view);

    // n36 is the polygon hit-area graph for the Shape Hit Test button; normally invisible
    // (it's only used as hitArea geometry). Make it visible and translucent here so the
    // hit-test shape can be seen/compared on screen against the image underneath.
    if (GObject* shapeBtn = _view->getChild("n33"))
    {
        if (GComponent* comp = shapeBtn->as<GComponent>())
        {
            if (GObject* n36 = comp->getChild("n36"))
            {
                n36->setVisible(false);
                n36->setAlpha(0.35f);
            }
        }
    }
}
