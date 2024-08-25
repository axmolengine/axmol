#include "GObject.h"
#include "GGroup.h"
#include "GList.h"
#include "GRoot.h"
#include "UIConfig.h"
#include "UIPackage.h"
#include "display/FUISprite.h"
#include "gears/GearDisplay.h"
#include "gears/GearDisplay2.h"
#include "utils/ByteBuffer.h"
#include "utils/WeakPtr.h"

NS_FGUI_BEGIN
using namespace ax;

GObject* GObject::_draggingObject = nullptr;

static Vec2 sGlobalDragStart;
static Rect sGlobalRect;
static bool sUpdateInDragging;

GObject::GObject() : _scale{1, 1},
                     _sizePercentInGroup(0.0f),
                     _pivotAsAnchor(false),
                     _alpha(1.0f),
                     _rotation(0.0f),
                     _visible(true),
                     _internalVisible(true),
                     _handlingController(false),
                     _touchable(true),
                     _grayed(false),
                     _finalGrayed(false),
                     _draggable(false),
                     _dragBounds(nullptr),
                     _dragTesting(false),
                     _sortingOrder(0),
                     _focusable(false),
                     _pixelSnapping(false),
                     _group(nullptr),
                     _parent(nullptr),
                     _displayObject(nullptr),
                     _sizeImplType(0),
                     _underConstruct(false),
                     _gearLocked(false),
                     _packageItem(nullptr),
                     _data(nullptr),
                     _touchDisabled(false),
                     _alignToBL(false),
                     _weakPtrRef(0)
{
    static uint64_t _gInstanceCounter = 1;
    _uid = _gInstanceCounter++;
    std::stringstream ss;
    ss << _uid;
    id = ss.str();
    _relations = new Relations(this);

    for (int i = 0; i < 10; i++)
        _gears[i] = nullptr;
}

GObject::~GObject()
{
    removeFromParent();

    if (_displayObject)
    {
        _displayObject->removeFromParent();
        AX_SAFE_RELEASE(_displayObject);
    }
    for (int i = 0; i < 10; i++)
        AX_SAFE_DELETE(_gears[i]);
    AX_SAFE_DELETE(_relations);
    AX_SAFE_DELETE(_dragBounds);

    if (_weakPtrRef > 0)
        WeakPtr::markDisposed(this);
}

bool GObject::init()
{
    handleInit();

    if (_displayObject != nullptr)
    {
        _displayObject->setAnchorPoint(Vec2(0, 1));
        _displayObject->setCascadeOpacityEnabled(true);
        _displayObject->setOnEnterCallback(AX_CALLBACK_0(GObject::onEnter, this));
        _displayObject->setOnExitCallback(AX_CALLBACK_0(GObject::onExit, this));
    }
    return true;
}

void GObject::setX(float value)
{
    setPosition(value, _position.y);
}

void GObject::setY(float value)
{
    setPosition(_position.x, value);
}

void GObject::setPosition(float xv, float yv)
{
    if (_position.x != xv || _position.y != yv)
    {
        float dx = xv - _position.x;
        float dy = yv - _position.y;
        _position.x = xv;
        _position.y = yv;

        handlePositionChanged();

        GGroup* g = dynamic_cast<GGroup*>(this);
        if (g != nullptr)
            g->moveChildren(dx, dy);

        updateGear(1);

        if (_parent != nullptr && dynamic_cast<GList*>(_parent) == nullptr)
        {
            _parent->setBoundsChangedFlag();
            if (_group != nullptr)
                _group->setBoundsChangedFlag(true);

            dispatchEvent(UIEventType::PositionChange);
        }

        if (_draggingObject == this && !sUpdateInDragging)
            sGlobalRect = localToGlobal(Rect(Vec2::ZERO, _size));
    }
}

float GObject::getXMin() const
{
    return _pivotAsAnchor ? (_position.x - _size.width * _pivot.x) : _position.x;
}

