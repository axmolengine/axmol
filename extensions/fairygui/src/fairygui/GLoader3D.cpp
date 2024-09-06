#include "GLoader3D.h"
#include "GComponent.h"
#include "GMovieClip.h"
#include "UIPackage.h"
#include "display/FUISprite.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

#include "spine/spine-cocos2dx.h"

NS_FGUI_BEGIN
using namespace ax;

GLoader3D::GLoader3D()
    : _autoSize(false),
    _align(TextHAlignment::LEFT),
    _verticalAlign(TextVAlignment::TOP),
    _fill(LoaderFillType::NONE),
    _shrinkOnly(false),
    _updatingLayout(false),
    _contentItem(nullptr),
    _content(nullptr),
    _container(nullptr),
    _playing(true),
    _frame(0),
    _loop(false),
    _color(255, 255, 255)
{
}

GLoader3D::~GLoader3D()
{
    AX_SAFE_RELEASE(_content);
    AX_SAFE_RELEASE(_container);
}

void GLoader3D::handleInit()
{
    FUIContainer* c = FUIContainer::create();
    c->retain();
    c->gOwner = this;
    _displayObject = c;

    _container = FUIContainer::create();
    _container->retain();
    _container->setAnchorPoint(Vec2::ZERO);
    _displayObject->addChild(_container);
}

void GLoader3D::setURL(const std::string& value)
{
    if (_url.compare(value) == 0)
        return;

    _url = value;
    loadContent();
    updateGear(7);
}

void GLoader3D::setAlign(ax::TextHAlignment value)
{
    if (_align != value)
    {
        _align = value;
        updateLayout();
    }
}

void GLoader3D::setVerticalAlign(ax::TextVAlignment value)
{
    if (_verticalAlign != value)
    {
        _verticalAlign = value;
        updateLayout();
    }
}

void GLoader3D::setAutoSize(bool value)
{
    if (_autoSize != value)
    {
        _autoSize = value;
        updateLayout();
    }
}

void GLoader3D::setFill(LoaderFillType value)
{
    if (_fill != value)
    {
        _fill = value;
        updateLayout();
    }
}

void GLoader3D::setShrinkOnly(bool value)
{
    if (_shrinkOnly != value)
    {
        _shrinkOnly = value;
        updateLayout();
    }
}

ax::Color3B GLoader3D::getColor() const
{
    return _color;
}

void GLoader3D::setColor(const ax::Color3B& value)
{
    _color = value;
    if (_content != nullptr)
        _content->setColor(_color);
}

void GLoader3D::setPlaying(bool value)
{
    if (_playing != value)
    {
        _playing = value;
        updateGear(5);
    }
}

int GLoader3D::getFrame() const
{
    return _frame;
}

void GLoader3D::setFrame(int value)
{
    if (_frame != value)
    {
        _frame = value;
        updateGear(5);
    }
}

void GLoader3D::setAnimationName(const std::string& value)
{
    _animationName = value;
    onChange();
}

void GLoader3D::setSkinName(const std::string& value)
{
    _skinName = value;
    onChange();
}

void GLoader3D::setLoop(bool value)
{
    _loop = value;
    onChange();
}

void GLoader3D::setContent(ax::Node* value)
{
    setURL(STD_STRING_EMPTY);

    _content = value;
    if (_content != nullptr)
    {
        _content->retain();
        _container->addChild(value);
    }
}

void GLoader3D::loadContent()
{
    clearContent();

    if (_url.length() == 0)
        return;

    if (_url.compare(0, 5, "ui://") == 0)
        loadFromPackage();
    else
        loadExternal();
}

