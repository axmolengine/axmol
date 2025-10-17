/****************************************************************************
Copyright (c) 2015 fairygui.com
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

The MIT License (MIT)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
****************************************************************************/

#ifndef __GGRAPH_H__
#define __GGRAPH_H__

#include "FairyGUIMacros.h"
#include "GObject.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class GGraph : public GObject
{
public:
    GGraph();
    virtual ~GGraph();

    CREATE_FUNC(GGraph);

    void drawRect(float aWidth, float aHeight, int lineSize, const ax::Color4F& lineColor, const ax::Color4F& fillColor);
    void drawEllipse(float aWidth, float aHeight, int lineSize, const ax::Color4F& lineColor, const ax::Color4F& fillColor);
    void drawPolygon(int lineSize, const ax::Color4F& lineColor, const ax::Color4F& fillColor, const ax::Vec2* points, int count);
    void drawRegularPolygon(int lineSize, const ax::Color4F& lineColor, const ax::Color4F& fillColor, int sides, float startAngle = 0, const float* distances = nullptr, int distanceCount = 0);
    bool isEmpty() const { return _type == 0; }

    ax::Color3B getColor() const;
    void setColor(const ax::Color3B& value);

    virtual ax::Value getProp(ObjectPropID propId) override;
    virtual void setProp(ObjectPropID propId, const ax::Value& value) override;

protected:
    virtual void handleInit() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;
    virtual void handleSizeChanged() override;

private:
    void updateShape();

    int _type;
    ax::Color4F _lineColor;
    ax::Color4F _fillColor;
    int _lineSize;
    float* _cornerRadius;
    std::vector<ax::Vec2>* _polygonPoints;
    float _polygonPointOffset;
    int _sides;
    float _startAngle;
    std::vector<float>* _distances;

    ax::DrawNode* _shape;
};

NS_FGUI_END

#endif
