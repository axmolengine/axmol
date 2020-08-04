#ifndef __GLABEL_H__
#define __GLABEL_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GComponent.h"

NS_FGUI_BEGIN

class GTextField;

class GLabel : public GComponent
{
public:
    GLabel();
    virtual ~GLabel();

    CREATE_FUNC(GLabel);

    const std::string& getTitle() { return getText(); }
    void setTitle(const std::string& value) { setText(value); };

    virtual const std::string& getText() const override;
    virtual void setText(const std::string& value) override;

    virtual const std::string& getIcon() const override;
    virtual void setIcon(const std::string& value) override;

    cocos2d::Color3B getTitleColor() const;
    void setTitleColor(const cocos2d::Color3B& value);

    int getTitleFontSize() const;
    void setTitleFontSize(int value);

    GTextField* getTextField() const;

    virtual cocos2d::Value getProp(ObjectPropID propId) override;
    virtual void setProp(ObjectPropID propId, const cocos2d::Value& value) override;

protected:
    virtual void constructExtension(ByteBuffer* buffer) override;
    virtual void setup_afterAdd(ByteBuffer* buffer, int beginPos) override;

private:
    GObject* _titleObject;
    GObject* _iconObject;
};

NS_FGUI_END

#endif
