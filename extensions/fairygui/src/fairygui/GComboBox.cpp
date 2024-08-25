#include "GComboBox.h"
#include "GRoot.h"
#include "PackageItem.h"
#include "UIConfig.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

// clang-format off
GComboBox::GComboBox()
    : visibleItemCount(UIConfig::defaultComboBoxVisibleItemCount),
      popupDirection(PopupDirection::AUTO),
      _dropdown(nullptr),
      _titleObject(nullptr),
      _iconObject(nullptr),
      _list(nullptr),
      _selectionController(nullptr),
      _itemsUpdated(true), 
      _selectedIndex(-1), 
      _buttonController(nullptr),
      _down(false),
      _over(false)
{
}
// clang-format on

GComboBox::~GComboBox()
{
    AX_SAFE_RELEASE(_dropdown);
}

const std::string& GComboBox::getTitle() const
{
    if (_titleObject != nullptr)
        return _titleObject->getText();
    else
        return STD_STRING_EMPTY;
}

void GComboBox::setTitle(const std::string& value)
{
    if (_titleObject != nullptr)
        _titleObject->setText(value);
    updateGear(6);
}

const ax::Color3B GComboBox::getTitleColor() const
{
    GTextField* tf = getTextField();
    if (tf)
        return tf->getColor();
    else
        return Color3B::BLACK;
}

void GComboBox::setTitleColor(const ax::Color3B& value)
{
    GTextField* tf = getTextField();
    if (tf)
        tf->setColor(value);
}

int GComboBox::getTitleFontSize() const
{
    GTextField* tf = getTextField();
    if (tf)
        return static_cast<int>(tf->getFontSize());
    else
        return 0;
}

void GComboBox::setTitleFontSize(int value)
{
    GTextField* tf = getTextField();
    if (tf)
        tf->setFontSize(static_cast<float>(value));
}

const std::string& GComboBox::getIcon() const
{
    if (_iconObject != nullptr)
        return _iconObject->getIcon();
    else
        return STD_STRING_EMPTY;
}

void GComboBox::setIcon(const std::string& value)
{
    if (_iconObject != nullptr)
        _iconObject->setIcon(value);
    updateGear(7);
}

const std::string& GComboBox::getValue() const
{
    if (_selectedIndex >= 0 && _selectedIndex < (int)_values.size())
        return _values[_selectedIndex];
    else
        return STD_STRING_EMPTY;
}

void GComboBox::setValue(const std::string& value)
{
    setSelectedIndex(ToolSet::findInStringArray(_values, value));
}

void GComboBox::setSelectedIndex(int value)
{
    if (_selectedIndex == value)
        return;

    _selectedIndex = value;
    if (_selectedIndex >= 0 && _selectedIndex < (int)_items.size())
    {
        setText(_items[_selectedIndex]);
        if (!_icons.empty() && _selectedIndex != -1 && _selectedIndex < (int)_icons.size())
            setIcon(_icons[_selectedIndex]);
    }
    else
    {
        setTitle(STD_STRING_EMPTY);
        if (!_icons.empty())
            setIcon(STD_STRING_EMPTY);
    }

    updateSelectionController();
}

void GComboBox::refresh()
{
    if (!_items.empty())
    {
        if (_selectedIndex >= (int)_items.size())
            _selectedIndex = (int)_items.size() - 1;
        else if (_selectedIndex == -1)
            _selectedIndex = 0;
        setTitle(_items[_selectedIndex]);
    }
    else
    {
        setTitle(STD_STRING_EMPTY);
        _selectedIndex = -1;
    }

    if (!_icons.empty())
    {
        if (_selectedIndex != -1 && _selectedIndex < (int)_icons.size())
            setIcon(_icons[_selectedIndex]);
        else
            setIcon(STD_STRING_EMPTY);
    }

    _itemsUpdated = true;
}

void GComboBox::setState(const std::string& value)
{
    if (_buttonController != nullptr)
        _buttonController->setSelectedPage(value);
}

void GComboBox::setCurrentState()
{
    if (isGrayed() && _buttonController != nullptr && _buttonController->hasPage(GButton::DISABLED))
        setState(GButton::DISABLED);
    else if (_dropdown != nullptr && _dropdown->getParent() != nullptr)
        setState(GButton::DOWN);
    else
        setState(_over ? GButton::OVER : GButton::UP);
}

void GComboBox::updateSelectionController()
{
    if (_selectionController != nullptr && !_selectionController->changing && _selectedIndex < _selectionController->getPageCount())
    {
        GController* c = _selectionController;
        _selectionController = nullptr;
        c->setSelectedIndex(_selectedIndex);
        _selectionController = c;
    }
}

void GComboBox::updateDropdownList()
{
    if (_itemsUpdated)
    {
        _itemsUpdated = false;
        renderDropdownList();
        _list->resizeToFit(visibleItemCount);
    }
}

void GComboBox::showDropdown()
{
    updateDropdownList();
    if (_list->getSelectionMode() == ListSelectionMode::SINGLE)
        _list->setSelectedIndex(-1);
    _dropdown->setWidth(_size.width);
    _list->ensureBoundsCorrect();

    UIRoot->togglePopup(_dropdown, this, popupDirection);
    if (_dropdown->getParent() != nullptr)
        setState(GButton::DOWN);
}

void GComboBox::renderDropdownList()
{
    _list->removeChildrenToPool();
    size_t cnt = _items.size();
    for (size_t i = 0; i < cnt; i++)
    {
        GObject* item = _list->addItemFromPool();
        item->setText(_items[i]);
        item->setIcon((!_icons.empty() && i < _icons.size()) ? _icons[i] : STD_STRING_EMPTY);
        item->name = i < _values.size() ? _values[i] : STD_STRING_EMPTY;
    }
}

