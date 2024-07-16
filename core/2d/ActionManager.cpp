/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2009      Valentin Milea
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

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

#include "2d/ActionManager.h"
#include "2d/Node.h"
#include "2d/Action.h"
#include "base/Scheduler.h"
#include "base/Macros.h"

NS_AX_BEGIN
//
// singleton stuff
//

ActionManager::ActionManager() : _currentTarget(nullptr), _currentTargetSalvaged(false) {}

ActionManager::~ActionManager()
{
    AXLOGV("deallocing ActionManager: {}", fmt::ptr(this));

    removeAllActions();
}

// private
void ActionManager::reserveActionCapacity(ActionHandle& element)
{
    // 4 actions per Node by default
    if (!element.actions.capacity())
    {
        element.actions.reserve(4);
    }
    else if (element.actions.size() == element.actions.capacity())
    {
        element.actions.reserve(element.actions.size() * 2);
    }
}
void ActionManager::removeActionAtIndex(ssize_t index,
                                        ActionHandle& element,
                                        std::unordered_map<Node*, ActionHandle>::iterator actionIt)
{
    Action* action = static_cast<Action*>(element.actions[index]);

    if (action == element.currentAction && (!element.currentActionSalvaged))
    {
        element.currentAction->retain();
        element.currentActionSalvaged = true;
    }

    element.actions.erase(index);

    // update actionIndex in case we are in tick. looping over the actions
    if (element.actionIndex >= index)
    {
        element.actionIndex--;
    }

    if (element.actions.empty())
    {
        if (_currentTarget == &element)
        {
            _currentTargetSalvaged = true;
        }
        else
        {
            eraseTargetActionHandle(actionIt);
        }
    }
}
// pause / resume

void ActionManager::pauseTarget(Node* target)
{
    auto it = _targets.find(target);
    if (it != _targets.end())
    {
        it->second.paused = true;
    }
}

void ActionManager::resumeTarget(Node* target)
{
    auto it = _targets.find(target);
    if (it != _targets.end())
    {
        it->second.paused = false;
    }
}

Vector<Node*> ActionManager::pauseAllRunningActions()
{
    Vector<Node*> idsWithActions;

    for (auto& [target, element] : _targets)
    {
        element.paused = true;
        idsWithActions.pushBack(const_cast<Node*>(target));
    }

    return idsWithActions;
}

void ActionManager::resumeTargets(const Vector<Node*>& targetsToResume)
{
    for (const auto& node : targetsToResume)
    {
        this->resumeTarget(node);
    }
}

// run

void ActionManager::addAction(Action* action, Node* target, bool paused)
{
    AXASSERT(action != nullptr, "action can't be nullptr!");
    AXASSERT(target != nullptr, "target can't be nullptr!");
    if (action == nullptr || target == nullptr)
        return;

    auto actionIt = _targets.find(target);
    if (actionIt == _targets.end())
    {
        actionIt                = _targets.emplace(target, ActionHandle{}).first;
        actionIt->second.paused = paused;
        target->retain();
    }

    auto& actionHandle = actionIt->second;
    reserveActionCapacity(actionHandle);

    AXASSERT(!actionHandle.actions.contains(action), "action already be added!");
    actionHandle.actions.pushBack(action);

    action->startWithTarget(target);
}

// remove

void ActionManager::removeAllActions()
{
    for (auto actionIt = _targets.begin(); actionIt != _targets.end();)
        removeTargetActionHandle(actionIt);
}

void ActionManager::removeAllActionsFromTarget(Node* target)
{
    // explicit null handling
    if (target == nullptr)
    {
        return;
    }

    auto actionIt = _targets.find(target);
    if (actionIt != _targets.end())
        removeTargetActionHandle(actionIt);
}

void ActionManager::removeTargetActionHandle(std::unordered_map<Node*, ActionHandle>::iterator& actionIt)
{
    auto& element = actionIt->second;
    if (element.actions.contains(element.currentAction) && !element.currentActionSalvaged)
    {
        element.currentAction->retain();
        element.currentActionSalvaged = true;
    }

    element.actions.clear();
    if (_currentTarget == &element)
    {
        _currentTargetSalvaged = true;
        ++actionIt;
    }
    else
    {
        eraseTargetActionHandle(actionIt);
    }
}

void ActionManager::eraseTargetActionHandle(std::unordered_map<Node*, ActionHandle>::iterator& actionIt)
{
    actionIt->first->release();
    actionIt = _targets.erase(actionIt);
}

void ActionManager::removeAction(Action* action)
{
    // explicit null handling
    if (action == nullptr)
    {
        return;
    }

    Object* target   = action->getOriginalTarget();
    auto actionIt = _targets.find(static_cast<Node*>(target));
    if (actionIt != _targets.end())
    {
        auto i = actionIt->second.actions.getIndex(action);
        if (i != AX_INVALID_INDEX)
        {
            removeActionAtIndex(i, actionIt->second, actionIt);
        }
    }
}

void ActionManager::removeActionByTag(int tag, Node* target)
{
    AXASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");
    AXASSERT(target != nullptr, "target can't be nullptr!");
    if (target == nullptr)
    {
        return;
    }

    auto actionIt = _targets.find(target);
    if (actionIt != _targets.end())
    {
        auto& element = actionIt->second;
        auto limit    = element.actions.size();
        for (int i = 0; i < limit; ++i)
        {
            Action* action = static_cast<Action*>(element.actions[i]);

            if (action->getTag() == (int)tag && action->getOriginalTarget() == target)
            {
                removeActionAtIndex(i, element, actionIt);
                break;
            }
        }
    }
}

