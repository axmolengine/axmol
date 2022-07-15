#include "PackageItem.h"
#include "GRoot.h"
#include "UIPackage.h"
#include "display/BitmapFont.h"
#include "utils/ByteBuffer.h"

NS_FGUI_BEGIN

PackageItem::PackageItem() : owner(nullptr),
                             objectType(ObjectType::COMPONENT),
                             width(0),
                             height(0),
                             rawData(nullptr),
                             texture(nullptr),
                             spriteFrame(nullptr),
                             scale9Grid(nullptr),
                             scaleByTile(false),
                             tileGridIndice(0),
                             animation(nullptr),
                             repeatDelay(0),
                             swing(false),
                             extensionCreator(nullptr),
                             translated(false),
                             bitmapFont(nullptr),
                             branches(nullptr),
                             highResolution(nullptr)
{
}

PackageItem::~PackageItem()
{
    CC_SAFE_DELETE(scale9Grid);

    CC_SAFE_DELETE(rawData);
    if (bitmapFont) //bitmapfont will be released by fontatlas
        bitmapFont->releaseAtlas();
    bitmapFont = nullptr;
    CC_SAFE_RELEASE(animation);
    CC_SAFE_RELEASE(texture);
    CC_SAFE_RELEASE(spriteFrame);

    CC_SAFE_DELETE(branches);
    CC_SAFE_DELETE(highResolution);
}

void PackageItem::load()
{
    owner->getItemAsset(this);
}

PackageItem* PackageItem::getBranch()
{
    if (branches != nullptr && owner->_branchIndex != -1)
    {
        std::string itemId = (*branches)[owner->_branchIndex];
        if (!itemId.empty())
            return owner->getItem(itemId);
    }

    return this;
}

PackageItem* PackageItem::getHighResolution()
{
    if (highResolution != nullptr && GRoot::contentScaleLevel > 0)
    {
        std::string itemId = (*highResolution)[GRoot::contentScaleLevel - 1];
        if (!itemId.empty())
            return owner->getItem(itemId);
    }

    return this;
}

NS_FGUI_END
