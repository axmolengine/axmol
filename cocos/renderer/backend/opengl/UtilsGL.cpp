/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2020 c4games.com

 http://www.cocos2d-x.org

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
 
#include "UtilsGL.h"
#include "ProgramGL.h"
#include "TextureGL.h"
#include "renderer/backend/Types.h"
#include "renderer/backend/TextureUtils.h"
#include "platform/CCGL.h"

 // In desktop OpenGL 4+ and OpenGL ES 3.0+, specific GL formats GL_x_INTEGER are used for integer textures.
 // For older desktop OpenGL contexts, GL names without _INTEGER suffix were used.
 // See http://docs.gl/gl4/glTexImage2D, http://docs.gl/gl3/glTexImage2D, http://docs.gl/es3/glTexImage2D
#if defined(GL_VERSION_4_0) || defined(CC_USE_GLES)
#	define RED_INTEGER  GL_RED_INTEGER
#	define RG_INTEGER   GL_RG_INTEGER
#	define RGB_INTEGER  GL_RGB_INTEGER
#	define RGBA_INTEGER GL_RGBA_INTEGER
#else
#	define RED_INTEGER  GL_RED
#	define RG_INTEGER   GL_RG
#	define RGB_INTEGER  GL_RGB
#	define RGBA_INTEGER GL_RGBA
#endif

CC_BACKEND_BEGIN

struct GPUTextureFormatInfo
{
    GLenum internalFmt;
    GLenum internalFmtSrgb;
    GLenum fmt;
    GLenum fmtSrgb;
    GLenum type;
};

