#include "GProgressBar.h"
#include "GImage.h"
#include "GLoader.h"
#include "PackageItem.h"
#include "tween/GTween.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN
using namespace ax;

GProgressBar::GProgressBar()
    : _min(0),
      _max(100),
      _value(0),
      _titleType(ProgressTitleType::PERCENT),
      _titleObject(nullptr),
      _barObjectH(nullptr),
      _barObjectV(nullptr),
      _barMaxWidth(0),
      _barMaxHeight(0),
      _barMaxWidthDelta(0),
      _barMaxHeightDelta(0),
      _barStartX(0),
      _barStartY(0)
{
}

GProgressBar::~GProgressBar()
{
}

void GProgressBar::setTitleType(ProgressTitleType value)
{
    if (_titleType != value)
    {
        _titleType = value;
        update(_value);
    }
}

void GProgressBar::setMin(double value)
{
    if (_min != value)
    {
        _min = value;
        update(_value);
    }
}

void GProgressBar::setMax(double value)
{
    if (_max != value)
    {
        _max = value;
        update(_value);
    }
}

void GProgressBar::setValue(double value)
{
    if (_value != value)
    {
        GTween::kill(this, TweenPropType::Progress, false);

        _value = value;
        update(_value);
    }
}

void GProgressBar::tweenValue(double value, float duration)
{
    double oldValule;

    GTweener* tweener = GTween::getTween(this, TweenPropType::Progress);
    if (tweener != nullptr)
    {
        oldValule = tweener->value.d;
        tweener->kill(false);
    }
    else
        oldValule = _value;

    _value = value;
    GTween::toDouble(oldValule, _value, duration)
        ->setEase(EaseType::Linear)
        ->setTarget(this, TweenPropType::Progress);
}

void GProgressBar::update(double newValue)
{
    float percent;
    if (_max == _min)
        percent = 0;
    else
        percent = clampf((newValue - _min) / (_max - _min), 0, 1);

    if (_titleObject != nullptr)
    {
        std::ostringstream oss;
        switch (_titleType)
        {
        case ProgressTitleType::PERCENT:
            oss << floor(percent * 100) << "%";
            break;

        case ProgressTitleType::VALUE_MAX:
            oss << floor(newValue) << "/" << floor(_max);
            break;

        case ProgressTitleType::VALUE:
            oss << floor(newValue);
            break;

        case ProgressTitleType::MAX:
            oss << floor(_max);
            break;
        }
        _titleObject->setText(oss.str());
    }

    float fullWidth = this->getWidth() - _barMaxWidthDelta;
    float fullHeight = this->getHeight() - _barMaxHeightDelta;
    if (!_reverse)
    {
        if (_barObjectH != nullptr)
        {
            if (!setFillAmount(_barObjectH, percent))
                _barObjectH->setWidth(round(fullWidth * percent));
        }
        if (_barObjectV != nullptr)
        {
            if (!setFillAmount(_barObjectV, percent))
                _barObjectV->setHeight(round(fullHeight * percent));
        }
    }
    else
    {
        if (_barObjectH != nullptr)
        {
            if (!setFillAmount(_barObjectH, 1 - percent))
            {
                _barObjectH->setWidth(round(fullWidth * percent));
                _barObjectH->setX(_barStartX + (fullWidth - _barObjectH->getWidth()));
            }
        }
        if (_barObjectV != nullptr)
        {
            if (!setFillAmount(_barObjectV, 1 - percent))
            {
                _barObjectV->setHeight(round(fullHeight * percent));
                _barObjectV->setY(_barStartY + (fullHeight - _barObjectV->getHeight()));
            }
        }
    }
}

bool GProgressBar::setFillAmount(GObject* bar, float amount)
{
    GImage* image = nullptr;
    GLoader* loader = nullptr;

    if ((image = dynamic_cast<GImage*>(bar)) != nullptr && image->getFillMethod() != FillMethod::None)
        image->setFillAmount(amount);
    else if ((loader = dynamic_cast<GLoader*>(bar)) != nullptr && loader->getFillMethod() != FillMethod::None)
        loader->setFillAmount(amount);
    else
        return false;

    return true;
}

void GProgressBar::handleSizeChanged()
{
    GComponent::handleSizeChanged();

    if (_barObjectH != nullptr)
        _barMaxWidth = getWidth() - _barMaxWidthDelta;
    if (_barObjectV != nullptr)
        _barMaxHeight = getHeight() - _barMaxHeightDelta;

    if (!_underConstruct)
        update(_value);
}

void GProgressBar::constructExtension(ByteBuffer* buffer)
{
    buffer->seek(0, 6);

    _titleType = (ProgressTitleType)buffer->readByte();
    _reverse = buffer->readBool();

    _titleObject = getChild("title");
    _barObjectH = getChild("bar");
    _barObjectV = getChild("bar_v");

    if (_barObjectH != nullptr)
    {
        _barMaxWidth = _barObjectH->getWidth();
        _barMaxWidthDelta = getWidth() - _barMaxWidth;
        _barStartX = _barObjectH->getX();
    }
    if (_barObjectV != nullptr)
    {
        _barMaxHeight = _barObjectV->getHeight();
        _barMaxHeightDelta = getHeight() - _barMaxHeight;
        _barStartY = _barObjectV->getY();
    }
}

void GProgressBar::setup_afterAdd(ByteBuffer* buffer, int beginPos)
{
    GComponent::setup_afterAdd(buffer, beginPos);

    if (!buffer->seek(beginPos, 6))
    {
        update(_value);
        return;
    }

    if ((ObjectType)buffer->readByte() != _packageItem->objectType)
    {
        update(_value);
        return;
    }

    _value = buffer->readInt();
    _max = buffer->readInt();
    if (buffer->version >= 2)
        _min = buffer->readInt();

    update(_value);
}

NS_FGUI_END