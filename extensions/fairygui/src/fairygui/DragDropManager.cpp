#include "DragDropManager.h"
#include "UIObjectFactory.h"
#include "GRoot.h"

NS_FGUI_BEGIN
using namespace ax;

DragDropManager* DragDropManager::_inst = nullptr;

DragDropManager::DragDropManager() :
    _agent(nullptr)
{
    _agent = (GLoader*)UIObjectFactory::newObject(ObjectType::LOADER);
    _agent->retain();
    _agent->setTouchable(false);
    _agent->setDraggable(true);
    _agent->setSize(100, 100);
    _agent->setPivot(0.5f, 0.5f, true);
    _agent->setAlign(TextHAlignment::CENTER);
    _agent->setVerticalAlign(TextVAlignment::CENTER);
    _agent->setSortingOrder(INT_MAX);
    _agent->addEventListener(UIEventType::DragEnd, AX_CALLBACK_1(DragDropManager::onDragEnd, this));
}

DragDropManager::~DragDropManager()
{
    AX_SAFE_RELEASE(_agent);
}

DragDropManager* DragDropManager::getInstance()
{
    if (_inst == nullptr)
        _inst = new DragDropManager();

    return _inst;
}

void DragDropManager::startDrag(const std::string & icon, const Value& sourceData, int touchPointID)
{
    if (_agent->getParent() != nullptr)
        return;

    _sourceData = sourceData;
    _agent->setURL(icon);
    UIRoot->addChild(_agent);
    Vec2 pt = UIRoot->globalToLocal(UIRoot->getTouchPosition(touchPointID));
    _agent->setPosition(pt.x, pt.y);
    _agent->startDrag(touchPointID);
}

void DragDropManager::cancel()
{
    if (_agent->getParent() != nullptr)
    {
        _agent->stopDrag();
        UIRoot->removeChild(_agent);
        _sourceData = Value::Null;
    }
}

void DragDropManager::onDragEnd(EventContext * context)
{
    if (_agent->getParent() == nullptr) //cancelled
        return;

    UIRoot->removeChild(_agent);

    GObject* obj = UIRoot->getTouchTarget();
    while (obj != nullptr)
    {
        if (dynamic_cast<GComponent*>(obj))
        {
            if (obj->hasEventListener(UIEventType::Drop))
            {
                //obj->requestFocus();
                obj->dispatchEvent(UIEventType::Drop, nullptr, _sourceData);
                return;
            }
        }

        obj = obj->getParent();
    }
}

NS_FGUI_END