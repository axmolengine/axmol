#include "Relations.h"
#include "GComponent.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

Relations::Relations(GObject* owner) :
    handling(nullptr)
{
    _owner = owner;
}

Relations::~Relations()
{
    clearAll();
}

void Relations::add(GObject * target, RelationType relationType)
{
    add(target, relationType, false);
}

void Relations::add(GObject * target, RelationType relationType, bool usePercent)
{
    AXASSERT(target, "target is null");

    for (auto it = _items.begin(); it != _items.end(); ++it)
    {
        if ((*it)->getTarget() == target)
        {
            (*it)->add(relationType, usePercent);
            return;
        }
    }
    RelationItem* newItem = new RelationItem(_owner);
    newItem->setTarget(target);
    newItem->add(relationType, usePercent);
    _items.push_back(newItem);
}

void Relations::remove(GObject * target, RelationType relationType)
{
    for (auto it = _items.begin(); it != _items.end(); )
    {
        if ((*it)->getTarget() == target)
        {
            (*it)->remove(relationType);
            if ((*it)->isEmpty())
            {
                delete (*it);
                it = _items.erase(it);
            }
            else
                it++;
        }
        else
            it++;
    }
}

bool Relations::contains(GObject * target)
{
    for (auto it = _items.begin(); it != _items.end(); ++it)
    {
        if ((*it)->getTarget() == target)
            return true;
    }

    return false;
}

void Relations::clearFor(GObject * target)
{
    for (auto it = _items.begin(); it != _items.end(); )
    {
        if ((*it)->getTarget() == target)
        {
            delete (*it);
            it = _items.erase(it);
        }
        else
            it++;
    }
}

void Relations::clearAll()
{
    for (auto it = _items.begin(); it != _items.end(); ++it)
        delete (*it);
    _items.clear();
}

void Relations::copyFrom(const Relations & source)
{
    clearAll();

    for (auto it = source._items.begin(); it != source._items.end(); ++it)
    {
        RelationItem* item = new RelationItem(_owner);
        item->copyFrom(**it);
        _items.push_back(item);
    }
}

void Relations::onOwnerSizeChanged(float dWidth, float dHeight, bool applyPivot)
{
    for (auto it = _items.begin(); it != _items.end(); ++it)
        (*it)->applyOnSelfSizeChanged(dWidth, dHeight, applyPivot);
}

bool Relations::isEmpty() const
{
    return _items.size() == 0;
}

void Relations::setup(ByteBuffer * buffer, bool parentToChild)
{
    int cnt = buffer->readByte();
    GObject* target;
    for (int i = 0; i < cnt; i++)
    {
        int targetIndex = buffer->readShort();
        if (targetIndex == -1)
            target = _owner->getParent();
        else if (parentToChild)
            target = (dynamic_cast<GComponent*>(_owner))->getChildAt(targetIndex);
        else
            target = _owner->getParent()->getChildAt(targetIndex);

        RelationItem* newItem = new RelationItem(_owner);
        newItem->setTarget(target);
        _items.push_back(newItem);

        int cnt2 = buffer->readByte();
        for (int j = 0; j < cnt2; j++)
        {
            RelationType rt = (RelationType)buffer->readByte();
            bool usePercent = buffer->readBool();
            newItem->internalAdd(rt, usePercent);
        }
    }
}

NS_FGUI_END