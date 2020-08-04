#ifndef __GEARICON_H__
#define __GEARICON_H__

#include "FairyGUIMacros.h"
#include "GearBase.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;

class GearIcon : public GearBase
{
public:
    GearIcon(GObject* owner);
    virtual ~GearIcon();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string& pageId, ByteBuffer* buffer) override;
    void init() override;

private:
    std::unordered_map<std::string, std::string> _storage;
    std::string _default;
};

NS_FGUI_END

#endif
