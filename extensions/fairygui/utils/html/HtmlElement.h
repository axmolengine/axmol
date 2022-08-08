#ifndef __HTMLELEMENT_H__
#define __HTMLELEMENT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "display/TextFormat.h"

NS_FGUI_BEGIN

class HtmlObject;

class HtmlElement
{
public:
    enum class Type
    {
        TEXT,
        IMAGE,
        LINK,
        INPUT,
        SELECT,
        OBJECT
    };

    HtmlElement(Type type);
    virtual ~HtmlElement() {};

    int getInt(const std::string& attrName, int defValue = 0);
    std::string getString(const std::string& attrName, const std::string& defValue = ax::STD_STRING_EMPTY);
    ax::ValueVector& getArray(const std::string& attrName);

    Type type;
    std::string text;
    TextFormat format;
    HtmlElement* link;
    HtmlObject *obj;
    int space;
    ax::ValueMap attrs;
};

NS_FGUI_END

#endif
