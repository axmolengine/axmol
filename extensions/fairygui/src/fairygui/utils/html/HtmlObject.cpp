/****************************************************************************

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

#include "HtmlObject.h"
#include "HtmlElement.h"
#include "display/FUIRichText.h"
#include "utils/ToolSet.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
using namespace ax;

using namespace std;

std::string HtmlObject::buttonResource;
std::string HtmlObject::inputResource;
std::string HtmlObject::selectResource;
bool HtmlObject::usePool = true;

GObjectPool HtmlObject::objectPool;
ax::Vector<GObject*> HtmlObject::loaderPool;

HtmlObject::HtmlObject() :_ui(nullptr), _hidden(false)
{
}

HtmlObject::~HtmlObject()
{
    if (_ui != nullptr)
    {
        destroy();

        if (usePool)
        {
            if (!_ui->getResourceURL().empty())
                objectPool.returnObject(_ui);
            else if (dynamic_cast<GLoader*>(_ui) != nullptr)
                loaderPool.pushBack(_ui);
        }
    }

    AX_SAFE_RELEASE(_ui);
}

void HtmlObject::create(FUIRichText* owner, HtmlElement* element)
{
    _owner = owner;
    _element = element;

    switch (element->type)
    {
    case HtmlElement::Type::IMAGE:
        createImage();
        break;

    case HtmlElement::Type::INPUT:
    {
        string type = element->getString("type");
        transform(type.begin(), type.end(), type.begin(), ::tolower);
        if (type == "button" || type == "submit")
            createButton();
        else
            createInput();
        break;
    }

    case HtmlElement::Type::SELECT:
        createSelect();
        break;

    case HtmlElement::Type::OBJECT:
        createCommon();
        break;
    }
}

void HtmlObject::destroy()
{
    switch (_element->type)
    {
    case HtmlElement::Type::IMAGE:
        ((GLoader*)_ui)->setURL(STD_STRING_EMPTY);
        break;
    }
}

void HtmlObject::createCommon()
{
    string src = _element->getString("src");
    if (!src.empty())
        _ui = objectPool.getObject(src);
    else
        _ui = GComponent::create();

    _ui->retain();

    int width = _element->getInt("width", _ui->sourceSize.width);
    int height = _element->getInt("height", _ui->sourceSize.height);

    _ui->setSize(width, height);
    _ui->setText(_element->getString("title"));
    _ui->setIcon(_element->getString("icon"));
}

void HtmlObject::createImage()
{
    int width = 0;
    int height = 0;
    string src = _element->getString("src");
    if (!src.empty()) {
        PackageItem* pi = UIPackage::getItemByURL(src);
        if (pi)
        {
            width = pi->width;
            height = pi->height;
        }
    }

    width = _element->getInt("width", width);
    height = _element->getInt("height", height);

    GLoader* loader;
    if (!loaderPool.empty())
    {
        loader = (GLoader*)loaderPool.back();
        loader->retain();
        loaderPool.popBack();
    }
    else
    {
        loader = GLoader::create();
        loader->retain();
    }

    _ui = loader;

    loader->setSize(width, height);
    loader->setFill(LoaderFillType::SCALE_FREE);
    loader->setURL(src);
}

void HtmlObject::createButton()
{
    if (!buttonResource.empty())
        _ui = objectPool.getObject(buttonResource);
    else
    {
        _ui = GComponent::create();
        AXLOGW("Set HtmlObject.buttonResource first");
    }

    _ui->retain();

    int width = _element->getInt("width", _ui->sourceSize.width);
    int height = _element->getInt("height", _ui->sourceSize.height);

    _ui->setSize(width, height);
    _ui->setText(_element->getString("value"));

    GButton *button = dynamic_cast<GButton*>(_ui);
    if (button != nullptr)
    {
        button->setSelected(_element->getString("checked") == "true");
    }
}

void HtmlObject::createInput()
{
    if (!inputResource.empty())
        _ui = objectPool.getObject(inputResource);
    else
    {
        _ui = GComponent::create();
        AXLOGW("Set HtmlObject.inputResource first");
    }

    _ui->retain();

    string type = _element->getString("type");
    transform(type.begin(), type.end(), type.begin(), ::tolower);
    _hidden = type == "hidden";

    int width = _element->getInt("width");
    int height = _element->getInt("height");

    if (width == 0)
    {
        width = _element->space;
        if (width > _owner->getContentSize().width / 2 || width < 100)
            width = _owner->getContentSize().width / 2;
    }

    if (height == 0)
        height = _element->format.fontSize + 10;

    _ui->setSize(width, height);
    _ui->setText(_element->getString("value"));

    GLabel *label = dynamic_cast<GLabel*>(_ui);
    if (label != nullptr)
    {
        GTextInput* input = dynamic_cast<GTextInput*>(label->getTextField());
        if (input != nullptr)
        {
            input->setPassword(type == "password");
        }
    }
}

void HtmlObject::createSelect()
{
    if (!selectResource.empty())
        _ui = objectPool.getObject(selectResource);
    else
    {
        _ui = GComponent::create();
        AXLOGW("Set HtmlObject.selectResource first");
    }

    _ui->retain();

    int width = _element->getInt("width", _ui->sourceSize.width);
    int height = _element->getInt("height", _ui->sourceSize.height);

    _ui->setSize(width, height);

    GComboBox* comboBox = dynamic_cast<GComboBox*>(_ui);
    if (comboBox != nullptr)
    {
        auto& items = _element->getArray("items");
        auto& values = _element->getArray("values");
        comboBox->getItems().clear();
        comboBox->getValues().clear();
        for (auto it : items)
            comboBox->getItems().push_back(it.asString());
        for (auto it : values)
            comboBox->getValues().push_back(it.asString());
        comboBox->setValue(_element->getString("value"));
        comboBox->refresh();
    }
}

NS_FGUI_END