static GPUTextureFormatInfo s_textureFormats[] =
{
    /* pvrtc v1 */
    { GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,          GL_COMPRESSED_SRGB_PVRTC_4BPPV1_EXT,          GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,   GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG,                 GL_ZERO, }, // PVRTC4
    { GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,         GL_COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT,    GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,  GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,                GL_ZERO, }, // PVRTC4A
    { GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,          GL_COMPRESSED_SRGB_PVRTC_2BPPV1_EXT,          GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,  GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG,                  GL_ZERO, },  // PVRTC2
    { GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,         GL_COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT,    GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,  GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,                GL_ZERO, }, // PVRTC2A

    // { GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,         GL_ZERO,                                      GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,  GL_COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,                GL_ZERO, }, // PVRTC4A v2
    // { GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,         GL_ZERO,                                      GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,  GL_COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,                GL_ZERO, }, // PVRTC2A v2

    /* etc */
    { GL_ETC1_RGB8_OES,                            GL_ZERO,                                      GL_ETC1_RGB8_OES,                            GL_ETC1_RGB8_OES,                            GL_ZERO, }, // ETC1
    { GL_COMPRESSED_RGB8_ETC2,                     GL_COMPRESSED_SRGB8_ETC2,                     GL_COMPRESSED_RGB8_ETC2,                     GL_COMPRESSED_RGB8_ETC2,                     GL_ZERO, }, // ETC2
    { GL_COMPRESSED_RGBA8_ETC2_EAC,                GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,          GL_COMPRESSED_RGBA8_ETC2_EAC,                GL_COMPRESSED_RGBA8_ETC2_EAC,                GL_ZERO, }, // ETC2A

    /* s3tc */
    { GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,       GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,            GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,            GL_ZERO, }, // S3TC_DXT1
    { GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,       GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            GL_COMPRESSED_RGBA_S3TC_DXT3_EXT,            GL_ZERO, }, // S3TC_DXT3
    { GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,       GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            GL_COMPRESSED_RGBA_S3TC_DXT5_EXT,            GL_ZERO, }, // S3TC_DXT5

    /* atc */
    { GL_ATC_RGB_AMD,                              GL_ZERO,                                      GL_ATC_RGB_AMD,                              GL_ATC_RGB_AMD,                              GL_ZERO, }, // ATC
    { GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,              GL_ZERO,                                      GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,              GL_ATC_RGBA_EXPLICIT_ALPHA_AMD,              GL_ZERO, }, // ATCE
    { GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,          GL_ZERO,                                      GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,          GL_ATC_RGBA_INTERPOLATED_ALPHA_AMD,          GL_ZERO, }, // ATCI

    /* astc */
    { GL_COMPRESSED_RGBA_ASTC_4x4_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,      GL_COMPRESSED_RGBA_ASTC_4x4_KHR,             GL_COMPRESSED_RGBA_ASTC_4x4_KHR,             GL_ZERO, }, // ASTC4x4
    { GL_COMPRESSED_RGBA_ASTC_5x5_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,      GL_COMPRESSED_RGBA_ASTC_5x5_KHR,             GL_COMPRESSED_RGBA_ASTC_5x5_KHR,             GL_ZERO, }, // ASTC5x5
    { GL_COMPRESSED_RGBA_ASTC_6x6_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,      GL_COMPRESSED_RGBA_ASTC_6x6_KHR,             GL_COMPRESSED_RGBA_ASTC_6x6_KHR,             GL_ZERO, }, // ASTC6x6
    { GL_COMPRESSED_RGBA_ASTC_8x5_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,      GL_COMPRESSED_RGBA_ASTC_8x5_KHR,             GL_COMPRESSED_RGBA_ASTC_8x5_KHR,             GL_ZERO, }, // ASTC8x5
    { GL_COMPRESSED_RGBA_ASTC_8x6_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,      GL_COMPRESSED_RGBA_ASTC_8x6_KHR,             GL_COMPRESSED_RGBA_ASTC_8x6_KHR,             GL_ZERO, }, // ASTC8x6
    { GL_COMPRESSED_RGBA_ASTC_8x8_KHR,             GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,      GL_COMPRESSED_RGBA_ASTC_8x8_KHR,             GL_COMPRESSED_RGBA_ASTC_8x8_KHR,             GL_ZERO, }, // ASTC8x8
    { GL_COMPRESSED_RGBA_ASTC_10x5_KHR,            GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,     GL_COMPRESSED_RGBA_ASTC_10x5_KHR,            GL_COMPRESSED_RGBA_ASTC_10x5_KHR,            GL_ZERO, }, // ASTC10x5

    /* nomral */
    { GL_RGBA,                                     GL_SRGB8_ALPHA8_EXT,                          GL_RGBA,                                     GL_RGBA,                                     GL_UNSIGNED_BYTE, }, // RGBA8
    { GL_RGBA,                                     GL_SRGB8_ALPHA8_EXT,                          GL_BGRA_EXT,                                 GL_BGRA_EXT,                                 GL_UNSIGNED_BYTE, }, // BGRA8
    { GL_RGB,                                      GL_SRGB_EXT,                                  GL_RGB,                                      GL_RGB,                                      GL_UNSIGNED_BYTE, }, // RGB8
    { GL_RGB565,                                   GL_ZERO,                                      GL_RGB,                                      GL_RGB,                                      GL_UNSIGNED_SHORT_5_6_5}, // RGB565 TO-CHECK: match to MTLPixelFormatB5G6R5Unorm
    { GL_RGBA,                                     GL_ZERO,                                      GL_RGBA,                                     GL_RGBA,                                     GL_UNSIGNED_SHORT_4_4_4_4}, // RGBA4 === MTLPixelFormatABGR4Unorm
    { GL_RGB5_A1,                                  GL_ZERO,                                      GL_BGRA_EXT,                                 GL_BGRA_EXT/*GL_BGRA_EXT*/,                  GL_UNSIGNED_SHORT_1_5_5_5_REV_EXT}, // RGB5A1 TO-CHECK: match to MTLPixelFormatBGR5A1Unorm
    { GL_ALPHA,                                    GL_ZERO,                                      GL_ALPHA,                                    GL_ALPHA,                                    GL_UNSIGNED_BYTE, }, // A8
    { GL_LUMINANCE,                                GL_ZERO,                                      GL_LUMINANCE,                                GL_LUMINANCE,                                GL_UNSIGNED_BYTE, }, // L8
    { GL_LUMINANCE_ALPHA,                          GL_ZERO,                                      GL_LUMINANCE_ALPHA,                          GL_LUMINANCE_ALPHA,                          GL_UNSIGNED_BYTE, }, // LA8
    
    /* depth stencil internalFormat | internalFormatSrgb | format | formatSrgb | type */
#if defined(CC_USE_GLES)
    { GL_DEPTH_STENCIL_OES,                        GL_ZERO,                                      GL_DEPTH_STENCIL_OES,                       GL_DEPTH_STENCIL_OES,                        GL_UNSIGNED_INT_24_8_OES, }, // D24S8
#else
    { GL_DEPTH24_STENCIL8,                         GL_ZERO,                                      GL_DEPTH_STENCIL,                            GL_DEPTH_STENCIL,                            GL_UNSIGNED_INT_24_8, }, // D24S8
#endif
};
static_assert(CC_ARRAYSIZE(s_textureFormats) == (int)PixelFormat::COUNT, "The OpenGL GPU texture format info table incomplete!");