void ActionManager::removeAllActionsByTag(int tag, Node* target)
{
    AXASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");
    AXASSERT(target != nullptr, "target can't be nullptr!");
    if (target == nullptr)
    {
        return;
    }

    auto actionIt = _targets.find(target);
    if (actionIt != _targets.end())
    {
        auto& element = actionIt->second;
        auto limit    = element.actions.size();
        for (int i = 0; i < limit;)
        {
            Action* action = static_cast<Action*>(element.actions[i]);

            if (action->getTag() == (int)tag && action->getOriginalTarget() == target)
            {
                removeActionAtIndex(i, element, actionIt);
                --limit;
            }
            else
            {
                ++i;
            }
        }
    }
}

void ActionManager::removeActionsByFlags(unsigned int flags, Node* target)
{
    if (flags == 0)
    {
        return;
    }
    AXASSERT(target != nullptr, "target can't be nullptr!");
    if (target == nullptr)
    {
        return;
    }

    auto actionIt = _targets.find(target);
    if (actionIt != _targets.end())
    {
        auto& element = actionIt->second;
        auto limit    = element.actions.size();
        for (int i = 0; i < limit;)
        {
            Action* action = static_cast<Action*>(element.actions[i]);

            if ((action->getFlags() & flags) != 0 && action->getOriginalTarget() == target)
            {
                removeActionAtIndex(i, element, actionIt);
                --limit;
            }
            else
            {
                ++i;
            }
        }
    }
}

// get

// FIXME: Passing "const O *" instead of "const O&" because HASH_FIND_IT requires the address of a pointer
// and, it is not possible to get the address of a reference
Action* ActionManager::getActionByTag(int tag, const Node* target) const
{
    AXASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");

    auto it = _targets.find(const_cast<Node*>(target));
    if (it != _targets.end())
    {
        auto& actions = it->second.actions;
        if (!actions.empty())
        {
            for (auto action : actions)
            {
                if (action->getTag() == tag)
                    return action;
            }
        }
    }

    return nullptr;
}

// FIXME: Passing "const O *" instead of "const O&" because HASH_FIND_IT requires the address of a pointer
// and, it is not possible to get the address of a reference
ssize_t ActionManager::getNumberOfRunningActionsInTarget(const Node* target) const
{
    auto it = _targets.find(const_cast<Node*>(target));
    return (it != _targets.end()) ? it->second.actions.size() : 0;
}

// FIXME: Passing "const O *" instead of "const O&" because HASH_FIND_IT requires the address of a pointer
// and, it is not possible to get the address of a reference
size_t ActionManager::getNumberOfRunningActionsInTargetByTag(const Node* target, int tag)
{
    AXASSERT(tag != Action::INVALID_TAG, "Invalid tag value!");

    auto it = _targets.find(const_cast<Node*>(target));
    if (it == _targets.end())
        return 0;

    auto& actions = it->second.actions;

    if (actions.empty())
        return 0;

    int count  = 0;
    auto limit = actions.size();
    for (int i = 0; i < limit; ++i)
    {
        auto action = actions[i];
        if (action->getTag() == tag)
            ++count;
    }

    return count;
}

ssize_t ActionManager::getNumberOfRunningActions() const
{
    ssize_t count = 0;
    for (auto& [_, element] : _targets)
        count += element.actions.size();
    return count;
}

// main loop
void ActionManager::update(float dt)
{
    for (auto actionIt = _targets.begin(); actionIt != _targets.end();)
    {
        auto elt               = &actionIt->second;
        _currentTarget         = elt;
        _currentTargetSalvaged = false;

        if (!_currentTarget->paused)
        {
            // The 'actions' MutableArray may change while inside this loop.
            for (_currentTarget->actionIndex = 0; _currentTarget->actionIndex < _currentTarget->actions.size();
                 _currentTarget->actionIndex++)
            {
                _currentTarget->currentAction =
                    static_cast<Action*>(_currentTarget->actions[_currentTarget->actionIndex]);
                if (_currentTarget->currentAction == nullptr)
                {
                    continue;
                }

                _currentTarget->currentActionSalvaged = false;

                _currentTarget->currentAction->step(dt);

                if (_currentTarget->currentActionSalvaged)
                {
                    // The currentAction told the node to remove it. To prevent the action from
                    // accidentally deallocating itself before finishing its step, we retained
                    // it. Now that step is done, it's safe to release it.
                    _currentTarget->currentAction->release();
                }
                else if (_currentTarget->currentAction->isDone())
                {
                    _currentTarget->currentAction->stop();

                    Action* action = _currentTarget->currentAction;
                    // Make currentAction nil to prevent removeAction from salvaging it.
                    _currentTarget->currentAction = nullptr;
                    removeAction(action);
                }

                _currentTarget->currentAction = nullptr;
            }
        }

        // elt, at this moment, is still valid
        // so it is safe to ask this here (issue #490)
        // elt = (tHashElement*)(elt->hh.next);

        // only delete currentTarget if no actions were scheduled during the cycle (issue #481)
        // if some node reference 'target', it's reference count >= 2 (issues #14050)
        if ((_currentTargetSalvaged && _currentTarget->actions.empty()) || actionIt->first->getReferenceCount() == 1)
        {
            eraseTargetActionHandle(actionIt);
        }
        else
            ++actionIt;
    }

    // issue #635
    _currentTarget = nullptr;
}

NS_AX_END
