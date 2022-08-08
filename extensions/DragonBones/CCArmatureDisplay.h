/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2012-2018 DragonBones team and other contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#ifndef DRAGONBONES_AX_ARMATURE_DISPLAY_CONTAINER_H
#define DRAGONBONES_AX_ARMATURE_DISPLAY_CONTAINER_H

#include "DragonBones/DragonBonesHeaders.h"
#include "cocos2d.h"

DRAGONBONES_NAMESPACE_BEGIN
/**
 * @inheritDoc
 */
class CCArmatureDisplay : public ax::Node, public virtual IArmatureProxy
{
    DRAGONBONES_DISALLOW_COPY_AND_ASSIGN(CCArmatureDisplay)

public:
    /**
     * @internal
     */
    static CCArmatureDisplay* create();

public:
    bool debugDraw;

protected:
    bool _debugDraw;
    Armature* _armature;
    ax::EventDispatcher* _dispatcher;

public:
    CCArmatureDisplay()
        : debugDraw(false)
        ,

        _debugDraw(false)
        , _armature(nullptr)
        , _dispatcher(nullptr)
    {
        _dispatcher = new ax::EventDispatcher();
        setEventDispatcher(_dispatcher);
        // _dispatcher->setEnabled(true);
    }
    virtual ~CCArmatureDisplay() {}

public:
    /**
     * @inheritDoc
     */
    virtual void dbInit(Armature* armature) override;
    /**
     * @inheritDoc
     */
    virtual void dbClear() override;
    /**
     * @inheritDoc
     */
    virtual void dbUpdate() override;
    /**
     * @inheritDoc
     */
    virtual void dispose(bool disposeProxy = true) override;
    /**
     * @inheritDoc
     */
    inline virtual bool hasDBEventListener(std::string_view type) const override { return _dispatcher->isEnabled(); }
    /**
     * @inheritDoc
     */
    virtual void dispatchDBEvent(std::string_view type, EventObject* value) override;
    /**
     * @inheritDoc
     */
    virtual void addDBEventListener(std::string_view type, const std::function<void(EventObject*)>& listener) override;
    /**
     * @inheritDoc
     */
    virtual void removeDBEventListener(std::string_view type,
                                       const std::function<void(EventObject*)>& listener) override;
    /**
     * @inheritDoc
     */
    inline virtual Armature* getArmature() const override { return _armature; }
    /**
     * @inheritDoc
     */
    inline virtual Animation* getAnimation() const override { return _armature->getAnimation(); }
    /**
     * @inheritDoc
     */
    virtual ax::Rect getBoundingBox() const override;
};
/**
 * @internal
 */
class DBCCSprite : public ax::Sprite
{
    DRAGONBONES_DISALLOW_COPY_AND_ASSIGN(DBCCSprite)

public:
    static DBCCSprite* create();

protected:
    DBCCSprite() {}
    virtual ~DBCCSprite() {}
    /**
     * Modify for polyInfo rect
     */
    bool _checkVisibility(const ax::Mat4& transform, const ax::Size& size, const ax::Rect& rect);

public:
    /**
     * Modify for polyInfo rect
     */
    virtual void draw(ax::Renderer* renderer, const ax::Mat4& transform, uint32_t flags) override;
    /**
     * Modify for cocos2dx 3.7, 3.8, 3.9
     */
    ax::PolygonInfo& getPolygonInfoModify();
};

DRAGONBONES_NAMESPACE_END
#endif  // DRAGONBONES_AX_ARMATURE_DISPLAY_CONTAINER_H
