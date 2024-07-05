/*
 * Copyright (c) 2012      Pierre-David Bélanger
 * Copyright (c) 2012      cocos2d-x.org
 * Copyright (c) 2013-2016 Chukong Technologies Inc.
 * Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
 * Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).
 *
 * https://axmol.dev/
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */
#include "2d/ClippingNode.h"
#include "renderer/Renderer.h"
#include "renderer/Shaders.h"
#include "renderer/backend/ProgramState.h"
#include "base/Director.h"
#include "base/StencilStateManager.h"

NS_AX_BEGIN

ClippingNode::ClippingNode() : _stencilStateManager(new StencilStateManager()) {}

ClippingNode::~ClippingNode()
{
    if (_stencil)
    {
        _stencil->stopAllActions();
        _stencil->release();
    }
    AX_SAFE_DELETE(_stencilStateManager);

    for (auto&& stencilProgramState : _originalStencilProgramState)
    {
        AX_SAFE_RELEASE(stencilProgramState.second);
    }
}

ClippingNode* ClippingNode::create()
{
    ClippingNode* ret = new ClippingNode();
    if (ret->init())
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }

    return ret;
}

ClippingNode* ClippingNode::create(Node* pStencil)
{
    ClippingNode* ret = new ClippingNode();
    if (ret->init(pStencil))
    {
        ret->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(ret);
    }

    return ret;
}

bool ClippingNode::init()
{
    return init(nullptr);
}

bool ClippingNode::init(Node* stencil)
{
    setStencil(stencil);
    return true;
}

void ClippingNode::onEnter()
{
    Node::onEnter();

    if (_stencil != nullptr)
    {
        _stencil->onEnter();
    }
    else
    {
        AXLOGW("ClippingNode warning: _stencil is nil.");
    }
}

void ClippingNode::onEnterTransitionDidFinish()
{
    Node::onEnterTransitionDidFinish();

    if (_stencil != nullptr)
    {
        _stencil->onEnterTransitionDidFinish();
    }
}

void ClippingNode::onExitTransitionDidStart()
{
    if (_stencil != nullptr)
    {
        _stencil->onExitTransitionDidStart();
    }

    Node::onExitTransitionDidStart();
}

void ClippingNode::onExit()
{
    if (_stencil != nullptr)
    {
        _stencil->onExit();
    }

    Node::onExit();
}

void ClippingNode::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (!_visible || !hasContent())
        return;

    uint32_t flags = processParentFlags(parentTransform, parentFlags);

    // IMPORTANT:
    // To ease the migration to v3.0, we still support the Mat4 stack,
    // but it is deprecated and your code should not rely on it
    AXASSERT(nullptr != _director, "Director is null when setting matrix stack");
    _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

    // Add group command

    auto* groupCommandStencil = renderer->getNextGroupCommand();
    groupCommandStencil->init(_globalZOrder);
    renderer->addCommand(groupCommandStencil);

    renderer->pushGroup(groupCommandStencil->getRenderQueueID());

    // _beforeVisitCmd.init(_globalZOrder);
    // _beforeVisitCmd.func = AX_CALLBACK_0(StencilStateManager::onBeforeVisit, _stencilStateManager);
    // renderer->addCommand(&_beforeVisitCmd);
    _stencilStateManager->onBeforeVisit(_globalZOrder);

    auto alphaThreshold = this->getAlphaThreshold();
    if (alphaThreshold < 1)
    {
        auto* program = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST);
        auto programState  = new backend::ProgramState(program);
        auto alphaLocation = programState->getUniformLocation("u_alpha_value");
        programState->setUniform(alphaLocation, &alphaThreshold, sizeof(alphaThreshold));
        setProgramStateRecursively(_stencil, programState);

        AX_SAFE_RELEASE_NULL(programState);
    }
    _stencil->visit(renderer, _modelViewTransform, flags);


    auto afterDrawStencilCmd = renderer->nextCallbackCommand();
    afterDrawStencilCmd->init(_globalZOrder);
    afterDrawStencilCmd->func = AX_CALLBACK_0(StencilStateManager::onAfterDrawStencil, _stencilStateManager);
    renderer->addCommand(afterDrawStencilCmd);

    bool visibleByCamera = isVisitableByVisitingCamera();

    // `_groupCommandChildren` is used as a barrier
    // to ensure commands above be executed before children nodes
    auto* groupCommandChildren = renderer->getNextGroupCommand();

    groupCommandChildren->init(_globalZOrder);
    renderer->addCommand(groupCommandChildren);

    renderer->pushGroup(groupCommandChildren->getRenderQueueID());

    if (!_children.empty())
    {
        sortAllChildren();
        // draw children zOrder < 0
        int i = 0;
        for (int size = static_cast<int>(_children.size()); i < size; ++i)
        {
            auto node = _children.at(i);

            if (node && node->getLocalZOrder() < 0)
                node->visit(renderer, _modelViewTransform, flags);
            else
                break;
        }
        // self draw
        if (visibleByCamera)
            this->draw(renderer, _modelViewTransform, flags);

        for (auto it = _children.cbegin() + i, itCend = _children.cend(); it != itCend; ++it)
            (*it)->visit(renderer, _modelViewTransform, flags);
    }
    else if (visibleByCamera)
    {
        this->draw(renderer, _modelViewTransform, flags);
    }

    renderer->popGroup();

    auto _afterVisitCmd = renderer->nextCallbackCommand();
    _afterVisitCmd->init(_globalZOrder);
    _afterVisitCmd->func = AX_CALLBACK_0(StencilStateManager::onAfterVisit, _stencilStateManager);
    renderer->addCommand(_afterVisitCmd);

    renderer->popGroup();

    _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void ClippingNode::setGlobalZOrder(float globalZOrder)
{
    Node::setGlobalZOrder(globalZOrder);
    
    if (_stencil) {
        // Make sure our stencil stays on the same globalZOrder:
        _stencil->setGlobalZOrder(globalZOrder);
    }
}

