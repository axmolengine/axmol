#include "FUIContainer.h"
#include "base/StencilStateManager.h"
#include "utils/ToolSet.h"
#include "GComponent.h"

NS_FGUI_BEGIN
using namespace ax;

#if COCOS2D_VERSION < 0x00040000
#if (AX_TARGET_PLATFORM == AX_PLATFORM_MAC || AX_TARGET_PLATFORM == AX_PLATFORM_WIN32 || AX_TARGET_PLATFORM == AX_PLATFORM_LINUX)
#define AX_CLIPPING_NODE_OPENGLES 0
#else
#define AX_CLIPPING_NODE_OPENGLES 1
#endif

#if AX_CLIPPING_NODE_OPENGLES
static void setProgram(Node *n, GLProgram *p)
{
    n->setGLProgram(p);

    auto& children = n->getChildren();
    for (const auto &child : children) {
        setProgram(child, p);
    }
}
#endif
#endif // COCOS2D_VERSION < 0x00040000

RectClippingSupport::RectClippingSupport() :
    _clippingEnabled(false),
    _scissorOldState(false),
    _clippingRectDirty(true)
{
}

StencilClippingSupport::StencilClippingSupport() :
    _stencil(nullptr),
    _originStencilProgram(nullptr),
    _stencilStateManager(new StencilStateManager())
{
}

FUIContainer::FUIContainer() :
    _rectClippingSupport(nullptr),
    _stencilClippingSupport(nullptr),
    gOwner(nullptr)
{
}

FUIContainer::~FUIContainer()
{
    AX_SAFE_DELETE(_rectClippingSupport);
    if (_stencilClippingSupport)
    {
        if (_stencilClippingSupport->_stencil)
        {
            _stencilClippingSupport->_stencil->stopAllActions();
            _stencilClippingSupport->_stencil->release();
        }
        AX_SAFE_DELETE(_stencilClippingSupport->_stencilStateManager);
        delete _stencilClippingSupport;
    }
}

bool FUIContainer::isClippingEnabled() const
{
    if (_rectClippingSupport != nullptr)
        return _rectClippingSupport->_clippingEnabled;
    else
        return false;
}

void FUIContainer::setClippingEnabled(bool value)
{
    if (_rectClippingSupport == nullptr)
    {
        if (!value)
            return;

        _rectClippingSupport = new RectClippingSupport();
    }

    _rectClippingSupport->_clippingEnabled = value;
}

const Rect & FUIContainer::getClippingRegion() const
{
    if (_rectClippingSupport != nullptr)
        return _rectClippingSupport->_clippingRegion;
    else
        return Rect::ZERO;
}

void FUIContainer::setClippingRegion(const Rect & clippingRegion)
{
    if (_rectClippingSupport == nullptr)
        _rectClippingSupport = new RectClippingSupport();

    _rectClippingSupport->_clippingRegion = clippingRegion;
}

ax::Node * FUIContainer::getStencil() const
{
    if (_stencilClippingSupport != nullptr)
        return _stencilClippingSupport->_stencil;
    else
        return nullptr;
}

void FUIContainer::setStencil(ax::Node * stencil)
{
    if (_stencilClippingSupport == nullptr)
    {
        if (stencil == nullptr)
            return;

        _stencilClippingSupport = new StencilClippingSupport();
    }

    //early out if the stencil is already set
    if (_stencilClippingSupport->_stencil == stencil)
        return;

#if AX_ENABLE_GC_FOR_NATIVE_OBJECTS
    auto sEngine = ScriptEngineManager::getInstance()->getScriptEngine();
    if (sEngine)
    {
        if (_stencilClippingSupport->_stencil)
            sEngine->releaseScriptObject(this, _stencilClippingSupport->_stencil);
        if (stencil)
            sEngine->retainScriptObject(this, stencil);
    }
#endif // AX_ENABLE_GC_FOR_NATIVE_OBJECTS

    //cleanup current stencil
    if (_stencilClippingSupport->_stencil != nullptr && _stencilClippingSupport->_stencil->isRunning())
    {
        _stencilClippingSupport->_stencil->onExitTransitionDidStart();
        _stencilClippingSupport->_stencil->onExit();
    }
    AX_SAFE_RELEASE_NULL(_stencilClippingSupport->_stencil);

    //initialise new stencil
    _stencilClippingSupport->_stencil = stencil;
    AX_SAFE_RETAIN(_stencilClippingSupport->_stencil);
    if (_stencilClippingSupport->_stencil != nullptr && this->isRunning())
    {
        _stencilClippingSupport->_stencil->onEnter();
        if (this->_isTransitionFinished)
        {
            _stencilClippingSupport->_stencil->onEnterTransitionDidFinish();
        }
    }

    if (_stencilClippingSupport->_stencil != nullptr)
#if COCOS2D_VERSION >= 0x00040000
        _stencilClippingSupport->_originStencilProgram = _stencilClippingSupport->_stencil->getProgramState();
#else
        _stencilClippingSupport->_originStencilProgram = _stencilClippingSupport->_stencil->getGLProgram();
#endif
}

