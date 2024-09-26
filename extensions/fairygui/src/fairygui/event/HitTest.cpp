#include "HitTest.h"
#include "GComponent.h"
#include "utils/ByteBuffer.h"

using namespace ax;
NS_FGUI_BEGIN

PixelHitTestData::PixelHitTestData() :
    pixels(nullptr),
    pixelsLength(0),
    pixelWidth(0),
    scale(1)
{
}

PixelHitTestData::~PixelHitTestData()
{
    AX_SAFE_DELETE(pixels);
}

void PixelHitTestData::load(ByteBuffer* buffer)
{
    buffer->skip(4);
    pixelWidth = buffer->readInt();
    scale = 1.0f / buffer->readByte();
    pixelsLength = buffer->readInt();
    pixels = new unsigned char[pixelsLength];
    for (size_t i = 0; i < pixelsLength; i++)
        pixels[i] = buffer->readByte();
}

PixelHitTest::PixelHitTest(PixelHitTestData * data, int offsetX, int offsetY) :
    offsetX(offsetX),
    offsetY(offsetY),
    scaleX(1),
    scaleY(1),
    _data(data)
{
}

bool PixelHitTest::hitTest(GComponent * obj, const ax::Vec2 & localPoint)
{
    int x = floor((localPoint.x / scaleX - offsetX) * _data->scale);
    int y = floor(((obj->getHeight() - localPoint.y) / scaleY - offsetY) * _data->scale);
    if (x < 0 || y < 0 || x >= _data->pixelWidth)
        return false;

    ssize_t pos = y * _data->pixelWidth + x;
    ssize_t pos2 = pos / 8;
    ssize_t pos3 = pos % 8;

    if (pos2 >= 0 && pos2 < (ssize_t)_data->pixelsLength)
        return ((_data->pixels[pos2] >> pos3) & 0x1) > 0;
    else
        return false;
}

NS_FGUI_END
