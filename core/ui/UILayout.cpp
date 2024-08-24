/****************************************************************************
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

#include "ui/UILayout.h"
#include "ui/UIHelper.h"
#include "ui/UIScale9Sprite.h"
#include "renderer/RenderState.h"
#include "base/Director.h"
#include "renderer/Renderer.h"
#include "ui/UILayoutManager.h"
#include "2d/DrawNode.h"
#include "2d/Layer.h"
#include "2d/Sprite.h"
#include "base/EventFocus.h"
#include "base/StencilStateManager.h"
#include <algorithm>

namespace ax
{

namespace ui
{

static const int BACKGROUNDIMAGE_Z         = (-1);
static const int BCAKGROUNDCOLORRENDERER_Z = (-2);

IMPLEMENT_CLASS_GUI_INFO(Layout)

Layout::Layout()
    : _backGroundScale9Enabled(false)
    , _backGroundImage(nullptr)
    , _backGroundImageFileName("")
    , _backGroundImageCapInsets(Rect::ZERO)
    , _colorType(BackGroundColorType::NONE)
    , _bgImageTexType(TextureResType::LOCAL)
    , _backGroundImageTextureSize(Vec2::ZERO)
    , _backGroundImageColor(Color3B::WHITE)
    , _backGroundImageOpacity(255)
    , _colorRender(nullptr)
    , _gradientRender(nullptr)
    , _cColor(Color3B::WHITE)
    , _gStartColor(Color3B::WHITE)
    , _gEndColor(Color3B::WHITE)
    , _alongVector(Vec2(0.0f, -1.0f))
    , _cOpacity(255)
    , _clippingEnabled(false)
    , _layoutType(Type::ABSOLUTE)
    , _clippingType(ClippingType::STENCIL)
    , _clippingStencil(nullptr)
    , _clippingRect(Rect::ZERO)
    , _clippingParent(nullptr)
    , _clippingRectDirty(true)
    , _stencilStateManager(new StencilStateManager())
    , _doLayoutDirty(true)
    , _isInterceptTouch(false)
    , _loopFocus(false)
    , _passFocusToChild(true)
    , _isFocusPassing(false)
{
    // no-op
}

Layout::~Layout()
{
    AX_SAFE_RELEASE(_clippingStencil);
    AX_SAFE_DELETE(_stencilStateManager);
}

void Layout::onEnter()
{
    Widget::onEnter();
    if (_clippingStencil)
    {
        _clippingStencil->onEnter();
    }
    _doLayoutDirty     = true;
    _clippingRectDirty = true;
}

void Layout::onExit()
{
    Widget::onExit();
    if (_clippingStencil)
    {
        _clippingStencil->onExit();
    }
}

void Layout::setGlobalZOrder(float globalZOrder)
{
    // _protectedChildren's global z order is set in ProtectedNode::setGlobalZOrder()

    Widget::setGlobalZOrder(globalZOrder);
    if (_clippingStencil)
        _clippingStencil->setGlobalZOrder(globalZOrder);

    for (auto&& child : _children)
        child->setGlobalZOrder(globalZOrder);
}

Layout* Layout::create()
{
    Layout* layout = new Layout();
    if (layout->init())
    {
        layout->autorelease();
        return layout;
    }
    AX_SAFE_DELETE(layout);
    return nullptr;
}

bool Layout::init()
{
    if (Widget::init())
    {
        ignoreContentAdaptWithSize(false);
        setContentSize(Vec2::ZERO);
        setAnchorPoint(Vec2::ZERO);
        onPassFocusToChild = AX_CALLBACK_2(Layout::findNearestChildWidgetIndex, this);
        return true;
    }
    return false;
}

void Layout::addChild(Node* child)
{
    Layout::addChild(child, child->getLocalZOrder(), child->getTag());
}

void Layout::addChild(Node* child, int localZOrder)
{
    Layout::addChild(child, localZOrder, child->getTag());
}

void Layout::addChild(Node* child, int zOrder, int tag)
{
    if (dynamic_cast<Widget*>(child))
    {
        supplyTheLayoutParameterLackToChild(static_cast<Widget*>(child));
    }
    child->setGlobalZOrder(_globalZOrder);
    Widget::addChild(child, zOrder, tag);
    _doLayoutDirty = true;
}

void Layout::addChild(Node* child, int zOrder, std::string_view name)
{
    if (dynamic_cast<Widget*>(child))
    {
        supplyTheLayoutParameterLackToChild(static_cast<Widget*>(child));
    }
    child->setGlobalZOrder(_globalZOrder);
    Widget::addChild(child, zOrder, name);
    _doLayoutDirty = true;
}

void Layout::removeChild(Node* child, bool cleanup)
{
    Widget::removeChild(child, cleanup);
    _doLayoutDirty = true;
}

void Layout::removeAllChildren()
{
    Widget::removeAllChildren();
    _doLayoutDirty = true;
}

void Layout::removeAllChildrenWithCleanup(bool cleanup)
{
    Widget::removeAllChildrenWithCleanup(cleanup);
    _doLayoutDirty = true;
}

bool Layout::isClippingEnabled() const
{
    return _clippingEnabled;
}

void Layout::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (!_visible)
    {
        return;
    }

    if (FLAGS_TRANSFORM_DIRTY & parentFlags || _transformUpdated || _contentSizeDirty)
        _clippingRectDirty = true;

    adaptRenderers();
    doLayout();

    if (_clippingEnabled)
    {
        switch (_clippingType)
        {
        case ClippingType::STENCIL:
            stencilClippingVisit(renderer, parentTransform, parentFlags);
            break;
        case ClippingType::SCISSOR:
            scissorClippingVisit(renderer, parentTransform, parentFlags);
            break;
        default:
            break;
        }
    }
    else
    {
        // no need to adapt render again
        ProtectedNode::visit(renderer, parentTransform, parentFlags);
    }
}

void Layout::stencilClippingVisit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (!_visible)
        return;

    uint32_t flags = processParentFlags(parentTransform, parentFlags);

    // IMPORTANT:
    // To ease the migration to v3.0, we still support the Mat4 stack,
    // but it is deprecated and your code should not rely on it
    _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);
    // Add group command

    auto* groupCommand = renderer->getNextGroupCommand();
    groupCommand->init(_globalZOrder);
    renderer->addCommand(groupCommand);
    renderer->pushGroup(groupCommand->getRenderQueueID());

    //    _beforeVisitCmdStencil.init(_globalZOrder);
    //    _beforeVisitCmdStencil.func = AX_CALLBACK_0(StencilStateManager::onBeforeVisit, _stencilStateManager);
    //    renderer->addCommand(&_beforeVisitCmdStencil);
    _stencilStateManager->onBeforeVisit(_globalZOrder);

    _clippingStencil->visit(renderer, _modelViewTransform, flags);

    auto afterDrawStencilCmd = renderer->nextCallbackCommand();
    afterDrawStencilCmd->init(_globalZOrder);
    afterDrawStencilCmd->func = AX_CALLBACK_0(StencilStateManager::onAfterDrawStencil, _stencilStateManager);
    renderer->addCommand(afterDrawStencilCmd);

    int i = 0;  // used by _children
    int j = 0;  // used by _protectedChildren

    sortAllChildren();
    sortAllProtectedChildren();

    //
    // draw children and protectedChildren zOrder < 0
    //
    for (auto size = _children.size(); i < size; i++)
    {
        auto node = _children.at(i);

        if (node && node->getLocalZOrder() < 0)
            node->visit(renderer, _modelViewTransform, flags);
        else
            break;
    }

    for (auto size = _protectedChildren.size(); j < size; j++)
    {
        auto node = _protectedChildren.at(j);

        if (node && node->getLocalZOrder() < 0)
            node->visit(renderer, _modelViewTransform, flags);
        else
            break;
    }

    //
    // draw self
    //
    this->draw(renderer, _modelViewTransform, flags);

    //
    // draw children and protectedChildren zOrder >= 0
    //
    for (auto it = _protectedChildren.cbegin() + j, itCend = _protectedChildren.cend(); it != itCend; ++it)
        (*it)->visit(renderer, _modelViewTransform, flags);

    for (auto it = _children.cbegin() + i, itCend = _children.cend(); it != itCend; ++it)
        (*it)->visit(renderer, _modelViewTransform, flags);

    auto afterVisitCmdStencil = renderer->nextCallbackCommand();
    afterVisitCmdStencil->init(_globalZOrder);
    afterVisitCmdStencil->func = AX_CALLBACK_0(StencilStateManager::onAfterVisit, _stencilStateManager);
    renderer->addCommand(afterVisitCmdStencil);

    renderer->popGroup();

    _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void Layout::onBeforeVisitScissor()
{
    auto glView = _director->getGLView();
    // apply scissor test
    _scissorOldState = glView->isScissorEnabled();
    if (false == _scissorOldState)
    {
        auto renderer = _director->getRenderer();
        renderer->setScissorTest(true);
    }

    // apply scissor box
    Rect clippingRect = getClippingRect();
    _clippingOldRect  = glView->getScissorRect();
    if (false == _clippingOldRect.equals(clippingRect))
    {
        glView->setScissorInPoints(clippingRect.origin.x, clippingRect.origin.y, clippingRect.size.width,
                                   clippingRect.size.height);
    }
}

void Layout::onAfterVisitScissor()
{
    if (_scissorOldState)
    {
        // revert scissor box
        if (false == _clippingOldRect.equals(_clippingRect))
        {
            auto glView = _director->getGLView();
            glView->setScissorInPoints(_clippingOldRect.origin.x, _clippingOldRect.origin.y,
                                       _clippingOldRect.size.width, _clippingOldRect.size.height);
        }
    }
    else
    {
        // revert scissor test
        auto renderer = _director->getRenderer();
        renderer->setScissorTest(false);
    }
}

void Layout::scissorClippingVisit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (parentFlags & FLAGS_DIRTY_MASK)
    {
        _clippingRectDirty = true;
    }

    _director->pushMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
    _director->loadMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW, _modelViewTransform);

    auto* groupCommand = renderer->getNextGroupCommand();
    groupCommand->init(_globalZOrder);
    renderer->addCommand(groupCommand);
    renderer->pushGroup(groupCommand->getRenderQueueID());

    auto beforeVisitCmdScissor = renderer->nextCallbackCommand();
    beforeVisitCmdScissor->init(_globalZOrder);
    beforeVisitCmdScissor->func = AX_CALLBACK_0(Layout::onBeforeVisitScissor, this);
    renderer->addCommand(beforeVisitCmdScissor);

    ProtectedNode::visit(renderer, parentTransform, parentFlags);

    auto afterVisitCmdScissor = renderer->nextCallbackCommand();
    afterVisitCmdScissor->init(_globalZOrder);
    afterVisitCmdScissor->func = AX_CALLBACK_0(Layout::onAfterVisitScissor, this);
    renderer->addCommand(afterVisitCmdScissor);

    renderer->popGroup();
    _director->popMatrix(MATRIX_STACK_TYPE::MATRIX_STACK_MODELVIEW);
}

void Layout::setClippingEnabled(bool able)
{
    if (able == _clippingEnabled)
    {
        return;
    }
    _clippingEnabled = able;
    switch (_clippingType)
    {
    case ClippingType::STENCIL:
        if (able)
        {
            _clippingStencil = DrawNode::create();
            _clippingStencil->setGlobalZOrder(_globalZOrder);
            if (_running)
            {
                _clippingStencil->onEnter();
            }
            _clippingStencil->retain();
            setStencilClippingSize(_contentSize);
        }
        else
        {
            if (_running)
            {
                _clippingStencil->onExit();
            }
            _clippingStencil->release();
            _clippingStencil = nullptr;
        }
        break;
    default:
        break;
    }
}

void Layout::setClippingType(ClippingType type)
{
    if (type == _clippingType)
    {
        return;
    }
    bool clippingEnabled = isClippingEnabled();
    setClippingEnabled(false);
    _clippingType = type;
    setClippingEnabled(clippingEnabled);
}

Layout::ClippingType Layout::getClippingType() const
{
    return _clippingType;
}

void Layout::setStencilClippingSize(const Vec2& /*size*/)
{
    if (_clippingEnabled && _clippingType == ClippingType::STENCIL)
    {
        _clippingStencil->clear();
        _clippingStencil->drawSolidRect(Vec2::ZERO, _contentSize, Color4F::GREEN);  // Fix issue #1546
    }
}

