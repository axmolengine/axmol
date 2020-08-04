#ifndef __GEARLOOK_H__
#define __GEARLOOK_H__

#include "FairyGUIMacros.h"
#include "GearBase.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;
class GTweener;

class GearLook : public GearBase
{
public:
    GearLook(GObject* owner);
    virtual ~GearLook();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string& pageId, ByteBuffer* buffer) override;
    void init() override;

private:
    void onTweenUpdate(GTweener* tweener);
    void onTweenComplete();

    class GearLookValue
    {
    public:
        float alpha;
        float rotation;
        bool grayed;
        bool touchable;

        GearLookValue();
        GearLookValue(float alpha, float rotation, bool grayed, bool touchable);
    };

    std::unordered_map<std::string, GearLookValue> _storage;
    GearLookValue _default;
};

NS_FGUI_END

#endif
