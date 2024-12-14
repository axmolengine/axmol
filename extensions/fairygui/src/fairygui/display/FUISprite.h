#ifndef __FUISPRITE_H__
#define __FUISPRITE_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class FUISprite : public ax::Sprite
{
public:
    FUISprite();
    virtual ~FUISprite();

    CREATE_FUNC(FUISprite);

    void clearContent();
    void setScale9Grid(ax::Rect* value);
    void setGrayed(bool value);

    FillMethod getFillMethod() const { return _fillMethod; }
    void setFillMethod(FillMethod value);

    FillOrigin getFillOrigin() const { return _fillOrigin; }
    void setFillOrigin(FillOrigin value);

    bool isFillClockwise() const { return _fillClockwise; }
    void setFillClockwise(bool value);

    float getFillAmount() const { return _fillAmount; }
    void setFillAmount(float value);

    bool isScaleByTile() const { return _scaleByTile; }
    void setScaleByTile(bool value);

    virtual void setContentSize(const ax::Size& size) override;

protected:
    virtual void draw(ax::Renderer *renderer, const ax::Mat4 &transform, uint32_t flags) override;

    ax::Tex2F textureCoordFromAlphaPoint(ax::Vec2 alpha);
    ax::Vec3 vertexFromAlphaPoint(ax::Vec2 alpha);
    void updateBar(void);
    void updateRadial(void);
    virtual void updateColor(void) override;
    ax::Vec2 boundaryTexCoord(char index);

    void setupFill();

private:
    FillMethod _fillMethod;
    FillOrigin _fillOrigin;
    float _fillAmount;
    bool _fillClockwise;
    bool _scaleByTile;
    int _vertexDataCount;
    ax::TrianglesCommand::Triangles _fillTriangles;
    ax::V3F_T2F_C4B *_vertexData;
    unsigned short *_vertexIndex;
    
    static ax::Texture2D* _empty;
};

NS_FGUI_END

#endif
