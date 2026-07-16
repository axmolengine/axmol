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

#include "axmol/ui/LayoutGroup.h"
#include "axmol/ui/UIHelper.h"
#include "axmol/ui/Scale9Sprite.h"
#include "axmol/renderer/RenderState.h"
#include "axmol/base/Director.h"
#include "axmol/renderer/Renderer.h"
#include "axmol/ui/LayoutManager.h"
#include "axmol/2d/DrawNode.h"
#include "axmol/2d/Layer.h"
#include "axmol/2d/Sprite.h"
#include "axmol/base/FocusEvent.h"
#include "axmol/base/StencilStateManager.h"
#include <algorithm>

namespace ax
{

namespace ui
{

static const int BACKGROUNDIMAGE_Z         = (-1);
static const int BCAKGROUNDCOLORRENDERER_Z = (-2);

IMPLEMENT_CLASS_GUI_INFO(LayoutGroup)

LayoutGroup::LayoutGroup()
    : _backGroundScale9Enabled(false)
    , _backGroundImage(nullptr)
    , _backGroundImageFileName("")
    , _backGroundImageCapInsets(Rect::zero)
    , _colorType(BackGroundColorType::NONE)
    , _bgImageTexType(TextureResType::LOCAL)
    , _backGroundImageTextureSize(Vec2::zero)
    , _backGroundImageColor(Color32::white)
    , _colorRender(nullptr)
    , _gradientRender(nullptr)
    , _cColor(Color32::white)
    , _gStartColor(Color32::white)
    , _gEndColor(Color32::white)
    , _alongVector(Vec2(0.0f, -1.0f))
    , _clippingEnabled(false)
    , _layoutType(Type::ABSOLUTE)
    , _clippingType(ClippingType::STENCIL)
    , _clippingStencil(nullptr)
    , _clippingRect(Rect::zero)
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

LayoutGroup::~LayoutGroup()
{
    AX_SAFE_RELEASE(_clippingStencil);
    AX_SAFE_DELETE(_stencilStateManager);
}

void LayoutGroup::onEnter()
{
    Widget::onEnter();
    if (_clippingStencil)
    {
        _clippingStencil->onEnter();
    }
    _doLayoutDirty     = true;
    _clippingRectDirty = true;
}

void LayoutGroup::onExit()
{
    Widget::onExit();
    if (_clippingStencil)
    {
        _clippingStencil->onExit();
    }
}

void LayoutGroup::setGlobalZOrder(float globalZOrder)
{
    // _protectedChildren's global z order is set in ProtectedNode::setGlobalZOrder()

    Widget::setGlobalZOrder(globalZOrder);
    if (_clippingStencil)
        _clippingStencil->setGlobalZOrder(globalZOrder);

    for (auto&& child : _children)
        child->setGlobalZOrder(globalZOrder);
}

LayoutGroup* LayoutGroup::create()
{
    LayoutGroup* layout = new LayoutGroup();
    if (layout->init())
    {
        layout->autorelease();
        return layout;
    }
    AX_SAFE_DELETE(layout);
    return nullptr;
}

bool LayoutGroup::init()
{
    if (Widget::init())
    {
        setAutoSize(false);
        setContentSize(Vec2::zero);
        setAnchorPoint(Vec2::zero);
        onPassFocusToChild = AX_CALLBACK_2(LayoutGroup::findNearestChildWidgetIndex, this);
        return true;
    }
    return false;
}

void LayoutGroup::addChild(Node* child)
{
    LayoutGroup::addChild(child, child->getLocalZOrder(), child->getTag());
}

void LayoutGroup::addChild(Node* child, int localZOrder)
{
    LayoutGroup::addChild(child, localZOrder, child->getTag());
}

void LayoutGroup::addChild(Node* child, int zOrder, int tag)
{
    if (dynamic_cast<Widget*>(child))
    {
        supplyTheLayoutParameterLackToChild(static_cast<Widget*>(child));
    }
    child->setGlobalZOrder(_globalZOrder);
    Widget::addChild(child, zOrder, tag);
    _doLayoutDirty = true;
}

void LayoutGroup::addChild(Node* child, int zOrder, std::string_view name)
{
    if (dynamic_cast<Widget*>(child))
    {
        supplyTheLayoutParameterLackToChild(static_cast<Widget*>(child));
    }
    child->setGlobalZOrder(_globalZOrder);
    Widget::addChild(child, zOrder, name);
    _doLayoutDirty = true;
}

void LayoutGroup::removeChild(Node* child, bool cleanup)
{
    Widget::removeChild(child, cleanup);
    _doLayoutDirty = true;
}

void LayoutGroup::removeAllChildren()
{
    Widget::removeAllChildren();
    _doLayoutDirty = true;
}

void LayoutGroup::removeAllChildrenWithCleanup(bool cleanup)
{
    Widget::removeAllChildrenWithCleanup(cleanup);
    _doLayoutDirty = true;
}

bool LayoutGroup::isClippingEnabled() const
{
    return _clippingEnabled;
}

void LayoutGroup::visit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (!_visible)
    {
        return;
    }

