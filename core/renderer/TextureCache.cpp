/****************************************************************************
Copyright (c) 2008-2010 Ricardo Quesada
Copyright (c) 2010-2012 cocos2d-x.org
Copyright (c) 2011      Zynga Inc.
Copyright (c) 2013-2016 Chukong Technologies Inc.
Copyright (c) 2017-2018 Xiamen Yaji Software Co., Ltd.
Copyright (c) 2019-present Axmol Engine contributors (see AUTHORS.md).

https://axmol.dev/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "renderer/TextureCache.h"

#include <errno.h>
#include <stack>
#include <cctype>
#include <list>

#include "renderer/Texture2D.h"
#include "base/Macros.h"
#include "base/UTF8.h"
#include "base/Director.h"
#include "base/Scheduler.h"
#include "platform/FileUtils.h"
#include "base/Utils.h"
#include "base/NinePatchImageParser.h"
#include "renderer/backend/DriverBase.h"

using namespace std;

namespace ax
{

std::string TextureCache::s_etc1AlphaFileSuffix = "@alpha";

// implementation TextureCache

void TextureCache::setETC1AlphaFileSuffix(std::string_view suffix)
{
    s_etc1AlphaFileSuffix = suffix;
}

std::string TextureCache::getETC1AlphaFileSuffix()
{
    return s_etc1AlphaFileSuffix;
}

TextureCache::TextureCache() : _needQuit(false) {}

TextureCache::~TextureCache()
{
    AXLOGD("deallocing TextureCache: {}", fmt::ptr(this));

    for (auto&& texture : _textures)
        texture.second->release();
}

std::string TextureCache::getDescription() const
{
    return fmt::format("<TextureCache | Number of textures = {}>", static_cast<int>(_textures.size()));
}

struct TextureCache::LoadRequest
{
public:
    LoadRequest(std::string fn, const std::function<void(Texture2D*)>& f, std::string_view key)
        : filename(std::move(fn))
        , callback(f)
        , callbackKey(std::move(key))
        , pixelFormat(Texture2D::getDefaultAlphaPixelFormat())
        , loadSuccess(false)
    {}

    std::string filename;
    std::function<void(Texture2D*)> callback;
    std::string callbackKey;
    Image image;
    Image imageAlpha;
    backend::PixelFormat pixelFormat;
    bool loadSuccess;
};

/**
 The addImageAsync logic follow the steps:
 - find the image has been add or not, if not add an LoadRequest to _requestQueue  (GL thread)
 - get LoadRequest from _requestQueue, load res and fill image data to LoadRequest.image, then add LoadRequest to
 _responseQueue (Load thread)
 - on schedule callback, get LoadRequest from _responseQueue, convert image to texture, then delete LoadRequest (GL
 thread)

 the Critical Area include these members:
 - _requestQueue: locked by _requestMutex
 - _responseQueue: locked by _responseMutex

 the object's life time:
 - LoadRequest: construct and destruct in GL thread
 - image data: new in Load thread, delete in GL thread(by Image instance)

 Note:
 - all LoadRequest referenced in _loadRequests, for unbind function use.

 How to deal add image many times?
 - At first, this situation is abnormal, we only ensure the logic is correct.
 - If the image has been loaded, the after load image call will return immediately.
 - If the image request is in queue already, there will be more than one request in queue,
 - In addImageAsyncCallback, will deduplicate the request to ensure only create one texture.

 Does process all response in addImageAsyncCallback consume more time?
 - Convert image to texture faster than load image from disk, so this isn't a
 problem.

 Call unbindImageAsync(path) to prevent the call to the callback when the
 texture is loaded.
 */
void TextureCache::addImageAsync(std::string_view path, const std::function<void(Texture2D*)>& callback)
{
    addImageAsync(path, callback, path);
}