void GObject::setXMin(float value)
{
    if (_pivotAsAnchor)
        setPosition(value + _size.width * _pivot.x, _position.y);
    else
        setPosition(value, _position.y);
}

float GObject::getYMin() const
{
    return _pivotAsAnchor ? (_position.y - _size.height * _pivot.y) : _position.y;
}

void GObject::setYMin(float value)
{
    if (_pivotAsAnchor)
        setPosition(_position.x, value + _size.height * _pivot.y);
    else
        setPosition(_position.x, value);
}

void GObject::setPixelSnapping(bool value)
{
    if (_pixelSnapping != value)
    {
        _pixelSnapping = value;
        handlePositionChanged();
    }
}

void GObject::setSize(float wv, float hv, bool ignorePivot /*= false*/)
{
    if (_rawSize.width != wv || _rawSize.height != hv)
    {
        _rawSize.width = wv;
        _rawSize.height = hv;
        if (wv < minSize.width)
            wv = minSize.width;
        else if (maxSize.width > 0 && wv > maxSize.width)
            wv = maxSize.width;
        if (hv < minSize.height)
            hv = minSize.height;
        else if (maxSize.height > 0 && hv > maxSize.height)
            hv = maxSize.height;
        float dWidth = wv - _size.width;
        float dHeight = hv - _size.height;
        _size.width = wv;
        _size.height = hv;

        handleSizeChanged();

        if (_pivot.x != 0 || _pivot.y != 0)
        {
            if (!_pivotAsAnchor)
            {
                if (!ignorePivot)
                    setPosition(_position.x - _pivot.x * dWidth, _position.y - _pivot.y * dHeight);
                else
                    handlePositionChanged();
            }
            else
                handlePositionChanged();
        }
        else
            handlePositionChanged();

        GGroup* g = dynamic_cast<GGroup*>(this);
        if (g != nullptr)
            g->resizeChildren(dWidth, dHeight);

        updateGear(2);

        if (_parent != nullptr)
        {
            _relations->onOwnerSizeChanged(dWidth, dHeight, _pivotAsAnchor || !ignorePivot);
            _parent->setBoundsChangedFlag();
            if (_group != nullptr)
                _group->setBoundsChangedFlag();
        }

        dispatchEvent(UIEventType::SizeChange);
    }
}

void GObject::setSizeDirectly(float wv, float hv)
{
    _rawSize.width = wv;
    _rawSize.height = hv;
    if (wv < 0)
        wv = 0;
    if (hv < 0)
        hv = 0;
    _size.width = wv;
    _size.height = hv;
}

void GObject::center(bool restraint /*= false*/)
{
    GComponent* r;
    if (_parent != nullptr)
        r = _parent;
    else
        r = UIRoot;

    setPosition((int)((r->_size.width - _size.width) / 2), (int)((r->_size.height - _size.height) / 2));
    if (restraint)
    {
        addRelation(r, RelationType::Center_Center);
        addRelation(r, RelationType::Middle_Middle);
    }
}

void GObject::makeFullScreen()
{
    setSize(UIRoot->getWidth(), UIRoot->getHeight());
}

void GObject::setPivot(float xv, float yv, bool asAnchor)
{
    if (_pivot.x != xv || _pivot.y != yv || _pivotAsAnchor != asAnchor)
    {
        _pivot.set(xv, yv);
        _pivotAsAnchor = asAnchor;
        if (_displayObject != nullptr)
            _displayObject->setAnchorPoint(Vec2(_pivot.x, 1 - _pivot.y));
        handlePositionChanged();
    }
}

void GObject::setScale(float xv, float yv)
{
    if (_scale.x != xv || _scale.y != yv)
    {
        _scale.x = xv;
        _scale.y = yv;
        handleScaleChanged();

        updateGear(2);
    }
}

void GObject::setSkewX(float value)
{
    _displayObject->setRotationSkewX(value);
}

void GObject::setSkewY(float value)
{
    _displayObject->setRotationSkewY(value);
}

