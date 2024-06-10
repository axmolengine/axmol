/****************************************************************************
Copyright (c) 2010      cocos2d-x.org
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

#include "base/Utils.h"

#include <cmath>
#include <stdlib.h>

#include <signal.h>
#if !defined(_WIN32)
// for unix/linux kill
#    include <unistd.h>
#endif

#include "openssl/evp.h"

#include "base/Director.h"
#include "base/EventDispatcher.h"
#include "base/Constants.h"
#include "base/UTF8.h"
#include "renderer/CustomCommand.h"
#include "renderer/Renderer.h"
#include "renderer/TextureCache.h"
#include "renderer/RenderState.h"
#include "renderer/backend/Types.h"
#include "renderer/backend/PixelBufferDescriptor.h"

#include "platform/Image.h"
#include "platform/FileUtils.h"
#include "2d/Sprite.h"
#include "2d/RenderTexture.h"

#include "base/base64.h"
#include "base/axstd.h"

using namespace std::string_view_literals;

NS_AX_BEGIN

namespace utils
{
namespace base64
{
inline int encBound(int sourceLen)
{
    return (sourceLen + 2) / 3 * 4;
}
inline int decBound(int sourceLen)
{
    return sourceLen / 4 * 3 + 1;
}
}  // namespace base64

int nextPOT(int x)
{
    --x;
    x |= (x >> 1);
    x |= (x >> 2);
    x |= (x >> 4);
    x |= (x >> 8);
    x |= (x >> 16);
    return ++x;
}

/*
 * Capture screen interface
 */
