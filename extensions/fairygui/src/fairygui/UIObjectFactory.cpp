#include "UIObjectFactory.h"
#include "GButton.h"
#include "GComboBox.h"
#include "GComponent.h"
#include "GGraph.h"
#include "GGroup.h"
#include "GImage.h"
#include "GLabel.h"
#include "GList.h"
#include "GLoader.h"
#include "GMovieClip.h"
#include "GProgressBar.h"
#include "GRichTextField.h"
#include "GScrollBar.h"
#include "GSlider.h"
#include "GTextField.h"
#include "GTextInput.h"
#include "GTree.h"
#include "GLoader3D.h"
#include "UIPackage.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN

using namespace std;

unordered_map<string, UIObjectFactory::GComponentCreator> UIObjectFactory::_packageItemExtensions;
UIObjectFactory::GLoaderCreator UIObjectFactory::_loaderCreator;

void UIObjectFactory::setPackageItemExtension(const string& url, GComponentCreator creator)
{
    if (url.size() == 0)
    {
        AXLOGD("Invaild url: {}", url);
        return;
    }
    PackageItem* pi = UIPackage::getItemByURL(url);
    if (pi)
        pi->extensionCreator = creator;

    _packageItemExtensions[url] = creator;
}

GObject* UIObjectFactory::newObject(PackageItem* pi)
{
    GObject* obj;
    if (pi->extensionCreator != nullptr)
        obj = pi->extensionCreator();
    else
        obj = newObject(pi->objectType);
    if (obj != nullptr)
        obj->_packageItem = pi;

    return obj;
}

GObject* UIObjectFactory::newObject(ObjectType type)
{
    switch (type)
    {
    case ObjectType::IMAGE:
        return GImage::create();

    case ObjectType::MOVIECLIP:
        return GMovieClip::create();

    case ObjectType::COMPONENT:
        return GComponent::create();

    case ObjectType::TEXT:
        return GBasicTextField::create();

    case ObjectType::RICHTEXT:
        return GRichTextField::create();

    case ObjectType::INPUTTEXT:
        return GTextInput::create();

    case ObjectType::GROUP:
        return GGroup::create();

    case ObjectType::LIST:
        return GList::create();

    case ObjectType::GRAPH:
        return GGraph::create();

    case ObjectType::LOADER:
        if (_loaderCreator != nullptr)
            return _loaderCreator();
        else
            return GLoader::create();

    case ObjectType::BUTTON:
        return GButton::create();

    case ObjectType::LABEL:
        return GLabel::create();

    case ObjectType::PROGRESSBAR:
        return GProgressBar::create();

    case ObjectType::SLIDER:
        return GSlider::create();

    case ObjectType::SCROLLBAR:
        return GScrollBar::create();

    case ObjectType::COMBOBOX:
        return GComboBox::create();

    case ObjectType::TREE:
        return GTree::create();

    case ObjectType::LOADER3D:
        return GLoader3D::create();

    default:
        return nullptr;
    }
}

void UIObjectFactory::setLoaderExtension(GLoaderCreator creator)
{
    _loaderCreator = creator;
}

void UIObjectFactory::resolvePackageItemExtension(PackageItem* pi)
{
    auto it = _packageItemExtensions.find(UIPackage::URL_PREFIX + pi->owner->getId() + pi->id);
    if (it != _packageItemExtensions.end())
    {
        pi->extensionCreator = it->second;
        return;
    }
    it = _packageItemExtensions.find(UIPackage::URL_PREFIX + pi->owner->getName() + "/" + pi->name);
    if (it != _packageItemExtensions.end())
    {
        pi->extensionCreator = it->second;
        return;
    }
    pi->extensionCreator = nullptr;
}

NS_FGUI_END