void GObject::setRotation(float value)
{
    if (_rotation != value)
    {
        _rotation = value;
        _displayObject->setRotation(_rotation);
        updateGear(3);
    }
}

void GObject::setAlpha(float value)
{
    if (_alpha != value)
    {
        _alpha = value;
        handleAlphaChanged();
        updateGear(3);
    }
}

void GObject::setGrayed(bool value)
{
    if (_grayed != value || _finalGrayed != value)
    {
        _grayed = value;
        handleGrayedChanged();
        updateGear(3);
    }
}

void GObject::setVisible(bool value)
{
    if (_visible != value)
    {
        _visible = value;
        handleVisibleChanged();
        if (_parent != nullptr)
            _parent->setBoundsChangedFlag();
        if (_group != nullptr && _group->isExcludeInvisibles())
            _group->setBoundsChangedFlag();
    }
}

bool GObject::internalVisible() const
{
    return _internalVisible && (_group == nullptr || _group->internalVisible());
}

bool GObject::internalVisible2() const
{
    return _visible && (_group == nullptr || _group->internalVisible2());
}

bool GObject::internalVisible3() const
{
    return _visible && _internalVisible;
}

void GObject::setTouchable(bool value)
{
    _touchable = value;
}

void GObject::setSortingOrder(int value)
{
    if (value < 0)
        value = 0;
    if (_sortingOrder != value)
    {
        int old = _sortingOrder;
        _sortingOrder = value;
        if (_parent != nullptr)
            _parent->childSortingOrderChanged(this, old, _sortingOrder);
    }
}

void GObject::setGroup(GGroup* value)
{
    if (_group != value)
    {
        if (_group != nullptr)
            _group->setBoundsChangedFlag();
        _group = value;
        if (_group != nullptr)
            _group->setBoundsChangedFlag();
        handleVisibleChanged();
        if (_parent)
            _parent->childStateChanged(this);
    }
}

const std::string& GObject::getText() const
{
    return STD_STRING_EMPTY;
}

void GObject::setText(const std::string& text)
{
}

const std::string& GObject::getIcon() const
{
    return STD_STRING_EMPTY;
}

void GObject::setIcon(const std::string& text)
{
}

void GObject::setTooltips(const std::string& value)
{
    _tooltips = value;
    if (!_tooltips.empty())
    {
        addEventListener(UIEventType::RollOver, AX_CALLBACK_1(GObject::onRollOver, this), EventTag(this));
        addEventListener(UIEventType::RollOut, AX_CALLBACK_1(GObject::onRollOut, this), EventTag(this));
    }
}

void GObject::onRollOver(EventContext* context)
{
    getRoot()->showTooltips(_tooltips);
}

void GObject::onRollOut(EventContext* context)
{
    getRoot()->hideTooltips();
}

void GObject::setDraggable(bool value)
{
    if (_draggable != value)
    {
        _draggable = value;
        initDrag();
    }
}

void GObject::setDragBounds(const ax::Rect& value)
{
    if (_dragBounds == nullptr)
        _dragBounds = new Rect();
    *_dragBounds = value;
}

void GObject::startDrag(int touchId)
{
    dragBegin(touchId);
}

void GObject::stopDrag()
{
    dragEnd();
}

std::string GObject::getResourceURL() const
{
    if (_packageItem != nullptr)
        return "ui://" + _packageItem->owner->getId() + _packageItem->id;
    else
        return STD_STRING_EMPTY;
}

Vec2 GObject::localToGlobal(const Vec2& pt)
{
    Vec2 pt2 = pt;
    if (_pivotAsAnchor)
    {
        pt2.x += _size.width * _pivot.x;
        pt2.y += _size.height * _pivot.y;
    }
    pt2.y = _size.height - pt2.y;
    pt2 = _displayObject->convertToWorldSpace(pt2);
    return UIRoot->worldToRoot(pt2);
}

