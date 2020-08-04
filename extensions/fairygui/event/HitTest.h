#ifndef __HITTEST_H__
#define __HITTEST_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GComponent;
class ByteBuffer;

class IHitTest
{
public:
    virtual bool hitTest(GComponent* obj, const cocos2d::Vec2& localPoint)
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

    virtual bool hitTest(GComponent* obj, const cocos2d::Vec2& localPoint) override;

    int offsetX;
    int offsetY;
    float scaleX;
    float scaleY;

private:
    PixelHitTestData* _data;
};

NS_FGUI_END

#endif