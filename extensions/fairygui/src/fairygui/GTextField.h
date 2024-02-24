#ifndef __GTEXTFIELD_H__
#define __GTEXTFIELD_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GObject.h"
#include "display/FUILabel.h"

NS_FGUI_BEGIN

class GTextField : public GObject
{
public:
    GTextField();
    virtual ~GTextField();

    virtual const std::string& getText() const override { return _text; }
    void setText(const std::string& value) override;

    bool isUBBEnabled() const { return _ubbEnabled; }
    virtual void setUBBEnabled(bool value);

    AutoSizeType getAutoSize() const { return _autoSize; }
    virtual void setAutoSize(AutoSizeType value) {};

    virtual bool isSingleLine() const { return false; }
    virtual void setSingleLine(bool value) {};

    virtual TextFormat* getTextFormat() const = 0;
    virtual void applyTextFormat() = 0;

    virtual const ax::Size& getTextSize() { return _displayObject->getContentSize(); }

    ax::Color3B getColor() const { return getTextFormat()->color; }
    void setColor(const ax::Color3B& value);

    float getFontSize() const { return getTextFormat()->fontSize; }
    void setFontSize(float value);

    ax::Color3B getOutlineColor() const { return getTextFormat()->outlineColor; }
    void setOutlineColor(const ax::Color3B& value);

    ax::ValueMap* getTemplateVars() { return _templateVars; }
    void setTemplateVars(ax::ValueMap* value);

    GTextField* setVar(const std::string& name, const ax::Value& value);
    void flushVars();

    virtual ax::Value getProp(ObjectPropID propId) override;
    virtual void setProp(ObjectPropID propId, const ax::Value& value) override;

protected:
    virtual void setTextFieldText() = 0;
    virtual void updateSize();

    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;
    virtual void setup_afterAdd(ByteBuffer* buffer, int beginPos) override;

    std::string parseTemplate(const char* text);

    std::string _text;
    bool _ubbEnabled;
    AutoSizeType _autoSize;
    ax::ValueMap* _templateVars;
};

class GBasicTextField : public GTextField
{
public:
    GBasicTextField();
    virtual ~GBasicTextField();

    CREATE_FUNC(GBasicTextField);

    virtual void setAutoSize(AutoSizeType value) override;

    virtual bool isSingleLine() const override { return _label->isWrapEnabled(); }
    virtual void setSingleLine(bool value) override;

    virtual TextFormat* getTextFormat() const override { return _label->getTextFormat(); }
    virtual void applyTextFormat() override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void handleGrayedChanged() override;

    virtual void setTextFieldText() override;
    virtual void updateSize() override;

private:
    FUILabel* _label;
    bool _updatingSize;
};

NS_FGUI_END

#endif
