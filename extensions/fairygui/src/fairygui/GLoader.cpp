#include "GLoader.h"
#include "GComponent.h"
#include "GMovieClip.h"
#include "UIPackage.h"
#include "display/FUISprite.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

GLoader::GLoader()
    : _autoSize(false),
    _align(TextHAlignment::LEFT),
    _verticalAlign(TextVAlignment::TOP),
    _fill(LoaderFillType::NONE),
    _shrinkOnly(false),
    _updatingLayout(false),
    _contentItem(nullptr),
    _contentStatus(0),
    _content(nullptr),
    _content2(nullptr),
    _playAction(nullptr),
    _playing(true),
    _frame(0)
{
}

GLoader::~GLoader()
{
    AX_SAFE_RELEASE(_playAction);
    AX_SAFE_RELEASE(_content);
    AX_SAFE_RELEASE(_content2);
}

void GLoader::handleInit()
{
    _content = FUISprite::create();
    _content->retain();
    _content->setAnchorPoint(Vec2::ZERO);
    _content->setCascadeOpacityEnabled(true);

    FUIContainer* c = FUIContainer::create();
    c->retain();
    c->gOwner = this;

    _displayObject = c;
    _displayObject->addChild(_content);
}

void GLoader::setURL(const std::string& value)
{
    if (_url.compare(value) == 0)
        return;

    _url = value;
    loadContent();
    updateGear(7);
}

void GLoader::setAlign(ax::TextHAlignment value)
{
    if (_align != value)
    {
        _align = value;
        updateLayout();
    }
}

void GLoader::setVerticalAlign(ax::TextVAlignment value)
{
    if (_verticalAlign != value)
    {
        _verticalAlign = value;
        updateLayout();
    }
}

void GLoader::setAutoSize(bool value)
{
    if (_autoSize != value)
    {
        _autoSize = value;
        updateLayout();
    }
}

void GLoader::setFill(LoaderFillType value)
{
    if (_fill != value)
    {
        _fill = value;
        updateLayout();
    }
}

void GLoader::setShrinkOnly(bool value)
{
    if (_shrinkOnly != value)
    {
        _shrinkOnly = value;
        updateLayout();
    }
}

const ax::Size & GLoader::getContentSize()
{
    return _content->getContentSize();
}

ax::Color3B GLoader::getColor() const
{
    return _content->getColor();
}

void GLoader::setColor(const ax::Color3B& value)
{
    _content->setColor(value);
}

void GLoader::setPlaying(bool value)
{
    if (_playing != value)
    {
        _playing = value;
        if (_playAction)
        {
            if (_playing)
                _content->runAction(_playAction);
            else
                _content->stopAction(_playAction);
        }
        updateGear(5);
    }
}

int GLoader::getFrame() const
{
    return _frame;
}

void GLoader::setFrame(int value)
{
    if (_frame != value)
    {
        _frame = value;
        if (_playAction)
            _playAction->setFrame(value);
        updateGear(5);
    }
}

FillMethod GLoader::getFillMethod() const
{
    return _content->getFillMethod();
}

void GLoader::setFillMethod(FillMethod value)
{
    _content->setFillMethod(value);
}

FillOrigin GLoader::getFillOrigin() const
{
    return _content->getFillOrigin();
}

void GLoader::setFillOrigin(FillOrigin value)
{
    _content->setFillOrigin(value);
}

bool GLoader::isFillClockwise() const
{
    return _content->isFillClockwise();
}

void GLoader::setFillClockwise(bool value)
{
    _content->setFillClockwise(value);
}

float GLoader::getFillAmount() const
{
    return _content->getFillAmount();
}

void GLoader::setFillAmount(float value)
{
    _content->setFillAmount(value);
}

void GLoader::loadContent()
{
    clearContent();

    if (_url.length() == 0)
        return;

    if (_url.compare(0, 5, "ui://") == 0)
        loadFromPackage();
    else
    {
        _contentStatus = 3;
        loadExternal();
    }
}