void ClippingNode::setCameraMask(unsigned short mask, bool applyChildren)
{
    Node::setCameraMask(mask, applyChildren);

    if (_stencil)
        _stencil->setCameraMask(mask, applyChildren);
}

Node* ClippingNode::getStencil() const
{
    return _stencil;
}

void ClippingNode::setStencil(Node* stencil, bool uniqueChildStencils)
{
    // early out if the stencil is already set
    if (_stencil == stencil)
        return;

#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        if (_stencil)
            sEngine->releaseScriptObject(this, _stencil);
        if (stencil)
            sEngine->retainScriptObject(this, stencil);
    }
#endif  // AX_ENABLE_GC_FOR_NATIVE_OBJECTS

    // cleanup current stencil
    if (_stencil != nullptr && _stencil->isRunning())
    {
        _stencil->onExitTransitionDidStart();
        _stencil->onExit();
    }
    AX_SAFE_RELEASE_NULL(_stencil);

    // initialise new stencil
    _uniqueChildStencils = uniqueChildStencils;
    _stencil = stencil;
    AX_SAFE_RETAIN(_stencil);
    if (_stencil != nullptr && this->isRunning())
    {
        _stencil->onEnter();
        if (this->_isTransitionFinished)
        {
            _stencil->onEnterTransitionDidFinish();
        }
    }

    // Clear all existing entries in _originalStencilProgramState since they belong to a different stencil
    for (auto&& stencilProgramState : _originalStencilProgramState)
    {
        AX_SAFE_RELEASE(stencilProgramState.second);
    }
    _originalStencilProgramState.clear();

    if (_stencil != nullptr)
    {
        // Make sure our stencil stays on the same globalZOrder:
        _stencil->setGlobalZOrder(getGlobalZOrder());

        auto* stencilProgramState = _stencil->getProgramState();
        AX_SAFE_RETAIN(stencilProgramState);
        _originalStencilProgramState[_stencil] = stencilProgramState;

        auto& children = _stencil->getChildren();
        for (const auto& child : children)
        {
            auto* existingProgramState = child->getProgramState();
            AX_SAFE_RETAIN(existingProgramState);
            _originalStencilProgramState[child] = existingProgramState;
        }
    }
}

bool ClippingNode::hasContent() const
{
    return !_children.empty();
}

float ClippingNode::getAlphaThreshold() const
{
    return _stencilStateManager->getAlphaThreshold();
}

void ClippingNode::setAlphaThreshold(float alphaThreshold)
{
    if (alphaThreshold == 1 && alphaThreshold != _stencilStateManager->getAlphaThreshold())
    {
        // should reset program used by _stencil
        if (_stencil)
            restoreAllProgramStates();
    }
    _stencilStateManager->setAlphaThreshold(alphaThreshold);
}

bool ClippingNode::isInverted() const
{
    return _stencilStateManager->isInverted();
}

void ClippingNode::setInverted(bool inverted)
{
    _stencilStateManager->setInverted(inverted);
}

void ClippingNode::setProgramStateRecursively(Node* node, backend::ProgramState* programState)
{
    if (_originalStencilProgramState.find(node) == _originalStencilProgramState.end())
    {
        auto* existingProgramState = node->getProgramState();
        AX_SAFE_RETAIN(existingProgramState);
        _originalStencilProgramState[node] = existingProgramState;
    }

    if (_uniqueChildStencils)
    {
        node->setProgramState(programState->clone(), true);
    }
    else
    {
        node->setProgramState(programState);
    }

    auto& children = node->getChildren();
    for (const auto& child : children)
    {
        setProgramStateRecursively(child, programState);
    }
}

void ClippingNode::restoreAllProgramStates()
{
    for (auto&& item : _originalStencilProgramState)
    {
        auto node         = item.first;
        auto programState = item.second;
        node->setProgramState(programState);
    }
}

NS_AX_END