static EventListenerCustom* s_captureScreenListener;
void captureScreen(std::function<void(RefPtr<Image>)> imageCallback)
{
    if (s_captureScreenListener)
    {
        AXLOG("Warning: CaptureScreen has been called already, don't call more than once in one frame.");
        return;
    }

    auto director        = Director::getInstance();
    auto renderer        = director->getRenderer();
    auto eventDispatcher = director->getEventDispatcher();

    // !!!Metal: needs setFrameBufferOnly before draw
#if defined(AX_USE_METAL)
    s_captureScreenListener =
        eventDispatcher->addCustomEventListener(Director::EVENT_BEFORE_DRAW, [=](EventCustom* /*event*/) {
#else
    s_captureScreenListener =
        eventDispatcher->addCustomEventListener(Director::EVENT_AFTER_DRAW, [=](EventCustom* /*event*/) {
#endif
        eventDispatcher->removeEventListener(s_captureScreenListener);
        s_captureScreenListener = nullptr;
        // !!!GL: AFTER_DRAW and BEFORE_END_FRAME
        renderer->readPixels(renderer->getDefaultRenderTarget(), [=](const backend::PixelBufferDescriptor& pbd) {
            if (pbd)
            {
                auto image = utils::makeInstance<Image>(&Image::initWithRawData, pbd._data.getBytes(),
                                                        pbd._data.getSize(), pbd._width, pbd._height, 8, false);
                imageCallback(image);
            }
            else
                imageCallback(nullptr);
        });
    });
}

static std::unordered_map<Node*, EventListenerCustom*> s_captureNodeListener;
void captureNode(Node* startNode, std::function<void(RefPtr<Image>)> imageCallback, float scale)
{
    if (s_captureNodeListener.find(startNode) != s_captureNodeListener.end())
    {
        AXLOG("Warning: current node has been captured already");
        return;
    }

    auto callback = [startNode, scale, imageCallback](EventCustom* /*event*/) {
        auto director            = Director::getInstance();
        auto captureNodeListener = s_captureNodeListener[startNode];
        director->getEventDispatcher()->removeEventListener((EventListener*)(captureNodeListener));
        s_captureNodeListener.erase(startNode);
        auto& size = startNode->getContentSize();

        director->setNextDeltaTimeZero(true);

        RenderTexture* finalRtx = nullptr;

        auto rtx =
            RenderTexture::create(size.width, size.height, backend::PixelFormat::RGBA8, PixelFormat::D24S8, false);
        // rtx->setKeepMatrix(true);
        Point savedPos = startNode->getPosition();
        Point anchor;
        if (!startNode->isIgnoreAnchorPointForPosition())
        {
            anchor = startNode->getAnchorPoint();
        }
        startNode->setPosition(Point(size.width * anchor.x, size.height * anchor.y));
        rtx->begin();
        startNode->visit();
        rtx->end();
        startNode->setPosition(savedPos);

        if (std::abs(scale - 1.0f) < 1e-6f /* no scale */)
            finalRtx = rtx;
        else
        {
            /* scale */
            auto finalRect = Rect(0, 0, size.width, size.height);
            Sprite* sprite = Sprite::createWithTexture(rtx->getSprite()->getTexture(), finalRect);
            sprite->setAnchorPoint(Point(0, 0));
            sprite->setFlippedY(true);
            finalRtx = RenderTexture::create(size.width * scale, size.height * scale, backend::PixelFormat::RGBA8,
                                             PixelFormat::D24S8, false);

            sprite->setScale(scale);  // or use finalRtx->setKeepMatrix(true);
            finalRtx->begin();
            sprite->visit();
            finalRtx->end();
        }

        director->getRenderer()->render();

        finalRtx->newImage(imageCallback);
    };

    auto listener =
        Director::getInstance()->getEventDispatcher()->addCustomEventListener(Director::EVENT_BEFORE_DRAW, callback);

    s_captureNodeListener[startNode] = listener;
}

// [DEPRECATED]
void captureScreen(std::function<void(bool, std::string_view)> afterCap, std::string_view filename)
{
    std::string outfile;
    if (FileUtils::getInstance()->isAbsolutePath(filename))
        outfile = filename;
    else
        outfile = FileUtils::getInstance()->getWritablePath().append(filename);

    captureScreen([_afterCap = std::move(afterCap), _outfile = std::move(outfile)](RefPtr<Image> image) mutable {
        Director::getInstance()->getJobSystem()->enqueue(
            [_afterCap = std::move(_afterCap), image = std::move(image), _outfile = std::move(_outfile)]() mutable {
            bool ok = image->saveToFile(_outfile);
            Director::getInstance()->getScheduler()->runOnAxmolThread(
                [ok, _afterCap = std::move(_afterCap), _outfile = std::move(_outfile)] { _afterCap(ok, _outfile); });
        });
    });
}

std::vector<Node*> findChildren(const Node& node, std::string_view name)
{
    std::vector<Node*> vec;

    node.enumerateChildren(name, [&vec](Node* nodeFound) -> bool {
        vec.emplace_back(nodeFound);
        return false;
    });

    return vec;
}

#define MAX_ITOA_BUFFER_SIZE 256
double atof(const char* str)
{
    if (str == nullptr)
    {
        return 0.0;
    }

    char buf[MAX_ITOA_BUFFER_SIZE];
    strncpy(buf, str, MAX_ITOA_BUFFER_SIZE);

    // strip string, only remain 7 numbers after '.'
    char* dot = strchr(buf, '.');
    if (dot != nullptr && dot - buf + 8 < MAX_ITOA_BUFFER_SIZE)
    {
        dot[8] = '\0';
    }

    return ::atof(buf);
}

double gettime()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);

    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000;
}

long long getTimeInMilliseconds()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (long long)tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

Rect getCascadeBoundingBox(Node* node)
{
    Rect cbb;
    Vec2 contentSize = node->getContentSize();

    // check all children bounding box, get maximize box
    Node* child = nullptr;
    bool merge  = false;
    for (auto&& object : node->getChildren())
    {
        child = dynamic_cast<Node*>(object);
        if (!child->isVisible())
            continue;

        const Rect box = getCascadeBoundingBox(child);
        if (box.size.width <= 0 || box.size.height <= 0)
            continue;

        if (!merge)
        {
            cbb   = box;
            merge = true;
        }
        else
        {
            cbb.merge(box);
        }
    }

    // merge content size
    if (contentSize.width > 0 && contentSize.height > 0)
    {
        const Rect box = RectApplyAffineTransform(Rect(0, 0, contentSize.width, contentSize.height),
                                                  node->getNodeToWorldAffineTransform());
        if (!merge)
        {
            cbb = box;
        }
        else
        {
            cbb.merge(box);
        }
    }

    return cbb;
}

