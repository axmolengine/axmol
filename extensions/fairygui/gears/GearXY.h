#ifndef __GEARXY_H__
#define __GEARXY_H__

#include "FairyGUIMacros.h"
#include "GearBase.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;
class GTweener;

class GearXY : public GearBase
{
public:
    GearXY(GObject* owner);
    virtual ~GearXY();

    void apply() override;
    void updateState() override;
    void updateFromRelations(float dx, float dy) override;

    bool positionsInPercent;
    void addExtStatus(const std::string& pageId, ByteBuffer* buffer);

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
