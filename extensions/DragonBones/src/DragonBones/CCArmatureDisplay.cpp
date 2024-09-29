#include "CCArmatureDisplay.h"
#include "CCSlot.h"

DRAGONBONES_NAMESPACE_BEGIN

CCArmatureDisplay* CCArmatureDisplay::create()
{
    CCArmatureDisplay* displayContainer = new CCArmatureDisplay();
    if (displayContainer->init())
    {
        displayContainer->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(displayContainer);
    }

    return displayContainer;
}

void CCArmatureDisplay::dbInit(Armature* armature)
{
    _armature = armature;
}

void CCArmatureDisplay::dbClear()
{
    setEventDispatcher(ax::Director::getInstance()->getEventDispatcher());

    _armature = nullptr;
    AX_SAFE_RELEASE(_dispatcher);
    release();
}

void CCArmatureDisplay::dispose(bool disposeProxy)
{
    if (_armature != nullptr)
    {
        _armature->dispose();
        _armature = nullptr;
    }
}

void CCArmatureDisplay::dbUpdate()
{
    const auto drawed = DragonBones::debugDraw;
    if (drawed || _debugDraw)
    {
        _debugDraw = drawed;
        if (_debugDraw)
        {}
        else
        {
            // TODO
        }
    }
}

void CCArmatureDisplay::addDBEventListener(std::string_view type, const std::function<void(EventObject*)>& callback)
{
    auto lambda = [callback](ax::EventCustom* event) -> void {
        callback(static_cast<EventObject*>(event->getUserData()));
    };
    _dispatcher->addCustomEventListener(type, lambda);
}

void CCArmatureDisplay::dispatchDBEvent(std::string_view type, EventObject* value)
{
    _dispatcher->dispatchCustomEvent(type, value);
}

void CCArmatureDisplay::removeDBEventListener(std::string_view type, const std::function<void(EventObject*)>& callback)
{
    // TODO
    _dispatcher->removeCustomEventListeners(type);
}

ax::Rect CCArmatureDisplay::getBoundingBox() const
{
    auto isFirst = true;
    float minX   = 0.0f;
    float minY   = 0.0f;
    float maxX   = 0.0f;
    float maxY   = 0.0f;

    for (const auto slot : _armature->getSlots())
    {
        if (!slot->getVisible() || !slot->getDisplay())
        {
            continue;
        }

        const auto display = static_cast<CCSlot*>(slot)->getCCDisplay();
        const auto bounds  = display->getBoundingBox();
        if (isFirst)
        {
            isFirst = false;
            minX    = bounds.getMinX();
            minY    = bounds.getMinY();
            maxX    = bounds.getMaxX();
            maxY    = bounds.getMaxY();
        }
        else
        {
            minX = std::min(minX, bounds.getMinX());
            minY = std::min(minY, bounds.getMinY());
            maxX = std::max(maxX, bounds.getMaxX());
            maxY = std::max(maxY, bounds.getMaxY());
        }
    }

    ax::Rect rect(minX, minY, maxX - minX, maxY - minY);

    return ax::RectApplyTransform(rect, getNodeToParentTransform());
}

DBCCSprite* DBCCSprite::create()
{
    DBCCSprite* sprite = new DBCCSprite();

    if (sprite->init())
    {
        sprite->autorelease();
    }
    else
    {
        AX_SAFE_DELETE(sprite);
    }

    return sprite;
}

bool DBCCSprite::_checkVisibility(const ax::Mat4& transform, const ax::Size& size, const ax::Rect& rect)
{
    auto scene = ax::Director::getInstance()->getRunningScene();

    // If draw to Rendertexture, return true directly.
    //  only cull the default camera. The culling algorithm is valid for default camera.
    if (!scene || (scene && scene->getDefaultCamera() != ax::Camera::getVisitingCamera()))
        return true;

    auto director = ax::Director::getInstance();
    ax::Rect visiableRect(director->getVisibleOrigin(), director->getVisibleSize());

    // transform center point to screen space
    float hSizeX = size.width / 2;
    float hSizeY = size.height / 2;

    ax::Vec3 v3p(hSizeX, hSizeY, 0);

    transform.transformPoint(&v3p);
    ax::Vec2 v2p = ax::Camera::getVisitingCamera()->projectGL(v3p);

    // convert content size to world coordinates
    float wshw = std::max(fabsf(hSizeX * transform.m[0] + hSizeY * transform.m[4]),
                          fabsf(hSizeX * transform.m[0] - hSizeY * transform.m[4]));
    float wshh = std::max(fabsf(hSizeX * transform.m[1] + hSizeY * transform.m[5]),
                          fabsf(hSizeX * transform.m[1] - hSizeY * transform.m[5]));

    // enlarge visible rect half size in screen coord
    visiableRect.origin.x -= wshw;
    visiableRect.origin.y -= wshh;
    visiableRect.size.width += wshw * 2;
    visiableRect.size.height += wshh * 2;
    bool ret = visiableRect.containsPoint(v2p);
    return ret;
}

void DBCCSprite::draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags)
{
#if AX_USE_CULLING
#    if COCOS2D_VERSION >= 0x00031400
    const auto& rect = _polyInfo.getRect();
#    else
    const auto& rect = _polyInfo.rect;
#    endif

    // Don't do calculate the culling if the transform was not updated
    auto visitingCamera = ax::Camera::getVisitingCamera();
    auto defaultCamera  = ax::Camera::getDefaultCamera();
    if (visitingCamera == defaultCamera)
    {
        _insideBounds = ((flags & FLAGS_TRANSFORM_DIRTY) || visitingCamera->isViewProjectionUpdated())
                            ? _checkVisibility(transform, _contentSize, rect)
                            : _insideBounds;
    }
    else
    {
        _insideBounds = _checkVisibility(transform, _contentSize, rect);
    }

    if (_insideBounds)
#endif
    {
#if COCOS2D_VERSION >= 0x00040000
        _trianglesCommand.init(_globalZOrder, _texture, _blendFunc, _polyInfo.triangles, transform, flags);
#else
        _trianglesCommand.init(_globalZOrder, _texture->getName(), getGLProgramState(), _blendFunc, _polyInfo.triangles,
                               transform, flags);
#endif
        renderer->addCommand(&_trianglesCommand);

#if AX_SPRITE_DEBUG_DRAW
        _debugDrawNode->clear();
        auto count   = _polyInfo.triangles.indexCount / 3;
        auto indices = _polyInfo.triangles.indices;
        auto verts   = _polyInfo.triangles.verts;
        for (ssize_t i = 0; i < count; i++)
        {
            // draw 3 lines
            auto from = verts[indices[i * 3]].vertices;
            auto to   = verts[indices[i * 3 + 1]].vertices;
            _debugDrawNode->drawLine(ax::Vec2(from.x, from.y), ax::Vec2(to.x, to.y), ax::Color4B::WHITE);

            from = verts[indices[i * 3 + 1]].vertices;
            to   = verts[indices[i * 3 + 2]].vertices;
            _debugDrawNode->drawLine(ax::Vec2(from.x, from.y), ax::Vec2(to.x, to.y), ax::Color4B::WHITE);

            from = verts[indices[i * 3 + 2]].vertices;
            to   = verts[indices[i * 3]].vertices;
            _debugDrawNode->drawLine(ax::Vec2(from.x, from.y), ax::Vec2(to.x, to.y), ax::Color4B::WHITE);
        }
#endif  // AX_SPRITE_DEBUG_DRAW
    }
}

ax::PolygonInfo& DBCCSprite::getPolygonInfoModify()
{
    return _polyInfo;
}

DRAGONBONES_NAMESPACE_END
