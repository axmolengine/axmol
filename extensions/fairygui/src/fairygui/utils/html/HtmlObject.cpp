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

static bool s_isGfxDidDrop = false;

HtmlObjectContext* HtmlObjectContext::s_htmlObjContext;
HtmlObjectContext* HtmlObjectContext::getInstance()
{
    if (s_htmlObjContext) [[likely]]
        return s_htmlObjContext;

    if (s_isGfxDidDrop)
        return nullptr;

    s_htmlObjContext = new HtmlObjectContext();

    Director::getInstance()->getEventDispatcher()->addCustomEventListener(Director::EVENT_BEFORE_GFX_DROP,
                                                                          [](EventCustom*) {
        s_isGfxDidDrop = true;
        HtmlObjectContext::destroyInstance();
    });

    return s_htmlObjContext;
}

void HtmlObjectContext::destroyInstance()
{
    if (s_htmlObjContext)
    {
        Director::getInstance()->getEventDispatcher()->removeCustomEventListeners(Director::EVENT_BEFORE_GFX_DROP);
        delete s_htmlObjContext;
        s_htmlObjContext = nullptr;
    }
}

void HtmlObjectContext::recycleObject(GObject* obj)
{
    this->objectPool.returnObject(obj);
}

void HtmlObjectContext::recycleLoader(GObject* loader)
{
    this->loaderPool.pushBack(loader);
}

GObject* HtmlObjectContext::acquireObject(const std::string& src, std::string_view dedicatedResourceHint)
{
    GObject* obj;
    if (!src.empty())
        obj = this->objectPool.getObject(src);
    else
    {
        if (!dedicatedResourceHint.empty())
            AXLOGW("Set HtmlObject.{} first", dedicatedResourceHint);
        obj = GComponent::create();
    }
    obj->retain();
    return obj;
}

GLoader* HtmlObjectContext::acquireLoader()
{
    GLoader* loader;
    if (!isLoaderPoolEmpty())
    {
        loader = (GLoader*)this->loaderPool.back();
        loader->retain();
        this->loaderPool.popBack();
    }
    else
    {
        loader = GLoader::create();
        loader->retain();
    }
    return loader;
}

bool HtmlObjectContext::isLoaderPoolEmpty() const
{
    return this->loaderPool.empty();
}

HtmlObject::HtmlObject() : _element(nullptr), _owner(nullptr), _ui(nullptr), _hidden(false) {}

HtmlObject::~HtmlObject()
{
    if (_ui != nullptr)
    {
        destroy();

        auto context = HtmlObjectContext::getInstance();

        if (context && context->usePool)
        {
            if (!_ui->getResourceURL().empty())
                context->recycleObject(_ui);
            else if (dynamic_cast<GLoader*>(_ui) != nullptr)
                context->recycleLoader(_ui);
        }
    }

    AX_SAFE_RELEASE(_ui);
}

void HtmlObject::create(FUIRichText* owner, HtmlElement* element)
{
    _owner   = owner;
    _element = element;

    switch (element->type)
    {
    case HtmlElement::Type::IMAGE:
        createImage();
        break;

    case HtmlElement::Type::INPUT:
    {
        std::string type = element->getString("type");
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
    std::string src = _element->getString("src");

    _ui = HtmlObjectContext::getInstance()->acquireObject(src, ""sv);

    int width  = _element->getInt("width", _ui->sourceSize.width);
    int height = _element->getInt("height", _ui->sourceSize.height);

    _ui->setSize(width, height);
    _ui->setText(_element->getString("title"));
    _ui->setIcon(_element->getString("icon"));
}

void HtmlObject::createImage()
{
    int width       = 0;
    int height      = 0;
    std::string src = _element->getString("src");
    if (!src.empty())
    {
        PackageItem* pi = UIPackage::getItemByURL(src);
        if (pi)
        {
            width  = pi->width;
            height = pi->height;
        }
    }

    width  = _element->getInt("width", width);
    height = _element->getInt("height", height);

    GLoader* loader = HtmlObjectContext::getInstance()->acquireLoader();
    _ui             = loader;

    loader->setSize(width, height);
    loader->setFill(LoaderFillType::SCALE_FREE);
    loader->setURL(src);
}

void HtmlObject::createButton()
{
    auto context = HtmlObjectContext::getInstance();

    _ui = context->acquireObject(context->buttonResource, "buttonResource"sv);

    int width  = _element->getInt("width", _ui->sourceSize.width);
    int height = _element->getInt("height", _ui->sourceSize.height);

    _ui->setSize(width, height);
    _ui->setText(_element->getString("value"));

    GButton* button = dynamic_cast<GButton*>(_ui);
    if (button != nullptr)
    {
        button->setSelected(_element->getString("checked") == "true");
    }
}

void HtmlObject::createInput()
{
    auto context = HtmlObjectContext::getInstance();

    _ui = context->acquireObject(context->inputResource, "inputResource"sv);

    std::string type = _element->getString("type");
    std::transform(type.begin(), type.end(), type.begin(), ::tolower);
    _hidden = type == "hidden";

    int width  = _element->getInt("width");
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

    GLabel* label = dynamic_cast<GLabel*>(_ui);
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
    auto context = HtmlObjectContext::getInstance();

    _ui = context->acquireObject(context->selectResource, "selectResource"sv);

    int width  = _element->getInt("width", _ui->sourceSize.width);
    int height = _element->getInt("height", _ui->sourceSize.height);

    _ui->setSize(width, height);

    GComboBox* comboBox = dynamic_cast<GComboBox*>(_ui);
    if (comboBox != nullptr)
    {
        auto& items  = _element->getArray("items");
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
