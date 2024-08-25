#include "ChangePageAction.h"
#include "Controller.h"
#include "GComponent.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

void ChangePageAction::setup(ByteBuffer* buffer)
{
    ControllerAction::setup(buffer);

    objectId = buffer->readS();
    controllerName = buffer->readS();
    targetPage = buffer->readS();
}

void ChangePageAction::enter(GController* controller)
{
    if (controllerName.empty())
        return;

    GComponent* gcom;
    if (!objectId.empty())
        gcom = controller->getParent()->getChildById(objectId)->as<GComponent>();
    else
        gcom = controller->getParent();
    if (gcom != nullptr)
    {
        GController* cc = gcom->getController(controllerName);
        if (cc != nullptr && cc != controller && !cc->changing)
        {
            if (targetPage.compare("~1") == 0)
            {
                if (controller->getSelectedIndex() < cc->getPageCount())
                    cc->setSelectedIndex(controller->getSelectedIndex());
            }
            else if (targetPage.compare("~2") == 0)
                cc->setSelectedPage(controller->getSelectedPage());
            else
                cc->setSelectedPageId(targetPage);
        }
    }
}

void ChangePageAction::leave(GController* controller)
{
}

NS_FGUI_END