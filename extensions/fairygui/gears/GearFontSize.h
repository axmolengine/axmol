#ifndef __GEARFONTSIZE_H__
#define __GEARFONTSIZE_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"
#include "GearBase.h"

NS_FGUI_BEGIN

class GObject;

class GearFontSize : public GearBase
{
public:
    GearFontSize(GObject* owner);
    virtual ~GearFontSize();

    void apply() override;
    void updateState() override;

protected:
    void addStatus(const std::string&  pageId, ByteBuffer* buffer) override;
    void init() override;

private:
    std::unordered_map<std::string, int> _storage;
    int _default;
};

NS_FGUI_END

#endif
