#ifndef __GRICHTEXTFIELD_H__
#define __GRICHTEXTFIELD_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"
#include "GTextField.h"
#include "display/FUIRichText.h"

NS_FGUI_BEGIN

class GRichTextField : public GTextField
{
public:
    GRichTextField();
    virtual ~GRichTextField();

    CREATE_FUNC(GRichTextField);

    virtual void setAutoSize(AutoSizeType value) override;

    virtual bool isSingleLine() const override { return false; }
    virtual void setSingleLine(bool value) override;

    virtual TextFormat* getTextFormat() const override { return _richText->getTextFormat(); }
    virtual void applyTextFormat() override;

protected:
    virtual void handleInit() override;
    virtual void handleSizeChanged() override;

    virtual void setTextFieldText() override;
    virtual void updateSize() override;

private:
    FUIRichText* _richText;
    bool _updatingSize;
};

NS_FGUI_END

#endif