/*
* !!!Note:
* PixelFormat::RGBA4: 
*   GL_BGRA_EXT, GL_BGRA_EXT, GL_UNSIGNED_SHORT_4_4_4_4_REV_EXT not match MTLPixelFormatABGR4Unorm
*   Unexpected render result is FPS label is PAN-BLUE
*/

GLenum UtilsGL::toGLAttributeType(VertexFormat vertexFormat)
{
    GLenum ret = GL_INT;
    switch (vertexFormat)
    {
    case VertexFormat::FLOAT4:
    case VertexFormat::FLOAT3:
    case VertexFormat::FLOAT2:
    case VertexFormat::FLOAT:
        ret = GL_FLOAT;
        break;
    case VertexFormat::INT4:
    case VertexFormat::INT3:
    case VertexFormat::INT2:
    case VertexFormat::INT:
        ret = GL_INT;
        break;
    case VertexFormat::UBYTE4:
        ret = GL_UNSIGNED_BYTE;
        break;
    default:
        break;
    }
    return ret;
}

GLsizei UtilsGL::getGLAttributeSize(VertexFormat vertexFormat)
{
    GLsizei ret = 0;
    switch (vertexFormat)
    {
    case VertexFormat::FLOAT4:
    case VertexFormat::INT4:
    case VertexFormat::UBYTE4:
        ret = 4;
        break;
    case VertexFormat::FLOAT3:
    case VertexFormat::INT3:
        ret = 3;
        break;
    case VertexFormat::FLOAT2:
    case VertexFormat::INT2:
        ret = 2;
        break;
    case VertexFormat::FLOAT:
    case VertexFormat::INT:
        ret = 1;
        break;
    default:
        break;
    }
    return ret;
}

GLsizei UtilsGL::getGLDataTypeSize(GLenum size)
{
    GLsizei ret = 0;
    switch (size)
    {
    case GL_BOOL:
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
        ret = sizeof(GLbyte);
        break;
    case GL_BOOL_VEC2:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
        ret = sizeof(GLshort);
        break;
    case GL_BOOL_VEC3:
        ret = sizeof(GLboolean);
        break;
    case GL_BOOL_VEC4:
    case GL_INT:
    case GL_UNSIGNED_INT:
    case GL_FLOAT:
        ret = sizeof(GLfloat);
        break;
    case GL_FLOAT_VEC2:
    case GL_INT_VEC2:
        ret = sizeof(GLfloat) * 2;
        break;
    case GL_FLOAT_VEC3:
    case GL_INT_VEC3:
        ret = sizeof(GLfloat) * 3;
        break;
    case GL_FLOAT_MAT2:
    case GL_FLOAT_VEC4:
    case GL_INT_VEC4:
        ret = sizeof(GLfloat) * 4;
        break;
    case GL_FLOAT_MAT3:
        ret = sizeof(GLfloat) * 9;
        break;
    case GL_FLOAT_MAT4:
        ret = sizeof(GLfloat) * 16;
        break;
    default:
        break;
    }
    return ret;
}