/**
 The addImageAsync logic follow the steps:
 - find the image has been add or not, if not add an LoadRequest to _requestQueue  (GL thread)
 - get LoadRequest from _requestQueue, load res and fill image data to LoadRequest.image, then add LoadRequest to
 _responseQueue (Load thread)
 - on schedule callback, get LoadRequest from _responseQueue, convert image to texture, then delete LoadRequest (GL
 thread)

 the Critical Area include these members:
 - _requestQueue: locked by _requestMutex
 - _responseQueue: locked by _responseMutex

 the object's life time:
 - LoadRequest: construct and destruct in GL thread
 - image data: new in Load thread, delete in GL thread(by Image instance)

 Note:
 - all LoadRequest referenced in _loadRequests, for unbind function use.

 How to deal add image many times?
 - At first, this situation is abnormal, we only ensure the logic is correct.
 - If the image has been loaded, the after load image call will return immediately.
 - If the image request is in queue already, there will be more than one request in queue,
 - In addImageAsyncCallback, will deduplicate the request to ensure only create one texture.

 Does process all response in addImageAsyncCallback consume more time?
 - Convert image to texture faster than load image from disk, so this isn't a
 problem.

 The callbackKey allows to unbind the callback in cases where the loading of
 path is requested by several sources simultaneously. Each source can then
 unbind the callback independently as needed whilst a call to
 unbindImageAsync(path) would be ambiguous.
 */
void TextureCache::addImageAsync(std::string_view path,
                                 const std::function<void(Texture2D*)>& callback,
                                 std::string_view callbackKey)
{
    Texture2D* texture = nullptr;

    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(path);

    auto it = _textures.find(fullpath);
    if (it != _textures.end())
        texture = it->second;

    if (texture != nullptr)
    {
        if (callback)
            callback(texture);
        return;
    }

    // check if file exists
    if (fullpath.empty() || !FileUtils::getInstance()->isFileExist(fullpath))
    {
        if (callback)
            callback(nullptr);
        return;
    }

    // lazy init
    if (_loadingThreads.empty())
    {
        // create a new thread to load images
        _needQuit = false;

        const size_t threadCount = std::max<size_t>(1, std::thread::hardware_concurrency());
        _loadingThreads.reserve(threadCount);

        for (size_t i = 0; i != threadCount; ++i)
            _loadingThreads.emplace_back(&TextureCache::loadImage, this);
    }

    if (_loadRequests.empty())
    {
        Director::getInstance()->getScheduler()->schedule(AX_SCHEDULE_SELECTOR(TextureCache::addImageAsyncCallBack),
                                                          this, 0, false);
    }

    std::unique_ptr<LoadRequest> data(new LoadRequest(std::move(fullpath), callback, callbackKey));

    _loadRequests.emplace_back(data.get());

    std::unique_lock<std::mutex> ul(_pendingMutex);
    _pendingRequests.emplace_back(std::move(data));
    _sleepCondition.notify_one();
}

void TextureCache::unbindImageAsync(std::string_view callbackKey)
{
    for (LoadRequest* const request : _loadRequests)
        if (request->callbackKey == callbackKey)
            request->callback = nullptr;
}

void TextureCache::unbindAllImageAsync()
{
    for (LoadRequest* const request : _loadRequests)
        request->callback = nullptr;
}

void TextureCache::loadImage()
{
    while (true)
    {
        std::unique_ptr<LoadRequest> request;

        {
            std::unique_lock<std::mutex> ul(_pendingMutex);

            if (!_pendingRequests.empty())
            {
                request = std::move(_pendingRequests.back());
                _pendingRequests.pop_back();
            }

            if (!request)
            {
                if (_needQuit)
                    break;

                _sleepCondition.wait(ul);
                continue;
            }
        }

        // load image
        request->loadSuccess = request->image.initWithImageFileThreadSafe(request->filename);

        // ETC1 ALPHA supports.
        if (request->loadSuccess && request->image.getFileType() == Image::Format::ETC1 &&
            !s_etc1AlphaFileSuffix.empty())
        {  // check whether alpha texture exists & load it
            auto alphaFile = request->filename + s_etc1AlphaFileSuffix;
            if (FileUtils::getInstance()->isFileExist(alphaFile))
                request->imageAlpha.initWithImageFileThreadSafe(alphaFile);
        }

        const std::unique_lock lock(_doneMutex);
        _doneRequests.emplace_back(std::move(request));
    }
}

