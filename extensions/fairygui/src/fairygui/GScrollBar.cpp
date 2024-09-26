#include "GScrollBar.h"
#include "PackageItem.h"
#include "ScrollPane.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GScrollBar::GScrollBar()
    : _grip(nullptr),
      _arrowButton1(nullptr),
      _arrowButton2(nullptr),
      _bar(nullptr),
      _target(nullptr),
      _vertical(false),
      _scrollPerc(0),
      _fixedGripSize(false),
      _gripDragging(false)
{
}

GScrollBar::~GScrollBar()
{
}

void GScrollBar::setScrollPane(ScrollPane* target, bool vertical)
{
    _target = target;
    _vertical = vertical;
}

void GScrollBar::setDisplayPerc(float value)
{
    if (_vertical)
    {
        if (!_fixedGripSize)
            _grip->setHeight(floor(value * _bar->getHeight()));
        _grip->setY(round(_bar->getY() + (_bar->getHeight() - _grip->getHeight()) * _scrollPerc));
    }
    else
    {
        if (!_fixedGripSize)
            _grip->setWidth(floor(value * _bar->getWidth()));
        _grip->setX(round(_bar->getX() + (_bar->getWidth() - _grip->getWidth()) * _scrollPerc));
    }

    _grip->setVisible(value != 0 && value != 1);
}

void GScrollBar::setScrollPerc(float value)
{
    _scrollPerc = value;
    if (_vertical)
        _grip->setY(round(_bar->getY() + (_bar->getHeight() - _grip->getHeight()) * _scrollPerc));
    else
        _grip->setX(round(_bar->getX() + (_bar->getWidth() - _grip->getWidth()) * _scrollPerc));
}

float GScrollBar::getMinSize()
{
    if (_vertical)
        return (_arrowButton1 != nullptr ? _arrowButton1->getHeight() : 0) + (_arrowButton2 != nullptr ? _arrowButton2->getHeight() : 0);
    else
        return (_arrowButton1 != nullptr ? _arrowButton1->getWidth() : 0) + (_arrowButton2 != nullptr ? _arrowButton2->getWidth() : 0);
}

void GScrollBar::constructExtension(ByteBuffer* buffer)
{
    buffer->seek(0, 6);

    _fixedGripSize = buffer->readBool();

    _grip = getChild("grip");
    AXASSERT(_grip != nullptr, "FairyGUI: should define grip");
    _bar = getChild("bar");
    AXASSERT(_bar != nullptr, "FairyGUI: should define bar");

    _arrowButton1 = getChild("arrow1");
    _arrowButton2 = getChild("arrow2");

    _grip->addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(GScrollBar::onGripTouchBegin, this));
    _grip->addEventListener(UIEventType::TouchMove, AX_CALLBACK_1(GScrollBar::onGripTouchMove, this));
    _grip->addEventListener(UIEventType::TouchEnd, AX_CALLBACK_1(GScrollBar::onGripTouchEnd, this));

    this->addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(GScrollBar::onTouchBegin, this));

    if (_arrowButton1 != nullptr)
        _arrowButton1->addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(GScrollBar::onArrowButton1Click, this));
    if (_arrowButton2 != nullptr)
        _arrowButton2->addEventListener(UIEventType::TouchBegin, AX_CALLBACK_1(GScrollBar::onArrowButton2Click, this));
}

void GScrollBar::onTouchBegin(EventContext* context)
{
    context->stopPropagation();

    InputEvent* evt = context->getInput();
    Vec2 pt = _grip->globalToLocal(evt->getPosition());
    if (_vertical)
    {
        if (pt.y < 0)
            _target->scrollUp(4, false);
        else
            _target->scrollDown(4, false);
    }
    else
    {
        if (pt.x < 0)
            _target->scrollLeft(4, false);
        else
            _target->scrollRight(4, false);
    }
}

void GScrollBar::onGripTouchBegin(EventContext* context)
{
    if (_bar == nullptr)
        return;

    context->stopPropagation();
    context->captureTouch();

    _gripDragging = true;
    _target->updateScrollBarVisible();

    _dragOffset = globalToLocal(context->getInput()->getPosition()) - _grip->getPosition();
}

void GScrollBar::onGripTouchMove(EventContext* context)
{
    Vec2 pt = globalToLocal(context->getInput()->getPosition());

    if (_vertical)
    {
        float curY = pt.y - _dragOffset.y;
        float diff = _bar->getHeight() - _grip->getHeight();
        if (diff == 0)
            _target->setPercY(0);
        else
            _target->setPercY((curY - _bar->getY()) / diff);
    }
    else
    {
        float curX = pt.x - _dragOffset.x;
        float diff = _bar->getWidth() - _grip->getWidth();
        if (diff == 0)
            _target->setPercX(0);
        else
            _target->setPercX((curX - _bar->getX()) / diff);
    }
}

void GScrollBar::onGripTouchEnd(EventContext* context)
{
    _gripDragging = false;
    _target->updateScrollBarVisible();
}

void GScrollBar::onArrowButton1Click(EventContext* context)
{
    context->stopPropagation();

    if (_vertical)
        _target->scrollUp();
    else
        _target->scrollLeft();
}

void GScrollBar::onArrowButton2Click(EventContext* context)
{
    context->stopPropagation();

    if (_vertical)
        _target->scrollDown();
    else
        _target->scrollRight();
}

NS_FGUI_END