void GLoader::loadFromPackage()
{
    _contentItem = UIPackage::getItemByURL(_url);

    if (_contentItem != nullptr)
    {
        _contentItem = _contentItem->getBranch();
        sourceSize.width = _contentItem->width;
        sourceSize.height = _contentItem->height;
        _contentItem = _contentItem->getHighResolution();
        _contentItem->load();

        if (_contentItem->type == PackageItemType::IMAGE)
        {
            _contentStatus = 1;
            _content->initWithSpriteFrame(_contentItem->spriteFrame);
            if (_contentItem->scale9Grid)
                ((FUISprite*)_content)->setScale9Grid(_contentItem->scale9Grid);
            updateLayout();
        }
        else if (_contentItem->type == PackageItemType::MOVIECLIP)
        {
            _contentStatus = 2;
            if (_playAction == nullptr)
            {
                _playAction = ActionMovieClip::create(_contentItem->animation, _contentItem->repeatDelay);
                _playAction->retain();
            }
            else
                _playAction->setAnimation(_contentItem->animation, _contentItem->repeatDelay);
            if (_playing)
                _content->runAction(_playAction);
            else
                _playAction->setFrame(_frame);

            updateLayout();
        }
        else if (_contentItem->type == PackageItemType::COMPONENT)
        {
            GObject* obj = UIPackage::createObjectFromURL(_url);
            if (obj == nullptr)
                setErrorState();
            else if (dynamic_cast<GComponent*>(obj) == nullptr)
            {
                setErrorState();
            }
            else
            {
                _content2 = obj->as<GComponent>();
                _content2->retain();
                _content2->addEventListener(UIEventType::SizeChange, [this](EventContext*) {
                    if (!_updatingLayout)
                        updateLayout();
                });
                _displayObject->addChild(_content2->displayObject());
                updateLayout();
            }
        }
        else
        {
            if (_autoSize)
                setSize(_contentItem->width, _contentItem->height);

            setErrorState();
        }
    }
    else
        setErrorState();
}

void GLoader::loadExternal()
{
    auto tex = Director::getInstance()->getTextureCache()->addImage(_url);
    if (tex)
    {
        auto sf = SpriteFrame::createWithTexture(tex, Rect(Vec2::ZERO, tex->getContentSize()));
        onExternalLoadSuccess(sf);
    }
    else
        onExternalLoadFailed();
}

void GLoader::freeExternal(ax::SpriteFrame* spriteFrame)
{
}

void GLoader::onExternalLoadSuccess(ax::SpriteFrame* spriteFrame)
{
    _contentStatus = 4;
    _content->setSpriteFrame(spriteFrame);
    sourceSize = spriteFrame->getRectInPixels().size;
    updateLayout();
}

void GLoader::onExternalLoadFailed()
{
    setErrorState();
}

void GLoader::clearContent()
{
    clearErrorState();

    if (_contentStatus == 4)
        freeExternal(_content->getSpriteFrame());

    if (_contentStatus == 2)
    {
        _playAction->setAnimation(nullptr);
        _content->stopAction(_playAction);
    }

    if (_content2 != nullptr)
    {
        _displayObject->removeChild(_content2->displayObject());
        AX_SAFE_RELEASE_NULL(_content2);
    }
    ((FUISprite*)_content)->clearContent();

    _contentItem = nullptr;
    _contentStatus = 0;
}

void GLoader::updateLayout()
{
    if (_content2 == nullptr && _contentStatus == 0)
    {
        if (_autoSize)
        {
            _updatingLayout = true;
            setSize(50, 30);
            _updatingLayout = false;
        }
        return;
    }

    Size contentSize = sourceSize;

    if (_autoSize)
    {
        _updatingLayout = true;
        if (contentSize.width == 0)
            contentSize.width = 50;
        if (contentSize.height == 0)
            contentSize.height = 30;
        setSize(contentSize.width, contentSize.height);
        _updatingLayout = false;

        if (_size.equals(contentSize))
        {
            if (_content2 != nullptr)
            {
                _content2->setScale(1, 1);
                _content2->setPosition(0, -_size.height);
            }
            else
            {
                _content->setScale(1, 1);
                _content->setAnchorPoint(Vec2::ZERO);
                _content->setPosition(0, 0);
            }
            return;
        }
    }

    float sx = 1, sy = 1;
    if (_fill != LoaderFillType::NONE)
    {
        sx = _size.width / sourceSize.width;
        sy = _size.height / sourceSize.height;

        if (sx != 1 || sy != 1)
        {
            if (_fill == LoaderFillType::SCALE_MATCH_HEIGHT)
                sx = sy;
            else if (_fill == LoaderFillType::SCALE_MATCH_WIDTH)
                sy = sx;
            else if (_fill == LoaderFillType::SCALE)
            {
                if (sx > sy)
                    sx = sy;
                else
                    sy = sx;
            }
            else if (_fill == LoaderFillType::SCALE_NO_BORDER)
            {
                if (sx > sy)
                    sy = sx;
                else
                    sx = sy;
            }

            if (_shrinkOnly)
            {
                if (sx > 1)
                    sx = 1;
                if (sy > 1)
                    sy = 1;
            }
            contentSize.width = floor(sourceSize.width * sx);
            contentSize.height = floor(sourceSize.height * sy);
        }
    }

    if (_content2 != nullptr)
    {
        _content2->setScale(sx, sy);
    }
    else
    {
        if (_contentItem != nullptr)
        {
            if (_contentItem->scale9Grid)
            {
                _content->setScale(1, 1);
                _content->setContentSize(contentSize);
            }
            else if (_contentItem->scaleByTile)
            {
                _content->setScale(1, 1);
                _content->setContentSize(sourceSize);
                _content->setTextureRect(Rect(Vec2::ZERO, contentSize));
            }
            else
            {
                _content->setContentSize(sourceSize);
                _content->setScale(sx, sy);
            }
        }
        else
        {
            _content->setContentSize(sourceSize);
            _content->setScale(sx, sy);
        }
        _content->setAnchorPoint(Vec2::ZERO);
    }

    float nx;
    float ny;
    if (_align == TextHAlignment::CENTER)
        nx = floor((_size.width - contentSize.width) / 2);
    else if (_align == TextHAlignment::RIGHT)
        nx = floor(_size.width - contentSize.width);
    else
        nx = 0;

    if (_content2 != nullptr)
    {
        if (_verticalAlign == TextVAlignment::CENTER)
            ny = floor(-contentSize.height - (_size.height - contentSize.height) / 2);
        else if (_verticalAlign == TextVAlignment::BOTTOM)
            ny = -contentSize.height;
        else
            ny = -_size.height;

        _content2->setPosition(nx, ny);
    }
    else
    {
        if (_verticalAlign == TextVAlignment::CENTER)
            ny = floor((_size.height - contentSize.height) / 2);
        else if (_verticalAlign == TextVAlignment::BOTTOM)
            ny = 0;
        else
            ny = _size.height - contentSize.height;

        _content->setPosition(nx, ny);
    }
}