const Rect& Layout::getClippingRect()
{
    if (_clippingRectDirty)
    {
        const auto worldPos1 = convertToWorldSpace(Vec2::ZERO);
        const auto worldPos2 = convertToWorldSpace(Vec2(_contentSize.width, _contentSize.height));

        // Node can be flipped
        const auto worldPos      = Vec2(std::min(worldPos1.x, worldPos2.x), std::min(worldPos1.y, worldPos2.y));
        const auto scissorWidth  = std::fabs(worldPos2.x - worldPos1.x);
        const auto scissorHeight = std::fabs(worldPos2.y - worldPos1.y);

        Layout* parent = this;

        while (parent)
        {
            parent = dynamic_cast<Layout*>(parent->getParent());
            if (parent)
            {
                if (parent->isClippingEnabled())
                {
                    _clippingParent = parent;
                    break;
                }
            }
        }

        if (_clippingParent)
        {
            const auto& parentClippingRect = _clippingParent->getClippingRect();

            _clippingRect.origin.x = std::max(parentClippingRect.origin.x, worldPos.x);
            _clippingRect.origin.y = std::max(parentClippingRect.origin.y, worldPos.y);

            const auto right =
                std::min(parentClippingRect.origin.x + parentClippingRect.size.width, worldPos.x + scissorWidth);
            const auto top =
                std::min(parentClippingRect.origin.y + parentClippingRect.size.height, worldPos.y + scissorHeight);

            _clippingRect.size.width  = std::max(0.0f, right - _clippingRect.origin.x);
            _clippingRect.size.height = std::max(0.0f, top - _clippingRect.origin.y);
        }
        else
        {
            _clippingRect.origin.x    = worldPos.x;
            _clippingRect.origin.y    = worldPos.y;
            _clippingRect.size.width  = scissorWidth;
            _clippingRect.size.height = scissorHeight;
        }
        _clippingRectDirty = false;
    }
    return _clippingRect;
}

