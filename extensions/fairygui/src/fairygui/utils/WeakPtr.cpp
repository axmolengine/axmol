#include "WeakPtr.h"
#include "GObject.h"

NS_FGUI_BEGIN
using namespace ax;
using namespace std;

std::unordered_map<uint64_t, GObject*> _weakPointers;

WeakPtr::WeakPtr() :_id(0)
{
}

WeakPtr::WeakPtr(GObject * obj)
{
    _id = add(obj);
}

WeakPtr::WeakPtr(const WeakPtr & other) :_id(0)
{
    *this = other;
}

WeakPtr::WeakPtr(WeakPtr && other) : _id(0)
{
    *this = std::move(other);
}

WeakPtr::~WeakPtr()
{
    if (_id != 0)
        remove(_id);
}

WeakPtr & WeakPtr::operator=(const WeakPtr & other)
{
    if (_id != 0)
        remove(_id);
    _id = add(other.ptr());
    return *this;
}

WeakPtr & WeakPtr::operator=(WeakPtr && other)
{
    if (this != &other)
    {
        if (_id != 0)
            remove(_id);
        _id = other._id;
        other._id = 0;
    }

    return *this;
}

WeakPtr & WeakPtr::operator=(GObject * obj)
{
    if (_id != 0)
        remove(_id);
    _id = add(obj);
    return *this;
}

bool WeakPtr::operator!=(const WeakPtr & v)
{
    return !(*this == v);
}

bool WeakPtr::operator!=(const WeakPtr & v) const
{
    return !(*this == v);
}

bool WeakPtr::operator==(const WeakPtr & v)
{
    const auto &t = *this;
    return t == v;
}

bool WeakPtr::operator==(const WeakPtr & v) const
{
    return _id == v._id;
}

bool WeakPtr::operator==(const GObject * v)
{
    const auto &t = *this;
    return t == v;
}

GObject * WeakPtr::ptr() const
{
    if (_id == 0 || _weakPointers.empty())
        return nullptr;

    auto it = _weakPointers.find(_id);
    if (it != _weakPointers.end())
        return it->second;
    else
        return nullptr;
}

bool WeakPtr::onStage() const
{
    GObject *p = ptr();
    return p  && p->onStage();
}

uint64_t WeakPtr::add(GObject * obj)
{
    if (obj)
    {
        if (obj->_weakPtrRef == 0)
            _weakPointers[obj->_uid] = obj;
        obj->_weakPtrRef++;
        return obj->_uid;
    }
    else
        return 0;
}

GObject* WeakPtr::remove(uint64_t id)
{
    if (_weakPointers.empty())
        return nullptr;

    auto it = _weakPointers.find(id);
    if (it != _weakPointers.end())
    {
        GObject* obj = it->second;
        obj->_weakPtrRef--;
        if (obj->_weakPtrRef == 0)
            _weakPointers.erase(it);
        return obj;
    }
    else
        return nullptr;
}

void WeakPtr::markDisposed(GObject * obj)
{
    if (_weakPointers.empty())
        return;

    auto it = _weakPointers.find(obj->_uid);
    if (it != _weakPointers.end())
        _weakPointers.erase(it);
}

NS_FGUI_END
