#ifndef __PACKAGEITEM_H__
#define __PACKAGEITEM_H__

#include "FairyGUIMacros.h"
#include "cocos2d.h"

NS_FGUI_BEGIN

class UIPackage;
class UIObjectFactory;
class GComponent;
class BitmapFont;
class PixelHitTestData;
class ByteBuffer;

class PackageItem : public ax::Object
{
public:
    PackageItem();
    virtual ~PackageItem();

    void load();
    PackageItem* getBranch();
    PackageItem* getHighResolution();

public:
    UIPackage* owner;

    PackageItemType type;
    ObjectType objectType;
    std::string id;
    std::string name;
    int width;
    int height;
    std::string file;
    ByteBuffer* rawData;
    std::vector<std::string>* branches;
    std::vector<std::string>* highResolution;

    //atlas
    ax::Texture2D* texture;

    //image
    ax::Rect* scale9Grid;
    bool scaleByTile;
    int tileGridIndice;
    ax::SpriteFrame* spriteFrame;
    PixelHitTestData* pixelHitTestData;

    //movieclip
    ax::Animation* animation;
    float delayPerUnit;
    float repeatDelay;
    bool swing;

    //component
    std::function<GComponent*()> extensionCreator;
    bool translated;

    //font
    BitmapFont* bitmapFont;

    //skeleton
    ax::Vec2* skeletonAnchor;
};

NS_FGUI_END

#endif
