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

#include "axmol/scene/ComponentContainer.h"
#include "axmol/scene/Component.h"
#include "axmol/scene/Node.h"

namespace ax
{

static void eraseUnordered(tlx::pod_vector<Component*>& comps, tlx::pod_vector<Component*>::iterator& it)
{
    // swap with last and pop_back
    if (it != comps.end() - 1)
        *it = comps.back();
    comps.pop_back();
}

ComponentContainer::ComponentContainer(Node* node) : _owner(node) {}

ComponentContainer::~ComponentContainer() {}

Component* ComponentContainer::query(std::string_view name) const
{
    return this->query([name](Component* com) { return com->getName() == name; });
}

Component* ComponentContainer::query(const std::function<bool(Component*)>& pred) const
{

    auto it = std::find_if(_components.begin(), _components.end(), pred);
    return it != _components.end() ? *it : nullptr;
}

bool ComponentContainer::add(Component* comp)
{
    bool ret = false;
    AXASSERT(comp != nullptr, "Component must be non-nil");
    if (!comp->getOwner())
    {
        attachComponent(comp);
        _components.push_back(comp);
        return true;
    }

    AXLOGW("Component already added. It can't be added again"sv);
    return false;
}

bool ComponentContainer::remove(std::string_view componentName)
{
    auto it = std::find_if(_components.begin(), _components.end(),
                           [componentName](Component* comp) { return comp->getName() == componentName; });
    if (it != _components.end())
    {
        auto comp = *it;
        detachComponent(comp);
        eraseUnordered(_components, it);
        return true;
    }
    return false;
}

bool ComponentContainer::remove(Component* comp)
{
    auto it = std::find(_components.begin(), _components.end(), comp);
    if (it != _components.end())
    {
        detachComponent(comp);
        eraseUnordered(_components, it);
        return true;
    }
    return false;
}

void ComponentContainer::removeAll()
{
    if (!_components.empty())
    {
        for (auto&& comp : _components)
        {
            detachComponent(comp);
        }

        _components.clear();
        _owner->unscheduleUpdate();
    }
}

void ComponentContainer::visit(float delta)
{
    if (!_components.empty())
    {
        AX_SAFE_RETAIN(_owner);
        for (auto&& comp : _components)
        {
            comp->update(delta);
        }
        AX_SAFE_RELEASE(_owner);
    }
}

void ComponentContainer::onEnter()
{
    for (auto&& comp : _components)
    {
        comp->onEnter();
    }
}

void ComponentContainer::onExit()
{
    for (auto&& comp : _components)
    {
        comp->onExit();
    }
}

void ComponentContainer::attachComponent(Component* comp)
{
    comp->retain();
    comp->setOwner(_owner);
    comp->onAdd();
}

void ComponentContainer::detachComponent(Component* comp)
{
    comp->onRemove();
    comp->setOwner(nullptr);
    comp->release();
}

}  // namespace ax