void Layout::onSizeChanged()
{
    Widget::onSizeChanged();
    setStencilClippingSize(_contentSize);
    _doLayoutDirty     = true;
    _clippingRectDirty = true;
    if (_backGroundImage)
    {
        _backGroundImage->setPosition(_contentSize.width / 2.0f, _contentSize.height / 2.0f);
        if (_backGroundScale9Enabled)
        {
            _backGroundImage->setPreferredSize(_contentSize);
        }
        else
        {
            _backGroundImage->setPreferredSize(_backGroundImageTextureSize);
        }
    }
    if (_colorRender)
    {
        _colorRender->setContentSize(_contentSize);
    }
    if (_gradientRender)
    {
        _gradientRender->setContentSize(_contentSize);
    }
}

void Layout::setBackGroundImageScale9Enabled(bool able)
{
    if (_backGroundScale9Enabled == able)
    {
        return;
    }
    _backGroundScale9Enabled = able;
    if (nullptr == _backGroundImage)
    {
        addBackGroundImage();
        setBackGroundImage(_backGroundImageFileName, _bgImageTexType);
    }
    if (_backGroundScale9Enabled)
    {
        _backGroundImage->setRenderingType(Scale9Sprite::RenderingType::SLICE);
        _backGroundImage->setPreferredSize(_contentSize);
    }
    else
    {
        _backGroundImage->setRenderingType(Scale9Sprite::RenderingType::SIMPLE);
        _backGroundImage->setPreferredSize(_backGroundImageTextureSize);
    }

    setBackGroundImageCapInsets(_backGroundImageCapInsets);
}

bool Layout::isBackGroundImageScale9Enabled() const
{
    return _backGroundScale9Enabled;
}

void Layout::setBackGroundImage(std::string_view fileName, TextureResType texType)
{
    if (fileName.empty())
    {
        return;
    }
    if (_backGroundImage == nullptr)
    {
        addBackGroundImage();
        if (_backGroundScale9Enabled)
        {
            _backGroundImage->setRenderingType(Scale9Sprite::RenderingType::SLICE);
        }
        else
        {
            _backGroundImage->setRenderingType(Scale9Sprite::RenderingType::SIMPLE);
        }
    }
    _backGroundImageFileName = fileName;
    _bgImageTexType          = texType;

    switch (_bgImageTexType)
    {
    case TextureResType::LOCAL:
        _backGroundImage->initWithFile(fileName);
        break;
    case TextureResType::PLIST:
        _backGroundImage->initWithSpriteFrameName(fileName);
        break;
    default:
        break;
    }

    _backGroundImageTextureSize = _backGroundImage->getContentSize();
    _backGroundImage->setPosition(_contentSize.width / 2.0f, _contentSize.height / 2.0f);
    if (_backGroundScale9Enabled)
    {
        _backGroundImage->setPreferredSize(_contentSize);
    }
    else
    {
        _backGroundImage->setPreferredSize(_backGroundImageTextureSize);
    }
    updateBackGroundImageRGBA();
}

void Layout::setBackGroundImageCapInsets(const Rect& capInsets)
{
    _backGroundImageCapInsets = capInsets;
    if (_backGroundScale9Enabled && _backGroundImage)
    {
        _backGroundImage->setCapInsets(capInsets);
    }
}

const Rect& Layout::getBackGroundImageCapInsets() const
{
    return _backGroundImageCapInsets;
}

void Layout::supplyTheLayoutParameterLackToChild(Widget* child)
{
    if (!child)
    {
        return;
    }
    switch (_layoutType)
    {
    case Type::ABSOLUTE:
        break;
    case Type::HORIZONTAL:
    case Type::VERTICAL:
    case Type::CENTER_VERTICAL:
    case Type::CENTER_HORIZONTAL:
    {
        LinearLayoutParameter* layoutParameter = dynamic_cast<LinearLayoutParameter*>(child->getLayoutParameter());
        if (!layoutParameter)
        {
            child->setLayoutParameter(LinearLayoutParameter::create());
        }
        break;
    }
    case Type::RELATIVE:
    {
        RelativeLayoutParameter* layoutParameter = dynamic_cast<RelativeLayoutParameter*>(child->getLayoutParameter());
        if (!layoutParameter)
        {
            child->setLayoutParameter(RelativeLayoutParameter::create());
        }
        break;
    }
    default:
        break;
    }
}

