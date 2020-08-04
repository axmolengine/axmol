#ifndef __GEARSIZE_H__
#define __GEARSIZE_H__

#include "FairyGUIMacros.h"
#include "GearBase.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;
class GTweener;

class GearSize : public GearBase
{
public:
    GearSize(GObject* owner);
    virtual ~GearSize();

    void apply() override;
    void updateState() override;
    void updateFromRelations(float dx, float dy) override;

protected:
    void addStatus(const std::string& pageId, ByteBuffer* buffer) override;
    void init() override;

private:
    void onTweenUpdate(GTweener* tweener);
    void onTweenComplete();

    std::unordered_map<std::string, cocos2d::Vec4> _storage;
    cocos2d::Vec4 _default;
};

NS_FGUI_END

#endif
