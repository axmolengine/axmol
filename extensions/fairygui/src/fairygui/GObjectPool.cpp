#include "GObjectPool.h"
#include "GObject.h"
#include "UIPackage.h"

NS_FGUI_BEGIN
using namespace ax;

GObjectPool::GObjectPool()
{
}

GObjectPool::~GObjectPool()
{
}

GObject* GObjectPool::getObject(const std::string & url)
{
    std::string url2 = UIPackage::normalizeURL(url);
    if (url2.length() == 0)
        return nullptr;

    GObject* ret;
    Vector<GObject*>& arr = _pool[url2];
    if (!arr.empty())
    {
        ret = arr.back();
        ret->retain();
        arr.popBack();
        ret->autorelease();
    }
    else
        ret = UIPackage::createObjectFromURL(url2);
    return ret;
}

void GObjectPool::returnObject(GObject* obj)
{
    _pool[obj->getResourceURL()].pushBack(obj);
}

NS_FGUI_END