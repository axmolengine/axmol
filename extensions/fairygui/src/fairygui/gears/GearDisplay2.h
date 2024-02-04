#ifndef __GEARDISPLAY2_H__
#define __GEARDISPLAY2_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearDisplay2 : public GearBase
{
public:
    GearDisplay2(GObject* owner);
    virtual ~GearDisplay2();

    void apply() override;
    void updateState() override;
    bool evaluate(bool connected);

    std::vector<std::string> pages;
    int condition;

protected:
    void addStatus(const std::string&  pageId, ByteBuffer* buffer) override;
    void init() override;

private:
    int _visible;
};

NS_FGUI_END

#endif
