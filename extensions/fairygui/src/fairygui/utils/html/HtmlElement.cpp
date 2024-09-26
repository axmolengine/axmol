#include "HtmlElement.h"
#include "HtmlObject.h"

NS_FGUI_BEGIN
using namespace ax;

using namespace std;

HtmlElement::HtmlElement(Type type) :
    type(type),
    link(nullptr),
    obj(nullptr),
    space(0)
{
}

int HtmlElement::getInt(const std::string& attrName, int defValue)
{
    if (attrs.empty())
        return defValue;

    auto it = attrs.find(attrName);
    if (it != attrs.end())
        return it->second.asInt();
    else
        return defValue;
}

std::string HtmlElement::getString(const std::string& attrName, const std::string& defValue)
{
    if (attrs.empty())
        return defValue;

    auto it = attrs.find(attrName);
    if (it != attrs.end())
        return it->second.asString();
    else
        return defValue;
}

ValueVector& HtmlElement::getArray(const std::string& attrName)
{
    if (!attrs.empty())
    {
        auto it = attrs.find(attrName);
        if (it != attrs.end())
            return it->second.asValueVector();
    }

    attrs[attrName] = ValueVector();
    return attrs[attrName].asValueVector();
}

NS_FGUI_END
