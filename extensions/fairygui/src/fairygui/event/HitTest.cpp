#include "HitTest.h"
#include "GGraph.h"
#include "GComponent.h"
#include "GObject.h"
#include "GRoot.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

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

bool ChildHitArea::hitTestCanvas(GComponent* obj, const ax::Vec2& canvasPoint)
{
    if (_child == nullptr || obj == nullptr || !_child->displayObject())
        return false;

    Vec2 logical = _child->globalToLocal(GRoot::getInstance()->worldToRoot(canvasPoint));

    if (GGraph* graph = dynamic_cast<GGraph*>(_child))
        return graph->hitTestShape(logical);

    Rect rect;
    rect.size = _child->getSize();
    return rect.containsPoint(logical);
}

bool ChildHitArea::hitTest(GComponent* obj, const Vec2& localPoint)
{
    if (_child == nullptr || obj == nullptr || !obj->displayObject())
        return false;

    ax::Node* compDisplay = obj->displayObject();
    if (!compDisplay)
        return false;

    Vec2 canvasPoint = compDisplay->convertToWorldSpace(localPoint);
    return hitTestCanvas(obj, canvasPoint);
}


NS_FGUI_END