float FUIContainer::getAlphaThreshold() const
{
    if (_stencilClippingSupport != nullptr)
        return _stencilClippingSupport->_stencilStateManager->getAlphaThreshold();
    else
        return 1;
}

void FUIContainer::setAlphaThreshold(float alphaThreshold)
{
    if (_stencilClippingSupport == nullptr)
        _stencilClippingSupport = new StencilClippingSupport();

#if COCOS2D_VERSION >= 0x00040000
    if (alphaThreshold == 1 && alphaThreshold != _stencilClippingSupport->_stencilStateManager->getAlphaThreshold()) {
        if (_stencilClippingSupport->_stencil) {
            restoreAllProgramStates();
        }
    }
#else
#if AX_CLIPPING_NODE_OPENGLES
    if (alphaThreshold == 1 && alphaThreshold != _stencilClippingSupport->_stencilStateManager->getAlphaThreshold())
    {
        // should reset program used by _stencil
        if (_stencilClippingSupport->_stencil)
            setProgram(_stencilClippingSupport->_stencil, _stencilClippingSupport->_originStencilProgram);
    }
#endif
#endif

    _stencilClippingSupport->_stencilStateManager->setAlphaThreshold(alphaThreshold);
}

bool FUIContainer::isInverted() const
{
    if (_stencilClippingSupport != nullptr)
        return _stencilClippingSupport->_stencilStateManager->isInverted();
    else
        return false;
}

void FUIContainer::setInverted(bool inverted)
{
    if (_stencilClippingSupport == nullptr)
        _stencilClippingSupport = new StencilClippingSupport();

    _stencilClippingSupport->_stencilStateManager->setInverted(inverted);
}

void FUIContainer::onEnter()
{
#if AX_ENABLE_SCRIPT_BINDING && COCOS2D_VERSION < 0x00040000
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnter))
            return;
    }
#endif

    Node::onEnter();

    if (_stencilClippingSupport != nullptr && _stencilClippingSupport->_stencil != nullptr)
        _stencilClippingSupport->_stencil->onEnter();

    if (_rectClippingSupport != nullptr)
        _rectClippingSupport->_clippingRectDirty = true;
}

void FUIContainer::onEnterTransitionDidFinish()
{
#if AX_ENABLE_SCRIPT_BINDING && COCOS2D_VERSION < 0x00040000
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnEnterTransitionDidFinish))
            return;
    }
#endif

    Node::onEnterTransitionDidFinish();

    if (_stencilClippingSupport != nullptr && _stencilClippingSupport->_stencil != nullptr)
    {
        _stencilClippingSupport->_stencil->onEnterTransitionDidFinish();
    }
}

void FUIContainer::onExitTransitionDidStart()
{
#if AX_ENABLE_SCRIPT_BINDING && COCOS2D_VERSION < 0x00040000
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExitTransitionDidStart))
            return;
    }
#endif

    if (_stencilClippingSupport != nullptr && _stencilClippingSupport->_stencil != nullptr)
        _stencilClippingSupport->_stencil->onExitTransitionDidStart();
    Node::onExitTransitionDidStart();
}

void FUIContainer::onExit()
{
#if AX_ENABLE_SCRIPT_BINDING && COCOS2D_VERSION < 0x00040000
    if (_scriptType == kScriptTypeJavascript)
    {
        if (ScriptEngineManager::sendNodeEventToJSExtended(this, kNodeOnExit))
            return;
    }
#endif

    if (_stencilClippingSupport != nullptr && _stencilClippingSupport->_stencil != nullptr)
        _stencilClippingSupport->_stencil->onExit();

    Node::onExit();
}

void FUIContainer::setCameraMask(unsigned short mask, bool applyChildren)
{
    Node::setCameraMask(mask, applyChildren);

    if (_stencilClippingSupport != nullptr && _stencilClippingSupport->_stencil != nullptr)
        _stencilClippingSupport->_stencil->setCameraMask(mask, applyChildren);
}

#if COCOS2D_VERSION >= 0x00040000
void FUIContainer::setProgramStateRecursively(Node* node, backend::ProgramState* programState)
{
    _originalStencilProgramState[node] = node->getProgramState();
    node->setProgramState(programState);

    auto& children = node->getChildren();
    for (const auto &child : children) {
        setProgramStateRecursively(child, programState);
    }
}