GLint UtilsGL::toGLMagFilter(SamplerFilter magFilter)
{
    GLint ret = GL_LINEAR;
    switch (magFilter)
    {
    case SamplerFilter::LINEAR:
        ret = GL_LINEAR;
        break;
    case SamplerFilter::NEAREST:
        ret = GL_NEAREST;
        break;
    default:
        break;
    }
    return ret;
}

GLint UtilsGL::toGLMinFilter(SamplerFilter minFilter, bool hasMipmaps, bool isPow2)
{
    if (hasMipmaps && !isPow2)
    {
        CCLOG("Change minification filter to either NEAREST or LINEAR since non-power-of-two texture occur in %s %s %d", __FILE__, __FUNCTION__, __LINE__);
        if (SamplerFilter::LINEAR == minFilter)
            return GL_LINEAR;
        else
            return GL_NEAREST;
    }

    switch (minFilter)
    {
        case SamplerFilter::LINEAR:
            return GL_LINEAR;
        case SamplerFilter::LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
        case SamplerFilter::LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        case SamplerFilter::NEAREST:
            return GL_NEAREST;
        case SamplerFilter::NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case SamplerFilter::NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        default:
            break;
    }

    return GL_NEAREST;
}

GLint UtilsGL::toGLAddressMode(SamplerAddressMode addressMode, bool isPow2)
{
    GLint ret = GL_REPEAT;
    if (!isPow2 && (addressMode != SamplerAddressMode::CLAMP_TO_EDGE))
    {
        CCLOG("Change texture wrap mode to CLAMP_TO_EDGE since non-power-of-two texture occur in %s %s %d", __FILE__, __FUNCTION__, __LINE__);
        return GL_CLAMP_TO_EDGE;
    }

    switch (addressMode)
    {
    case SamplerAddressMode::REPEAT:
        ret = GL_REPEAT;
        break;
    case SamplerAddressMode::MIRROR_REPEAT:
        ret = GL_MIRRORED_REPEAT;
        break;
    case SamplerAddressMode::CLAMP_TO_EDGE:
        ret = GL_CLAMP_TO_EDGE;
        break;
    default:
        break;
    }
    return ret;
}


void UtilsGL::toGLTypes(PixelFormat textureFormat, GLint &internalFormat, GLuint &format, GLenum &type, bool &isCompressed)
{
    if (UTILS_LIKELY(textureFormat < PixelFormat::COUNT)) {
        auto& info = s_textureFormats[(int)textureFormat];
        internalFormat = info.internalFmt;
        format = info.fmt;
        type = info.type;
        isCompressed = PixelFormatUtils::isCompressed(textureFormat);
    }
}


GLenum UtilsGL::toGLComareFunction(CompareFunction compareFunction)
{
    GLenum ret = GL_ALWAYS;
    switch (compareFunction)
    {
    case CompareFunction::NEVER:
        ret = GL_NEVER;
        break;
    case CompareFunction::LESS:
        ret = GL_LESS;
        break;
    case CompareFunction::LESS_EQUAL:
        ret = GL_LEQUAL;
        break;
    case CompareFunction::GREATER:
        ret = GL_GREATER;
        break;
    case CompareFunction::GREATER_EQUAL:
        ret = GL_GEQUAL;
        break;
    case CompareFunction::NOT_EQUAL:
        ret = GL_NOTEQUAL;
        break;
    case CompareFunction::EQUAL:
        ret = GL_EQUAL;
        break;
    case CompareFunction::ALWAYS:
        ret = GL_ALWAYS;
        break;
    default:
        break;
    }
    return ret;
}

GLenum UtilsGL::toGLStencilOperation(StencilOperation stencilOperation)
{
    GLenum ret = GL_KEEP;
    switch (stencilOperation)
    {
    case StencilOperation::KEEP:
        ret = GL_KEEP;
        break;
    case StencilOperation::ZERO:
        ret = GL_ZERO;
        break;
    case StencilOperation::REPLACE:
        ret = GL_REPLACE;
        break;
    case StencilOperation::INVERT:
        ret = GL_INVERT;
        break;
    case StencilOperation::INCREMENT_WRAP:
        ret = GL_INCR_WRAP;
        break;
    case StencilOperation::DECREMENT_WRAP:
        ret = GL_DECR_WRAP;
        break;
    default:
        break;
    }
    return ret;
}

