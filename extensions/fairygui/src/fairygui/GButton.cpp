#include "GButton.h"
#include "GLabel.h"
#include "GRoot.h"
#include "GTextField.h"
#include "PackageItem.h"
#include "UIConfig.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

const std::string GButton::UP = "up";
const std::string GButton::DOWN = "down";
const std::string GButton::OVER = "over";
const std::string GButton::SELECTED_OVER = "selectedOver";
const std::string GButton::DISABLED = "disabled";
const std::string GButton::SELECTED_DISABLED = "selectedDisabled";

GButton::GButton() : _mode(ButtonMode::COMMON),
                     _titleObject(nullptr),
                     _iconObject(nullptr),
                     _buttonController(nullptr),
                     _relatedController(nullptr),
                     _selected(false),
                     _over(false),
                     _down(false),
                     _downEffect(0),
                     _downScaled(false),
                     _downEffectValue(0.8f), 
                     _changeStateOnClick(true), 
                     _sound(UIConfig::buttonSound), 
                     _soundVolumeScale(UIConfig::buttonSoundVolumeScale)
{
}

GButton::~GButton()
{
}

void GButton::setTitle(const std::string& value)
{
    _title = value;
    if (_titleObject != nullptr)
        _titleObject->setText((_selected && _selectedTitle.length() > 0) ? _selectedTitle : _title);
    updateGear(6);
}

void GButton::setIcon(const std::string& value)
{
    _icon = value;
    if (_iconObject != nullptr)
        _iconObject->setIcon((_selected && _selectedIcon.length() > 0) ? _selectedIcon : _icon);
    updateGear(7);
}

void GButton::setSelectedTitle(const std::string& value)
{
    _selectedTitle = value;
    if (_titleObject != nullptr)
        _titleObject->setText((_selected && _selectedTitle.length() > 0) ? _selectedTitle : _title);
}

void GButton::setSelectedIcon(const std::string& value)
{
    _selectedIcon = value;
    if (_iconObject != nullptr)
        _iconObject->setIcon((_selected && _selectedIcon.length() > 0) ? _selectedIcon : _icon);
}

ax::Color3B GButton::getTitleColor() const
{
    GTextField* tf = getTextField();
    if (tf)
        return tf->getColor();
    else
        return Color3B::BLACK;
}

void GButton::setTitleColor(const ax::Color3B& value)
{
    GTextField* tf = getTextField();
    if (tf)
        tf->setColor(value);
}

int GButton::getTitleFontSize() const
{
    GTextField* tf = getTextField();
    if (tf)
        return tf->getFontSize();
    else
        return 0;
}

void GButton::setTitleFontSize(int value)
{
    GTextField* tf = getTextField();
    if (tf)
        tf->setFontSize(value);
}

void GButton::setSelected(bool value)
{
    if (_mode == ButtonMode::COMMON)
        return;

    if (_selected != value)
    {
        _selected = value;
        setCurrentState();
        if (!_selectedTitle.empty() && _titleObject != nullptr)
            _titleObject->setText(_selected ? _selectedTitle : _title);
        if (!_selectedIcon.empty())
        {
            const std::string& str = _selected ? _selectedIcon : _icon;
            if (_iconObject != nullptr)
                _iconObject->setIcon(str);
        }
        if (_relatedController != nullptr && getParent() != nullptr && !getParent()->_buildingDisplayList)
        {
            if (_selected)
            {
                _relatedController->setSelectedPageId(_relatedPageId);
                if (_relatedController->autoRadioGroupDepth)
                    getParent()->adjustRadioGroupDepth(this, _relatedController);
            }
            else if (_mode == ButtonMode::CHECK && _relatedController->getSelectedPageId().compare(_relatedPageId) == 0)
                _relatedController->setOppositePageId(_relatedPageId);
        }
    }
}

void GButton::setRelatedController(GController* c)
{
    _relatedController = c;
}