void Layout::addBackGroundImage()
{
    _backGroundImage = Scale9Sprite::create();
    _backGroundImage->setRenderingType(Scale9Sprite::RenderingType::SIMPLE);

    addProtectedChild(_backGroundImage, BACKGROUNDIMAGE_Z, -1);

    _backGroundImage->setPosition(_contentSize.width / 2.0f, _contentSize.height / 2.0f);
}

void Layout::removeBackGroundImage()
{
    if (!_backGroundImage)
    {
        return;
    }
    removeProtectedChild(_backGroundImage);
    _backGroundImage            = nullptr;
    _backGroundImageFileName    = "";
    _backGroundImageTextureSize = Vec2::ZERO;
}

void Layout::setBackGroundColorType(BackGroundColorType type)
{
    if (_colorType == type)
    {
        return;
    }
    switch (_colorType)
    {
    case BackGroundColorType::NONE:
        if (_colorRender)
        {
            removeProtectedChild(_colorRender);
            _colorRender = nullptr;
        }
        if (_gradientRender)
        {
            removeProtectedChild(_gradientRender);
            _gradientRender = nullptr;
        }
        break;
    case BackGroundColorType::SOLID:
        if (_colorRender)
        {
            removeProtectedChild(_colorRender);
            _colorRender = nullptr;
        }
        break;
    case BackGroundColorType::GRADIENT:
        if (_gradientRender)
        {
            removeProtectedChild(_gradientRender);
            _gradientRender = nullptr;
        }
        break;
    default:
        break;
    }
    _colorType = type;
    switch (_colorType)
    {
    case BackGroundColorType::NONE:
        break;
    case BackGroundColorType::SOLID:
        _colorRender = LayerColor::create();
        _colorRender->setContentSize(_contentSize);
        _colorRender->setOpacity(_cOpacity);
        _colorRender->setColor(_cColor);
        addProtectedChild(_colorRender, BCAKGROUNDCOLORRENDERER_Z, -1);
        break;
    case BackGroundColorType::GRADIENT:
        _gradientRender = LayerGradient::create();
        _gradientRender->setContentSize(_contentSize);
        _gradientRender->setOpacity(_cOpacity);
        _gradientRender->setStartColor(_gStartColor);
        _gradientRender->setEndColor(_gEndColor);
        _gradientRender->setVector(_alongVector);
        addProtectedChild(_gradientRender, BCAKGROUNDCOLORRENDERER_Z, -1);
        break;
    default:
        break;
    }
}

Layout::BackGroundColorType Layout::getBackGroundColorType() const
{
    return _colorType;
}

void Layout::setBackGroundColor(const Color3B& color)
{
    _cColor = color;
    if (_colorRender)
    {
        _colorRender->setColor(color);
    }
}

const Color3B& Layout::getBackGroundColor() const
{
    return _cColor;
}

void Layout::setBackGroundColor(const Color3B& startColor, const Color3B& endColor)
{
    _gStartColor = startColor;
    if (_gradientRender)
    {
        _gradientRender->setStartColor(startColor);
    }
    _gEndColor = endColor;
    if (_gradientRender)
    {
        _gradientRender->setEndColor(endColor);
    }
}

const Color3B& Layout::getBackGroundStartColor() const
{
    return _gStartColor;
}

const Color3B& Layout::getBackGroundEndColor() const
{
    return _gEndColor;
}

void Layout::setBackGroundColorOpacity(uint8_t opacity)
{
    _cOpacity = opacity;
    switch (_colorType)
    {
    case BackGroundColorType::NONE:
        break;
    case BackGroundColorType::SOLID:
        _colorRender->setOpacity(opacity);
        break;
    case BackGroundColorType::GRADIENT:
        _gradientRender->setOpacity(opacity);
        break;
    default:
        break;
    }
}

uint8_t Layout::getBackGroundColorOpacity() const
{
    return _cOpacity;
}

void Layout::setBackGroundColorVector(const Vec2& vector)
{
    _alongVector = vector;
    if (_gradientRender)
    {
        _gradientRender->setVector(vector);
    }
}

const Vec2& Layout::getBackGroundColorVector() const
{
    return _alongVector;
}

void Layout::setBackGroundImageColor(const Color3B& color)
{
    _backGroundImageColor = color;
    updateBackGroundImageColor();
}

void Layout::setBackGroundImageOpacity(uint8_t opacity)
{
    _backGroundImageOpacity = opacity;
    updateBackGroundImageOpacity();
}

const Color3B& Layout::getBackGroundImageColor() const
{
    return _backGroundImageColor;
}

uint8_t Layout::getBackGroundImageOpacity() const
{
    return _backGroundImageOpacity;
}

void Layout::updateBackGroundImageColor()
{
    if (_backGroundImage)
    {
        _backGroundImage->setColor(_backGroundImageColor);
    }
}

void Layout::updateBackGroundImageOpacity()
{
    if (_backGroundImage)
    {
        _backGroundImage->setOpacity(_backGroundImageOpacity);
    }
}

void Layout::updateBackGroundImageRGBA()
{
    if (_backGroundImage)
    {
        _backGroundImage->setColor(_backGroundImageColor);
        _backGroundImage->setOpacity(_backGroundImageOpacity);
    }
}

const Vec2& Layout::getBackGroundImageTextureSize() const
{
    return _backGroundImageTextureSize;
}

void Layout::setLayoutType(Type type)
{
    _layoutType = type;

    for (auto&& child : _children)
    {
        Widget* widgetChild = dynamic_cast<Widget*>(child);
        if (widgetChild)
        {
            supplyTheLayoutParameterLackToChild(static_cast<Widget*>(child));
        }
    }
    _doLayoutDirty = true;
}

Layout::Type Layout::getLayoutType() const
{
    return _layoutType;
}

void Layout::forceDoLayout()
{
    this->requestDoLayout();
    this->doLayout();
}