void TextureCache::addImageAsyncCallBack(float /*dt*/)
{
    while (true)
    {
        std::unique_ptr<LoadRequest> request;
        {
            std::unique_lock<std::mutex> lock(_doneMutex);

            if (!_doneRequests.empty())
            {
                request = std::move(_doneRequests.back());
                _doneRequests.pop_back();
            }
        }

        if (!request)
            break;

        _loadRequests.erase(std::find(_loadRequests.begin(), _loadRequests.end(), request.get()));

        Texture2D* texture;

        // check the image has been convert to texture or not
        const auto it = _textures.find(request->filename);
        if (it != _textures.end())
        {
            texture = it->second;
        }
        else
        {
            // convert image to texture
            if (request->loadSuccess)
            {
                Image* image = &(request->image);
                // generate texture in render thread
                texture = new Texture2D();
                texture->retain();
                texture->autorelease();

                texture->initWithImage(image, request->pixelFormat);
                // parse 9-patch info
                parseNinePatchImage(image, texture, request->filename);
#if AX_ENABLE_CACHE_TEXTURE_DATA
                // cache the texture file name
                VolatileTextureMgr::addImageTexture(texture, request->filename);
#endif
                // cache the texture. retain it, since it is added in the map
                _textures.emplace(request->filename, texture);
                // ETC1 ALPHA supports.
                if (request->imageAlpha.getFileType() == Image::Format::ETC1)
                {
                    texture->updateWithImage(&request->imageAlpha, request->pixelFormat, 1);
                }
            }
            else
            {
                texture = nullptr;
                AXLOGW("axmol: failed to call TextureCache::addImageAsync({})", request->filename);
            }
        }

        // call callback function
        if (request->callback)
        {
            (request->callback)(texture);
        }
    }

    if (_loadRequests.empty())
    {
        Director::getInstance()->getScheduler()->unschedule(AX_SCHEDULE_SELECTOR(TextureCache::addImageAsyncCallBack),
                                                            this);
    }
}

Texture2D* TextureCache::getWhiteTexture()
{
    constexpr std::string_view key = "/white-texture"sv;
    return getWhiteTexture(key, 0xFF);
}

Texture2D* TextureCache::getWhiteTexture(std::string_view key, uint8_t luma)
{
    // Gets the texture by key firstly.
    auto texture = this->getTextureForKey(key);
    if (texture)
        return texture;

    // If texture wasn't in cache, create it from RAW data.
    unsigned char texls[] = {
        // RGBA8888
        luma, luma, luma, 0xFF, luma, luma, luma, 0xFF, luma, luma, luma, 0xFF, luma, luma, luma, 0xFF};

    Image* image        = new Image();  // Notes: andorid: VolatileTextureMgr traits image as dynmaic object
    bool AX_UNUSED isOK = image->initWithRawData(texls, sizeof(texls), 2, 2, 8);
    AXASSERT(isOK, "The 2x2 empty texture was created unsuccessfully.");

    texture = this->addImage(image, key);
    image->release();
    return texture;
}

Texture2D* TextureCache::getDummyTexture()
{
    constexpr std::string_view key = "/dummy-texture"sv;
    // Gets the texture by key firstly.
    auto texture = this->getTextureForKey(key);
    if (texture)
        return texture;

    // If texture wasn't in cache, create it from RAW data.
#ifdef NDEBUG
    unsigned char texls[] = {0, 0, 0, 0};  // 1*1 transparent picture
#else
    unsigned char texls[] = {255, 0, 0, 255};  // 1*1 red picture
#endif
    Image* image        = new Image();  // Notes: andorid: VolatileTextureMgr traits image as dynmaic object
    bool AX_UNUSED isOK = image->initWithRawData(texls, sizeof(texls), 1, 1, sizeof(unsigned char));
    texture             = this->addImage(image, key);
    image->release();
    return texture;
}

Texture2D* TextureCache::addImage(std::string_view path)
{
    return addImage(path, Texture2D::getDefaultAlphaPixelFormat());
}

Texture2D* TextureCache::addImage(std::string_view path, PixelFormat format)
{
    Texture2D* texture = nullptr;
    Image* image       = nullptr;
    // Split up directory and filename
    // MUTEX:
    // Needed since addImageAsync calls this method from a different thread

    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(path);
    if (fullpath.empty())
    {
        return nullptr;
    }
    auto it = _textures.find(fullpath);
    if (it != _textures.end())
        texture = it->second;

    if (!texture)
    {
        // all images are handled by UIImage except PVR extension that is handled by our own handler
        do
        {
            image = new Image();

            bool bRet = image->initWithImageFile(fullpath);
            AX_BREAK_IF(!bRet);

            texture = new Texture2D();

            if (texture->initWithImage(image, format))
            {
#if AX_ENABLE_CACHE_TEXTURE_DATA
                // cache the texture file name
                VolatileTextureMgr::addImageTexture(texture, fullpath);
#endif
                // texture already retained, no need to re-retain it
                _textures.emplace(fullpath, texture);

                //-- ANDROID ETC1 ALPHA SUPPORTS.
                std::string alphaFullPath{path};
                alphaFullPath += s_etc1AlphaFileSuffix;
                if (image->getFileType() == Image::Format::ETC1 && !s_etc1AlphaFileSuffix.empty() &&
                    FileUtils::getInstance()->isFileExist(alphaFullPath))
                {
                    Image imageAlpha;
                    if (imageAlpha.initWithImageFile(alphaFullPath))
                    {
                        texture->updateWithImage(&imageAlpha, format, 1);
                    }
                }

                // parse 9-patch info
                this->parseNinePatchImage(image, texture, path);
            }
            else
            {
                AXLOGW("Couldn't create texture for file:{} in TextureCache", path);
                AX_SAFE_RELEASE(texture);
                texture = nullptr;
            }
        } while (0);
    }

    AX_SAFE_RELEASE(image);

    return texture;
}