Sprite* createSpriteFromBase64Cached(const char* base64String, const char* key)
{
    Texture2D* texture = Director::getInstance()->getTextureCache()->getTextureForKey(key);

    if (texture == nullptr)
    {
        unsigned char* decoded;
        int length = base64Decode((const unsigned char*)base64String, (unsigned int)strlen(base64String), &decoded);

        Image* image     = new Image();
        bool imageResult = image->initWithImageData(decoded, length, true);
        AXASSERT(imageResult, "Failed to create image from base64!");

        if (!imageResult)
        {
            AX_SAFE_RELEASE_NULL(image);
            return nullptr;
        }

        texture = Director::getInstance()->getTextureCache()->addImage(image, key);
        image->release();
    }

    Sprite* sprite = Sprite::createWithTexture(texture);

    return sprite;
}

Sprite* createSpriteFromBase64(const char* base64String)
{
    unsigned char* decoded;
    int length = base64Decode((const unsigned char*)base64String, (unsigned int)strlen(base64String), &decoded);

    Image* image     = new Image();
    bool imageResult = image->initWithImageData(decoded, length, decoded);
    AXASSERT(imageResult, "Failed to create image from base64!");

    if (!imageResult)
    {
        AX_SAFE_RELEASE_NULL(image);
        return nullptr;
    }

    Texture2D* texture = new Texture2D();
    texture->initWithImage(image);
    texture->setAliasTexParameters();
    image->release();

    Sprite* sprite = Sprite::createWithTexture(texture);
    texture->release();

    return sprite;
}

Node* findChild(Node* levelRoot, std::string_view name)
{
    if (levelRoot == nullptr || name.empty())
        return nullptr;

    // Find this node
    auto target = levelRoot->getChildByName(name);
    if (target != nullptr)
        return target;

    // Find recursively
    for (auto&& child : levelRoot->getChildren())
    {
        target = findChild(child, name);
        if (target != nullptr)
            return target;
    }
    return nullptr;
}

Node* findChild(Node* levelRoot, int tag)
{
    if (levelRoot == nullptr || tag == Node::INVALID_TAG)
        return nullptr;

    // Find this node
    auto target = levelRoot->getChildByTag(tag);
    if (target != nullptr)
        return target;

    // Find recursively
    for (auto&& child : levelRoot->getChildren())
    {
        target = findChild(child, tag);
        if (target != nullptr)
            return target;
    }

    return nullptr;
}

std::string getFileMD5Hash(std::string_view filename, uint32_t bufferSize)
{
    return computeFileDigest(filename, "md5"sv, bufferSize);
}

std::string getDataMD5Hash(const Data& data)
{
    // if (data.isNull())
    //     return std::string{};

    return computeDigest(std::string_view{(const char*)data.getBytes(), (size_t)data.getSize()}, "md5"sv);
}

std::string getStringMD5Hash(std::string_view str)
{
    return computeDigest(str, "md5"sv);
}

std::string computeDigest(std::string_view data, std::string_view algorithm, bool toHex)
{
    unsigned char mdValue[EVP_MAX_MD_SIZE] = {0};
    unsigned int mdLen                     = 0;

    OpenSSL_add_all_digests();
    const EVP_MD* md = EVP_get_digestbyname(algorithm.data());
    if (!md)
        return std::string{};

    EVP_MD_CTX* mdctx = EVP_MD_CTX_create();
    auto ok           = EVP_DigestInit(mdctx, md);
    if (!ok)
    {
        EVP_MD_CTX_destroy(mdctx);
        return std::string{};
    }
    EVP_DigestUpdate(mdctx, data.data(), data.size());
    EVP_DigestFinal(mdctx, mdValue, &mdLen);
    EVP_MD_CTX_destroy(mdctx);

    return toHex ? bin2hex(std::string_view{(const char*)mdValue, (size_t)mdLen})
                 : std::string{(const char*)mdValue, (size_t)mdLen};
}