void Layout::requestDoLayout()
{
    _doLayoutDirty = true;
}

Vec2 Layout::getLayoutContentSize() const
{
    return this->getContentSize();
}

const Vector<Node*>& Layout::getLayoutElements() const
{
    return this->getChildren();
}

LayoutManager* Layout::createLayoutManager()
{
    LayoutManager* exe = nullptr;
    switch (_layoutType)
    {
    case Type::VERTICAL:
        exe = LinearVerticalLayoutManager::create();
        break;
    case Type::CENTER_VERTICAL:
        exe = LinearCenterVerticalLayoutManager::create();
        break;
    case Type::CENTER_HORIZONTAL:
        exe = LinearCenterHorizontalLayoutManager::create();
        break;
    case Type::HORIZONTAL:
        exe = LinearHorizontalLayoutManager::create();
        break;
    case Type::RELATIVE:
        exe = RelativeLayoutManager::create();
        break;
    default:
        break;
    }
    return exe;
}

void Layout::doLayout()
{

    if (!_doLayoutDirty)
    {
        return;
    }

    sortAllChildren();

    LayoutManager* executant = this->createLayoutManager();

    if (executant)
    {
        executant->doLayout(this);
    }

    _doLayoutDirty = false;
}

std::string Layout::getDescription() const
{
    return "Layout";
}

Widget* Layout::createCloneInstance()
{
    return Layout::create();
}

void Layout::copyClonedWidgetChildren(Widget* model)
{
    Widget::copyClonedWidgetChildren(model);
}

void Layout::copySpecialProperties(Widget* widget)
{
    Layout* layout = dynamic_cast<Layout*>(widget);
    if (layout)
    {
        setBackGroundImageScale9Enabled(layout->_backGroundScale9Enabled);
        setBackGroundImage(layout->_backGroundImageFileName, layout->_bgImageTexType);
        setBackGroundImageCapInsets(layout->_backGroundImageCapInsets);
        setBackGroundColorType(layout->_colorType);
        setBackGroundColor(layout->_cColor);
        setBackGroundColor(layout->_gStartColor, layout->_gEndColor);
        setBackGroundColorOpacity(layout->_cOpacity);
        setBackGroundColorVector(layout->_alongVector);
        setLayoutType(layout->_layoutType);
        setClippingEnabled(layout->_clippingEnabled);
        setClippingType(layout->_clippingType);
        _loopFocus        = layout->_loopFocus;
        _passFocusToChild = layout->_passFocusToChild;
        _isInterceptTouch = layout->_isInterceptTouch;
    }
}

void Layout::setLoopFocus(bool loop)
{
    _loopFocus = loop;
}

bool Layout::isLoopFocus() const
{
    return _loopFocus;
}

void Layout::setPassFocusToChild(bool pass)
{
    _passFocusToChild = pass;
}

bool Layout::isPassFocusToChild() const
{
    return _passFocusToChild;
}

Vec2 Layout::getLayoutAccumulatedSize() const
{
    const auto& children = this->getChildren();
    Vec2 layoutSize      = Vec2::ZERO;
    int widgetCount      = 0;
    for (const auto& widget : children)
    {
        Layout* layout = dynamic_cast<Layout*>(widget);
        if (nullptr != layout)
        {
            layoutSize = layoutSize + layout->getLayoutAccumulatedSize();
        }
        else
        {
            Widget* w = dynamic_cast<Widget*>(widget);
            if (w)
            {
                widgetCount++;
                Margin m   = w->getLayoutParameter()->getMargin();
                layoutSize = layoutSize + w->getContentSize() + Vec2(m.right + m.left, m.top + m.bottom) * 0.5;
            }
        }
    }

    // subtract extra size
    Type type = this->getLayoutType();
    if (type == Type::HORIZONTAL || type == Type::CENTER_HORIZONTAL)
    {
        layoutSize = layoutSize - Vec2(0, layoutSize.height / widgetCount * (widgetCount - 1));
    }
    if (type == Type::VERTICAL || type == Type::CENTER_VERTICAL)
    {
        layoutSize = layoutSize - Vec2(layoutSize.width / widgetCount * (widgetCount - 1), 0);
    }
    return layoutSize;
}

Vec2 Layout::getWorldCenterPoint(Widget* widget) const
{
    Layout* layout = dynamic_cast<Layout*>(widget);
    // FIXEDME: we don't need to calculate the content size of layout anymore
    Vec2 widgetSize = layout ? layout->getLayoutAccumulatedSize() : widget->getContentSize();
    //    AXLOGD("content size : width = {}, height = {}", widgetSize.width, widgetSize.height);
    return widget->convertToWorldSpace(Vec2(widgetSize.width / 2, widgetSize.height / 2));
}

float Layout::calculateNearestDistance(Widget* baseWidget)
{
    float distance = FLT_MAX;

    Vec2 widgetPosition = this->getWorldCenterPoint(baseWidget);

    for (Node* node : _children)
    {
        Layout* layout = dynamic_cast<Layout*>(node);
        int length;
        if (layout)
        {
            length = layout->calculateNearestDistance(baseWidget);
        }
        else
        {
            Widget* w = dynamic_cast<Widget*>(node);
            if (w && w->isFocusEnabled())
            {
                Vec2 wPosition = this->getWorldCenterPoint(w);
                length         = (wPosition - widgetPosition).length();
            }
            else
            {
                continue;
            }
        }

        if (length < distance)
        {
            distance = length;
        }
    }
    return distance;
}

float Layout::calculateFarthestDistance(ax::ui::Widget* baseWidget)
{
    float distance = -FLT_MAX;

    Vec2 widgetPosition = this->getWorldCenterPoint(baseWidget);

    for (Node* node : _children)
    {
        Layout* layout = dynamic_cast<Layout*>(node);
        int length;
        if (layout)
        {
            length = layout->calculateFarthestDistance(baseWidget);
        }
        else
        {
            Widget* w = dynamic_cast<Widget*>(node);
            if (w && w->isFocusEnabled())
            {
                Vec2 wPosition = this->getWorldCenterPoint(w);
                length         = (wPosition - widgetPosition).length();
            }
            else
            {
                continue;
            }
        }

        if (length > distance)
        {
            distance = length;
        }
    }
    return distance;
}

