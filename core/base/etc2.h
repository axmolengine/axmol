/****************************************************************************
 Copyright (c) 2010-2012 cocos2d-x.org
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

/* The etc1/etc2 compression tools
 * https://developer.arm.com/tools-and-software/graphics-and-gaming/mali-texture-compression-tool
 * https://github.com/google/etc2comp
 */

#ifndef __etc2_h__
#define __etc2_h__
/// @cond DO_NOT_SHOW

typedef unsigned char etc2_byte;
typedef int etc2_bool;
typedef unsigned int etc2_uint32;

#ifndef GL_COMPRESSED_RGB8_ETC2
#    define GL_COMPRESSED_RGB8_ETC2 0x9274
#endif

#ifndef GL_COMPRESSED_RGBA8_ETC2_EAC
#    define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#endif

// Size of a PKM header, in bytes.
#define ETC2_PKM_HEADER_SIZE 16

#define ETC2_RGB_NO_MIPMAPS 1
#define ETC2_RGBA_NO_MIPMAPS 3

// Check if a PKM header is correctly formatted.

etc2_bool etc2_pkm_is_valid(const etc2_byte* pHeader);

// Read the image width from a PKM header
etc2_uint32 etc2_pkm_get_width(const etc2_byte* pHeader);

// Read the image height from a PKM header
etc2_uint32 etc2_pkm_get_height(const etc2_byte* pHeader);

// Read the image format from a PKM header
etc2_uint32 etc2_pkm_get_format(const etc2_byte* pHeader);

/// <summary>
/// Decode ETC2_RGB/ETC2_RGBA to RGBA8888
/// Copy from:  https://github.com/google/angle/tree/master/src/image_util
/// </summary>
/// <param name="format">Must be ETC2_RGBA_NO_MIPMAPS or ETC2_RGB_NO_MIPMAPS</param>
/// <param name="input"></param>
/// <param name="output"></param>
/// <param name="width">pixelsHeight</param>
/// <param name="height">pixelsWidth</param>
/// <returns>0: success, -1: failed</returns>
int etc2_decode_image(int format, const etc2_byte* input, etc2_byte* output, etc2_uint32 width, etc2_uint32 height);

/// @endcond
#endif