void GButton::setState(const std::string& value)
{
    if (_buttonController != nullptr)
        _buttonController->setSelectedPage(value);

    if (_downEffect == 1)
    {
        int cnt = this->numChildren();
        if (value == DOWN || value == SELECTED_OVER || value == SELECTED_DISABLED)
        {
            int c = _downEffectValue * 255;
            Value color = Value((c << 16) + (c << 8) + c);
            for (int i = 0; i < cnt; i++)
            {
                GObject* obj = this->getChildAt(i);
                if (dynamic_cast<GTextField*>(obj) == nullptr)
                    obj->setProp(ObjectPropID::Color, color);
            }
        }
        else
        {
            Value color = Value(0xFFFFFF);
            for (int i = 0; i < cnt; i++)
            {
                GObject* obj = this->getChildAt(i);
                if (dynamic_cast<GTextField*>(obj) == nullptr)
                    obj->setProp(ObjectPropID::Color, color);
            }
        }
    }
    else if (_downEffect == 2)
    {
        if (value == DOWN || value == SELECTED_OVER || value == SELECTED_DISABLED)
        {
            if (!_downScaled)
            {
                _downScaled = true;
                setScale(getScaleX() * _downEffectValue, getScaleY() * _downEffectValue);
            }
        }
        else
        {
            if (_downScaled)
            {
                _downScaled = false;
                setScale(getScaleX() / _downEffectValue, getScaleY() / _downEffectValue);
            }
        }
    }
}

void GButton::setCurrentState()
{
    if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(DISABLED))
    {
        if (_selected)
            setState(SELECTED_DISABLED);
        else
            setState(DISABLED);
    }
    else
    {
        if (_selected)
            setState(_over ? SELECTED_OVER : DOWN);
        else
            setState(_over ? OVER : UP);
    }
}

GTextField* GButton::getTextField() const
{
    if (dynamic_cast<GTextField*>(_titleObject))
        return dynamic_cast<GTextField*>(_titleObject);
    else if (dynamic_cast<GLabel*>(_titleObject))
        return dynamic_cast<GLabel*>(_titleObject)->getTextField();
    else if (dynamic_cast<GButton*>(_titleObject))
        return dynamic_cast<GButton*>(_titleObject)->getTextField();
    else
        return nullptr;
}

ax::Value GButton::getProp(ObjectPropID propId)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        return Value(ToolSet::colorToInt(getTitleColor()));
    case ObjectPropID::OutlineColor:
    {
        GTextField* tf = getTextField();
        if (tf != nullptr)
            return Value(ToolSet::colorToInt(tf->getOutlineColor()));
        else
            return Value::Null;
    }
    case ObjectPropID::FontSize:
        return Value(getTitleFontSize());
    case ObjectPropID::Selected:
        return Value(isSelected());
    default:
        return GComponent::getProp(propId);
    }
}

void GButton::setProp(ObjectPropID propId, const ax::Value& value)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        setTitleColor(ToolSet::intToColor(value.asUnsignedInt()));
        break;
    case ObjectPropID::OutlineColor:
    {
        GTextField* tf = getTextField();
        if (tf != nullptr)
            tf->setOutlineColor(ToolSet::intToColor(value.asUnsignedInt()));
        break;
    }
    case ObjectPropID::FontSize:
        setTitleFontSize(value.asInt());
        break;
    case ObjectPropID::Selected:
        setSelected(value.asBool());
        break;
    default:
        GComponent::setProp(propId, value);
        break;
    }
}

void GButton::constructExtension(ByteBuffer* buffer)
{
    buffer->seek(0, 6);

    _mode = (ButtonMode)buffer->readByte();
    buffer->readS(_sound);
    _soundVolumeScale = buffer->readFloat();
    _downEffect = buffer->readByte();
    _downEffectValue = buffer->readFloat();
    if (_downEffect == 2)
        setPivot(0.5f, 0.5f, isPivotAsAnchor());

    _buttonController = getController("button");
    _titleObject = getChild("title");
    _iconObject = getChild("icon");
    if (_titleObject != nullptr)
        _title = _titleObject->getText();
    if (_iconObject != nullptr)
        _icon = _iconObject->getIcon();

    if (_mode == ButtonMode::COMMON)
        setState(UP);

    addEventListener(UIEventType::RollOver, AX_CALLBACK_1(GButton::onRollOver, this));
    addEventListener(UIEventType::RollOut, AX_CALLBACK_1(GButton::onRollOut, this));
    addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(GButton::onTouchBegin, this));
    addEventListener(UIEventType::TouchEnd, AX_CALLBACK_1(GButton::onTouchEnd, this));
    addEventListener(UIEventType::Click, AX_CALLBACK_1(GButton::onClick, this));
    addEventListener(UIEventType::Exit, AX_CALLBACK_1(GButton::onExit, this));
}

