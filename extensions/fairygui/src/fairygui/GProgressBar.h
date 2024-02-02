#ifndef __GPROGRESSBAR_H__
#define __GPROGRESSBAR_H__

#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GProgressBar : public GComponent
{
public:
    GProgressBar();
    virtual ~GProgressBar();

    CREATE_FUNC(GProgressBar);

    ProgressTitleType getTitleType() const { return _titleType; }
    void setTitleType(ProgressTitleType value);

    double getMin() const { return _min; }
    void setMin(double value);

    double getMax() const { return _max; }
    void setMax(double value);

    double getValue() const { return _value; }
    void setValue(double value);

    void tweenValue(double value, float duration);
    void update(double newValue);

protected:
    virtual void handleSizeChanged() override;
    virtual void constructExtension(ByteBuffer* buffer) override;
    virtual void setup_afterAdd(ByteBuffer* buffer, int beginPos) override;

    bool setFillAmount(GObject* bar, float amount);

private:
    double _min;
    double _max;
    double _value;
    ProgressTitleType _titleType;
    bool _reverse;

    GObject* _titleObject;
    GObject* _barObjectH;
    GObject* _barObjectV;
    float _barMaxWidth;
    float _barMaxHeight;
    float _barMaxWidthDelta;
    float _barMaxHeightDelta;
    float _barStartX;
    float _barStartY;
};

NS_FGUI_END

#endif