ax::Rect GObject::localToGlobal(const ax::Rect& rect)
{
    Rect ret;
    Vec2 v = localToGlobal(rect.origin);
    ret.origin.x = v.x;
    ret.origin.y = v.y;
    v = localToGlobal(Vec2(rect.getMaxX(), rect.getMaxY()));
    ret.size.width = v.x - ret.origin.x;
    ret.size.height = v.y - ret.origin.y;
    return ret;
}

Vec2 GObject::globalToLocal(const Vec2& pt)
{
    Vec2 pt2 = UIRoot->rootToWorld(pt);
    pt2 = _displayObject->convertToNodeSpace(pt2);
    pt2.y = _size.height - pt2.y;
    if (_pivotAsAnchor)
    {
        pt2.x -= _size.width * _pivot.x;
        pt2.y -= _size.height * _pivot.y;
    }
    return pt2;
}

ax::Rect GObject::globalToLocal(const ax::Rect& rect)
{
    Rect ret;
    Vec2 v = globalToLocal(rect.origin);
    ret.origin.x = v.x;
    ret.origin.y = v.y;
    v = globalToLocal(Vec2(rect.getMaxX(), rect.getMaxY()));
    ret.size.width = v.x - ret.origin.x;
    ret.size.height = v.y - ret.origin.y;
    return ret;
}

ax::Rect GObject::transformRect(const ax::Rect& rect, GObject* targetSpace)
{
    if (targetSpace == this)
        return rect;

    if (targetSpace == _parent) // optimization
    {
        return Rect((_position.x + rect.origin.x) * _scale.x,
                    (_position.y + rect.origin.y) * _scale.y,
                    rect.size.width * _scale.x,
                    rect.size.height * _scale.y);
    }
    else
    {
        float result[4]{FLT_MAX, FLT_MAX, -FLT_MAX, -FLT_MAX};

        transformRectPoint(rect.origin, result, targetSpace);
        transformRectPoint(Vec2(rect.getMaxX(), rect.origin.y), result, targetSpace);
        transformRectPoint(Vec2(rect.origin.x, rect.getMaxY()), result, targetSpace);
        transformRectPoint(Vec2(rect.getMaxX(), rect.getMaxY()), result, targetSpace);

        return Rect(result[0], result[1], result[2] - result[0], result[3] - result[1]);
    }
}

void GObject::transformRectPoint(const Vec2& pt, float rect[], GObject* targetSpace)
{
    Vec2 v = localToGlobal(pt);
    if (targetSpace != nullptr)
        v = targetSpace->globalToLocal(v);

    if (rect[0] > v.x)
        rect[0] = v.x;
    if (rect[2] < v.x)
        rect[2] = v.x;
    if (rect[1] > v.y)
        rect[1] = v.y;
    if (rect[3] < v.y)
        rect[3] = v.y;
}

void GObject::addRelation(GObject* target, RelationType relationType, bool usePercent)
{
    _relations->add(target, relationType, usePercent);
}

void GObject::removeRelation(GObject* target, RelationType relationType)
{
    _relations->remove(target, relationType);
}

GearBase* GObject::getGear(int index)
{
    GearBase* gear = _gears[index];
    if (gear == nullptr)
    {
        gear = GearBase::create(this, index);
        _gears[index] = gear;
    }
    return gear;
}

void GObject::updateGear(int index)
{
    if (_underConstruct || _gearLocked)
        return;

    GearBase* gear = _gears[index];
    if (gear != nullptr && gear->getController() != nullptr)
        gear->updateState();
}

bool GObject::checkGearController(int index, GController* c)
{
    return _gears[index] != nullptr && _gears[index]->getController() == c;
}

void GObject::updateGearFromRelations(int index, float dx, float dy)
{
    if (_gears[index] != nullptr)
        _gears[index]->updateFromRelations(dx, dy);
}

