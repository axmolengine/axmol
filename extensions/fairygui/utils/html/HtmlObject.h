#ifndef __HTMLOBJECT_H__
#define __HTMLOBJECT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObjectPool.h"

NS_FGUI_BEGIN

class FUIRichText;
class HtmlElement;
class GObject;

class HtmlObject
{
public:
    static std::string buttonResource;
    static std::string inputResource;
    static std::string selectResource;
    static bool usePool;

    static GObjectPool objectPool;
    static cocos2d::Vector<GObject*> loaderPool;

    HtmlObject();
    virtual ~HtmlObject();

    HtmlElement* getElement() const { return _element; }
    GObject* getUI() const { return _ui; }
    bool isHidden() const { return _hidden; }

    virtual void create(FUIRichText* owner, HtmlElement* element);
    virtual void destroy();

protected:
    void createCommon();
    void createImage();
    void createButton();
    void createInput();
    void createSelect();

    HtmlElement* _element;
    FUIRichText* _owner;
    GObject* _ui;
    bool _hidden;
};

NS_FGUI_END

#endif
