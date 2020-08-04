#ifndef __GOBJECTPOOL_H__
#define __GOBJECTPOOL_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;

class GObjectPool
{
public:
    GObjectPool();
    ~GObjectPool();

    GObject* getObject(const std::string& url);
    void returnObject(GObject* obj);

private:
    std::unordered_map<std::string, cocos2d::Vector<GObject*>> _pool;
};

NS_FGUI_END

#endif