uint32_t GObject::addDisplayLock()
{
    GearDisplay* gearDisplay = (GearDisplay*)_gears[0];
    if (gearDisplay != nullptr && gearDisplay->getController() != nullptr)
    {
        uint32_t ret = gearDisplay->addLock();
        checkGearDisplay();

        return ret;
    }
    else
        return 0;
}

void GObject::releaseDisplayLock(uint32_t token)
{
    GearDisplay* gearDisplay = (GearDisplay*)_gears[0];
    if (gearDisplay != nullptr && gearDisplay->getController() != nullptr)
    {
        gearDisplay->releaseLock(token);
        checkGearDisplay();
    }
}

void GObject::checkGearDisplay()
{
    if (_handlingController)
        return;

    bool connected = _gears[0] == nullptr || ((GearDisplay*)_gears[0])->isConnected();
    if (_gears[8] != nullptr)
        connected = dynamic_cast<GearDisplay2*>(_gears[8])->evaluate(connected);

    if (connected != _internalVisible)
    {
        _internalVisible = connected;
        if (_parent != nullptr)
            _parent->childStateChanged(this);
        if (_group != nullptr && _group->isExcludeInvisibles())
            _group->setBoundsChangedFlag();
    }
}

bool GObject::onStage() const
{
    return _displayObject->getScene() != nullptr;
}

GObject* GObject::findParent() const
{
    if (_parent != nullptr)
        return _parent;

    Node* pn = _displayObject->getParent();
    if (pn == nullptr)
        return nullptr;

    while (pn != nullptr)
    {
        FUIContainer* fc = dynamic_cast<FUIContainer*>(pn);
        if (fc != nullptr && fc->gOwner)
            return fc->gOwner;

        pn = pn->getParent();
    }

    return nullptr;
}

GRoot* GObject::getRoot() const
{
    GObject* p = (GObject*)this;
    while (p->_parent != nullptr)
        p = p->_parent;

    GRoot* root = dynamic_cast<GRoot*>(p);
    if (root != nullptr)
        return root;
    else
        return UIRoot;
}

void GObject::removeFromParent()
{
    if (_parent != nullptr)
        _parent->removeChild(this);
}

ax::Value GObject::getProp(ObjectPropID propId)
{
    switch (propId)
    {
    case ObjectPropID::Text:
        return Value(getText());
    case ObjectPropID::Icon:
        return Value(getIcon());
    default:
        return Value::Null;
    }
}

void GObject::setProp(ObjectPropID propId, const ax::Value& value)
{
    switch (propId)
    {
    case ObjectPropID::Text:
        return setText(value.asString());
    case ObjectPropID::Icon:
        return setIcon(value.asString());
    default:
        break;
    }
}

void GObject::constructFromResource()
{
}

GObject* GObject::hitTest(const Vec2& worldPoint, const Camera* camera)
{
    if (_touchDisabled || !_touchable || !_displayObject->isVisible() || !_displayObject->getParent())
        return nullptr;

    Rect rect;
    rect.size = _size;
    //if (isScreenPointInRect(worldPoint, camera, _displayObject->getWorldToNodeTransform(), rect, nullptr))
    if (rect.containsPoint(_displayObject->convertToNodeSpace(worldPoint)))
        return this;
    else
        return nullptr;
}

void GObject::handleInit()
{
    _displayObject = Node::create();
    _displayObject->retain();
}

void GObject::onEnter()
{
    dispatchEvent(UIEventType::Enter);
}

void GObject::onExit()
{
    dispatchEvent(UIEventType::Exit);
}

void GObject::handlePositionChanged()
{
    if (_displayObject)
    {
        Vec2 pt = _position;
        pt.y = -pt.y;
        if (!_pivotAsAnchor)
        {
            pt.x += _size.width * _pivot.x;
            pt.y -= _size.height * _pivot.y;
        }
        if (_alignToBL)
        {
            if (_displayObject->getParent())
                pt.y += _displayObject->getParent()->getContentSize().height;
            else if (_parent != nullptr)
                pt.y += _parent->_displayObject->getContentSize().height;
        }
        if (_pixelSnapping)
        {
            pt.x = (int)pt.x;
            pt.y = (int)pt.y;
        }
        _displayObject->setPosition(pt);
    }
}

