#ifndef __GTEXTINPUT_H__
#define __GTEXTINPUT_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GTextField.h"
#include "display/FUIInput.h"

NS_FGUI_BEGIN

class GTextInput : public GTextField, ax::ui::EditBoxDelegate
{
public:
    GTextInput();
    virtual ~GTextInput();

    CREATE_FUNC(GTextInput);

    virtual bool isSingleLine() const override;
    virtual void setSingleLine(bool value) override;

    virtual TextFormat* getTextFormat() const override { return _input->getTextFormat(); }
    virtual void applyTextFormat() override;

    void setPrompt(const std::string& value);
    void setPassword(bool value);
    void setKeyboardType(int value);
    void setMaxLength(int value);
    void setRestrict(const std::string& value);

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;

    virtual void setTextFieldText() override;

    virtual void editBoxReturn(ax::ui::EditBox* editBox) override;
    virtual void editBoxTextChanged(ax::ui::EditBox* editBox, std::string_view text) override;

private:
    FUIInput* _input;
};

NS_FGUI_END

#endif
