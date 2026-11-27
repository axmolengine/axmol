#ifndef __HITTEST_H__
#define __HITTEST_H__

#include "FairyGUIMacros.h"
#include "GObject.h"
#include "axmol/axmol.h"

NS_FGUI_BEGIN

class GComponent;
class ByteBuffer;

class IHitTest
{
public:
    virtual ~IHitTest() {}
    virtual bool hitTest(GComponent* obj, const ax::Vec2& localPoint)
    {
        return true;
    }
};

class PixelHitTestData
{
public:
    int pixelWidth;
    float scale;
    unsigned char* pixels;
    size_t pixelsLength;

    PixelHitTestData();
    ~PixelHitTestData();

    void load(ByteBuffer* buffer);
};

class PixelHitTest : public IHitTest
{
public:
    PixelHitTest(PixelHitTestData* data, int offsetX, int offsetY);

    virtual bool hitTest(GComponent* obj, const ax::Vec2& localPoint) override;

    int offsetX;
    int offsetY;
    float scaleX;
    float scaleY;

private:
    PixelHitTestData* _data;
};

class ChildHitArea : public IHitTest
{
public:
    explicit ChildHitArea(GObject* child) : _child(child) {}

    virtual bool hitTest(GComponent* obj, const ax::Vec2& localPoint) override;
    bool hitTestCanvas(GComponent* obj, const ax::Vec2& canvasPoint);

    GObject* getHitChild() const { return _child; }

private:
    GObject* _child = nullptr;
};

NS_FGUI_END

#endif