int Layout::findFirstFocusEnabledWidgetIndex()
{
    ssize_t index = 0;
    ssize_t count = this->getChildren().size();
    while (index < count)
    {
        Widget* w = dynamic_cast<Widget*>(_children.at(index));
        if (w && w->isFocusEnabled())
        {
            return (int)index;
        }
        index++;
    }
    AXASSERT(0, "invalid operation");
    return 0;
}

int Layout::findNearestChildWidgetIndex(FocusDirection direction, Widget* baseWidget)
{
    if (baseWidget == nullptr || baseWidget == this)
    {
        return this->findFirstFocusEnabledWidgetIndex();
    }
    int index     = 0;
    ssize_t count = this->getChildren().size();

    float distance = FLT_MAX;
    int found      = 0;
    if (direction == FocusDirection::LEFT || direction == FocusDirection::RIGHT || direction == FocusDirection::DOWN ||
        direction == FocusDirection::UP)
    {
        Vec2 widgetPosition = this->getWorldCenterPoint(baseWidget);
        while (index < count)
        {
            Widget* w = dynamic_cast<Widget*>(this->getChildren().at(index));
            if (w && w->isFocusEnabled())
            {
                Vec2 wPosition = this->getWorldCenterPoint(w);
                float length;
                Layout* layout = dynamic_cast<Layout*>(w);
                if (layout)
                {
                    length = layout->calculateNearestDistance(baseWidget);
                }
                else
                {
                    length = (wPosition - widgetPosition).getLength();
                }

                if (length < distance)
                {
                    found    = index;
                    distance = length;
                }
            }
            index++;
        }
        return found;
    }

    AXASSERT(0, "invalid focus direction!!!");
    return 0;
}

int Layout::findFarthestChildWidgetIndex(FocusDirection direction, ax::ui::Widget* baseWidget)
{
    if (baseWidget == nullptr || baseWidget == this)
    {
        return this->findFirstFocusEnabledWidgetIndex();
    }
    int index     = 0;
    ssize_t count = this->getChildren().size();

    float distance = -FLT_MAX;
    int found      = 0;
    if (direction == FocusDirection::LEFT || direction == FocusDirection::RIGHT || direction == FocusDirection::DOWN ||
        direction == FocusDirection::UP)
    {
        Vec2 widgetPosition = this->getWorldCenterPoint(baseWidget);
        while (index < count)
        {
            Widget* w = dynamic_cast<Widget*>(this->getChildren().at(index));
            if (w && w->isFocusEnabled())
            {
                Vec2 wPosition = this->getWorldCenterPoint(w);
                float length;
                Layout* layout = dynamic_cast<Layout*>(w);
                if (layout)
                {
                    length = layout->calculateFarthestDistance(baseWidget);
                }
                else
                {
                    length = (wPosition - widgetPosition).getLength();
                }

                if (length > distance)
                {
                    found    = index;
                    distance = length;
                }
            }
            index++;
        }
        return found;
    }

    AXASSERT(0, "invalid focus direction!!!");
    return 0;
}

Widget* Layout::findFocusEnabledChildWidgetByIndex(ssize_t index)
{

    Widget* widget = this->getChildWidgetByIndex(index);

    if (widget)
    {
        if (widget->isFocusEnabled())
        {
            return widget;
        }
        index = index + 1;
        return this->findFocusEnabledChildWidgetByIndex(index);
    }
    return nullptr;
}

Widget* Layout::findFirstNonLayoutWidget()
{
    Widget* widget = nullptr;
    for (Node* node : _children)
    {
        Layout* layout = dynamic_cast<Layout*>(node);
        if (layout)
        {
            widget = layout->findFirstNonLayoutWidget();
            if (widget != nullptr)
            {
                return widget;
            }
        }
        else
        {
            Widget* w = dynamic_cast<Widget*>(node);
            if (w)
            {
                widget = w;
                break;
            }
        }
    }

    return widget;
}

void Layout::findProperSearchingFunctor(FocusDirection dir, Widget* baseWidget)
{
    if (baseWidget == nullptr)
    {
        return;
    }

    Vec2 previousWidgetPosition = this->getWorldCenterPoint(baseWidget);

    Vec2 widgetPosition = this->getWorldCenterPoint(this->findFirstNonLayoutWidget());

    if (dir == FocusDirection::LEFT)
    {
        if (previousWidgetPosition.x > widgetPosition.x)
        {
            onPassFocusToChild = AX_CALLBACK_2(Layout::findNearestChildWidgetIndex, this);
        }
        else
        {
            onPassFocusToChild = AX_CALLBACK_2(Layout::findFarthestChildWidgetIndex, this);
        }
    }
    else if (dir == FocusDirection::RIGHT)
    {
        if (previousWidgetPosition.x > widgetPosition.x)
        {
            onPassFocusToChild = AX_CALLBACK_2(Layout::findFarthestChildWidgetIndex, this);
        }
        else
        {
            onPassFocusToChild = AX_CALLBACK_2(Layout::findNearestChildWidgetIndex, this);
        }
    }
    else if (dir == FocusDirection::DOWN)
    {
        if (previousWidgetPosition.y > widgetPosition.y)
        {
            onPassFocusToChild = AX_CALLBACK_2(Layout::findNearestChildWidgetIndex, this);
        }
        else
        {
            onPassFocusToChild = AX_CALLBACK_2(Layout::findFarthestChildWidgetIndex, this);
        }
    }
    else if (dir == FocusDirection::UP)
    {
        if (previousWidgetPosition.y < widgetPosition.y)
        {
            onPassFocusToChild = AX_CALLBACK_2(Layout::findNearestChildWidgetIndex, this);
        }
        else
        {
            onPassFocusToChild = AX_CALLBACK_2(Layout::findFarthestChildWidgetIndex, this);
        }
    }
    else
    {
        AXASSERT(0, "invalid direction!");
    }
}

