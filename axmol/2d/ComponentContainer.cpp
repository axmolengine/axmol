/****************************************************************************
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "2d/ComponentContainer.h"
#include "2d/Component.h"
#include "2d/Node.h"

namespace ax
{

ComponentContainer::ComponentContainer(Node* node) : _owner(node) {}

ComponentContainer::~ComponentContainer() {}

Component* ComponentContainer::get(std::string_view name) const
{
    Component* ret = nullptr;

    auto it = _componentMap.find(name);
    if (it != _componentMap.end())
    {
        ret = it->second;
    }

    return ret;
}

bool ComponentContainer::add(Component* com)
{
    bool ret = false;
    AXASSERT(com != nullptr, "Component must be non-nil");
    AXASSERT(com->getOwner() == nullptr, "Component already added. It can't be added again");
    do
    {
        auto componentName = com->getName();

        if (_componentMap.find(componentName) != _componentMap.end())
        {
            AXASSERT(false, "ComponentContainer already have this kind of component");
            break;
        }
        hlookup::set_item(_componentMap, componentName, com);  //_componentMap[componentName] = com;
        com->retain();
        com->setOwner(_owner);
        com->onAdd();

        ret = true;
    } while (0);
    return ret;
}

bool ComponentContainer::remove(std::string_view componentName)
{
    bool ret = false;
    do
    {
        auto iter = _componentMap.find(componentName);
        AX_BREAK_IF(iter == _componentMap.end());

        auto component = iter->second;
        _componentMap.erase(componentName);

        component->onRemove();
        component->setOwner(nullptr);
        component->release();

        ret = true;
    } while (0);

    return ret;
}

bool ComponentContainer::remove(Component* com)
{
    return remove(com->getName());
}

void ComponentContainer::removeAll()
{
    if (!_componentMap.empty())
    {
        for (auto&& iter : _componentMap)
        {
            iter.second->onRemove();
            iter.second->setOwner(nullptr);
            iter.second->release();
        }

        _componentMap.clear();
        _owner->unscheduleUpdate();
    }
}

void ComponentContainer::visit(float delta)
{
    if (!_componentMap.empty())
    {
        AX_SAFE_RETAIN(_owner);
        for (auto&& iter : _componentMap)
        {
            iter.second->update(delta);
        }
        AX_SAFE_RELEASE(_owner);
    }
}

void ComponentContainer::onEnter()
{
    for (auto&& iter : _componentMap)
    {
        iter.second->onEnter();
    }
}

void ComponentContainer::onExit()
{
    for (auto&& iter : _componentMap)
    {
        iter.second->onExit();
    }
}

}