void GButton::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GComponent::setup_afterAdd(buffer, beginPos);

    if (!buffer->seek(beginPos, 6))
        return;

    if ((ObjectType)buffer->readByte() != _packageItem->objectType)
        return;

    const std::string* str;

    if ((str = buffer->readSP()))
        setTitle(*str);
    if ((str = buffer->readSP()))
        setSelectedTitle(*str);
    if ((str = buffer->readSP()))
        setIcon(*str);
    if ((str = buffer->readSP()))
        setSelectedIcon(*str);
    if (buffer->readBool())
        setTitleColor((Color3B)buffer->readColor());
    int iv = buffer->readInt();
    if (iv != 0)
        setTitleFontSize(iv);
    iv = buffer->readShort();
    if (iv >= 0)
        _relatedController = _parent->getControllerAt(iv);
    _relatedPageId = buffer->readS();

    buffer->readS(_sound);
    if (buffer->readBool())
        _soundVolumeScale = buffer->readFloat();

    setSelected(buffer->readBool());
}

void GButton::handleControllerChanged(GController* c)
{
    GObject::handleControllerChanged(c);

    if (_relatedController == c)
        setSelected(_relatedPageId.compare(c->getSelectedPageId()) == 0);
}

void GButton::onRollOver(EventContext* context)
{
    if (_buttonController == nullptr || !_buttonController->hasPage(OVER))
        return;

    _over = true;
    if (_down)
        return;

    if (isGrayed() && _buttonController->hasPage(DISABLED))
        return;

    setState(_selected ? SELECTED_OVER : OVER);
}

void GButton::onRollOut(EventContext* context)
{
    if (_buttonController == nullptr || !_buttonController->hasPage(OVER))
        return;

    _over = false;
    if (_down)
        return;

    if (isGrayed() && _buttonController->hasPage(DISABLED))
        return;

    setState(_selected ? DOWN : UP);
}

void GButton::onTouchBegin(EventContext* context)
{
    if (context->getInput()->getButton() != EventMouse::MouseButton::BUTTON_LEFT)
        return;

    _down = true;
    context->captureTouch();

    if (_mode == ButtonMode::COMMON)
    {
        if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(DISABLED))
            setState(SELECTED_DISABLED);
        else
            setState(DOWN);
    }
}

void GButton::onTouchEnd(EventContext* context)
{
    if (context->getInput()->getButton() != EventMouse::MouseButton::BUTTON_LEFT)
        return;

    if (_down)
    {
        _down = false;
        if (_mode == ButtonMode::COMMON)
        {
            if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(DISABLED))
                setState(DISABLED);
            else if (_over)
                setState(OVER);
            else
                setState(UP);
        }
        else
        {
            if (!_over && _buttonController != nullptr && (_buttonController->getSelectedPage() == OVER || _buttonController->getSelectedPage() == SELECTED_OVER))
            {
                setCurrentState();
            }
        }
    }
}

void GButton::onClick(EventContext* context)
{
    if (!_sound.empty())
        UIRoot->playSound(_sound, _soundVolumeScale);

    if (_mode == ButtonMode::CHECK)
    {
        if (_changeStateOnClick)
        {
            setSelected(!_selected);
            dispatchEvent(UIEventType::Changed);
        }
    }
    else if (_mode == ButtonMode::RADIO)
    {
        if (_changeStateOnClick && !_selected)
        {
            setSelected(true);
            dispatchEvent(UIEventType::Changed);
        }
    }
    else
    {
        if (_relatedController != nullptr)
            _relatedController->setSelectedPageId(_relatedPageId);
    }
}

void GButton::onExit(EventContext* context)
{
    if (_over)
        onRollOut(context);
}

NS_FGUI_END
