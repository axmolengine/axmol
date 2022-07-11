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

    void drawRect(float aWidth, float aHeight, int lineSize, const axis::Color4F& lineColor, const axis::Color4F& fillColor);
    void drawEllipse(float aWidth, float aHeight, int lineSize, const axis::Color4F& lineColor, const axis::Color4F& fillColor);
    void drawPolygon(int lineSize, const axis::Color4F& lineColor, const axis::Color4F& fillColor, const axis::Vec2* points, int count);
    void drawRegularPolygon(int lineSize, const axis::Color4F& lineColor, const axis::Color4F& fillColor, int sides, float startAngle = 0, const float* distances = nullptr, int distanceCount = 0);
    bool isEmpty() const { return _type == 0; }

    axis::Color3B getColor() const;
    void setColor(const axis::Color3B& value);

    virtual axis::Value getProp(ObjectPropID propId) override;
    virtual void setProp(ObjectPropID propId, const axis::Value& value) override;

protected:
    virtual void handleInit() override;
    virtual void setup_beforeAdd(ByteBuffer* buffer, int beginPos) override;
    virtual void handleSizeChanged() override;

private:
    void updateShape();

    int _type;
    axis::Color4F _lineColor;
    axis::Color4F _fillColor;
    int _lineSize;
    float* _cornerRadius;
    std::vector<axis::Vec2>* _polygonPoints;
    float _polygonPointOffset;
    int _sides;
    float _startAngle;
    std::vector<float>* _distances;

    axis::DrawNode* _shape;
};

NS_FGUI_END

#endif