void GLoader3D::loadFromPackage()
{
    _contentItem = UIPackage::getItemByURL(_url);

    if (_contentItem != nullptr)
    {
        _contentItem = _contentItem->getBranch();
        sourceSize.width = _contentItem->width;
        sourceSize.height = _contentItem->height;
        _contentItem = _contentItem->getHighResolution();
        _contentItem->load();

        if (_contentItem->type == PackageItemType::SPINE)
        {
            size_t pos = _contentItem->file.find_last_of('.');
            std::string atlasFile = _contentItem->file.substr(0, pos + 1).append("atlas");
            if (!ToolSet::isFileExist(atlasFile))
                atlasFile = _contentItem->file.substr(0, pos + 1).append("atlas.txt");
            spine::SkeletonAnimation* skeletonAni;
            if (FileUtils::getPathExtension(_contentItem->file) == ".skel")
                skeletonAni = spine::SkeletonAnimation::createWithBinaryFile(_contentItem->file, atlasFile);
            else
                skeletonAni = spine::SkeletonAnimation::createWithJsonFile(_contentItem->file, atlasFile);
            skeletonAni->setPosition(_contentItem->skeletonAnchor->x, _contentItem->skeletonAnchor->y);
            skeletonAni->retain();

            _content = skeletonAni;
            _container->addChild(_content);

            onChangeSpine();
            updateLayout();
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

void GLoader3D::onChange()
{
    onChangeSpine();
}

void GLoader3D::onChangeSpine()
{
    spine::SkeletonAnimation* skeletonAni = dynamic_cast<spine::SkeletonAnimation*>(_content);
    if (skeletonAni == nullptr)
        return;

#if !defined(AX_SPINE_VERSION) || AX_SPINE_VERSION >= 0x030700
    spine::AnimationState* state = skeletonAni->getState();

    spine::Animation* aniToUse = !_animationName.empty() ? skeletonAni->findAnimation(_animationName) : nullptr;
    if (aniToUse != nullptr)
    {
        spine::TrackEntry* entry = state->getCurrent(0);
        if (entry == nullptr || strcmp(entry->getAnimation()->getName().buffer(), _animationName.c_str()) != 0
            || entry->isComplete() && !entry->getLoop())
            entry = state->setAnimation(0, aniToUse, _loop);
        else
            entry->setLoop(_loop);

        if (_playing)
            entry->setTimeScale(1);
        else
        {
            entry->setTimeScale(0);
            entry->setTrackTime(MathUtil::lerp(0, entry->getAnimationEnd() - entry->getAnimationStart(), _frame / 100.0f));
        }
    }
    else
        state->clearTrack(0);

    skeletonAni->setSkin(_skinName);
#else
    auto state = skeletonAni->getState();

    auto aniToUse = !_animationName.empty() ? skeletonAni->findAnimation(_animationName) : nullptr;
    if (aniToUse != nullptr)
    {
        auto entry = state->tracks[0];
        if (entry == nullptr || strcmp(entry->animation->name, _animationName.c_str()) != 0
            || (entry->trackTime >= (entry->animationEnd - entry->animationStart)) && !entry->loop)
            entry = spAnimationState_setAnimation(state, 0, aniToUse, _loop);
        else
            entry->loop = _loop;

        if (_playing)
            entry->timeScale = 1;
        else
        {
            entry->timeScale = 0;
            entry->trackTime = MathUtil::lerp(0, entry->animationEnd - entry->animationStart, _frame / 100.0f);
        }
    }
    else
        spAnimationState_clearTrack(state, 0);

    skeletonAni->setSkin(_skinName);
#endif
}

void GLoader3D::loadExternal()
{
}

void GLoader3D::freeExternal(ax::SpriteFrame* spriteFrame)
{
}

void GLoader3D::onExternalLoadSuccess(ax::SpriteFrame* spriteFrame)
{
}

void GLoader3D::onExternalLoadFailed()
{
    setErrorState();
}

void GLoader3D::clearContent()
{
    clearErrorState();
    if (_content != nullptr)
    {
        _container->removeChild(_content);
        AX_SAFE_RELEASE_NULL(_content);
    }
    
    _contentItem = nullptr;
}

void GLoader3D::updateLayout()
{
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
            _container->setScale(1, 1);
            _container->setAnchorPoint(Vec2::ZERO);
            _container->setPosition(0, 0);
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

    _container->setScale(sx, sy);
    _container->setAnchorPoint(Vec2::ZERO);

    float nx;
    float ny;
    if (_align == TextHAlignment::CENTER)
        nx = floor((_size.width - contentSize.width) / 2);
    else if (_align == TextHAlignment::RIGHT)
        nx = floor(_size.width - contentSize.width);
    else
        nx = 0;

    if (_verticalAlign == TextVAlignment::CENTER)
        ny = floor((_size.height - contentSize.height) / 2);
    else if (_verticalAlign == TextVAlignment::BOTTOM)
        ny = 0;
    else
        ny = _size.height - contentSize.height;

    _container->setPosition(nx, ny);
}

void GLoader3D::setErrorState()
{
}

void GLoader3D::clearErrorState()
{
}

void GLoader3D::handleSizeChanged()
{
    GObject::handleSizeChanged();

    if (!_updatingLayout)
        updateLayout();
}

void GLoader3D::handleGrayedChanged()
{
    GObject::handleGrayedChanged();
}

ax::Value GLoader3D::getProp(ObjectPropID propId)
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
        return Value(1);
    default:
        return GObject::getProp(propId);
    }
}

void GLoader3D::setProp(ObjectPropID propId, const ax::Value& value)
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
        break;
    case ObjectPropID::DeltaTime:
        break;
    default:
        GObject::setProp(propId, value);
        break;
    }
}

void GLoader3D::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 5);

    _url = buffer->readS();
    _align = (TextHAlignment)buffer->readByte();
    _verticalAlign = (TextVAlignment)buffer->readByte();
    _fill = (LoaderFillType)buffer->readByte();
    _shrinkOnly = buffer->readBool();
    _autoSize = buffer->readBool();
    _animationName = buffer->readS();
    _skinName = buffer->readS();
    _playing = buffer->readBool();
    _frame = buffer->readInt();
    _loop = buffer->readBool();

    if (buffer->readBool())
        setColor((Color3B)buffer->readColor());

    if (_url.length() > 0)
        loadContent();
}

GObject* GLoader3D::hitTest(const Vec2& worldPoint, const Camera* camera)
{
    if (!_touchable || !_displayObject->isVisible() || !_displayObject->getParent())
        return nullptr;

    Rect rect;
    rect.size = _size;
    //if (isScreenPointInRect(worldPoint, camera, _displayObject->getWorldToNodeTransform(), rect, nullptr))
    if (rect.containsPoint(_displayObject->convertToNodeSpace(worldPoint)))
        return this;
    else
        return nullptr;
}

NS_FGUI_END