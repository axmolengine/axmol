#ifndef __GEARTEXT_H__
#define __GEARTEXT_H__

#include "FairyGUIMacros.h"
#include "GearBase.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GObject;

class GearText : public GearBase
{
public:
    GearText(GObject* owner);
    virtual ~GearText();

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