void GObject::handleSizeChanged()
{
    if (_displayObject)
    {
        if (_sizeImplType == 0 || sourceSize.width == 0 || sourceSize.height == 0)
            _displayObject->setContentSize(_size);
        else
            _displayObject->setScale(_scale.x * _size.width / sourceSize.width, _scale.y * _size.height / sourceSize.height);
    }
}

void GObject::handleScaleChanged()
{
    if (_sizeImplType == 0 || sourceSize.width == 0 || sourceSize.height == 0)
        _displayObject->setScale(_scale.x, _scale.y);
    else
        _displayObject->setScale(_scale.x * _size.width / sourceSize.width, _scale.y * _size.height / sourceSize.height);
}

void GObject::handleAlphaChanged()
{
    _displayObject->setOpacity(_alpha * 255);
}

void GObject::handleGrayedChanged()
{
    _finalGrayed = (_parent && _parent->_finalGrayed) || _grayed;
}

void GObject::handleVisibleChanged()
{
    _displayObject->setVisible(internalVisible2());
}

void GObject::handleControllerChanged(GController* c)
{
    _handlingController = true;
    for (int i = 0; i < 10; i++)
    {
        GearBase* gear = _gears[i];
        if (gear != nullptr && gear->getController() == c)
            gear->apply();
    }
    _handlingController = false;

    checkGearDisplay();
}

void GObject::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    buffer->seek(beginPos, 0);
    buffer->skip(5);

    id = buffer->readS();
    name = buffer->readS();
    float f1 = buffer->readInt();
    float f2 = buffer->readInt();
    setPosition(f1, f2);

    if (buffer->readBool())
    {
        initSize.width = buffer->readInt();
        initSize.height = buffer->readInt();
        setSize(initSize.width, initSize.height, true);
    }

    if (buffer->readBool())
    {
        minSize.width = buffer->readInt();
        maxSize.width = buffer->readInt();
        minSize.height = buffer->readInt();
        maxSize.height = buffer->readInt();
    }

    if (buffer->readBool())
    {
        f1 = buffer->readFloat();
        f2 = buffer->readFloat();
        setScale(f1, f2);
    }

    if (buffer->readBool())
    {
        f1 = buffer->readFloat();
        f2 = buffer->readFloat();
        setSkewX(f1);
        setSkewY(f2);
    }

    if (buffer->readBool())
    {
        f1 = buffer->readFloat();
        f2 = buffer->readFloat();
        setPivot(f1, f2, buffer->readBool());
    }

    f1 = buffer->readFloat();
    if (f1 != 1)
        setAlpha(f1);

    f1 = buffer->readFloat();
    if (f1 != 0)
        setRotation(f1);

    if (!buffer->readBool())
        setVisible(false);
    if (!buffer->readBool())
        setTouchable(false);
    if (buffer->readBool())
        setGrayed(true);
    buffer->readByte(); //blendMode
    buffer->readByte(); //filter

    const std::string& str = buffer->readS();
    if (!str.empty())
        _customData = Value(str);
}

void GObject::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    buffer->seek(beginPos, 1);

    const std::string& str = buffer->readS();
    if (!str.empty())
        setTooltips(str);

    int groupId = buffer->readShort();
    if (groupId >= 0)
        _group = dynamic_cast<GGroup*>(_parent->getChildAt(groupId));

    buffer->seek(beginPos, 2);

    int cnt = buffer->readShort();
    for (int i = 0; i < cnt; i++)
    {
        int nextPos = buffer->readUshort();
        nextPos += buffer->getPos();

        GearBase* gear = getGear(buffer->readByte());
        gear->setup(buffer);

        buffer->setPos(nextPos);
    }
}

