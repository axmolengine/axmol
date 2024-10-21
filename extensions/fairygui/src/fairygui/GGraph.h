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

    void drawRect(float aWidth, float aHeight, int lineSize, const ax::Color4B& lineColor, const ax::Color4B& fillColor);
    void drawEllipse(float aWidth, float aHeight, int lineSize, const ax::Color4B& lineColor, const ax::Color4B& fillColor);
    void drawPolygon(int lineSize, const ax::Color4B& lineColor, const ax::Color4B& fillColor, const ax::Vec2* points, int count);
    void drawRegularPolygon(int lineSize, const ax::Color4B& lineColor, const ax::Color4B& fillColor, int sides, float startAngle = 0, const float* distances = nullptr, int distanceCount = 0);
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
    ax::Color4B _lineColor;
    ax::Color4B _fillColor;
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
