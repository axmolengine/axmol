/****************************************************************************
Copyright (c) 2015 fairygui.com
Copyright (c) 2019-present Simdsoft Limited.

SPDX-License-Identifier: MIT
****************************************************************************/

#include "GGraph.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

static void drawVertRect(ax::DrawNode* shape, float x, float y, float width, float height, const ax::Color& color)
{
    float mx = x + width;
    float my = y + height;
    shape->drawSolidTriangle(Vec2(x, y), Vec2(mx, y), Vec2(x, my), color, Color(), 0.0f);
    shape->drawSolidTriangle(Vec2(mx, y), Vec2(mx, my), Vec2(x, my), color, Color(), 0.0f);
}

GGraph::GGraph()
    : _shape(nullptr)
    , _type(0)
    , _lineSize(1)
    , _lineColor(ax::Color::black)
    , _fillColor(ax::Color::white)
    , _cornerRadius(nullptr)
    , _polygonPoints(nullptr)
    , _distances(nullptr)
{
    _touchDisabled = true;
}

GGraph::~GGraph()
{
    AX_SAFE_DELETE_ARRAY(_cornerRadius);
    AX_SAFE_DELETE(_polygonPoints);
    AX_SAFE_DELETE(_distances);
}

void GGraph::handleInit()
{
    _shape = DrawNode::create();
    _shape->retain();

    _displayObject = _shape;
}

void GGraph::drawRect(float aWidth, float aHeight, int lineSize, const ax::Color& lineColor, const ax::Color& fillColor)
{
    _type = 0;  // avoid updateshape call in handleSizeChange
    setSize(aWidth, aHeight);
    _type      = 1;
    _lineSize  = lineSize;
    _lineColor = lineColor;
    _fillColor = fillColor;
    updateShape();
}

void GGraph::drawEllipse(float aWidth,
                         float aHeight,
                         int lineSize,
                         const ax::Color& lineColor,
                         const ax::Color& fillColor)
{
    _type = 0;  // avoid updateshape call in handleSizeChange
    setSize(aWidth, aHeight);
    _type      = 2;
    _lineSize  = lineSize;
    _lineColor = lineColor;
    _fillColor = fillColor;
    updateShape();
}

void GGraph::drawPolygon(int lineSize,
                         const ax::Color& lineColor,
                         const ax::Color& fillColor,
                         const ax::Vec2* points,
                         int count)
{
    _type      = 3;
    _lineSize  = lineSize;
    _lineColor = lineColor;
    _fillColor = fillColor;
    if (_polygonPoints == nullptr)
        _polygonPoints = new std::vector<Vec2>();
    else
        _polygonPoints->clear();
    float h             = getHeight();
    _polygonPointOffset = h;
    for (int i = 0; i < count; i++)
    {
        Vec2 pt = *(points + i);
        pt.y    = h - pt.y;
        _polygonPoints->push_back(*(points + i));
    }
    updateShape();
}

void GGraph::drawRegularPolygon(int lineSize,
                                const ax::Color& lineColor,
                                const ax::Color& fillColor,
                                int sides,
                                float startAngle,
                                const float* distances,
                                int count)
{
    _type       = 4;
    _lineSize   = lineSize;
    _lineColor  = lineColor;
    _fillColor  = fillColor;
    _sides      = sides;
    _startAngle = startAngle;
    if (distances != nullptr)
    {
        if (_distances == nullptr)
            _distances = new std::vector<float>();
        else
            _distances->clear();
        for (int i = 0; i < count; i++)
            _distances->push_back(*(distances + i));
    }
    else if (_distances != nullptr)
        _distances->clear();
}