std::string computeFileDigest(std::string_view filename, std::string_view algorithm, uint32_t bufferSize, bool toHex)
{
    if (filename.empty())
        return std::string{};

    auto fs = FileUtils::getInstance()->openFileStream(filename, FileStream::Mode::READ);

    if (!fs || !fs->isOpen())
        return std::string{};

    std::unique_ptr<uint8_t[]> buffer(new uint8_t[bufferSize]);
    unsigned char mdValue[EVP_MAX_MD_SIZE] = {0};
    unsigned int mdLen                     = 0;

    OpenSSL_add_all_digests();
    const EVP_MD* md = EVP_get_digestbyname(algorithm.data());
    if (!md)
        return std::string{};

    EVP_MD_CTX* mdctx = EVP_MD_CTX_create();
    auto ok           = EVP_DigestInit(mdctx, md);
    if (!ok)
    {
        EVP_MD_CTX_destroy(mdctx);
        return std::string{};
    }

    int bytesRead;
    while ((bytesRead = fs->read(buffer.get(), bufferSize)) > 0)
    {
        EVP_DigestUpdate(mdctx, buffer.get(), bytesRead);
    }

    EVP_DigestFinal(mdctx, mdValue, &mdLen);
    EVP_MD_CTX_destroy(mdctx);
    return toHex ? bin2hex(std::string_view{(const char*)mdValue, (size_t)mdLen})
                 : std::string{(const char*)mdValue, (size_t)mdLen};
}

LanguageType getLanguageTypeByISO2(const char* code)
{
    // this function is used by all platforms to get system language
    // except windows: core/platform/win32/CCApplication-win32.cpp
    LanguageType ret = LanguageType::ENGLISH;

    if (strncmp(code, "zh", 2) == 0)
    {
        ret = LanguageType::CHINESE;
    }
    else if (strncmp(code, "ja", 2) == 0)
    {
        ret = LanguageType::JAPANESE;
    }
    else if (strncmp(code, "fr", 2) == 0)
    {
        ret = LanguageType::FRENCH;
    }
    else if (strncmp(code, "it", 2) == 0)
    {
        ret = LanguageType::ITALIAN;
    }
    else if (strncmp(code, "de", 2) == 0)
    {
        ret = LanguageType::GERMAN;
    }
    else if (strncmp(code, "es", 2) == 0)
    {
        ret = LanguageType::SPANISH;
    }
    else if (strncmp(code, "nl", 2) == 0)
    {
        ret = LanguageType::DUTCH;
    }
    else if (strncmp(code, "ru", 2) == 0)
    {
        ret = LanguageType::RUSSIAN;
    }
    else if (strncmp(code, "hu", 2) == 0)
    {
        ret = LanguageType::HUNGARIAN;
    }
    else if (strncmp(code, "pt", 2) == 0)
    {
        ret = LanguageType::PORTUGUESE;
    }
    else if (strncmp(code, "ko", 2) == 0)
    {
        ret = LanguageType::KOREAN;
    }
    else if (strncmp(code, "ar", 2) == 0)
    {
        ret = LanguageType::ARABIC;
    }
    else if (strncmp(code, "nb", 2) == 0)
    {
        ret = LanguageType::NORWEGIAN;
    }
    else if (strncmp(code, "pl", 2) == 0)
    {
        ret = LanguageType::POLISH;
    }
    else if (strncmp(code, "tr", 2) == 0)
    {
        ret = LanguageType::TURKISH;
    }
    else if (strncmp(code, "uk", 2) == 0)
    {
        ret = LanguageType::UKRAINIAN;
    }
    else if (strncmp(code, "ro", 2) == 0)
    {
        ret = LanguageType::ROMANIAN;
    }
    else if (strncmp(code, "bg", 2) == 0)
    {
        ret = LanguageType::BULGARIAN;
    }
    else if (strncmp(code, "be", 2) == 0)
    {
        ret = LanguageType::BELARUSIAN;
    }
    return ret;
}

