#ifndef __HTMLOBJECT_H__
#define __HTMLOBJECT_H__

#include "axmol/cocos2d.h"
#include "../../FairyGUIMacros.h"
#include "../../GObjectPool.h"

NS_FGUI_BEGIN

class FUIRichText;
class HtmlElement;
class GObject;
class GLoader;

class HtmlObjectContext
{
public:
    static HtmlObjectContext* getInstance();
    static void destroyInstance();

    GObject* acquireObject(const std::string& src, std::string_view dedicatedResourceHint);

    GLoader* acquireLoader();

    void recycleObject(GObject* obj);
    void recycleLoader(GObject* loader);

    bool isLoaderPoolEmpty() const;

    std::string buttonResource;
    std::string inputResource;
    std::string selectResource;
    bool usePool = true;

protected:
    GObjectPool objectPool;
    ax::Vector<GObject*> loaderPool;

    static HtmlObjectContext* s_htmlObjContext;

    HtmlObjectContext()  = default;
    ~HtmlObjectContext() = default;

    HtmlObjectContext(const HtmlObjectContext&)            = delete;
    HtmlObjectContext& operator=(const HtmlObjectContext&) = delete;
};

class HtmlObject
{
public:
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