void FUIContainer::restoreAllProgramStates()
{
    for (auto item : _originalStencilProgramState)
    {
        auto node = item.first;
        auto programState = item.second;
        node->setProgramState(programState);
    }
}
#endif

void FUIContainer::onBeforeVisitScissor()
{
    auto glView = Director::getInstance()->getGLView();
    _rectClippingSupport->_scissorOldState = glView->isScissorEnabled();
    Rect clippingRect = getClippingRect();
    if (false == _rectClippingSupport->_scissorOldState)
    {
#if COCOS2D_VERSION >= 0x00040000
        Director::getInstance()->getRenderer()->setScissorTest(true);
#else
        glEnable(GL_SCISSOR_TEST);
#endif
    }
    else
    {
        _rectClippingSupport->_clippingOldRect = glView->getScissorRect();
        clippingRect = ToolSet::intersection(clippingRect, _rectClippingSupport->_clippingOldRect);
    }

    glView->setScissorInPoints(clippingRect.origin.x,
        clippingRect.origin.y,
        clippingRect.size.width,
        clippingRect.size.height);
}

void FUIContainer::onAfterVisitScissor()
{
    if (_rectClippingSupport->_scissorOldState)
    {
        auto glView = Director::getInstance()->getGLView();
        glView->setScissorInPoints(_rectClippingSupport->_clippingOldRect.origin.x,
            _rectClippingSupport->_clippingOldRect.origin.y,
            _rectClippingSupport->_clippingOldRect.size.width,
            _rectClippingSupport->_clippingOldRect.size.height);
    }
    else
    {
        // revert scissor test
#if COCOS2D_VERSION >= 0x00040000
        Director::getInstance()->getRenderer()->setScissorTest(false);
#else
        glDisable(GL_SCISSOR_TEST);
#endif
    }
}

const Rect& FUIContainer::getClippingRect()
{
    if (_rectClippingSupport->_clippingRectDirty)
    {
        Vec2 worldPos = convertToWorldSpaceAR(_rectClippingSupport->_clippingRegion.origin);
        AffineTransform t = getNodeToWorldAffineTransform();
        float scissorWidth = _rectClippingSupport->_clippingRegion.size.width*t.a;
        float scissorHeight = _rectClippingSupport->_clippingRegion.size.height*t.d;
        _rectClippingSupport->_clippingRect.setRect(worldPos.x - (scissorWidth * _anchorPoint.x), worldPos.y - (scissorHeight * _anchorPoint.y), scissorWidth, scissorHeight);
        _rectClippingSupport->_clippingRectDirty = false;
    }
    return _rectClippingSupport->_clippingRect;
}