backend::BlendFactor toBackendBlendFactor(int factor)
{
    switch (factor)
    {
    case GLBlendConst::ONE:
        return backend::BlendFactor::ONE;
    case GLBlendConst::ZERO:
        return backend::BlendFactor::ZERO;
    case GLBlendConst::SRC_COLOR:
        return backend::BlendFactor::SRC_COLOR;
    case GLBlendConst::ONE_MINUS_SRC_COLOR:
        return backend::BlendFactor::ONE_MINUS_SRC_COLOR;
    case GLBlendConst::SRC_ALPHA:
        return backend::BlendFactor::SRC_ALPHA;
    case GLBlendConst::ONE_MINUS_SRC_ALPHA:
        return backend::BlendFactor::ONE_MINUS_SRC_ALPHA;
    case GLBlendConst::DST_COLOR:
        return backend::BlendFactor::DST_COLOR;
    case GLBlendConst::ONE_MINUS_DST_COLOR:
        return backend::BlendFactor::ONE_MINUS_DST_COLOR;
    case GLBlendConst::DST_ALPHA:
        return backend::BlendFactor::DST_ALPHA;
    case GLBlendConst::ONE_MINUS_DST_ALPHA:
        return backend::BlendFactor::ONE_MINUS_DST_ALPHA;
    case GLBlendConst::SRC_ALPHA_SATURATE:
        return backend::BlendFactor::SRC_ALPHA_SATURATE;
    case GLBlendConst::BLEND_COLOR:
        return backend::BlendFactor::BLEND_COLOR;
    case GLBlendConst::CONSTANT_ALPHA:
        return backend::BlendFactor::CONSTANT_ALPHA;
    case GLBlendConst::ONE_MINUS_CONSTANT_ALPHA:
        return backend::BlendFactor::ONE_MINUS_CONSTANT_ALPHA;
    default:
        assert(false);
        break;
    }
    return backend::BlendFactor::ONE;
}

int toGLBlendFactor(backend::BlendFactor blendFactor)
{
    int ret = GLBlendConst::ONE;
    switch (blendFactor)
    {
    case backend::BlendFactor::ZERO:
        ret = GLBlendConst::ZERO;
        break;
    case backend::BlendFactor::ONE:
        ret = GLBlendConst::ONE;
        break;
    case backend::BlendFactor::SRC_COLOR:
        ret = GLBlendConst::SRC_COLOR;
        break;
    case backend::BlendFactor::ONE_MINUS_SRC_COLOR:
        ret = GLBlendConst::ONE_MINUS_SRC_COLOR;
        break;
    case backend::BlendFactor::SRC_ALPHA:
        ret = GLBlendConst::SRC_ALPHA;
        break;
    case backend::BlendFactor::ONE_MINUS_SRC_ALPHA:
        ret = GLBlendConst::ONE_MINUS_SRC_ALPHA;
        break;
    case backend::BlendFactor::DST_COLOR:
        ret = GLBlendConst::DST_COLOR;
        break;
    case backend::BlendFactor::ONE_MINUS_DST_COLOR:
        ret = GLBlendConst::ONE_MINUS_DST_COLOR;
        break;
    case backend::BlendFactor::DST_ALPHA:
        ret = GLBlendConst::DST_ALPHA;
        break;
    case backend::BlendFactor::ONE_MINUS_DST_ALPHA:
        ret = GLBlendConst::ONE_MINUS_DST_ALPHA;
        break;
    case backend::BlendFactor::SRC_ALPHA_SATURATE:
        ret = GLBlendConst::SRC_ALPHA_SATURATE;
        break;
    case backend::BlendFactor::BLEND_COLOR:
        ret = GLBlendConst::BLEND_COLOR;
        break;
    default:
        break;
    }
    return ret;
}

