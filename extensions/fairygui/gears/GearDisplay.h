#ifndef __GEARDISPLAY_H__
#define __GEARDISPLAY_H__

#include "FairyGUIMacros.h"
#include "GearBase.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;

class GearDisplay : public GearBase
{
public:
    GearDisplay(GObject* owner);
    virtual ~GearDisplay();

    void apply() override;
    void updateState() override;

    uint32_t addLock();
    void releaseLock(uint32_t token);
    bool isConnected();

    std::vector<std::string> pages;

protected:
    void addStatus(const std::string& pageId, ByteBuffer* buffer) override;
    void init() override;

private:
    int _visible;
    uint32_t _displayLockToken;
};

NS_FGUI_END

#endif