void GLoader::setErrorState()
{
}

void GLoader::clearErrorState()
{
}

void GLoader::handleSizeChanged()
{
    GObject::handleSizeChanged();

    if (!_updatingLayout)
        updateLayout();
}

void GLoader::handleGrayedChanged()
{
    GObject::handleGrayedChanged();

    ((FUISprite*)_content)->setGrayed(_finalGrayed);
    if (_content2 != nullptr)
        _content2->setGrayed(_finalGrayed);
}

ax::Value GLoader::getProp(ObjectPropID propId)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        return Value(ToolSet::colorToInt(getColor()));
    case ObjectPropID::Playing:
        return Value(isPlaying());
    case ObjectPropID::Frame:
        return Value(getFrame());
    case ObjectPropID::TimeScale:
        if (_playAction)
            return Value(_playAction->getTimeScale());
        else
            return Value(1);
    default:
        return GObject::getProp(propId);
    }
}

void GLoader::setProp(ObjectPropID propId, const ax::Value& value)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        setColor(ToolSet::intToColor(value.asUnsignedInt()));
        break;
    case ObjectPropID::Playing:
        setPlaying(value.asBool());
        break;
    case ObjectPropID::Frame:
        setFrame(value.asInt());
        break;
    case ObjectPropID::TimeScale:
        if (_playAction)
            _playAction->setTimeScale(value.asFloat());
        break;
    case ObjectPropID::DeltaTime:
        if (_playAction)
            _playAction->advance(value.asFloat());
        break;
    default:
        GObject::setProp(propId, value);
        break;
    }
}

void GLoader::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 5);

    _url = buffer->readS();
    _align = (TextHAlignment)buffer->readByte();
    _verticalAlign = (TextVAlignment)buffer->readByte();
    _fill = (LoaderFillType)buffer->readByte();
    _shrinkOnly = buffer->readBool();
    _autoSize = buffer->readBool();
    buffer->readBool(); //_showErrorSign
    _playing = buffer->readBool();
    _frame = buffer->readInt();

    if (buffer->readBool())
        setColor((Color3B)buffer->readColor());
    int fillMethod = buffer->readByte();
    if (fillMethod != 0)
    {
        _content->setFillMethod((FillMethod)fillMethod);
        _content->setFillOrigin((FillOrigin)buffer->readByte());
        _content->setFillClockwise(buffer->readBool());
        _content->setFillAmount(buffer->readFloat());
    }

    if (_url.length() > 0)
        loadContent();
}

GObject* GLoader::hitTest(const Vec2& worldPoint, const Camera* camera)
{
    if (!_touchable || !_displayObject->isVisible() || !_displayObject->getParent())
        return nullptr;

    if (_content2 != nullptr)
    {
        GObject* obj = _content2->hitTest(worldPoint, camera);
        if (obj != nullptr)
            return obj;
    }

    Rect rect;
    rect.size = _size;
    //if (isScreenPointInRect(worldPoint, camera, _displayObject->getWorldToNodeTransform(), rect, nullptr))
    if (rect.containsPoint(_displayObject->convertToNodeSpace(worldPoint)))
        return this;
    else
        return nullptr;
}

NS_FGUI_END