backend::SamplerFilter toBackendSamplerFilter(int mode)
{
    switch (mode)
    {
    case GLTexParamConst::LINEAR:
    case GLTexParamConst::LINEAR_MIPMAP_LINEAR:
    case GLTexParamConst::LINEAR_MIPMAP_NEAREST:
    case GLTexParamConst::NEAREST_MIPMAP_LINEAR:
        return backend::SamplerFilter::LINEAR;
    case GLTexParamConst::NEAREST:
    case GLTexParamConst::NEAREST_MIPMAP_NEAREST:
        return backend::SamplerFilter::NEAREST;
    default:
        AXASSERT(false, "invalid GL sampler filter!");
        return backend::SamplerFilter::LINEAR;
    }
}

backend::SamplerAddressMode toBackendAddressMode(int mode)
{
    switch (mode)
    {
    case GLTexParamConst::REPEAT:
        return backend::SamplerAddressMode::REPEAT;
    case GLTexParamConst::CLAMP:
    case GLTexParamConst::CLAMP_TO_EDGE:
        return backend::SamplerAddressMode::CLAMP_TO_EDGE;
    case GLTexParamConst::MIRROR_REPEAT:
        return backend::SamplerAddressMode::MIRROR_REPEAT;
    default:
        AXASSERT(false, "invalid GL address mode");
        return backend::SamplerAddressMode::REPEAT;
    }
}

const Mat4& getAdjustMatrix()
{
    static ax::Mat4 adjustMatrix = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0.5, 0.5, 0, 0, 0, 1};

    return adjustMatrix;
}

Mat3 getNormalMat3OfMat4(const Mat4& mat)
{
    Mat4 mvInverse  = mat;
    mvInverse.m[12] = mvInverse.m[13] = mvInverse.m[14] = 0.0f;
    mvInverse.inverse();
    mvInverse.transpose();
    return Mat3{mvInverse.m[0], mvInverse.m[1], mvInverse.m[2], mvInverse.m[4], mvInverse.m[5],
                mvInverse.m[6], mvInverse.m[8], mvInverse.m[9], mvInverse.m[10]};
}

std::vector<int> parseIntegerList(std::string_view intsString)
{
    std::vector<int> result;

    if (!intsString.empty())
    {
        const char* cStr = intsString.data();
        char* endptr;

        for (auto i = strtol(cStr, &endptr, 10); endptr != cStr; i = strtol(cStr, &endptr, 10))
        {
            if (errno == ERANGE)
            {
                errno = 0;
                AXLOGWARN("%s contains out of range integers", intsString.data());
            }
            result.emplace_back(static_cast<int>(i));
            cStr = endptr;
        }
    }

    return result;
}

std::string bin2hex(std::string_view binary /*charstring also regard as binary in C/C++*/, int delim, bool prefix)
{
    char low;
    char high;
    size_t len = binary.length();

    bool delim_needed = delim != -1 || delim == ' ';

    std::string result;
    result.reserve((len << 1) + (delim_needed ? len : 0) + (prefix ? (len << 1) : 0));

    for (size_t i = 0; i < len; ++i)
    {
        auto ch = binary[i];
        high    = (ch >> 4) & 0x0f;
        low     = ch & 0x0f;
        if (prefix)
        {
            result.push_back('0');
            result.push_back('x');
        }

        auto hic = nibble2hex(high);
        auto lic = nibble2hex(low);
        result.push_back(hic);
        result.push_back(lic);
        if (delim_needed)
        {
            result.push_back(delim);
        }
    }

    return result;
}

void killCurrentProcess()
{
#if !defined(_WIN32)
    ::kill(::getpid(), SIGKILL);
#else
    ::TerminateProcess(::GetCurrentProcess(), SIGTERM);
#endif
}

