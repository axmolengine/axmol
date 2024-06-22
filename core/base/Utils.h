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
#ifndef __AX_UTILS_H__
#define __AX_UTILS_H__

#include <vector>
#include <string>
#include "2d/Node.h"
#include "base/Macros.h"
#include "base/RefPtr.h"
#include "base/Data.h"
#include "renderer/backend/Types.h"
#include "math/Mat4.h"
#include "math/Mat3.h"
#include "base/axstd.h"

/** @file ccUtils.h
Misc free functions
*/

NS_AX_BEGIN

class Sprite;
class Image;

namespace utils
{
/** Returns the Next Power of Two value.

Examples:
- If "value" is 15, it will return 16.
- If "value" is 16, it will return 16.
- If "value" is 17, it will return 32.
@param value The value to get next power of two.
@return Returns the next power of two value.
@since axmol-2.1.3: move ax::ccNextPOT to ax::utils::nextPOT
*/
int nextPOT(int value);

/** Capture the entire screen.
 * To ensure the snapshot is applied after everything is updated and rendered in the current frame,
 * we need to wrap the operation with a custom command which is then inserted into the tail of the render queue.
 * @param afterCaptured specify the callback function which will be invoked after the snapshot is done.
 * @param filename specify a filename where the snapshot is stored. This parameter can be either an absolute path or a
 * simple base filename ("hello.png" etc.), don't use a relative path containing directory names.("mydir/hello.png"
 * etc.).
 * @since v4.0 with axmol
 */
AX_DLL void captureScreen(std::function<void(RefPtr<Image>)> imageCallback);

/** Capture a specific Node.
 * @param startNode specify the snapshot Node. It should be ax::Scene
 * @param scale
 * @returns: return a Image, then can call saveToFile to save the image as "xxx.png or xxx.jpg".
 * @since v4.0 with axmol
 */
AX_DLL void captureNode(Node* startNode, std::function<void(RefPtr<Image>)> imageCallback, float scale = 1.0f);

/** Capture the entire screen. V4-copmatiable only, [DEPRECATED]
 * To ensure the snapshot is applied after everything is updated and rendered in the current frame,
 * we need to wrap the operation with a custom command which is then inserted into the tail of the render queue.
 * @param afterCaptured specify the callback function which will be invoked after the snapshot is done.
 * @param filename specify a filename where the snapshot is stored. This parameter can be either an absolute path or a
 * simple base filename ("hello.png" etc.), don't use a relative path containing directory names.("mydir/hello.png"
 * etc.).
 * @since v4.0
 */
AX_DLL void captureScreen(std::function<void(bool, std::string_view)> afterCap, std::string_view filename);

/** Find children by name, it will return all child that has the same name.
 * It supports c++ 11 regular expression. It is  a helper function of `Node::enumerateChildren()`.
 * You can refer to `Node::enumerateChildren()` for detail information.
 *
 * @param node The node to find
 * @param name The name to search for, it supports c++ 11 expression
 * @return Array of Nodes that matches the name
 * @since v3.2
 */
AX_DLL std::vector<Node*> findChildren(const Node& node, std::string_view name);

/** Same to ::atof, but strip the string, remain 7 numbers after '.' before call atof.
 * Why we need this? Because in android c++_static, atof ( and std::atof ) is unsupported for numbers have long decimal
 * part and contain several numbers can approximate to 1 ( like 90.099998474121094 ), it will return inf. This function
 * is used to fix this bug.
 * @param str The string be to converted to double.
 * @return Returns converted value of a string.
 */
AX_DLL double atof(const char* str);

/** Get current exact time, accurate to nanoseconds.
 * @return Returns the time in seconds since the Epoch.
 */
AX_DLL double gettime();

/**
 * Get current time in milliseconds, accurate to nanoseconds
 *
 * @return  Returns the time in milliseconds since the Epoch.
 */
AX_DLL long long getTimeInMilliseconds();

/**
 * Calculate unionof bounding box of a node and its children.
 * @return Returns unionof bounding box of a node and its children.
 */
AX_DLL Rect getCascadeBoundingBox(Node* node);

/**
 * Create a sprite instance from base64 encoded image and adds the texture to the Texture Cache.

 * @return Returns an instance of sprite
 */
AX_DLL Sprite* createSpriteFromBase64Cached(const char* base64String, const char* key);

/**
* Create a sprite instance from base64 encoded image.

* @return Returns an instance of sprite
*/
AX_DLL Sprite* createSpriteFromBase64(const char* base64String);

/**
 * Find a child by name recursively

 * @return  Returns found node or nullptr
 */
AX_DLL Node* findChild(Node* levelRoot, std::string_view name);

/**
 * Find a child by tag recursively

 * @return Returns found node or nullptr
 */
AX_DLL Node* findChild(Node* levelRoot, int tag);

/**
 * Find a child by name recursively

 * @return  Returns found node or nullptr with specified type 'T'
 */
template <typename T>
inline T findChild(Node* levelRoot, std::string_view name)
{
    return dynamic_cast<T>(findChild(levelRoot, name));
}

/**
 * Find a child by tag recursively

 * @return  Returns found node or nullptr with specified type 'T'
 */
template <typename T>
inline T findChild(Node* levelRoot, int tag)
{
    return dynamic_cast<T>(findChild(levelRoot, tag));
}

/**
 *  Gets the md5 hash for the given file.
 *  @param filename The file to calculate md5 hash.
 *  @param bufferSize The size of buffer used for reading the file
 *  @return The md5 hash for the file
 */
AX_DLL std::string getFileMD5Hash(std::string_view filename, uint32_t bufferSize = 16 * 1024);

/**
 *  Gets the md5 hash for the given buffer.
 *  @param data The buffer to calculate md5 hash.
 *  @return The md5 hash for the data
 */
AX_DLL std::string getDataMD5Hash(const Data& data);
/**
 *  Gets the md5 hash for the given string.
 *  @param str The string to calculate md5 hash.
 *  @return The md5 hash for the str
 */
AX_DLL std::string getStringMD5Hash(std::string_view str);

/**
 *  Gets the hash for the given buffer with specific algorithm.
 *  @param data The buffer to calculate the hash.
 *  @param algorithm The hash algorithm, support "md5", "sha1", "sha256", "sha512" and more
 *  @param toHex Convert to lowercase hex string
 *  @return The hash for the data
 */
AX_DLL std::string computeDigest(std::string_view data, std::string_view algorithm, bool toHex = true);

/**
 *  Gets the hash for the given file with specific algorithm.
 *  @param filename The path of the file to hash.
 *  @param algorithm The hash algorithm, support "md5", "sha1", "sha256", "sha512" and more
 *  @param bufferSize Chunk size used for hashing
 *  @param toHex Convert to lowercase hex string
 *  @return The hash for the file
 */
AX_DLL std::string computeFileDigest(std::string_view filename,
                                     std::string_view algorithm,
                                     uint32_t bufferSize = 16 * 1024,
                                     bool toHex          = true);

/**
@brief Converts language iso 639-1 code to LanguageType enum.
@return LanguageType enum.
* @js NA
* @lua NA
*/
AX_DLL LanguageType getLanguageTypeByISO2(const char* code);

AX_DLL backend::BlendFactor toBackendBlendFactor(int factor);

AX_DLL int toGLBlendFactor(backend::BlendFactor blendFactor);

AX_DLL backend::SamplerFilter toBackendSamplerFilter(int mode);

AX_DLL backend::SamplerAddressMode toBackendAddressMode(int mode);

// Adjust matrix for metal.
AX_DLL const Mat4& getAdjustMatrix();

/**
Get the Normal Matrix of matrixMV
*/
AX_DLL Mat3 getNormalMat3OfMat4(const Mat4& mat);

/**
@brief Parses a list of space-separated integers.
@return Vector of ints.
* @js NA
* @lua NA
*/
AX_DLL std::vector<int> parseIntegerList(std::string_view intsString);

/**
@brief translate charstring/binarystream to hexstring.
@return hexstring.
* @js NA
* @lua NA
*/
AX_DLL std::string bin2hex(std::string_view binary /*charstring also regard as binary in C/C++*/,
                           int delim   = -1,
                           bool prefix = false);

/**
@brief killCurrentProcess immidiately, any object's destructor never call
@return NA.
* @js NA
* @lua NA
*/
AX_DLL void killCurrentProcess();

/**
* Create a Game Object, like CREATE_FUNC, but more powerful

* @return  Returns a RefPtr<T> game object
* @remark  Auto manage ax::Object reference count, use std::unique_ptr
* @limition  The init function finit must be public
*/
template <typename T, typename F, typename... Ts>
static RefPtr<T> makeInstance(F&& memf, Ts&&... args)
{
    RefPtr<T> pRet(ReferencedObject<T>{new T()});
    if (pRet && std::mem_fn(memf)(pRet.get(), std::forward<Ts>(args)...))
        return pRet;

    pRet.reset();
    return pRet;
}

/**
* Create a Game Object with 'bool T::init()', like CREATE_FUNC, but more powerful

* @return  Returns a autorelease game object
* @remark  Auto manage ax::Object reference count, use std::unique_ptr
* @limition  The init function finit must be public
*/
template <typename T>
inline static RefPtr<T> makeInstance()
{
    return makeInstance<T>(&T::init);
}

/**
* Create a Game Object, like CREATE_FUNC, but more powerful

* @return  Returns a autorelease game object
* @remark  Auto manage ax::Object reference count, use AutoReleasePool
* @limition The init function finit must be public
*/
template <typename T, typename F, typename... Ts>
inline T* createInstance(F&& finit, Ts&&... args)
{
    T* pRet = new T();
    if (std::mem_fn(finit)(pRet, std::forward<Ts>(args)...))
        pRet->autorelease();
    else
    {
        delete pRet;
        pRet = nullptr;
    }
    return pRet;
}

/**
* Create a Game Object with 'bool T::init()', like CREATE_FUNC, but more powerful

* @return  Returns a autorelease game object
* @remark  Auto manage ax::Object reference count, use AutoReleasePool
* @limition  The init function finit must be public
*/
template <typename T>
inline T* createInstance()
{
    return ::ax::utils::createInstance<T>(&T::init);
}

/**
* Create a Game Object with 'bool T::init()', like CREATE_FUNC, but more powerful

* @return  Returns a game object
* @remark  You need call release after you don't want use it manually
* @limition  The init function finit must be public
*/
template <typename T, typename F, typename... Ts>
static T* newInstance(F&& memf, Ts&&... args)
{
    T* pRet = new T();
    if (std::mem_fn(memf)(pRet, std::forward<Ts>(args)...))
        return pRet;
    else
    {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

/**
* Create a Game Object with 'bool T::init()', like CREATE_FUNC, but more powerful

* @return  Returns a game object
* @remark  You need call release after you don't want use it manually
* @limition  The init function finit must be public
*/
template <typename T>
inline static T* newInstance()
{
    return newInstance<T>(&T::init);
}

// check a number is power of two.
inline bool isPOT(int number)
{
    return ((number > 0) && (number & (number - 1)) == 0);
}

// Convert ASCII hex digit to a nibble (four bits, 0 - 15).
//
// Use unsigned to avoid signed overflow UB.
inline unsigned char hex2nibble(unsigned char c)
{
    if (c >= '0' && c <= '9')
    {
        return c - '0';
    }
    else if (c >= 'a' && c <= 'f')
    {
        return 10 + (c - 'a');
    }
    else if (c >= 'A' && c <= 'F')
    {
        return 10 + (c - 'A');
    }
    return 0;
}

// Convert a nibble ASCII hex digit
inline unsigned char nibble2hex(unsigned char c, unsigned char a = 'a')
{
    return ((c) < 0xa ? ((c) + '0') : ((c) + a - 10));
}

// Convert ASCII hex string (two characters) to byte.
//
// E.g., "0B" => 0x0B, "af" => 0xAF.
inline char hex2char(const char* p)
{
    return hex2nibble((uint8_t)p[0]) << 4 | hex2nibble(p[1]);
}

// Convert byte to ASCII hex string (two characters).
inline char* char2hex(char* p, unsigned char c, unsigned char a = 'a')
{
    p[0] = nibble2hex(c >> 4, a);
    p[1] = nibble2hex(c & (uint8_t)0xf, a);
    return p;
}

AX_DLL std::string urlEncode(std::string_view s);

AX_DLL std::string urlDecode(std::string_view st);

/**
 * Encodes bytes into a 64base buffer
 * @returns base64 encoded string
 *
 @since axmol-2.1.0
 */
AX_DLL std::string base64Encode(const void* in, size_t inlen);

template<typename _Ty, size_t _Extent = std::dynamic_extent>
inline std::string base64Encode(std::span<_Ty, _Extent> in)
{
    return base64Encode(in.data(), in.size_bytes());
}

/**
 * Decodes a 64base encoded buffer
 * @returns palintext
 *
 @since axmol-1.0.0
 */
AX_DLL yasio::byte_buffer base64Decode(std::string_view s);

/**
 * Encodes bytes into a 64base encoded memory with terminating '\0' character.
 * The encoded memory is expected to be freed by the caller by calling `free()`
 *
 * @returns the length of the out buffer
 *
 @since v2.1.4, axmol-1.0.0 move from namespace ax to ax::utils
 */
AX_DLL int base64Encode(const unsigned char* in, unsigned int inLength, char** out);

/**
 * Decodes a 64base encoded memory. The decoded memory is
 * expected to be freed by the caller by calling `free()`
 *
 * @returns the length of the out buffer
 *
 @since v0.8.1, axmol-1.0.0 move from namespace ax to ax::utils
 */
AX_DLL int base64Decode(const unsigned char* in, unsigned int inLength, unsigned char** out);

AX_DLL uint32_t fourccValue(std::string_view str);

}  // namespace utils

NS_AX_END

#endif  // __SUPPORT_AX_UTILS_H__
