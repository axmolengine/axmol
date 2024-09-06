#include "UIPackage.h"
#include "UIObjectFactory.h"
#include "display/BitmapFont.h"
#include "event/HitTest.h"
#include "utils/ByteBuffer.h"
#include "utils/ToolSet.h"

NS_FGUI_BEGIN
using namespace ax;

using namespace std;

const string UIPackage::URL_PREFIX = "ui://";
int UIPackage::_constructing = 0;
std::string UIPackage::_branch;
std::unordered_map<std::string, std::string> UIPackage::_vars;

const unsigned char* emptyTextureData = new unsigned char[16]{ 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

std::unordered_map<std::string, UIPackage*> UIPackage::_packageInstById;
std::unordered_map<std::string, UIPackage*> UIPackage::_packageInstByName;
std::vector<UIPackage*> UIPackage::_packageList;

Texture2D* UIPackage::_emptyTexture;

struct AtlasSprite
{
    PackageItem* atlas;
    Rect rect;
    Size originalSize;
    Vec2 offset;
    bool rotated;
};

UIPackage::UIPackage()
    : _branchIndex(-1)
{
}

UIPackage::~UIPackage()
{
    for (auto& it : _items)
        it->release();
    for (auto& it : _sprites)
        delete it.second;
}

void UIPackage::setBranch(const std::string& value)
{
    _branch = value;
    for (auto& it : _packageList)
    {
        if (it->_branches.size() > 0)
        {
            it->_branchIndex = ToolSet::findInStringArray(it->_branches, value);
        }
    }
}

const std::string& UIPackage::getVar(const std::string& key)
{
    auto it = _vars.find(key);
    if (it != _vars.end())
        return it->second;
    else
        return STD_STRING_EMPTY;
}

void UIPackage::setVar(const std::string& key, const std::string& value)
{
    _vars[key] = value;
}

UIPackage* UIPackage::getById(const string& id)
{
    auto it = _packageInstById.find(id);
    if (it != _packageInstById.end())
        return it->second;
    else
        return nullptr;
}

UIPackage* UIPackage::getByName(const string& name)
{
    auto it = _packageInstByName.find(name);
    if (it != _packageInstByName.end())
        return it->second;
    else
        return nullptr;
}

UIPackage* UIPackage::addPackage(const string& assetPath)
{
    auto it = _packageInstById.find(assetPath);
    if (it != _packageInstById.end())
        return it->second;

    if (_emptyTexture == nullptr)
    {
        Image* emptyImage = new Image();
        emptyImage->initWithRawData(emptyTextureData, 16, 2, 2, 4, false);
        _emptyTexture = new Texture2D();
        _emptyTexture->initWithImage(emptyImage);
        delete emptyImage;
    }

    Data data;

    if (FileUtils::getInstance()->getContents(assetPath + ".fui", &data) != FileUtils::Status::OK)
    {
        AXLOGE("FairyGUI: cannot load package from '{}'", assetPath);
        return nullptr;
    }

    ssize_t size;
    char* p = (char*)data.takeBuffer(&size);
    ByteBuffer buffer(p, 0, (int)size, true);

    UIPackage* pkg = new UIPackage();
    pkg->_assetPath = assetPath;
    if (!pkg->loadPackage(&buffer))
    {
        delete pkg;
        return nullptr;
    }

    _packageInstById[pkg->getId()] = pkg;
    _packageInstByName[pkg->getName()] = pkg;
    _packageInstById[assetPath] = pkg;
    _packageList.push_back(pkg);

    return pkg;
}

void UIPackage::removePackage(const string& packageIdOrName)
{
    UIPackage* pkg = UIPackage::getByName(packageIdOrName);
    if (!pkg)
        pkg = getById(packageIdOrName);

    if (pkg)
    {
        auto it = std::find(_packageList.cbegin(), _packageList.cend(), pkg);
        if (it != _packageList.cend())
            _packageList.erase(it);

        _packageInstById.erase(pkg->getId());
        _packageInstById.erase(pkg->_assetPath);
        _packageInstByName.erase(pkg->getName());

        pkg->release();
    }
    else
        AXLOGE("FairyGUI: invalid package name or id: {}", packageIdOrName);
}

void UIPackage::removeAllPackages()
{
    for (auto& it : _packageList)
        it->release();

    _packageInstById.clear();
    _packageInstByName.clear();
    _packageList.clear();
}

GObject* UIPackage::createObject(const string& pkgName, const string& resName)
{
    UIPackage* pkg = UIPackage::getByName(pkgName);
    if (pkg)
        return pkg->createObject(resName);
    else
    {
        AXLOGE("FairyGUI: package not found - {}", pkgName);
        return nullptr;
    }
}

GObject* UIPackage::createObjectFromURL(const string& url)
{
    PackageItem* pi = UIPackage::getItemByURL(url);
    if (pi)
        return pi->owner->createObject(pi);
    else
    {
        AXLOGE("FairyGUI: resource not found - {}", url);
        return nullptr;
    }
}

string UIPackage::getItemURL(const string& pkgName, const string& resName)
{
    UIPackage* pkg = UIPackage::getByName(pkgName);
    if (pkg)
    {
        PackageItem* pi = pkg->getItemByName(resName);
        if (pi)
            return URL_PREFIX + pkg->getId() + pi->id;
    }
    return STD_STRING_EMPTY;
}

PackageItem* UIPackage::getItemByURL(std::string_view url)
{
    if (url.size() == 0)
        return nullptr;

    ssize_t pos1 = url.find('/');
    if (pos1 == -1)
        return nullptr;

    ssize_t pos2 = url.find('/', pos1 + 2);
    if (pos2 == -1)
    {
        if (url.size() > 13)
        {
            std::string pkgId{url.substr(5, 8)};
            UIPackage* pkg = getById(pkgId);
            if (pkg != nullptr)
            {
                std::string srcId{url.substr(13)};
                return pkg->getItem(srcId);
            }
        }
    }
    else
    {
        std::string pkgName{url.substr(pos1 + 2, pos2 - pos1 - 2)};
        UIPackage* pkg = getByName(pkgName);
        if (pkg != nullptr)
        {
            std::string srcName{url.substr(pos2 + 1)};
            return pkg->getItemByName(srcName);
        }
    }

    return nullptr;
}

string UIPackage::normalizeURL(const string& url)
{
    if (url.size() == 0)
        return url;

    ssize_t pos1 = url.find('/');
    if (pos1 == -1)
        return url;

    ssize_t pos2 = url.find('/', pos1 + 2);
    if (pos2 == -1)
        return url;
    else
    {
        string pkgName = url.substr(pos1 + 2, pos2 - pos1 - 2);
        string srcName = url.substr(pos2 + 1);
        return getItemURL(pkgName, srcName);
    }
}

void* UIPackage::getItemAsset(const std::string& pkgName, const std::string& resName, PackageItemType type)
{
    UIPackage* pkg = UIPackage::getByName(pkgName);
    if (pkg)
    {
        PackageItem* pi = pkg->getItemByName(resName);
        if (pi)
        {
            if (type != PackageItemType::UNKNOWN && pi->type != type)
                return nullptr;
            else
                return pkg->getItemAsset(pi);
        }
    }
    return nullptr;
}

void* UIPackage::getItemAssetByURL(std::string_view url, PackageItemType type)
{
    PackageItem* pi = UIPackage::getItemByURL(url);
    if (pi)
    {
        if (type != PackageItemType::UNKNOWN && pi->type != type)
            return nullptr;
        else
            return pi->owner->getItemAsset(pi);
    }
    else
        return nullptr;
}

PackageItem* UIPackage::getItem(const string& itemId)
{
    auto it = _itemsById.find(itemId);
    if (it != _itemsById.end())
        return it->second;
    else
        return nullptr;
}

PackageItem* UIPackage::getItemByName(const string& itemName)
{
    auto it = _itemsByName.find(itemName);
    if (it != _itemsByName.end())
        return it->second;
    else
        return nullptr;
}

GObject* UIPackage::createObject(const string& resName)
{
    PackageItem* pi = getItemByName(resName);
#    if _AX_DEBUG > 0
    auto msg =  fmt::format("FairyGUI: resource not found - {} in  {}",
                                    resName, _name);
    AXASSERT(pi, msg.c_str());
#    endif
    return createObject(pi);
}

GObject* UIPackage::createObject(PackageItem* item)
{
    GObject* g = UIObjectFactory::newObject(item);
    if (g == nullptr)
        return nullptr;

    _constructing++;
    g->constructFromResource();
    _constructing--;
    return g;
}

bool UIPackage::loadPackage(ByteBuffer* buffer)
{
    if (buffer->readUint() != 0x46475549)
    {
        AXLOGE("FairyGUI: old package format found in '{}'", _assetPath);
        return false;
    }

    buffer->version = buffer->readInt();
    bool ver2 = buffer->version >= 2;
    buffer->readBool(); //compressed
    _id = buffer->readString();
    _name = buffer->readString();
    buffer->skip(20);
    int indexTablePos = buffer->getPos();
    int cnt;

    buffer->seek(indexTablePos, 4);

    cnt = buffer->readInt();
    _stringTable.resize(cnt);
    for (int i = 0; i < cnt; i++)
        _stringTable[i] = buffer->readString();
    buffer->setStringTable(&_stringTable);

    buffer->seek(indexTablePos, 0);
    cnt = buffer->readShort();
    for (int i = 0; i < cnt; i++)
    {
        std::unordered_map<std::string, std::string> info;
        info["id"] = buffer->readS();
        info["name"] = buffer->readS();

        _dependencies.push_back(info);
    }

    bool branchIncluded = false;
    if (ver2)
    {
        cnt = buffer->readShort();
        if (cnt > 0)
        {
            buffer->readSArray(_branches, cnt);
            if (_branch.size() > 0)
                _branchIndex = ToolSet::findInStringArray(_branches, _branch);
        }

        branchIncluded = cnt > 0;
    }

    buffer->seek(indexTablePos, 1);

    PackageItem* pi;
    string path = _assetPath;
    size_t pos = path.find('/');
    string shortPath = pos == -1 ? STD_STRING_EMPTY : path.substr(0, pos + 1);
    path += "_";

    cnt = buffer->readShort();
    for (int i = 0; i < cnt; i++)
    {
        int nextPos = buffer->readInt();
        nextPos += buffer->getPos();

        pi = new PackageItem();
        pi->owner = this;
        pi->type = (PackageItemType)buffer->readByte();
        pi->id = buffer->readS();
        pi->name = buffer->readS();
        buffer->skip(2); //path
        pi->file = buffer->readS();
        buffer->readBool(); //exported
        pi->width = buffer->readInt();
        pi->height = buffer->readInt();

        switch (pi->type)
        {
        case PackageItemType::IMAGE:
        {
            pi->objectType = ObjectType::IMAGE;
            int scaleOption = buffer->readByte();
            if (scaleOption == 1)
            {
                pi->scale9Grid = new Rect();
                pi->scale9Grid->origin.x = buffer->readInt();
                pi->scale9Grid->origin.y = buffer->readInt();
                pi->scale9Grid->size.width = buffer->readInt();
                pi->scale9Grid->size.height = buffer->readInt();
                pi->tileGridIndice = buffer->readInt();
            }
            else if (scaleOption == 2)
                pi->scaleByTile = true;

            buffer->readBool(); //smoothing
            break;
        }

        case PackageItemType::MOVIECLIP:
        {
            buffer->readBool(); //smoothing
            pi->objectType = ObjectType::MOVIECLIP;
            pi->rawData = buffer->readBuffer();
            break;
        }

        case PackageItemType::FONT:
        {
            pi->rawData = buffer->readBuffer();
            break;
        }

        case PackageItemType::COMPONENT:
        {
            int extension = buffer->readByte();
            if (extension > 0)
                pi->objectType = (ObjectType)extension;
            else
                pi->objectType = ObjectType::COMPONENT;
            pi->rawData = buffer->readBuffer();

            UIObjectFactory::resolvePackageItemExtension(pi);
            break;
        }

        case PackageItemType::ATLAS:
        case PackageItemType::SOUND:
        case PackageItemType::MISC:
        {
            pi->file = path + pi->file;
            break;
        }

        case PackageItemType::SPINE:
        case PackageItemType::DRAGONBONES:
        {
            pi->file = shortPath + pi->file;
            pi->skeletonAnchor = new Vec2();
            pi->skeletonAnchor->x = buffer->readFloat();
            pi->skeletonAnchor->y = buffer->readFloat();
            break;
        }

        default:
            break;
        }

        if (ver2)
        {
            std::string str = buffer->readS(); //branch
            if (!str.empty())
                pi->name = str + "/" + pi->name;

            int branchCnt = buffer->readUbyte();
            if (branchCnt > 0)
            {
                if (branchIncluded)
                {
                    pi->branches = new std::vector<std::string>();
                    buffer->readSArray(*pi->branches, branchCnt);
                }
                else
                    _itemsById[buffer->readS()] = pi;
            }

            int highResCnt = buffer->readUbyte();
            if (highResCnt > 0)
            {
                pi->highResolution = new std::vector<std::string>();
                buffer->readSArray(*pi->highResolution, highResCnt);
            }
        }

        _items.push_back(pi);
        _itemsById[pi->id] = pi;
        if (!pi->name.empty())
            _itemsByName[pi->name] = pi;

        buffer->setPos(nextPos);
    }

    buffer->seek(indexTablePos, 2);

    cnt = buffer->readShort();
    for (int i = 0; i < cnt; i++)
    {
        int nextPos = buffer->readUshort();
        nextPos += buffer->getPos();

        const string& itemId = buffer->readS();
        pi = _itemsById[buffer->readS()];

        AtlasSprite* sprite = new AtlasSprite();
        sprite->atlas = pi;
        sprite->rect.origin.x = buffer->readInt();
        sprite->rect.origin.y = buffer->readInt();
        sprite->rect.size.width = buffer->readInt();
        sprite->rect.size.height = buffer->readInt();
        sprite->rotated = buffer->readBool();
        if (ver2 && buffer->readBool())
        {
            sprite->offset.x = buffer->readInt();
            sprite->offset.y = buffer->readInt();
            sprite->originalSize.width = buffer->readInt();
            sprite->originalSize.height = buffer->readInt();
        }
        else
        {
            sprite->offset.setZero();
            sprite->originalSize.width = sprite->rect.size.width;
            sprite->originalSize.height = sprite->rect.size.height;
        }
        _sprites[itemId] = sprite;

        buffer->setPos(nextPos);
    }

    if (buffer->seek(indexTablePos, 3))
    {
        cnt = buffer->readShort();
        for (int i = 0; i < cnt; i++)
        {
            int nextPos = buffer->readInt();
            nextPos += buffer->getPos();

            auto it = _itemsById.find(buffer->readS());
            if (it != _itemsById.end())
            {
                pi = it->second;
                if (pi->type == PackageItemType::IMAGE)
                {
                    pi->pixelHitTestData = new PixelHitTestData();
                    pi->pixelHitTestData->load(buffer);
                }
            }

            buffer->setPos(nextPos);
        }
    }

    return true;
}

void* UIPackage::getItemAsset(PackageItem* item)
{
    switch (item->type)
    {
    case PackageItemType::IMAGE:
        if (item->spriteFrame == nullptr)
            loadImage(item);
        return item->spriteFrame;

    case PackageItemType::ATLAS:
        if (item->texture == nullptr)
            loadAtlas(item);
        return item->texture;

    case PackageItemType::FONT:
        if (item->bitmapFont == nullptr)
            loadFont(item);
        return item->bitmapFont;

    case PackageItemType::MOVIECLIP:
        if (item->animation == nullptr)
            loadMovieClip(item);
        return item->animation;

    default:
        return nullptr;
    }
}

void UIPackage::loadAtlas(PackageItem* item)
{
    Image* image = new Image();
#if COCOS2D_VERSION < 0x00031702
    Image::setPNGPremultipliedAlphaEnabled(false);
#endif
    if (!image->initWithImageFile(item->file))
    {
        item->texture = _emptyTexture;
        _emptyTexture->retain();
        delete image;
#if COCOS2D_VERSION < 0x00031702
        Image::setPNGPremultipliedAlphaEnabled(true);
#endif
        AXLOGW("FairyGUI: texture '{}' not found in {}", item->file, _name);
        return;
    }
#if COCOS2D_VERSION < 0x00031702
    Image::setPNGPremultipliedAlphaEnabled(true);
#endif

    Texture2D* tex = new Texture2D();
    tex->initWithImage(image);
    item->texture = tex;
    delete image;

    string alphaFilePath;
    string ext = FileUtils::getPathExtension(item->file);
    size_t pos = item->file.find_last_of('.');
    if (pos != -1)
        alphaFilePath = item->file.substr(0, pos) + "!a" + ext;
    else
        alphaFilePath = item->file + "!a" + ext;

    bool hasAlphaTexture = ToolSet::isFileExist(alphaFilePath);
    if (hasAlphaTexture)
    {
        image = new Image();
        if (!image->initWithImageFile(alphaFilePath))
        {
            delete image;
            return;
        }

#if defined(AX_VERSION)
        if(image->getFileType() == Image::Format::ETC1)
            tex->updateWithImage(image, Texture2D::getDefaultAlphaPixelFormat(), 1);
#else
        tex = new Texture2D();
        tex->initWithImage(image);
        item->texture->setAlphaTexture(tex);
        tex->release();
#endif
        delete image;
    }
}

AtlasSprite* UIPackage::getSprite(const std::string& spriteId)
{
    auto it = _sprites.find(spriteId);
    if (it != _sprites.end())
        return it->second;
    else
        return nullptr;
}

//note: SpriteFrame.ref=1 not autorelease.
SpriteFrame* UIPackage::createSpriteTexture(AtlasSprite* sprite)
{
    getItemAsset(sprite->atlas);

    //not using createWithTexture for saving a autorelease call.
    SpriteFrame* spriteFrame = new SpriteFrame();
    spriteFrame->initWithTexture(sprite->atlas->texture, sprite->rect, sprite->rotated,
        Vec2(sprite->offset.x - (sprite->originalSize.width - sprite->rect.size.width) / 2, -(sprite->offset.y - (sprite->originalSize.height - sprite->rect.size.height) / 2)),
        sprite->originalSize);

    return spriteFrame;
}

void UIPackage::loadImage(PackageItem* item)
{
    AtlasSprite* sprite = getSprite(item->id);
    if (sprite != nullptr)
        item->spriteFrame = createSpriteTexture(sprite);
    else
    {
        item->spriteFrame = new SpriteFrame();
        item->spriteFrame->initWithTexture(_emptyTexture, Rect());
    }
    if (item->scaleByTile)
    {
#if COCOS2D_VERSION >= 0x00040000
        Texture2D::TexParams tp(backend::SamplerFilter::LINEAR, backend::SamplerFilter::LINEAR,
            backend::SamplerAddressMode::REPEAT, backend::SamplerAddressMode::REPEAT);
#else
        Texture2D::TexParams tp = { GL_LINEAR, GL_LINEAR, GL_REPEAT, GL_REPEAT };
#endif
        item->spriteFrame->getTexture()->setTexParameters(tp);
}
}

void UIPackage::loadMovieClip(PackageItem* item)
{
    item->animation = Animation::create();
    item->animation->retain();

    ByteBuffer* buffer = item->rawData;

    buffer->seek(0, 0);

    float interval = buffer->readInt() / 1000.0f;
    item->swing = buffer->readBool();
    item->repeatDelay = buffer->readInt() / 1000.0f;

    buffer->seek(0, 1);

    int frameCount = buffer->readShort();
    Vector<AnimationFrame*> frames(frameCount);

    Size mcSizeInPixels = Size(item->width, item->height);
    Size mcSize = AX_SIZE_PIXELS_TO_POINTS(mcSizeInPixels);

    AtlasSprite* sprite;
    SpriteFrame* spriteFrame;

    for (int i = 0; i < frameCount; i++)
    {
        int nextPos = buffer->readUshort();
        nextPos += buffer->getPos();

        Rect rect;
        rect.origin.x = buffer->readInt();
        rect.origin.y = buffer->readInt();
        rect.size.width = buffer->readInt();
        rect.size.height = buffer->readInt();
        float addDelay = buffer->readInt() / 1000.0f;
        const string& spriteId = buffer->readS();

        if (!spriteId.empty() && (sprite = getSprite(spriteId)) != nullptr)
        {
            spriteFrame = createSpriteTexture(sprite);
            spriteFrame->setOriginalSizeInPixels(mcSizeInPixels);
            spriteFrame->setOriginalSize(mcSize);
        }
        else
        {
            //dont use createWithTexture
            spriteFrame = new SpriteFrame();
            spriteFrame->initWithTexture(_emptyTexture, Rect());
        }

        spriteFrame->setOffset(Vec2(rect.origin.x - (mcSize.width - rect.size.width) / 2, -(rect.origin.y - (mcSize.height - rect.size.height) / 2)));
        AnimationFrame* frame = AnimationFrame::create(spriteFrame, addDelay / interval + 1, ValueMapNull);
        frames.pushBack(frame);
        //tranfer to AnimationFrame
        spriteFrame->release();

        buffer->setPos(nextPos);
    }

    item->animation->initWithAnimationFrames(frames, interval, 1);
    delete buffer;
    item->rawData = nullptr;
}

void UIPackage::loadFont(PackageItem* item)
{
    item->bitmapFont = BitmapFont::create();
    auto bitmapFont  = item->bitmapFont;
#if defined(AX_VERSION)
    auto fontAtlas = bitmapFont->resetFontAtlas(bitmapFont->newFontAtlas());
#else
    auto fontAtlas = bitmapFont->resetFontAtlas(bitmapFont->createFontAtlas());
#endif
    ByteBuffer* buffer = item->rawData;

    buffer->seek(0, 0);

    bool ttf = buffer->readBool();
    item->bitmapFont->_canTint = buffer->readBool();
    item->bitmapFont->_resizable = buffer->readBool();
    buffer->readBool(); //hasChannel
    int fontSize = buffer->readInt();
    int xadvance = buffer->readInt();
    int lineHeight = buffer->readInt();

    Texture2D* mainTexture = nullptr;
    AtlasSprite* mainSprite = nullptr;

    if (ttf && (mainSprite = getSprite(item->id)) != nullptr)
        mainTexture = (Texture2D*)getItemAsset(mainSprite->atlas);

    buffer->seek(0, 1);

    FontLetterDefinition def;
    int bx = 0, by = 0;
    int bw = 0, bh = 0;
    PackageItem* charImg = nullptr;

    int cnt = buffer->readInt();
    for (int i = 0; i < cnt; i++)
    {
        int nextPos = buffer->readUshort();
        nextPos += buffer->getPos();

        memset(&def, 0, sizeof(def));

        unsigned short ch = buffer->readUshort();
        const string& img = buffer->readS();
        bx = buffer->readInt();
        by = buffer->readInt();
        def.offsetX = buffer->readInt();
        def.offsetY = buffer->readInt();
        bw = buffer->readInt();
        bh = buffer->readInt();
        def.xAdvance = buffer->readInt();
        buffer->readByte(); //channel

        if (ttf)
        {
            Rect tempRect = Rect(bx + mainSprite->rect.origin.x, by + mainSprite->rect.origin.y, bw, bh);
            tempRect = AX_RECT_PIXELS_TO_POINTS(tempRect);
            def.U = tempRect.origin.x;
            def.V = tempRect.origin.y;
            def.width = tempRect.size.width;
            def.height = tempRect.size.height;
            def.validDefinition = true;
        }
        else
        {
            charImg = getItem(img);
            if (charImg)
            {
                charImg = charImg->getBranch();
                bw = charImg->width;
                bh = charImg->height;

                AtlasSprite* sprite = getSprite(img);
                if (sprite != nullptr)
                {
                    def.offsetX += sprite->offset.x;
                    def.offsetY += sprite->offset.y;
                }

                charImg = charImg->getHighResolution();
                getItemAsset(charImg);

                Rect tempRect = charImg->spriteFrame->getRectInPixels();
                tempRect = AX_RECT_PIXELS_TO_POINTS(tempRect);
                def.U = tempRect.origin.x;
                def.V = tempRect.origin.y;
                def.width = tempRect.size.width;
                def.height = tempRect.size.height;
                if (mainTexture == nullptr)
                    mainTexture = charImg->spriteFrame->getTexture();
                def.validDefinition = true;

                if (def.xAdvance == 0)
                {
                    if (xadvance == 0)
                        def.xAdvance = def.offsetX + bw;
                    else
                        def.xAdvance = xadvance;
                }

                if (fontSize == 0)
                    fontSize = bh;
                lineHeight = MAX(fontSize, lineHeight);
            }
        }

        fontAtlas->addLetterDefinition(ch, def);
        buffer->setPos(nextPos);
    }

    if (mainTexture != nullptr)
        fontAtlas->setTexture(0, mainTexture);
    fontAtlas->setLineHeight(lineHeight);
    item->bitmapFont->_originalFontSize = fontSize;

    delete buffer;
    item->rawData = nullptr;
}

NS_FGUI_END
