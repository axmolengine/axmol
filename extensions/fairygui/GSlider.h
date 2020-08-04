#ifndef __GSLIDER_H__
#define __GSLIDER_H__

#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GSlider : public GComponent
{
public:
    GSlider();
    virtual ~GSlider();

    CREATE_FUNC(GSlider);

    ProgressTitleType getTitleType() const { return _titleType; }
    void setTitleType(ProgressTitleType value);

    double getMin() const { return _min; }
    void setMin(double value);

    double getMax() const { return _max; }
    void setMax(double value);

    double getValue() const { return _value; }
    void setValue(double value);

    bool getWholeNumbers() const { return _wholeNumbers; }
    void setWholeNumbers(bool value);

    bool changeOnClick;
    bool canDrag;

protected:
    virtual void handleSizeChanged() override;
    virtual void constructExtension(ByteBuffer* buffer) override;
    virtual void setup_afterAdd(ByteBuffer* buffer, int beginPos) override;

    void update();
    void updateWithPercent(float percent, bool manual);

private:
    void onTouchBegin(EventContext* context);
    void onGripTouchBegin(EventContext* context);
    void onGripTouchMove(EventContext* context);

    double _min;
    double _max;
    double _value;
    ProgressTitleType _titleType;
    bool _reverse;
    bool _wholeNumbers;

    GObject* _titleObject;
    GObject* _barObjectH;
    GObject* _barObjectV;
    float _barMaxWidth;
    float _barMaxHeight;
    float _barMaxWidthDelta;
    float _barMaxHeightDelta;
    GObject* _gripObject;
    cocos2d::Vec2 _clickPos;
    float _clickPercent;
    float _barStartX;
    float _barStartY;
};

NS_FGUI_END

#endif