void GComboBox::handleControllerChanged(GController* c)
{
    GComponent::handleControllerChanged(c);

    if (_selectionController == c)
        setSelectedIndex(c->getSelectedIndex());
}

void GComboBox::handleGrayedChanged()
{
    if (_buttonController != nullptr && _buttonController->hasPage(GButton::DISABLED))
    {
        if (isGrayed())
            setState(GButton::DISABLED);
        else
            setState(GButton::UP);
    }
    else
        GComponent::handleGrayedChanged();
}

GTextField* GComboBox::getTextField() const
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

ax::Value GComboBox::getProp(ObjectPropID propId)
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
    default:
        return GComponent::getProp(propId);
    }
}

void GComboBox::setProp(ObjectPropID propId, const ax::Value& value)
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
    default:
        GComponent::setProp(propId, value);
        break;
    }
}

void GComboBox::constructExtension(ByteBuffer* buffer)
{
    buffer->seek(0, 6);

    _buttonController = getController("button");
    _titleObject = getChild("title");
    _iconObject = getChild("icon");

    const std::string& dropdown = buffer->readS();
    if (!dropdown.empty())
    {
        _dropdown = dynamic_cast<GComponent*>(UIPackage::createObjectFromURL(dropdown));
        AXASSERT(_dropdown != nullptr, "FairyGUI: should be a component.");

        _dropdown->retain();

        _list = dynamic_cast<GList*>(_dropdown->getChild("list"));
        AXASSERT(_list != nullptr, "FairyGUI: should container a list component named list.");

        _list->addEventListener(UIEventType::ClickItem, AX_CALLBACK_1(GComboBox::onClickItem, this));

        _list->addRelation(_dropdown, RelationType::Width);
        _list->removeRelation(_dropdown, RelationType::Height);

        _dropdown->addRelation(_list, RelationType::Height);
        _dropdown->removeRelation(_list, RelationType::Width);

        _dropdown->addEventListener(UIEventType::Exit, AX_CALLBACK_1(GComboBox::onPopupWinClosed, this));
    }

    addEventListener(UIEventType::RollOver, AX_CALLBACK_1(GComboBox::onRollover, this));
    addEventListener(UIEventType::RollOut, AX_CALLBACK_1(GComboBox::onRollout, this));
    addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(GComboBox::onTouchBegin, this));
    addEventListener(UIEventType::TouchEnd, AX_CALLBACK_1(GComboBox::onTouchEnd, this));
}

void GComboBox::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GComponent::setup_afterAdd(buffer, beginPos);

    if (!buffer->seek(beginPos, 6))
        return;

    if ((ObjectType)buffer->readByte() != _packageItem->objectType)
        return;

    const std::string* str;
    bool hasIcon = false;
    int itemCount = buffer->readShort();
    for (int i = 0; i < itemCount; i++)
    {
        int nextPos = buffer->readShort();
        nextPos += buffer->getPos();

        _items.push_back(buffer->readS());
        _values.push_back(buffer->readS());
        if ((str = buffer->readSP()))
        {
            if (!hasIcon)
            {
                for (int j = 0; j < (int)_items.size() - 1; j++)
                    _icons.push_back(STD_STRING_EMPTY);
            }
            _icons.push_back(*str);
        }

        buffer->setPos(nextPos);
    }

    if ((str = buffer->readSP()))
    {
        setTitle(*str);
        _selectedIndex = ToolSet::findInStringArray(_items, *str);
    }
    else if (!_items.empty())
    {
        _selectedIndex = 0;
        setTitle(_items[0]);
    }
    else
        _selectedIndex = -1;

    if ((str = buffer->readSP()))
        setIcon(*str);

    if (buffer->readBool())
        setTitleColor((Color3B)buffer->readColor());
    int iv = buffer->readInt();
    if (iv > 0)
        visibleItemCount = iv;
    popupDirection = (PopupDirection)buffer->readByte();

    iv = buffer->readShort();
    if (iv >= 0)
        _selectionController = _parent->getControllerAt(iv);
}

void GComboBox::onClickItem(EventContext* context)
{
    if (dynamic_cast<GRoot*>(_dropdown->getParent()))
        ((GRoot*)_dropdown->getParent())->hidePopup(_dropdown);
    _selectedIndex = INT_MIN;
    setSelectedIndex(_list->getChildIndex((GObject*)context->getData()));

    dispatchEvent(UIEventType::Changed);
}

void GComboBox::onRollover(EventContext* context)
{
    _over = true;
    if (_down || (_dropdown != nullptr && _dropdown->getParent() != nullptr))
        return;

    setCurrentState();
}

void GComboBox::onRollout(EventContext* context)
{
    _over = false;
    if (_down || (_dropdown != nullptr && _dropdown->getParent() != nullptr))
        return;

    setCurrentState();
}

void GComboBox::onTouchBegin(EventContext* context)
{
    if (context->getInput()->getButton() != EventMouse::MouseButton::BUTTON_LEFT)
        return;

    if (dynamic_cast<GTextInput*>(context->getInput()->getTarget()))
        return;

    _down = true;

    if (_dropdown != nullptr)
        showDropdown();

    context->captureTouch();
}

void GComboBox::onTouchEnd(EventContext* context)
{
    if (context->getInput()->getButton() != EventMouse::MouseButton::BUTTON_LEFT)
        return;

    if (_down)
    {
        _down = false;
        if (_dropdown != nullptr && _dropdown->getParent() != nullptr)
            setCurrentState();
    }
}

void GComboBox::onPopupWinClosed(EventContext* context)
{
    setCurrentState();
}

NS_FGUI_END
