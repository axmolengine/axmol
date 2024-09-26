#include "UIEventDispatcher.h"
#include "GComponent.h"
#include "InputProcessor.h"
#include "utils/WeakPtr.h"

using namespace ax;
NS_FGUI_BEGIN

const EventTag EventTag::None;

EventTag::EventTag() :_value(0)
{
}

EventTag::EventTag(void * ptr) : _value((uintptr_t)ptr)
{
}

EventTag::EventTag(int value) : _value(value)
{
}

EventTag::EventTag(const EventTag & other)
{
    *this = other;
}

EventTag::EventTag(EventTag && other)
{
    *this = std::move(other);
}

EventTag::~EventTag()
{
}

EventTag & EventTag::operator=(const EventTag & other)
{
    if (this != &other)
        _value = other._value;
    return *this;
}

EventTag & EventTag::operator=(EventTag && other)
{
    if (this != &other)
    {
        _value = other._value;
        other._value = 0;
    }
    return *this;
}

EventTag & EventTag::operator=(void * ptr)
{
    _value = (uintptr_t)ptr;
    return *this;
}

EventTag & EventTag::operator=(int v)
{
    _value = v;
    return *this;
}

bool EventTag::operator!=(const EventTag & v)
{
    return _value != v._value;
}

bool EventTag::operator!=(const EventTag & v) const
{
    return _value != v._value;
}

bool EventTag::operator==(const EventTag & v)
{
    return _value == v._value;
}

bool EventTag::operator==(const EventTag & v) const
{
    return _value == v._value;
}

UIEventDispatcher::UIEventDispatcher() :_dispatching(0)
{
}

UIEventDispatcher::~UIEventDispatcher()
{
    _dispatching = 0;
    removeEventListeners();
}

void UIEventDispatcher::addEventListener(int eventType, const EventCallback& callback, const EventTag& tag)
{
    if (!tag.isNone())
    {
        for (auto it = _callbacks.begin(); it != _callbacks.end(); it++)
        {
            if ((*it)->eventType == eventType && (*it)->tag == tag)
            {
                (*it)->callback = callback;
                return;
            }
        }
    }

    EventCallbackItem* item = new EventCallbackItem();
    item->callback = callback;
    item->eventType = eventType;
    item->tag = tag;
    item->dispatching = 0;
    _callbacks.push_back(item);
}

void UIEventDispatcher::removeEventListener(int eventType, const EventTag& tag)
{
    if (_callbacks.empty())
        return;

    for (auto it = _callbacks.begin(); it != _callbacks.end(); )
    {
        if ((*it)->eventType == eventType && ((*it)->tag == tag || tag.isNone()))
        {
            if (_dispatching > 0)
            {
                (*it)->callback = nullptr;
                it++;
            }
            else
            {
                delete (*it);
                it = _callbacks.erase(it);
            }
        }
        else
            it++;
    }
}

void UIEventDispatcher::removeEventListeners()
{
    if (_callbacks.empty())
        return;

    if (_dispatching > 0)
    {
        for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it)
            (*it)->callback = nullptr;
    }
    else
    {
        for (auto it = _callbacks.begin(); it != _callbacks.end(); it++)
            delete (*it);
        _callbacks.clear();
    }
}

bool UIEventDispatcher::hasEventListener(int eventType, const EventTag& tag) const
{
    if (_callbacks.empty())
        return false;

    for (auto it = _callbacks.cbegin(); it != _callbacks.cend(); ++it)
    {
        if ((*it)->eventType == eventType && ((*it)->tag == tag || tag.isNone()) && (*it)->callback != nullptr)
            return true;
    }
    return false;
}

bool UIEventDispatcher::dispatchEvent(int eventType, void* data, const Value& dataValue)
{
    if (_callbacks.size() == 0)
        return false;

    EventContext context;
    context._sender = this;
    context._type = eventType;
    if (InputProcessor::_activeProcessor)
        context._inputEvent = InputProcessor::_activeProcessor->getRecentInput();
    context._dataValue = dataValue;
    context._data = data;

    doDispatch(eventType, &context);

    return context._defaultPrevented;
}

bool UIEventDispatcher::bubbleEvent(int eventType, void* data, const Value& dataValue)
{
    EventContext context;
    if (InputProcessor::_activeProcessor)
        context._inputEvent = InputProcessor::_activeProcessor->getRecentInput();
    context._type = eventType;
    context._dataValue = dataValue;
    context._data = data;

    doBubble(eventType, &context);

    return context._defaultPrevented;
}

bool UIEventDispatcher::isDispatchingEvent(int eventType)
{
    for (auto it = _callbacks.begin(); it != _callbacks.end(); ++it)
    {
        if ((*it)->eventType == eventType)
            return (*it)->dispatching > 0;
    }
    return false;
}

void UIEventDispatcher::doDispatch(int eventType, EventContext* context)
{
    retain();

    _dispatching++;
    context->_sender = this;
    bool hasDeletedItems = false;

    size_t cnt = _callbacks.size(); //dont use iterator, because new item would be added in callback.
    for (size_t i = 0; i < cnt; i++)
    {
        EventCallbackItem* ci = _callbacks[i];
        if (ci->callback == nullptr)
        {
            hasDeletedItems = true;
            continue;
        }
        if (ci->eventType == eventType)
        {
            ci->dispatching++;
            context->_touchCapture = 0;
            ci->callback(context);
            ci->dispatching--;
            if (context->_touchCapture != 0 && dynamic_cast<GObject*>(this))
            {
                if (context->_touchCapture == 1 && eventType == UIEventType::TouchBegin)
                    context->getInput()->getProcessor()->addTouchMonitor(context->getInput()->getTouchId(), dynamic_cast<GObject*>(this));
                else if (context->_touchCapture == 2)
                    context->getInput()->getProcessor()->removeTouchMonitor(dynamic_cast<GObject*>(this));
            }
        }
    }

    _dispatching--;
    if (hasDeletedItems && _dispatching == 0)
    {
        for (auto it = _callbacks.begin(); it != _callbacks.end(); )
        {
            if ((*it)->callback == nullptr)
            {
                delete (*it);
                it = _callbacks.erase(it);
            }
            else
                it++;
        }
    }

    release();
}

void UIEventDispatcher::doBubble(int eventType, EventContext* context)
{
    //parent maybe disposed in callbacks
    WeakPtr wptr(((GObject*)this)->findParent());

    if (!_callbacks.empty())
    {
        context->_isStopped = false;
        doDispatch(eventType, context);
        if (context->_isStopped)
            return;
    }

    GObject* p = wptr.ptr();
    if (p)
        p->doBubble(eventType, context);
}

NS_FGUI_END