std::string urlEncode(std::string_view s)
{
    std::string encoded;
    if (!s.empty())
    {
        encoded.reserve(s.length() * 3 / 2);
        for (const char c : s)
        {
            if (isalnum((uint8_t)c) || c == '-' || c == '_' || c == '.' || c == '~')
            {
                encoded.push_back(c);
            }
            else
            {
                encoded.push_back('%');

                char hex[2];
                encoded.append(char2hex(hex, c, 'A'), sizeof(hex));
            }
        }
    }
    return encoded;
}

std::string urlDecode(std::string_view st)
{
    std::string decoded;
    if (!st.empty())
    {
        const char* s       = st.data();
        const size_t length = st.length();
        decoded.reserve(length * 2 / 3);
        for (unsigned int i = 0; i < length; ++i)
        {
            if (!s[i])
                break;

            if (s[i] == '%')
            {
                decoded.push_back(hex2char(s + i + 1));
                i = i + 2;
            }
            else if (s[i] == '+')
            {
                decoded.push_back(' ');
            }
            else
            {
                decoded.push_back(s[i]);
            }
        }
    }
    return decoded;
}

AX_DLL std::string base64Encode(const void* in, size_t inlen)
{
    size_t n = ax::base64::encoded_size(inlen);
    if (n > 0)
    {
        std::string ret;
        /**
         * @brief resize_and_overrite avaialbe on vs2022 17.1
         *  refer to:
         *    - https://learn.microsoft.com/en-us/cpp/overview/visual-cpp-language-conformance?view=msvc-170
         *    - https://github.com/microsoft/STL/wiki/Changelog#vs-2022-171
         *    - https://learn.microsoft.com/en-us/cpp/preprocessor/predefined-macros?view=msvc-170
         *
         */
#if _AX_HAS_CXX23
        ret.resize_and_overwrite(n, [in, inlen](char* out, size_t) { return ax::base64::encode(out, in, inlen); });
#else
        ret.resize(n);
        ret.resize(ax::base64::encode(&ret.front(), in, inlen));
#endif

        return ret;
    }
    return std::string{};
}

AX_DLL yasio::byte_buffer base64Decode(std::string_view s)
{
    size_t n = ax::base64::decoded_size(s.length());
    if (n > 0)
    {
        axstd::byte_buffer ret;
        ret.resize_and_overwrite(n, [&s](uint8_t* out, size_t) { return ax::base64::decode(out, s.data(), s.size()); });
        return ret;
    }
    return yasio::byte_buffer{};
}

int base64Encode(const unsigned char* in, unsigned int inLength, char** out)
{
    auto n = ax::base64::encoded_size(inLength);
    // should be enough to store 8-bit buffers in 6-bit buffers
    char* tmp = nullptr;
    if (n > 0 && (tmp = (char*)malloc(n + 1)))
    {
        auto ret = ax::base64::encode(tmp, in, inLength);
        tmp[ret] = '\0';
        *out     = tmp;
        return static_cast<int>(ret);
    }
    *out = nullptr;
    return 0;
}

AX_DLL int base64Decode(const unsigned char* in, unsigned int inLength, unsigned char** out)
{
    size_t n           = ax::base64::decoded_size(inLength);
    unsigned char* tmp = nullptr;
    if (n > 0 && (tmp = (unsigned char*)malloc(n)))
    {
        n = static_cast<int>(ax::base64::decode(tmp, reinterpret_cast<const char*>(in), inLength));
        if (n > 0)
            *out = tmp;
        else
        {
            *out = nullptr;
            free(tmp);
        }
        return static_cast<int>(n);
    }
    *out = nullptr;
    return 0;
}

AX_DLL uint32_t fourccValue(std::string_view str)
{
    if (str.empty() || str[0] != '#')
        return (uint32_t)-1;
    uint32_t value = 0;
    memcpy(&value, str.data() + 1, std::min(sizeof(value), str.size() - 1));
    return value;
}

}  // namespace utils

NS_AX_END
