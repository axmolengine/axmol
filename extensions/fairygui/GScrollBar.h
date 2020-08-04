#ifndef __GSCROLLBAR_H__
#define __GSCROLLBAR_H__

#include "FairyGUIMacros.h"
#include "GComponent.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GScrollBar : public GComponent
{
public:
    GScrollBar();
    virtual ~GScrollBar();

    CREATE_FUNC(GScrollBar);

    void setScrollPane(ScrollPane* target, bool vertical);
    void setDisplayPerc(float value);
    void setScrollPerc(float value);
    float getMinSize();

    bool _gripDragging;

protected:
    virtual void constructExtension(ByteBuffer* buffer) override;

private:
    void onTouchBegin(EventContext* context);
    void onGripTouchBegin(EventContext* context);
    void onGripTouchMove(EventContext* context);
    void onGripTouchEnd(EventContext* context);
    void onArrowButton1Click(EventContext* context);
    void onArrowButton2Click(EventContext* context);

    GObject* _grip;
    GObject* _arrowButton1;
    GObject* _arrowButton2;
    GObject* _bar;
    ScrollPane* _target;

    bool _vertical;
    float _scrollPerc;
    bool _fixedGripSize;

    cocos2d::Vec2 _dragOffset;
};

NS_FGUI_END

#endif
