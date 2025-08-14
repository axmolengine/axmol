#ifndef __GEARCOLOR_H__
#define __GEARCOLOR_H__

#include "FairyGUIMacros.h"
#include "axmol/cocos2d.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;
class GTweener;

class GearColor : public GearBase
{
public:
    GearColor(GObject* owner);
    virtual ~GearColor();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string&  pageId, ByteBuffer* buffer) override;
    void init() override;

private:
    void onTweenUpdate(GTweener* tweener);
    void onTweenComplete();

    class GearColorValue
    {
    public:
        ax::Color32 color;
        ax::Color32 outlineColor;

        GearColorValue();
        GearColorValue(const ax::Color32& color, const ax::Color32& outlineColor);
    };

    std::unordered_map<std::string, GearColorValue> _storage;
    GearColorValue _default;
};

NS_FGUI_END

#endif