void GObject::initDrag()
{
    if (_draggable)
    {
        addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(GObject::onTouchBegin, this), EventTag(this));
        addEventListener(UIEventType::TouchMove, AX_CALLBACK_1(GObject::onTouchMove, this), EventTag(this));
        addEventListener(UIEventType::TouchEnd, AX_CALLBACK_1(GObject::onTouchEnd, this), EventTag(this));
    }
    else
    {
        removeEventListener(UIEventType::TouchBegin, EventTag(this));
        removeEventListener(UIEventType::TouchMove, EventTag(this));
        removeEventListener(UIEventType::TouchEnd, EventTag(this));
    }
}

void GObject::dragBegin(int touchId)
{
    if (_draggingObject != nullptr)
    {
        GObject* tmp = _draggingObject;
        _draggingObject->stopDrag();
        _draggingObject = nullptr;
        tmp->dispatchEvent(UIEventType::DragEnd);
    }

    sGlobalDragStart = UIRoot->getTouchPosition(touchId);
    sGlobalRect = localToGlobal(Rect(Vec2::ZERO, _size));

    _draggingObject = this;
    _dragTesting = true;
    UIRoot->getInputProcessor()->addTouchMonitor(touchId, this);

    addEventListener(UIEventType::TouchMove, AX_CALLBACK_1(GObject::onTouchMove, this), EventTag(this));
    addEventListener(UIEventType::TouchEnd, AX_CALLBACK_1(GObject::onTouchEnd, this), EventTag(this));
}

void GObject::dragEnd()
{
    if (_draggingObject == this)
    {
        _dragTesting = false;
        _draggingObject = nullptr;
    }
}

void GObject::onTouchBegin(EventContext* context)
{
    _dragTouchStartPos = context->getInput()->getPosition();
    _dragTesting = true;
    context->captureTouch();
}

void GObject::onTouchMove(EventContext* context)
{
    InputEvent* evt = context->getInput();

    if (_draggingObject != this && _draggable && _dragTesting)
    {
        int sensitivity;
#ifdef AX_PLATFORM_PC
        sensitivity = UIConfig::clickDragSensitivity;
#else
        sensitivity = UIConfig::touchDragSensitivity;
#endif
        if (std::abs(_dragTouchStartPos.x - evt->getPosition().x) < sensitivity && std::abs(_dragTouchStartPos.y - evt->getPosition().y) < sensitivity)
            return;

        _dragTesting = false;
        if (!dispatchEvent(UIEventType::DragStart))
            dragBegin(evt->getTouchId());
    }

    if (_draggingObject == this)
    {
        float xx = evt->getPosition().x - sGlobalDragStart.x + sGlobalRect.origin.x;
        float yy = evt->getPosition().y - sGlobalDragStart.y + sGlobalRect.origin.y;

        if (_dragBounds != nullptr)
        {
            Rect rect = UIRoot->localToGlobal(*_dragBounds);
            if (xx < rect.origin.x)
                xx = rect.origin.x;
            else if (xx + sGlobalRect.size.width > rect.getMaxX())
            {
                xx = rect.getMaxX() - sGlobalRect.size.width;
                if (xx < rect.origin.x)
                    xx = rect.origin.x;
            }

            if (yy < rect.origin.y)
                yy = rect.origin.y;
            else if (yy + sGlobalRect.size.height > rect.getMaxY())
            {
                yy = rect.getMaxY() - sGlobalRect.size.height;
                if (yy < rect.origin.y)
                    yy = rect.origin.y;
            }
        }

        Vec2 pt = _parent->globalToLocal(Vec2(xx, yy));

        sUpdateInDragging = true;
        setPosition(round(pt.x), round(pt.y));
        sUpdateInDragging = false;

        dispatchEvent(UIEventType::DragMove);
    }
}

void GObject::onTouchEnd(EventContext* context)
{
    if (_draggingObject == this)
    {
        _draggingObject = nullptr;
        dispatchEvent(UIEventType::DragEnd);
    }
}

NS_FGUI_END