    if (FLAGS_TRANSFORM_DIRTY & parentFlags || _transformUpdated || _contentSizeDirty)
        _clippingRectDirty = true;

    updateLayout();
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

void LayoutGroup::stencilClippingVisit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (!_visible)
        return;

    uint32_t flags = processParentFlags(parentTransform, parentFlags);

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
}

void LayoutGroup::onBeforeVisitScissor()
{
    auto renderView = _director->getRenderView();
    // apply scissor test
    _scissorOldState = renderView->isScissorEnabled();
    if (false == _scissorOldState)
    {
        auto renderer = _director->getRenderer();
        renderer->setScissorTest(true);
    }

    // apply scissor box
    Rect clippingRect = getClippingRect();
    _clippingOldRect  = renderView->getScissorInPoints();
    if (false == _clippingOldRect.equals(clippingRect))
    {
        renderView->setScissorInPoints(clippingRect.origin.x, clippingRect.origin.y, clippingRect.size.width,
                                       clippingRect.size.height);
    }
}

void LayoutGroup::onAfterVisitScissor()
{
    if (_scissorOldState)
    {
        // revert scissor box
        if (false == _clippingOldRect.equals(_clippingRect))
        {
            auto renderView = _director->getRenderView();
            renderView->setScissorInPoints(_clippingOldRect.origin.x, _clippingOldRect.origin.y,
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

void LayoutGroup::scissorClippingVisit(Renderer* renderer, const Mat4& parentTransform, uint32_t parentFlags)
{
    if (parentFlags & FLAGS_DIRTY_MASK)
    {
        _clippingRectDirty = true;
    }

    auto* groupCommand = renderer->getNextGroupCommand();
    groupCommand->init(_globalZOrder);
    renderer->addCommand(groupCommand);
    renderer->pushGroup(groupCommand->getRenderQueueID());

    auto beforeVisitCmdScissor = renderer->nextCallbackCommand();
    beforeVisitCmdScissor->init(_globalZOrder);
    beforeVisitCmdScissor->func = AX_CALLBACK_0(LayoutGroup::onBeforeVisitScissor, this);
    renderer->addCommand(beforeVisitCmdScissor);

    ProtectedNode::visit(renderer, parentTransform, parentFlags);

    auto afterVisitCmdScissor = renderer->nextCallbackCommand();
    afterVisitCmdScissor->init(_globalZOrder);
    afterVisitCmdScissor->func = AX_CALLBACK_0(LayoutGroup::onAfterVisitScissor, this);
    renderer->addCommand(afterVisitCmdScissor);

    renderer->popGroup();
}

void LayoutGroup::setClippingEnabled(bool able)
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

void LayoutGroup::setClippingType(ClippingType type)
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

LayoutGroup::ClippingType LayoutGroup::getClippingType() const
{
    return _clippingType;
}

void LayoutGroup::setStencilClippingSize(const Vec2& /*size*/)
{
    if (_clippingEnabled && _clippingType == ClippingType::STENCIL)
    {
        _clippingStencil->clear();
        _clippingStencil->drawSolidRect(Vec2::zero, _contentSize, Color::green);  // Fix issue #1546
    }
}

const Rect& LayoutGroup::getClippingRect()
{
    if (_clippingRectDirty)
    {
        const auto worldPos1 = convertToWorldSpace(Vec2::zero);
        const auto worldPos2 = convertToWorldSpace(Vec2(_contentSize.width, _contentSize.height));

        // Node can be flipped
        const auto worldPos      = Vec2(std::min(worldPos1.x, worldPos2.x), std::min(worldPos1.y, worldPos2.y));
        const auto scissorWidth  = std::fabs(worldPos2.x - worldPos1.x);
        const auto scissorHeight = std::fabs(worldPos2.y - worldPos1.y);

        LayoutGroup* parent = this;

        while (parent)
        {
            parent = dynamic_cast<LayoutGroup*>(parent->getParent());
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

void LayoutGroup::onSizeChanged()
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
            _backGroundImage->setContentSize(_contentSize);
        }
        else
        {
            _backGroundImage->setContentSize(_backGroundImageTextureSize);
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

void LayoutGroup::setBackGroundImageScale9Enabled(bool able)
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
        _backGroundImage->setContentSize(_contentSize);
    }
    else
    {
        _backGroundImage->setRenderingType(Scale9Sprite::RenderingType::SIMPLE);
        _backGroundImage->setContentSize(_backGroundImageTextureSize);
    }

    setBackGroundImageCapInsets(_backGroundImageCapInsets);
}

bool LayoutGroup::isBackGroundImageScale9Enabled() const
{
    return _backGroundScale9Enabled;
}

void LayoutGroup::setBackGroundImage(std::string_view fileName, TextureResType texType)
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
        _backGroundImage->setContentSize(_contentSize);
    }
    else
    {
        _backGroundImage->setContentSize(_backGroundImageTextureSize);
    }
    updateBackGroundImageRGBA();
}

void LayoutGroup::setBackGroundImageCapInsets(const Rect& capInsets)
{
    _backGroundImageCapInsets = capInsets;
    if (_backGroundScale9Enabled && _backGroundImage)
    {
        _backGroundImage->setCapInsets(capInsets);
    }
}

const Rect& LayoutGroup::getBackGroundImageCapInsets() const
{
    return _backGroundImageCapInsets;
}

void LayoutGroup::supplyTheLayoutParameterLackToChild(Widget* child)
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

void LayoutGroup::addBackGroundImage()
{
    _backGroundImage = Scale9Sprite::create();
    _backGroundImage->setRenderingType(Scale9Sprite::RenderingType::SIMPLE);

    addProtectedChild(_backGroundImage, BACKGROUNDIMAGE_Z, -1);

    _backGroundImage->setPosition(_contentSize.width / 2.0f, _contentSize.height / 2.0f);
}

void LayoutGroup::removeBackGroundImage()
{
    if (!_backGroundImage)
    {
        return;
    }
    removeProtectedChild(_backGroundImage);
    _backGroundImage            = nullptr;
    _backGroundImageFileName    = "";
    _backGroundImageTextureSize = Vec2::zero;
}

void LayoutGroup::setBackGroundColorType(BackGroundColorType type)
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
        _colorRender->setColor(_cColor);
        addProtectedChild(_colorRender, BCAKGROUNDCOLORRENDERER_Z, -1);
        break;
    case BackGroundColorType::GRADIENT:
        _gradientRender = LayerGradient::create();
        _gradientRender->setContentSize(_contentSize);
        _gradientRender->setOpacity(_cColor.a);
        _gradientRender->setStartColor(_gStartColor);
        _gradientRender->setEndColor(_gEndColor);
        _gradientRender->setVector(_alongVector);
        addProtectedChild(_gradientRender, BCAKGROUNDCOLORRENDERER_Z, -1);
        break;
    default:
        break;
    }
}

LayoutGroup::BackGroundColorType LayoutGroup::getBackGroundColorType() const
{
    return _colorType;
}

void LayoutGroup::setBackGroundColor(const Color32& color)
{
    _cColor = color;
    if (_colorRender)
    {
        _colorRender->setColor(color);
    }
}

const Color32& LayoutGroup::getBackGroundColor() const
{
    return _cColor;
}

void LayoutGroup::setBackGroundColor(const Color32& startColor, const Color32& endColor)
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

const Color32& LayoutGroup::getBackGroundStartColor() const
{
    return _gStartColor;
}

const Color32& LayoutGroup::getBackGroundEndColor() const
{
    return _gEndColor;
}

void LayoutGroup::setBackGroundColorOpacity(uint8_t opacity)
{
    _cColor.a = opacity;
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

uint8_t LayoutGroup::getBackGroundColorOpacity() const
{
    return _cColor.a;
}

void LayoutGroup::setBackGroundColorVector(const Vec2& vector)
{
    _alongVector = vector;
    if (_gradientRender)
    {
        _gradientRender->setVector(vector);
    }
}

const Vec2& LayoutGroup::getBackGroundColorVector() const
{
    return _alongVector;
}

void LayoutGroup::setBackGroundImageColor(const Color32& color)
{
    _backGroundImageColor = color;
    updateBackGroundImageColor();
}

void LayoutGroup::setBackGroundImageOpacity(uint8_t opacity)
{
    _backGroundImageColor.a = opacity;
    updateBackGroundImageOpacity();
}

const Color32& LayoutGroup::getBackGroundImageColor() const
{
    return _backGroundImageColor;
}

uint8_t LayoutGroup::getBackGroundImageOpacity() const
{
    return _backGroundImageColor.a;
}

void LayoutGroup::updateBackGroundImageColor()
{
    if (_backGroundImage)
    {
        _backGroundImage->setColor(_backGroundImageColor);
    }
}

void LayoutGroup::updateBackGroundImageOpacity()
{
    if (_backGroundImage)
    {
        _backGroundImage->setOpacity(_backGroundImageColor.a);
    }
}

void LayoutGroup::updateBackGroundImageRGBA()
{
    if (_backGroundImage)
    {
        _backGroundImage->setColor(_backGroundImageColor);
    }
}

const Vec2& LayoutGroup::getBackGroundImageTextureSize() const
{
    return _backGroundImageTextureSize;
}

void LayoutGroup::setLayoutType(Type type)
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

LayoutGroup::Type LayoutGroup::getLayoutType() const
{
    return _layoutType;
}

void LayoutGroup::forceDoLayout()
{
    this->requestDoLayout();
    this->doLayout();
}

void LayoutGroup::requestDoLayout()
{
    _doLayoutDirty = true;
}

Vec2 LayoutGroup::getLayoutContentSize() const
{
    return this->getContentSize();
}

const Vector<Node*>& LayoutGroup::getLayoutElements() const
{
    return this->getChildren();
}

LayoutManager* LayoutGroup::createLayoutManager()
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

void LayoutGroup::doLayout()
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

std::string LayoutGroup::getDescription() const
{
    return "LayoutGroup";
}

Widget* LayoutGroup::createCloneInstance()
{
    return LayoutGroup::create();
}

void LayoutGroup::copyClonedWidgetChildren(Widget* model)
{
    Widget::copyClonedWidgetChildren(model);
}

void LayoutGroup::copySpecialProperties(Widget* widget)
{
    LayoutGroup* layout = dynamic_cast<LayoutGroup*>(widget);
    if (layout)
    {
        setBackGroundImageScale9Enabled(layout->_backGroundScale9Enabled);
        setBackGroundImage(layout->_backGroundImageFileName, layout->_bgImageTexType);
        setBackGroundImageCapInsets(layout->_backGroundImageCapInsets);
        setBackGroundColorType(layout->_colorType);
        setBackGroundColor(layout->_cColor);
        setBackGroundColor(layout->_gStartColor, layout->_gEndColor);
        setBackGroundColorVector(layout->_alongVector);
        setLayoutType(layout->_layoutType);
        setClippingEnabled(layout->_clippingEnabled);
        setClippingType(layout->_clippingType);
        _loopFocus        = layout->_loopFocus;
        _passFocusToChild = layout->_passFocusToChild;
        _isInterceptTouch = layout->_isInterceptTouch;
    }
}

void LayoutGroup::setLoopFocus(bool loop)
{
    _loopFocus = loop;
}

bool LayoutGroup::isLoopFocus() const
{
    return _loopFocus;
}

void LayoutGroup::setPassFocusToChild(bool pass)
{
    _passFocusToChild = pass;
}

bool LayoutGroup::isPassFocusToChild() const
{
    return _passFocusToChild;
}

Vec2 LayoutGroup::getLayoutAccumulatedSize() const
{
    const auto& children = this->getChildren();
    Vec2 layoutSize      = Vec2::zero;
    int widgetCount      = 0;
    for (const auto& widget : children)
    {
        LayoutGroup* layout = dynamic_cast<LayoutGroup*>(widget);
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

Vec2 LayoutGroup::getWorldCenterPoint(Widget* widget) const
{
    LayoutGroup* layout = dynamic_cast<LayoutGroup*>(widget);
    // FIXEDME: we don't need to calculate the content size of layout anymore
    Vec2 widgetSize = layout ? layout->getLayoutAccumulatedSize() : widget->getContentSize();
    //    AXLOGD("content size : width = {}, height = {}", widgetSize.width, widgetSize.height);
    return widget->convertToWorldSpace(Vec2(widgetSize.width / 2, widgetSize.height / 2));
}

float LayoutGroup::calculateNearestDistance(Widget* baseWidget)
{
    float distance = FLT_MAX;

    Vec2 widgetPosition = this->getWorldCenterPoint(baseWidget);

    for (Node* node : _children)
    {
        LayoutGroup* layout = dynamic_cast<LayoutGroup*>(node);
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

float LayoutGroup::calculateFarthestDistance(ax::ui::Widget* baseWidget)
{
    float distance = -FLT_MAX;

    Vec2 widgetPosition = this->getWorldCenterPoint(baseWidget);

    for (Node* node : _children)
    {
        LayoutGroup* layout = dynamic_cast<LayoutGroup*>(node);
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

int LayoutGroup::findFirstFocusEnabledWidgetIndex()
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

int LayoutGroup::findNearestChildWidgetIndex(FocusDirection direction, Widget* baseWidget)
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
                LayoutGroup* layout = dynamic_cast<LayoutGroup*>(w);
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

int LayoutGroup::findFarthestChildWidgetIndex(FocusDirection direction, ax::ui::Widget* baseWidget)
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
                LayoutGroup* layout = dynamic_cast<LayoutGroup*>(w);
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

Widget* LayoutGroup::findFocusEnabledChildWidgetByIndex(ssize_t index)
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

Widget* LayoutGroup::findFirstNonLayoutWidget()
{
    Widget* widget = nullptr;
    for (Node* node : _children)
    {
        LayoutGroup* layout = dynamic_cast<LayoutGroup*>(node);
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

void LayoutGroup::findProperSearchingFunctor(FocusDirection dir, Widget* baseWidget)
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
            onPassFocusToChild = AX_CALLBACK_2(LayoutGroup::findNearestChildWidgetIndex, this);
        }
        else
        {
            onPassFocusToChild = AX_CALLBACK_2(LayoutGroup::findFarthestChildWidgetIndex, this);
        }
    }
    else if (dir == FocusDirection::RIGHT)
    {
        if (previousWidgetPosition.x > widgetPosition.x)
        {
            onPassFocusToChild = AX_CALLBACK_2(LayoutGroup::findFarthestChildWidgetIndex, this);
        }
        else
        {
            onPassFocusToChild = AX_CALLBACK_2(LayoutGroup::findNearestChildWidgetIndex, this);
        }
    }
    else if (dir == FocusDirection::DOWN)
    {
        if (previousWidgetPosition.y > widgetPosition.y)
        {
            onPassFocusToChild = AX_CALLBACK_2(LayoutGroup::findNearestChildWidgetIndex, this);
        }
        else
        {
            onPassFocusToChild = AX_CALLBACK_2(LayoutGroup::findFarthestChildWidgetIndex, this);
        }
    }
    else if (dir == FocusDirection::UP)
    {
        if (previousWidgetPosition.y < widgetPosition.y)
        {
            onPassFocusToChild = AX_CALLBACK_2(LayoutGroup::findNearestChildWidgetIndex, this);
        }
        else
        {
            onPassFocusToChild = AX_CALLBACK_2(LayoutGroup::findFarthestChildWidgetIndex, this);
        }
    }
    else
    {
        AXASSERT(0, "invalid direction!");
    }
}

Widget* LayoutGroup::passFocusToChild(FocusDirection dir, ax::ui::Widget* current)
{
    if (checkFocusEnabledChild())
    {
        Widget* previousWidget = this->getCurrentFocusedWidget();

        this->findProperSearchingFunctor(dir, previousWidget);

        int index = onPassFocusToChild(dir, previousWidget);

        Widget* widget      = this->getChildWidgetByIndex(index);
        LayoutGroup* layout = dynamic_cast<LayoutGroup*>(widget);
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

bool LayoutGroup::checkFocusEnabledChild() const
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

Widget* LayoutGroup::getChildWidgetByIndex(ssize_t index) const
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

Widget* LayoutGroup::getPreviousFocusedWidget(FocusDirection direction, Widget* current)
{
    Widget* nextWidget        = nullptr;
    ssize_t previousWidgetPos = _children.getIndex(current);
    previousWidgetPos         = previousWidgetPos - 1;
    if (previousWidgetPos >= 0)
    {
        nextWidget = this->getChildWidgetByIndex(previousWidgetPos);
        if (nextWidget->isFocusEnabled())
        {
            LayoutGroup* layout = dynamic_cast<LayoutGroup*>(nextWidget);
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
                    LayoutGroup* layout = dynamic_cast<LayoutGroup*>(nextWidget);
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
                if (dynamic_cast<LayoutGroup*>(current))
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
                if (dynamic_cast<LayoutGroup*>(current))
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

Widget* LayoutGroup::getNextFocusedWidget(FocusDirection direction, Widget* current)
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
                LayoutGroup* layout = dynamic_cast<LayoutGroup*>(nextWidget);
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
                    LayoutGroup* layout = dynamic_cast<LayoutGroup*>(nextWidget);
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
                if (dynamic_cast<LayoutGroup*>(current))
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
                if (dynamic_cast<LayoutGroup*>(current))
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

bool LayoutGroup::isLastWidgetInContainer(Widget* widget, FocusDirection direction) const
{
    LayoutGroup* parent = dynamic_cast<LayoutGroup*>(widget->getParent());
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

bool LayoutGroup::isWidgetAncestorSupportLoopFocus(Widget* widget, FocusDirection direction) const
{
    LayoutGroup* parent = dynamic_cast<LayoutGroup*>(widget->getParent());
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

Widget* LayoutGroup::findNextFocusedWidget(FocusDirection direction, Widget* current)
{
    if (_isFocusPassing || this->isFocused())
    {
        LayoutGroup* parent = dynamic_cast<LayoutGroup*>(this->getParent());
        _isFocusPassing     = false;

        if (_passFocusToChild)
        {
            Widget* w = this->passFocusToChild(direction, current);
            if (dynamic_cast<LayoutGroup*>(w))
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
    else if (current->isFocused() || dynamic_cast<LayoutGroup*>(current))
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
            AXASSERT(0, "Un Supported LayoutGroup type, please use VBox and HBox instead!!!");
            return current;
        }
    }
    else
    {
        return current;
    }
}

void LayoutGroup::setCameraMask(unsigned short mask, bool applyChildren)
{
    Widget::setCameraMask(mask, applyChildren);
    if (_clippingStencil)
    {
        _clippingStencil->setCameraMask(mask, applyChildren);
    }
}

ResourceData LayoutGroup::getRenderFile()
{
    ResourceData rData;
    rData.type = (int)_bgImageTexType;
    rData.file = _backGroundImageFileName;
    return rData;
}

}  // namespace ui
}  // namespace ax