Widget* Layout::passFocusToChild(FocusDirection dir, ax::ui::Widget* current)
{
    if (checkFocusEnabledChild())
    {
        Widget* previousWidget = this->getCurrentFocusedWidget();

        this->findProperSearchingFunctor(dir, previousWidget);

        int index = onPassFocusToChild(dir, previousWidget);

        Widget* widget = this->getChildWidgetByIndex(index);
        Layout* layout = dynamic_cast<Layout*>(widget);
        if (layout)
        {
            layout->_isFocusPassing = true;
            return layout->findNextFocusedWidget(dir, layout);
        }
        else
        {
            this->dispatchFocusEvent(current, widget);
            return widget;
        }
    }
    else
    {
        return this;
    }
}

bool Layout::checkFocusEnabledChild() const
{
    bool ret = false;
    for (Node* node : _children)
    {
        Widget* widget = dynamic_cast<Widget*>(node);
        if (widget && widget->isFocusEnabled())
        {
            ret = true;
            break;
        }
    }
    return ret;
}

Widget* Layout::getChildWidgetByIndex(ssize_t index) const
{
    ssize_t size     = _children.size();
    int count        = 0;
    ssize_t oldIndex = index;
    Widget* widget   = nullptr;
    while (index < size)
    {
        Widget* firstChild = dynamic_cast<Widget*>(_children.at(index));
        if (firstChild)
        {
            widget = firstChild;
            break;
        }
        count++;
        index++;
    }

    if (nullptr == widget)
    {
        int begin = 0;
        while (begin < oldIndex)
        {
            Widget* firstChild = dynamic_cast<Widget*>(_children.at(begin));
            if (firstChild)
            {
                widget = firstChild;
                break;
            }
            count++;
            begin++;
        }
    }

    return widget;
}

Widget* Layout::getPreviousFocusedWidget(FocusDirection direction, Widget* current)
{
    Widget* nextWidget        = nullptr;
    ssize_t previousWidgetPos = _children.getIndex(current);
    previousWidgetPos         = previousWidgetPos - 1;
    if (previousWidgetPos >= 0)
    {
        nextWidget = this->getChildWidgetByIndex(previousWidgetPos);
        if (nextWidget->isFocusEnabled())
        {
            Layout* layout = dynamic_cast<Layout*>(nextWidget);
            if (layout)
            {
                layout->_isFocusPassing = true;
                return layout->findNextFocusedWidget(direction, layout);
            }
            this->dispatchFocusEvent(current, nextWidget);
            return nextWidget;
        }
        else
        {
            // handling the disabled widget, there is no actual focus lose or get, so we don't need any event
            return this->getPreviousFocusedWidget(direction, nextWidget);
        }
    }
    else
    {
        if (_loopFocus)
        {
            if (checkFocusEnabledChild())
            {
                previousWidgetPos = _children.size() - 1;
                nextWidget        = this->getChildWidgetByIndex(previousWidgetPos);
                if (nextWidget->isFocusEnabled())
                {
                    Layout* layout = dynamic_cast<Layout*>(nextWidget);
                    if (layout)
                    {
                        layout->_isFocusPassing = true;
                        return layout->findNextFocusedWidget(direction, layout);
                    }
                    else
                    {
                        this->dispatchFocusEvent(current, nextWidget);
                        return nextWidget;
                    }
                }
                else
                {
                    return this->getPreviousFocusedWidget(direction, nextWidget);
                }
            }
            else
            {
                if (dynamic_cast<Layout*>(current))
                {
                    return current;
                }
                else
                {
                    return _focusedWidget;
                }
            }
        }
        else
        {
            if (isLastWidgetInContainer(current, direction))
            {
                if (isWidgetAncestorSupportLoopFocus(this, direction))
                {
                    return Widget::findNextFocusedWidget(direction, this);
                }
                if (dynamic_cast<Layout*>(current))
                {
                    return current;
                }
                else
                {
                    return _focusedWidget;
                }
            }
            else
            {
                return Widget::findNextFocusedWidget(direction, this);
            }
        }
    }
}

Widget* Layout::getNextFocusedWidget(FocusDirection direction, Widget* current)
{
    Widget* nextWidget        = nullptr;
    ssize_t previousWidgetPos = _children.getIndex(current);
    previousWidgetPos         = previousWidgetPos + 1;
    if (previousWidgetPos < _children.size())
    {
        nextWidget = this->getChildWidgetByIndex(previousWidgetPos);
        // handle widget
        if (nextWidget)
        {
            if (nextWidget->isFocusEnabled())
            {
                Layout* layout = dynamic_cast<Layout*>(nextWidget);
                if (layout)
                {
                    layout->_isFocusPassing = true;
                    return layout->findNextFocusedWidget(direction, layout);
                }
                else
                {
                    this->dispatchFocusEvent(current, nextWidget);
                    return nextWidget;
                }
            }
            else
            {
                return this->getNextFocusedWidget(direction, nextWidget);
            }
        }
        else
        {
            return current;
        }
    }
    else
    {
        if (_loopFocus)
        {
            if (checkFocusEnabledChild())
            {
                previousWidgetPos = 0;
                nextWidget        = this->getChildWidgetByIndex(previousWidgetPos);
                if (nextWidget->isFocusEnabled())
                {
                    Layout* layout = dynamic_cast<Layout*>(nextWidget);
                    if (layout)
                    {
                        layout->_isFocusPassing = true;
                        return layout->findNextFocusedWidget(direction, layout);
                    }
                    else
                    {
                        this->dispatchFocusEvent(current, nextWidget);
                        return nextWidget;
                    }
                }
                else
                {
                    return this->getNextFocusedWidget(direction, nextWidget);
                }
            }
            else
            {
                if (dynamic_cast<Layout*>(current))
                {
                    return current;
                }
                else
                {
                    return _focusedWidget;
                }
            }
        }
        else
        {
            if (isLastWidgetInContainer(current, direction))
            {
                if (isWidgetAncestorSupportLoopFocus(this, direction))
                {
                    return Widget::findNextFocusedWidget(direction, this);
                }
                if (dynamic_cast<Layout*>(current))
                {
                    return current;
                }
                else
                {
                    return _focusedWidget;
                }
            }
            else
            {
                return Widget::findNextFocusedWidget(direction, this);
            }
        }
    }
}