void FUIContainer::visit(ax::Renderer * renderer, const ax::Mat4 & parentTransform, uint32_t parentFlags)
{
    if (_stencilClippingSupport != nullptr)
    {
        if (!_visible || _children.empty())
            return;

        uint32_t flags = processParentFlags(parentTransform, parentFlags);

        // IMPORTANT:
        // To ease the migration to v3.0, we still support the Mat4 stack,
        // but it is deprecated and your code should not rely on it
        Director* director = Director::getInstance();
        AXASSERT(nullptr != director, "Director is null when setting matrix stack");
        director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
        director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

        //Add group command
#if defined(AX_VERSION)
        auto* stencilGroupCommand = renderer->getNextGroupCommand();
        stencilGroupCommand->init(_globalZOrder);
        renderer->addCommand(stencilGroupCommand);
        renderer->pushGroup(stencilGroupCommand->getRenderQueueID());
#else
        _stencilClippingSupport->_groupCommand.init(_globalZOrder);
        renderer->addCommand(&_stencilClippingSupport->_groupCommand);
        renderer->pushGroup(_stencilClippingSupport->_groupCommand.getRenderQueueID());
#endif

#if COCOS2D_VERSION >= 0x00040000
        _stencilClippingSupport->_stencilStateManager->onBeforeVisit(_globalZOrder);
#else
        _stencilClippingSupport->_beforeVisitCmd.init(_globalZOrder);
        _stencilClippingSupport->_beforeVisitCmd.func = AX_CALLBACK_0(StencilStateManager::onBeforeVisit, _stencilClippingSupport->_stencilStateManager);
        renderer->addCommand(&_stencilClippingSupport->_beforeVisitCmd);
#endif

        auto alphaThreshold = this->getAlphaThreshold();
        if (alphaThreshold < 1)
        {
#if COCOS2D_VERSION >= 0x00040000
            auto* program = backend::Program::getBuiltinProgram(backend::ProgramType::POSITION_TEXTURE_COLOR_ALPHA_TEST);
            auto programState = new backend::ProgramState(program);
            auto alphaLocation = programState->getUniformLocation("u_alpha_value");
            programState->setUniform(alphaLocation, &alphaThreshold, sizeof(alphaThreshold));
            setProgramStateRecursively(_stencilClippingSupport->_stencil, programState);
            AX_SAFE_RELEASE_NULL(programState);
#else
#if AX_CLIPPING_NODE_OPENGLES
            // since glAlphaTest do not exists in OES, use a shader that writes
            // pixel only if greater than an alpha threshold
            GLProgram *program = GLProgramCache::getInstance()->getGLProgram(GLProgram::SHADER_NAME_POSITION_TEXTURE_ALPHA_TEST_NO_MV);
            GLint alphaValueLocation = glGetUniformLocation(program->getProgram(), GLProgram::UNIFORM_NAME_ALPHA_TEST_VALUE);
            // set our alphaThreshold
            program->use();
            program->setUniformLocationWith1f(alphaValueLocation, alphaThreshold);
            // we need to recursively apply this shader to all the nodes in the stencil node
            // FIXME: we should have a way to apply shader to all nodes without having to do this
            setProgram(_stencilClippingSupport->_stencil, program);
#endif
#endif

        }
        _stencilClippingSupport->_stencil->visit(renderer, _modelViewTransform, flags);

        auto afterDrawStencilCmd = renderer->nextCallbackCommand();
        afterDrawStencilCmd->init(_globalZOrder);
        afterDrawStencilCmd->func = AX_CALLBACK_0(StencilStateManager::onAfterDrawStencil, _stencilClippingSupport->_stencilStateManager);
        renderer->addCommand(afterDrawStencilCmd);

        int i = 0;
        bool visibleByCamera = isVisitableByVisitingCamera();

        if (!_children.empty())
        {
            sortAllChildren();
            // draw children zOrder < 0
            for (auto size = _children.size(); i < size; ++i)
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

        auto afterVisitCmd = renderer->nextCallbackCommand();
        afterVisitCmd->init(_globalZOrder);
        afterVisitCmd->func = AX_CALLBACK_0(StencilStateManager::onAfterVisit, _stencilClippingSupport->_stencilStateManager);
        renderer->addCommand(afterVisitCmd);

        renderer->popGroup();

        director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    }
    else if (_rectClippingSupport != nullptr && _rectClippingSupport->_clippingEnabled)
    {
        if (_transformUpdated || _contentSizeDirty || (parentFlags & FLAGS_DIRTY_MASK))
        {
            _rectClippingSupport->_clippingRectDirty = true;
        }
#if defined(AX_VERSION)
        auto* rectClippingGroupCommand = renderer->getNextGroupCommand();
        rectClippingGroupCommand->init(_globalZOrder);
        renderer->addCommand(rectClippingGroupCommand);
        renderer->pushGroup(rectClippingGroupCommand->getRenderQueueID());
#else
#    if COCOS2D_VERSION >= 0x00040000
        _rectClippingSupport->_groupCommand.init(_globalZOrder);
        renderer->addCommand(&_rectClippingSupport->_groupCommand);
        renderer->pushGroup(_rectClippingSupport->_groupCommand.getRenderQueueID());
#    endif
#endif

        auto beforeVisitCmdScissor = renderer->nextCallbackCommand();
        beforeVisitCmdScissor->init(_globalZOrder);
        beforeVisitCmdScissor->func = AX_CALLBACK_0(FUIContainer::onBeforeVisitScissor, this);
        renderer->addCommand(beforeVisitCmdScissor);

        Node::visit(renderer, parentTransform, parentFlags);

        auto afterVisitCmdScissor = renderer->nextCallbackCommand();
        afterVisitCmdScissor->init(_globalZOrder);
        afterVisitCmdScissor->func = AX_CALLBACK_0(FUIContainer::onAfterVisitScissor, this);
        renderer->addCommand(afterVisitCmdScissor);
#if COCOS2D_VERSION >= 0x00040000
        renderer->popGroup();
#endif
    }
    else
        Node::visit(renderer, parentTransform, parentFlags);
}

void FUIContainer::setGlobalZOrder(float globalZOrder)
{
    Node::setGlobalZOrder(globalZOrder);
    if (_stencilClippingSupport && _stencilClippingSupport->_stencil)
    {
        _stencilClippingSupport->_stencil->setGlobalZOrder(globalZOrder);
    }
}


NS_FGUI_END