void GGraph::updateShape()
{
    _shape->clear();
    if (_type == 0)  // Empty
    {
        _touchDisabled = false;
        return;
    }

    const float lineSize05 = _lineSize * 0.5f;
    const float lineSize2  = _lineSize * 2.0f;

    switch (_type)
    {
    case 1:  // Rectangle
    {
        float wl  = _size.width - _lineSize;
        float hl  = _size.height - _lineSize;
        float wl2 = _size.width - lineSize05;
        float hl2 = _size.height - lineSize05;
        if (_cornerRadius)
        {
            if (_lineSize > 0)
                _shape->drawSolidCornerRect(Vec2(lineSize05, lineSize05), Vec2(wl2, hl2), _fillColor, _lineColor,
                                            lineSize2, _cornerRadius[0], _cornerRadius[1], _cornerRadius[2],
                                            _cornerRadius[3]);

            else
                _shape->drawCornerRect(Vec2(lineSize05, lineSize05), Vec2(wl2, hl2), _lineColor, lineSize2,
                                       _cornerRadius[0], _cornerRadius[1], _cornerRadius[2], _cornerRadius[3]);
        }
        else if (_lineSize > 0)
        {
            drawVertRect(_shape, 0, 0, wl, _lineSize, _lineColor);
            drawVertRect(_shape, wl, 0, _lineSize, hl, _lineColor);
            drawVertRect(_shape, _lineSize, hl, wl, _lineSize, _lineColor);
            drawVertRect(_shape, 0, _lineSize, _lineSize, hl, _lineColor);

            drawVertRect(_shape, _lineSize, _lineSize, _size.width - lineSize2, _size.height - lineSize2, _fillColor);
        }
        else
            drawVertRect(_shape, 0, 0, _size.width, _size.height, _fillColor);
        break;
    }
    case 2:  // Circle
    {
        const float cx     = _size.width * 0.5f;
        const float cy     = _size.height * 0.5f;
        const float rx     = cx - lineSize05;
        const float scaleY = (_size.width > 0.0f) ? (_size.height / _size.width) : 1.0f;

        _shape->drawSolidCircle(Vec2(cx, cy), rx, 0, AX_DRAWNODE_PRE_CALCULATING_VERTICES, 1, scaleY, _fillColor,
                                lineSize2, _lineColor);
        break;
    }
    case 3:  // Polygon
    {
        _shape->drawPolygon(_polygonPoints->data(), _polygonPoints->size(), _fillColor, lineSize2, _lineColor, false);
        break;
    }

    case 4:  // Regular Polygon
    {
        float h             = getHeight();
        _polygonPointOffset = h;
        if (_polygonPoints == nullptr)
            _polygonPoints = new std::vector<Vec2>();
        else
            _polygonPoints->clear();

        float radius     = MIN(getWidth(), h) * 0.5f - lineSize05;
        float angle      = MATH_DEG_TO_RAD(_startAngle);
        float deltaAngle = 2 * M_PI / _sides;
        float dist;
        for (int i = 0; i < _sides; i++)
        {
            if (_distances != nullptr && i < _distances->size())
            {
                dist = (*_distances)[i];
                if (dist <= 0)
                    dist = 1;
            }
            else
                dist = 1.0f;

            float xv = radius + radius * dist * cos(angle) + lineSize05;
            float yv = h - (radius + radius * dist * sin(angle)) - lineSize05;
            _polygonPoints->push_back(Vec2(xv, yv));

            angle += deltaAngle;
        }

        _shape->drawPolygon(_polygonPoints->data(), _polygonPoints->size(), _fillColor, lineSize2, _lineColor, true);
        break;
    }
    }
}

ax::Color32 GGraph::getColor() const
{
    return Color32{_fillColor};
}

void GGraph::setColor(const ax::Color32& value)
{
    _fillColor = ax::Color{value};
    updateShape();
}

ax::Value GGraph::getProp(ObjectPropID propId)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        return Value(ToolSet::colorToInt(getColor()));
    default:
        return GObject::getProp(propId);
    }
}

void GGraph::setProp(ObjectPropID propId, const ax::Value& value)
{
    switch (propId)
    {
    case ObjectPropID::Color:
        setColor(ToolSet::intToColor(value.asUnsignedInt()));
        break;
    default:
        GObject::setProp(propId, value);
        break;
    }
}

void GGraph::handleSizeChanged()
{
    GObject::handleSizeChanged();

    if (_type == 3 || _type == 4)
    {
        float h   = getHeight();
        int count = (int)_polygonPoints->size();
        for (int i = 0; i < count; i++)
        {
            Vec2 pt              = (*_polygonPoints)[i];
            pt.y                 = h - (_polygonPointOffset - pt.y);
            (*_polygonPoints)[i] = pt;
        }
        _polygonPointOffset = h;
    }

    updateShape();
}

void GGraph::setup_beforeAdd(ByteBuffer* buffer, int beginPos)
{
    GObject::setup_beforeAdd(buffer, beginPos);

    buffer->seek(beginPos, 5);

    _type = buffer->readByte();
    if (_type != 0)
    {
        _lineSize  = buffer->readInt();
        _lineColor = ax::Color{buffer->readColor()};
        _fillColor = ax::Color{buffer->readColor()};
        if (buffer->readBool())
        {
            _cornerRadius = new float[4];
            for (int i = 0; i < 4; i++)
                _cornerRadius[i] = buffer->readFloat();
        }

        if (_type == 3)
        {
            int cnt             = buffer->readShort() / 2;
            _polygonPoints      = new std::vector<Vec2>(cnt);
            float h             = getHeight();
            _polygonPointOffset = h;
            for (int i = 0; i < cnt; i++)
            {
                float f1             = buffer->readFloat();
                float f2             = h - buffer->readFloat();
                (*_polygonPoints)[i] = Vec2(f1, f2);
            }
        }
        else if (_type == 4)
        {
            _sides      = buffer->readShort();
            _startAngle = buffer->readFloat();
            int cnt     = buffer->readShort();
            if (cnt > 0)
            {
                _distances = new std::vector<float>(cnt);
                for (int i = 0; i < cnt; i++)
                    (*_distances)[i] = buffer->readFloat();
            }
        }

        updateShape();
    }
}

NS_FGUI_END