void TextureCache::parseNinePatchImage(ax::Image* image, ax::Texture2D* texture, std::string_view path)
{
    if (NinePatchImageParser::isNinePatchImage(path))
    {
        Rect frameRect = Rect(0, 0, image->getWidth(), image->getHeight());
        NinePatchImageParser parser(image, frameRect, false);
        texture->addSpriteFrameCapInset(nullptr, parser.parseCapInset());
    }
}

Texture2D* TextureCache::addImage(Image* image, std::string_view key)
{
    return addImage(image, key, Texture2D::getDefaultAlphaPixelFormat());
}

Texture2D* TextureCache::addImage(Image* image, std::string_view key, PixelFormat format)
{
    AXASSERT(image != nullptr, "TextureCache: image MUST not be nil");
    AXASSERT(image->getData() != nullptr, "TextureCache: image MUST not be nil");

    Texture2D* texture = nullptr;

    do
    {
        auto it = _textures.find(key);
        if (it != _textures.end())
        {
            texture = it->second;
            return texture;
        }

        texture = new Texture2D();
        if (texture->initWithImage(image, format))
        {
            _textures.emplace(key, texture);
        }
        else
        {
            AX_SAFE_RELEASE(texture);
            texture = nullptr;
            AXLOGD("initWithImage failed!");
        }

    } while (0);

#if AX_ENABLE_CACHE_TEXTURE_DATA
    VolatileTextureMgr::addImage(texture, image);
#endif

    return texture;
}

Texture2D* TextureCache::addImage(const Data& imageData, std::string_view key)
{
    AXASSERT(!imageData.isNull() && !key.empty(), "TextureCache: imageData MUST not be empty and key not empty");

    Texture2D* texture = nullptr;

    do
    {
        auto it = _textures.find(key);
        if (it != _textures.end())
        {
            texture = it->second;
            break;
        }

        Image* image = new Image();
        AX_BREAK_IF(nullptr == image);

        bool bRet = image->initWithImageData(imageData.getBytes(), imageData.getSize());
        AX_BREAK_IF(!bRet);

        texture = new Texture2D();

        if (texture)
        {
            if (texture->initWithImage(image))
            {

#if AX_ENABLE_CACHE_TEXTURE_DATA
                VolatileTextureMgr::addImage(texture, image);
#endif
                _textures.emplace(key, texture);
            }
            else
            {
                AX_SAFE_RELEASE(texture);
                texture = nullptr;
                AXLOGW("initWithImage failed!");
            }
        }
        else
        {
            AXLOGW("Allocating memory for Texture2D failed!");
        }

        AX_SAFE_RELEASE(image);

    } while (0);

    return texture;
}

bool TextureCache::reloadTexture(std::string_view fileName)
{
    Texture2D* texture = nullptr;

    std::string fullpath = FileUtils::getInstance()->fullPathForFilename(fileName);
    if (fullpath.empty())
    {
        return false;
    }

    auto it = _textures.find(fullpath);
    if (it != _textures.end())
    {
        texture = it->second;
    }

    bool ret = false;
    if (!texture)
    {
        texture = this->addImage(fullpath);
        ret     = (texture != nullptr);
    }
    else
    {
        do
        {
            Image image;

            bool bRet = image.initWithImageFile(fullpath);
            AX_BREAK_IF(!bRet);

            ret = texture->initWithImage(&image);
        } while (0);
    }

    return ret;
}

// TextureCache - Remove

void TextureCache::removeAllTextures()
{
    for (auto&& texture : _textures)
    {
        texture.second->release();
    }
    _textures.clear();
}