bool Layout::isLastWidgetInContainer(Widget* widget, FocusDirection direction) const
{
    Layout* parent = dynamic_cast<Layout*>(widget->getParent());
    if (parent == nullptr)
    {
        return true;
    }

    auto& container             = parent->getChildren();
    ssize_t index               = container.getIndex(widget);
    const auto parentLayoutType = parent->getLayoutType();
    if (parentLayoutType == Type::HORIZONTAL)
    {
        if (direction == FocusDirection::LEFT)
        {
            if (index == 0)
            {
                return isLastWidgetInContainer(parent, direction);
            }
            else
            {
                return false;
            }
        }
        if (direction == FocusDirection::RIGHT)
        {
            if (index == container.size() - 1)
            {
                return isLastWidgetInContainer(parent, direction);
            }
            else
            {
                return false;
            }
        }
        if (direction == FocusDirection::DOWN)
        {
            return isLastWidgetInContainer(parent, direction);
        }

        if (direction == FocusDirection::UP)
        {
            return isLastWidgetInContainer(parent, direction);
        }
    }
    else if (parentLayoutType == Type::VERTICAL || parentLayoutType == Type::CENTER_VERTICAL)
    {
        if (direction == FocusDirection::UP)
        {
            if (index == 0)
            {
                return isLastWidgetInContainer(parent, direction);
            }
            else
            {
                return false;
            }
        }
        if (direction == FocusDirection::DOWN)
        {
            if (index == container.size() - 1)
            {
                return isLastWidgetInContainer(parent, direction);
            }
            else
            {
                return false;
            }
        }
        if (direction == FocusDirection::LEFT)
        {
            return isLastWidgetInContainer(parent, direction);
        }

        if (direction == FocusDirection::RIGHT)
        {
            return isLastWidgetInContainer(parent, direction);
        }
    }
    else
    {
        AXASSERT(0, "invalid layout Type");
        return false;
    }

    return false;
}

bool Layout::isWidgetAncestorSupportLoopFocus(Widget* widget, FocusDirection direction) const
{
    Layout* parent = dynamic_cast<Layout*>(widget->getParent());
    if (parent == nullptr)
    {
        return false;
    }
    if (parent->isLoopFocus())
    {
        const auto layoutType = parent->getLayoutType();
        if (layoutType == Type::HORIZONTAL || _layoutType == Type::CENTER_HORIZONTAL)
        {
            if (direction == FocusDirection::LEFT || direction == FocusDirection::RIGHT)
            {
                return true;
            }
            else
            {
                return isWidgetAncestorSupportLoopFocus(parent, direction);
            }
        }
        if (layoutType == Type::VERTICAL || layoutType == Type::CENTER_VERTICAL)
        {
            if (direction == FocusDirection::DOWN || direction == FocusDirection::UP)
            {
                return true;
            }
            else
            {
                return isWidgetAncestorSupportLoopFocus(parent, direction);
            }
        }
        else
        {
            AXASSERT(0, "invalid layout type");
            return false;
        }
    }
    else
    {
        return isWidgetAncestorSupportLoopFocus(parent, direction);
    }
}

Widget* Layout::findNextFocusedWidget(FocusDirection direction, Widget* current)
{
    if (_isFocusPassing || this->isFocused())
    {
        Layout* parent  = dynamic_cast<Layout*>(this->getParent());
        _isFocusPassing = false;

        if (_passFocusToChild)
        {
            Widget* w = this->passFocusToChild(direction, current);
            if (dynamic_cast<Layout*>(w))
            {
                if (parent)
                {
                    parent->_isFocusPassing = true;
                    return parent->findNextFocusedWidget(direction, this);
                }
            }
            return w;
        }

        if (nullptr == parent)
        {
            return this;
        }
        parent->_isFocusPassing = true;
        return parent->findNextFocusedWidget(direction, this);
    }
    else if (current->isFocused() || dynamic_cast<Layout*>(current))
    {
        if (_layoutType == Type::HORIZONTAL || _layoutType == Type::CENTER_HORIZONTAL)
        {
            switch (direction)
            {
            case FocusDirection::LEFT:
            {
                return this->getPreviousFocusedWidget(direction, current);
            }
            break;
            case FocusDirection::RIGHT:
            {
                return this->getNextFocusedWidget(direction, current);
            }
            break;
            case FocusDirection::DOWN:
            case FocusDirection::UP:
            {
                if (isLastWidgetInContainer(this, direction))
                {
                    if (isWidgetAncestorSupportLoopFocus(current, direction))
                    {
                        return Widget::findNextFocusedWidget(direction, this);
                    }
                    return current;
                }
                else
                {
                    return Widget::findNextFocusedWidget(direction, this);
                }
            }
            break;
            default:
            {
                AXASSERT(0, "Invalid Focus Direction");
                return current;
            }
            break;
            }
        }
        else if (_layoutType == Type::VERTICAL || _layoutType == Type::CENTER_VERTICAL)
        {
            switch (direction)
            {
            case FocusDirection::LEFT:
            case FocusDirection::RIGHT:
            {
                if (isLastWidgetInContainer(this, direction))
                {
                    if (isWidgetAncestorSupportLoopFocus(current, direction))
                    {
                        return Widget::findNextFocusedWidget(direction, this);
                    }
                    return current;
                }
                else
                {
                    return Widget::findNextFocusedWidget(direction, this);
                }
            }
            break;
            case FocusDirection::DOWN:
            {
                return getNextFocusedWidget(direction, current);
            }
            break;
            case FocusDirection::UP:
            {
                return getPreviousFocusedWidget(direction, current);
            }
            break;
            default:
            {
                AXASSERT(0, "Invalid Focus Direction");
                return current;
            }
            break;
            }
        }
        else
        {
            AXASSERT(0, "Un Supported Layout type, please use VBox and HBox instead!!!");
            return current;
        }
    }
    else
    {
        return current;
    }
}

void Layout::setCameraMask(unsigned short mask, bool applyChildren)
{
    Widget::setCameraMask(mask, applyChildren);
    if (_clippingStencil)
    {
        _clippingStencil->setCameraMask(mask, applyChildren);
    }
}

ResourceData Layout::getRenderFile()
{
    ResourceData rData;
    rData.type = (int)_bgImageTexType;
    rData.file = _backGroundImageFileName;
    return rData;
}

}  // namespace ui
}
