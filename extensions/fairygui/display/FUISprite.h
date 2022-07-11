#ifndef __FUISPRITE_H__
#define __FUISPRITE_H__

#include "cocos2d.h"
#include "FairyGUIMacros.h"

NS_FGUI_BEGIN

class FUISprite : public axis::Sprite
{
public:
    FUISprite();
    virtual ~FUISprite();

    CREATE_FUNC(FUISprite);

    void clearContent();
    void setScale9Grid(axis::Rect* value);
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

    virtual void setContentSize(const axis::Size& size) override;

protected:
    virtual void draw(axis::Renderer *renderer, const axis::Mat4 &transform, uint32_t flags) override;

    axis::Tex2F textureCoordFromAlphaPoint(axis::Vec2 alpha);
    axis::Vec3 vertexFromAlphaPoint(axis::Vec2 alpha);
    void updateBar(void);
    void updateRadial(void);
    virtual void updateColor(void) override;
    axis::Vec2 boundaryTexCoord(char index);

    void setupFill();

private:
    FillMethod _fillMethod;
    FillOrigin _fillOrigin;
    float _fillAmount;
    bool _fillClockwise;
    bool _scaleByTile;
    int _vertexDataCount;
    axis::TrianglesCommand::Triangles _fillTriangles;
    axis::V3F_C4B_T2F *_vertexData;
    unsigned short *_vertexIndex;
    
    static axis::Texture2D* _empty;
};

NS_FGUI_END

#endif