void TextureCache::removeUnusedTextures()
{
    for (auto it = _textures.cbegin(); it != _textures.cend(); /* nothing */)
    {
        Texture2D* tex = it->second;
        if (tex->getReferenceCount() == 1)
        {
            AXLOGD("TextureCache: removing unused texture: {}", it->first);

            tex->release();
            it = _textures.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void TextureCache::removeTexture(Texture2D* texture)
{
    if (!texture)
    {
        return;
    }

    for (auto it = _textures.cbegin(); it != _textures.cend(); /* nothing */)
    {
        if (it->second == texture)
        {
            it->second->release();
            it = _textures.erase(it);
            break;
        }
        else
            ++it;
    }
}

void TextureCache::removeTextureForKey(std::string_view textureKeyName)
{
    auto it = _textures.find(textureKeyName);

    if (it == _textures.end())
    {
        auto key = FileUtils::getInstance()->fullPathForFilename(textureKeyName);
        it       = _textures.find(key);
    }

    if (it != _textures.end())
    {
        it->second->release();
        _textures.erase(it);
    }
}

Texture2D* TextureCache::getTextureForKey(std::string_view textureKeyName) const
{
    auto it = _textures.find(textureKeyName);

    if (it == _textures.end())
    {
        auto key = FileUtils::getInstance()->fullPathForFilename(textureKeyName);
        it       = _textures.find(key);
    }

    if (it != _textures.end())
        return it->second;
    return nullptr;
}

std::string TextureCache::getTextureFilePath(ax::Texture2D* texture) const
{
    for (auto&& item : _textures)
    {
        if (item.second == texture)
        {
            return item.first;
            break;
        }
    }
    return "";
}

void TextureCache::waitForQuit()
{
    {
        // notify sub thread to quit.
        const std::unique_lock<std::mutex> ul(_pendingMutex);
        _needQuit = true;
        _pendingRequests.clear();
        _sleepCondition.notify_all();
    }

    for (std::thread& t : _loadingThreads)
        t.join();

    _loadingThreads.clear();

    _loadRequests.clear();
    _doneRequests.clear();
}

std::string TextureCache::getCachedTextureInfo() const
{
    std::string ret;

    char tmp[1024];

    unsigned int count      = 0;
    unsigned int totalBytes = 0;

    for (auto&& texture : _textures)
    {
        Texture2D* tex   = texture.second;
        unsigned int bpp = tex->getBitsPerPixelForFormat();
        // Each texture takes up width * height * bytesPerPixel bytes.
        auto bytes = tex->getPixelsWide() * tex->getPixelsHigh() * bpp / 8;
        totalBytes += bytes;
        count++;
        auto msg = fmt::format_to_z(tmp, "\"{}\" rc={} id={} {} x {} @ {} bpp => {} KB\n", texture.first,
                                    tex->getReferenceCount(), fmt::ptr(tex->getBackendTexture()), tex->getPixelsWide(),
                                    tex->getPixelsHigh(), bpp, bytes / 1024);

        ret += msg;
    }

    auto msg = fmt::format_to_z(tmp, "TextureCache dumpDebugInfo: {} textures, for {} KB ({:.2f} MB)\n", count,
                                totalBytes / 1024, totalBytes / (1024.0f * 1024.0f));
    ret += msg;

    return ret;
}

void TextureCache::renameTextureWithKey(std::string_view srcName, std::string_view dstName)
{
    auto it = _textures.find(srcName);

    if (it == _textures.end())
    {
        auto key = FileUtils::getInstance()->fullPathForFilename(srcName);
        it       = _textures.find(key);
    }

    if (it != _textures.end())
    {
        std::string fullpath = FileUtils::getInstance()->fullPathForFilename(dstName);
        Texture2D* tex       = it->second;

        Image image;
        bool ret = image.initWithImageFile(dstName);
        if (ret)
        {
            tex->initWithImage(&image);
            _textures.emplace(fullpath, tex);
            _textures.erase(it);
        }
    }
}

#if AX_ENABLE_CACHE_TEXTURE_DATA

std::list<VolatileTexture*> VolatileTextureMgr::_textures;
bool VolatileTextureMgr::_isReloading = false;

VolatileTexture::VolatileTexture(Texture2D* t)
    : _texture(t)
    , _uiImage(nullptr)
    , _cashedImageType(kInvalid)
    , _textureData(nullptr)
    , _pixelFormat(backend::PixelFormat::RGBA8)
    , _fileName("")
    , _text("")
{}

VolatileTexture::~VolatileTexture()
{
    AX_SAFE_RELEASE(_uiImage);
}

void VolatileTextureMgr::addImageTexture(Texture2D* tt, std::string_view imageFileName)
{
    if (_isReloading)
    {
        return;
    }

    VolatileTexture* vt = getOrAddVolatileTexture(tt);

    vt->_cashedImageType = VolatileTexture::kImageFile;
    vt->_fileName        = imageFileName;
    vt->_pixelFormat     = tt->getPixelFormat();
}

void VolatileTextureMgr::addImage(Texture2D* tt, Image* image)
{
    if (tt == nullptr || image == nullptr)
        return;

    VolatileTexture* vt = getOrAddVolatileTexture(tt);

    if (vt->_uiImage != image)
    {
        AX_SAFE_RELEASE(vt->_uiImage);
        image->retain();
        vt->_uiImage         = image;
        vt->_cashedImageType = VolatileTexture::kImage;
        vt->_pixelFormat     = tt->getPixelFormat();
    }
}

VolatileTexture* VolatileTextureMgr::getOrAddVolatileTexture(Texture2D* tt)
{
    VolatileTexture* vt = nullptr;
    for (const auto& texture : _textures)
    {
        VolatileTexture* v = texture;
        if (v->_texture == tt)
        {
            vt = v;
            break;
        }
    }

    if (!vt)
    {
        vt = new VolatileTexture(tt);
        _textures.emplace_back(vt);
    }

    return vt;
}

void VolatileTextureMgr::addDataTexture(Texture2D* tt,
                                        void* data,
                                        int dataLen,
                                        backend::PixelFormat pixelFormat,
                                        const Vec2& contentSize)
{
    if (_isReloading)
    {
        return;
    }

    VolatileTexture* vt = getOrAddVolatileTexture(tt);

    vt->_cashedImageType = VolatileTexture::kImageData;
    vt->_textureData     = data;
    vt->_dataLen         = dataLen;
    vt->_pixelFormat     = pixelFormat;
    vt->_textureSize     = contentSize;
}

void VolatileTextureMgr::addStringTexture(Texture2D* tt, std::string_view text, const FontDefinition& fontDefinition)
{
    if (_isReloading)
    {
        return;
    }

    VolatileTexture* vt = getOrAddVolatileTexture(tt);

    vt->_cashedImageType = VolatileTexture::kString;
    vt->_text            = text;
    vt->_fontDefinition  = fontDefinition;
}

void VolatileTextureMgr::removeTexture(Texture2D* t)
{
    for (auto&& item : _textures)
    {
        VolatileTexture* vt = item;
        if (vt->_texture == t)
        {
            _textures.remove(vt);
            delete vt;
            break;
        }
    }
}

void VolatileTextureMgr::reloadAllTextures()
{
    _isReloading = true;
    AXLOGD("reload all texture");

    for (auto&& texture : _textures)
    {
        VolatileTexture* vt = texture;

        switch (vt->_cashedImageType)
        {
        case VolatileTexture::kImageFile:
        {
            reloadTexture(vt->_texture, vt->_fileName, vt->_pixelFormat);

            // etc1 support check whether alpha texture exists & load it
            Image image;
            if (image.initWithImageFile(vt->_fileName + TextureCache::getETC1AlphaFileSuffix()))
            {
                vt->_texture->updateWithImage(&image, vt->_pixelFormat, 1);
            }
        }
        break;
        case VolatileTexture::kImageData:
        {
            vt->_texture->initWithData(vt->_textureData, vt->_dataLen, vt->_pixelFormat, vt->_textureSize.width,
                                       vt->_textureSize.height);
        }
        break;
        case VolatileTexture::kString:
        {
            vt->_texture->initWithString(vt->_text, vt->_fontDefinition);
        }
        break;
        case VolatileTexture::kImage:
        {
            vt->_texture->initWithImage(vt->_uiImage, vt->_pixelFormat);
        }
        break;
        default:
            break;
        }
    }

    _isReloading = false;
}

void VolatileTextureMgr::reloadTexture(Texture2D* texture, std::string_view filename, backend::PixelFormat pixelFormat)
{
    if (!texture)
        return;

    Image image;

    if (image.initWithImageFile(filename))
        texture->initWithImage(&image, pixelFormat);
}

#endif  // AX_ENABLE_CACHE_TEXTURE_DATA

}  // namespace ax
