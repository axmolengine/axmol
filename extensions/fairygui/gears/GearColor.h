#ifndef __GEARCOLOR_H__
#define __GEARCOLOR_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
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
        cocos2d::Color3B color;
        cocos2d::Color3B outlineColor;

        GearColorValue();
        GearColorValue(const cocos2d::Color3B& color, const cocos2d::Color3B& outlineColor);
    };

    std::unordered_map<std::string, GearColorValue> _storage;
    GearColorValue _default;
};

NS_FGUI_END

#endif