GLenum UtilsGL::toGLBlendOperation(BlendOperation blendOperation)
{
    GLenum ret = GL_FUNC_ADD;
    switch (blendOperation)
    {
    case BlendOperation::ADD:
        ret = GL_FUNC_ADD;
        break;
    case BlendOperation::SUBTRACT:
        ret = GL_FUNC_SUBTRACT;
        break;
    case BlendOperation::RESERVE_SUBTRACT:
        ret = GL_FUNC_REVERSE_SUBTRACT;
        break;
    default:
        break;
    }
    return ret;
}

GLenum UtilsGL::toGLBlendFactor(BlendFactor blendFactor)
{
    GLenum ret = GL_ONE;
    switch (blendFactor)
    {
    case BlendFactor::ZERO:
        ret = GL_ZERO;
        break;
    case BlendFactor::ONE:
        ret = GL_ONE;
        break;
    case BlendFactor::SRC_COLOR:
        ret = GL_SRC_COLOR;
        break;
    case BlendFactor::ONE_MINUS_SRC_COLOR:
        ret = GL_ONE_MINUS_SRC_COLOR;
        break;
    case BlendFactor::SRC_ALPHA:
        ret = GL_SRC_ALPHA;
        break;
    case BlendFactor::ONE_MINUS_SRC_ALPHA:
        ret = GL_ONE_MINUS_SRC_ALPHA;
        break;
    case BlendFactor::DST_COLOR:
        ret = GL_DST_COLOR;
        break;
    case BlendFactor::ONE_MINUS_DST_COLOR:
        ret = GL_ONE_MINUS_DST_COLOR;
        break;
    case BlendFactor::DST_ALPHA:
        ret = GL_DST_ALPHA;
        break;
    case BlendFactor::ONE_MINUS_DST_ALPHA:
        ret = GL_ONE_MINUS_DST_ALPHA;
        break;
    case BlendFactor::SRC_ALPHA_SATURATE:
        ret = GL_SRC_ALPHA_SATURATE;
        break;
    case BlendFactor::BLEND_CLOLOR:
        ret = GL_BLEND_COLOR;
        break;
    default:
        break;
    }
    return ret;
}

GLenum UtilsGL::toGLFrontFace(Winding winding)
{
    if (Winding::CLOCK_WISE == winding)
        return GL_CW;
    else
        return GL_CCW;
}

GLenum UtilsGL::toGLPrimitiveType(PrimitiveType primitiveType)
{
    GLenum ret = GL_TRIANGLES;
    switch (primitiveType)
    {
    case PrimitiveType::POINT:
        ret = GL_POINTS;
        break;
    case PrimitiveType::LINE:
        ret = GL_LINES;
        break;
    case PrimitiveType::LINE_STRIP:
        ret = GL_LINE_STRIP;
        break;
    case PrimitiveType::TRIANGLE:
        ret = GL_TRIANGLES;
        break;
    case PrimitiveType::TRIANGLE_STRIP:
        ret = GL_TRIANGLE_STRIP;
        break;
    default:
        break;
    }
    return ret;
}

GLenum UtilsGL::toGLIndexType(IndexFormat indexType)
{
    GLenum ret = GL_BYTE;
    switch (indexType)
    {
    case IndexFormat::U_INT:
        ret = GL_UNSIGNED_INT;
        break;
    case IndexFormat::U_SHORT:
        ret = GL_UNSIGNED_SHORT;
        break;
    default:
        break;
    }
    return ret;
}

GLenum UtilsGL::toGLCullMode(CullMode mode)
{
    if (CullMode::BACK == mode)
        return GL_BACK;
    else
        return GL_FRONT;
}

CC_BACKEND_END
