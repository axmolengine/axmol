/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
 Copyright (c) 2020 C4games Ltd

 https://axmolengine.github.io/

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

#include "DeviceInfoGL.h"
#include "platform/GL.h"
#include "base/axstd.h"

#if !defined(GL_COMPRESSED_RGBA8_ETC2_EAC)
#    define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#endif

#if !defined(GL_COMPRESSED_RGBA_ASTC_4x4)
#    define GL_COMPRESSED_RGBA_ASTC_4x4 0x93B0
#endif

NS_AX_BACKEND_BEGIN

static GLuint compileShader(GLenum shaderType, const GLchar* source)
{
    auto shader = glCreateShader(shaderType);
    if (!shader)
        return 0;

    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);

    GLint status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status)
        return shader;

    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

    if (logLength > 1)
    {
        auto errorLog = axstd::make_unique_for_overwrite<char[]>(static_cast<size_t>(logLength));
        glGetShaderInfoLog(shader, logLength, nullptr, static_cast<GLchar*>(errorLog.get()));
        ax::print("axmol:ERROR: Failed to compile shader, detail: %s\n%s", errorLog.get(), source);
    }
    else
    {
        ax::print("axmol:ERROR: Failed to compile shader without errors.");
    }

    glDeleteShader(shader);
    return 0;
}

static void setTexParams() {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

/// <summary>
///  render 1x1 to back framebuffer to detect whether GPU support astc compressed texture really
/// </summary>
/// <returns>true: support, false: not support</returns>
static bool checkReallySupportsASTC()
{
    // 1x1/2x2 astc 4x4 compressed texels srgb
    uint8_t astctexels[] = {0xfc, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0xff, 0xff, 0x80, 0x80, 0x00, 0x00, 0xff, 0xff};

    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    setTexParams();

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glCompressedTexImage2D(GL_TEXTURE_2D,
                           0,                                // level
                           GL_COMPRESSED_RGBA_ASTC_4x4_KHR,  // format
                           1, 1, // texture dim
                           0,                   // border
                           sizeof(astctexels),  // dataLen,
                           astctexels);

    bool matched = false;
    auto error = glGetError();
    if (!error)
    {
        // prepare a back frame buffer
        GLuint defaultFBO{0};
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&defaultFBO));

        GLuint fbo{0};
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        /// color attachment0
        GLuint colorAttachment = 0;
        glGenTextures(1, &colorAttachment);
        glBindTexture(GL_TEXTURE_2D, colorAttachment);
        setTexParams();

        uint8_t black1x1[] = {0x00, 0x00, 0x00, 0x00};  // 1*1*4
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, black1x1);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorAttachment, 0);
        //GLenum buffers[1] = {GL_COLOR_ATTACHMENT0};
        //glDrawBuffers(1, buffers);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        { // does back framebuffer reandy for render
            // render 1 frame to back framebuffer
            // set up vertex data (and buffer(s)) and configure vertex attributes
            // ------------------------------------------------------------------
            float vertices[] = {
                // positions        // texture coords
                1.0f,  1.0f,  0.0f, 1.0f, 1.0f,  // top right
                1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom left
                -1.0f, 1.0f,  0.0f, 0.0f, 1.0f   // top left
            };
            unsigned int indices[] = {
                0, 1, 3,  // first triangle
                1, 2, 3   // second triangle
            };
            GLuint defaultVAO{0};
            glGetIntegerv(GL_VERTEX_ARRAY_BINDING, reinterpret_cast<GLint*>(&defaultVAO));

            unsigned int VBO, VAO, EBO;
            glGenVertexArrays(1, &VAO);
            glGenBuffers(1, &VBO);
            glGenBuffers(1, &EBO);

            glBindVertexArray(VAO);

            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

            // position attribute
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<const void*>(0));
            glEnableVertexAttribArray(0);
            // texture coord attribute
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<const void*>(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // create shader program
            const char* vertexShader = R"(#version 100
precision highp float;
attribute vec3 aPos;
attribute vec2 aTexCoord;
varying vec2 TexCoord;
void main()
{
	gl_Position = vec4(aPos, 1.0);
	TexCoord = vec2(aTexCoord.x, aTexCoord.y);
})";

            const char* fragShader = R"(#version 100
precision highp float;
varying vec2 TexCoord;
uniform sampler2D u_tex0;
void main()
{
	gl_FragColor = texture2D(u_tex0, TexCoord);
}
)";
            auto vShader = compileShader(GL_VERTEX_SHADER, vertexShader);
            auto fShader = compileShader(GL_FRAGMENT_SHADER, fragShader);
            auto program = glCreateProgram();
            glAttachShader(program, vShader);
            glAttachShader(program, fShader);
            glLinkProgram(program);

            // render frame
            glUseProgram(program);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texID);
            glUniform1i(glGetUniformLocation(program, "u_tex0"), 0);

            glClearColor(0.0, 0.0, 0.0, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

            // read pixel RGB: should be: 255, 128, 0
            uint8_t pixels[4] = {0};
            glReadPixels(0, 0, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
            matched = (pixels[0] == 255 || pixels[1] == 128);

            // clean render resources: VBO, VAO, EBO, program, vShader, fShader
            glDeleteBuffers(1, &VBO);
            glDeleteBuffers(1, &EBO);
            glDeleteVertexArrays(1, &VAO);
            glDeleteProgram(program);
            glDeleteShader(vShader);
            glDeleteShader(fShader);

            // restore binding to defaultVAO
            glBindVertexArray(defaultVAO);
        }

        // clean framebuffer resources
        glDeleteTextures(1, &colorAttachment);
        glDeleteFramebuffers(1, &fbo);

        // restore binding to defaultFBO
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
    }

    // clean test astc texture
    glDeleteTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, 0);

    return matched;
}

bool DeviceInfoGL::init()
{
    // These queries work with all GL/GLES versions!
    _vendor    = (char const*)glGetString(GL_VENDOR);
    _renderer  = (char const*)glGetString(GL_RENDERER);
    _version   = (char const*)glGetString(GL_VERSION);
    _shaderVer = (char const*)glGetString(GL_SHADING_LANGUAGE_VERSION);

    // caps
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_maxAttributes);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_maxTextureUnits);

    // exts
#if AX_GLES_PROFILE != 200 // NOT GLES2.0
    GLint NumberOfExtensions{0};
    glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
    for (GLint i = 0; i < NumberOfExtensions; ++i)
    {
        auto extName = (const char*)glGetStringi(GL_EXTENSIONS, i);
        if (extName)
            _glExtensions.emplace(extName);
    }
#else
    auto extensions = (const char*)glGetString(GL_EXTENSIONS);
    if (extensions)
        _glExtensions    = extensions;
#endif
    return true;
}

const char* DeviceInfoGL::getVendor() const
{
    return _vendor;
}
const char* DeviceInfoGL::getRenderer() const
{
    return _renderer;
}

const char* DeviceInfoGL::getVersion() const
{
    return _version;
}

const char* DeviceInfoGL::getShaderVersion() const
{
    return _shaderVer;
}

bool DeviceInfoGL::checkForFeatureSupported(FeatureType feature)
{
    bool featureSupported = false;
    switch (feature)
    {
    case FeatureType::ETC1:
        featureSupported = hasExtension("GL_OES_compressed_ETC1_RGB8_texture"sv);
        break;
    case FeatureType::ETC2:
        featureSupported = checkSupportsCompressedFormat(GL_COMPRESSED_RGBA8_ETC2_EAC);
        break;
    case FeatureType::S3TC:
#ifdef GL_EXT_texture_compression_s3tc
        featureSupported = hasExtension("GL_EXT_texture_compression_s3tc"sv);
#endif
        break;
    case FeatureType::AMD_COMPRESSED_ATC:
        featureSupported = hasExtension("GL_AMD_compressed_ATC_texture"sv);
        break;
    case FeatureType::PVRTC:
        featureSupported = hasExtension("GL_IMG_texture_compression_pvrtc"sv);
        break;
    case FeatureType::IMG_FORMAT_BGRA8888:
        featureSupported = hasExtension("GL_IMG_texture_format_BGRA8888"sv);
        break;
    case FeatureType::DISCARD_FRAMEBUFFER:
        featureSupported = hasExtension("GL_EXT_discard_framebuffer"sv);
        break;
    case FeatureType::PACKED_DEPTH_STENCIL:
        featureSupported = hasExtension("GL_OES_packed_depth_stencil"sv);
        break;
    case FeatureType::VAO:
#ifdef AX_PLATFORM_PC
        featureSupported = hasExtension("vertex_array_object"sv);
#else
        featureSupported = hasExtension("GL_OES_vertex_array_object"sv);
#endif
        break;
    case FeatureType::MAPBUFFER:
        featureSupported = hasExtension("GL_OES_mapbuffer"sv);
        break;
    case FeatureType::DEPTH24:
        featureSupported = hasExtension("GL_OES_depth24"sv);
        break;
    case FeatureType::ASTC:
        featureSupported = checkReallySupportsASTC();
        break;
    default:
        break;
    }
    return featureSupported;
}

bool DeviceInfoGL::hasExtension(std::string_view searchName) const
{
#if AX_GLES_PROFILE != 200
    return _glExtensions.find(searchName) != _glExtensions.end();
#else
    return _glExtensions.find(searchName) != std::string::npos;
#endif
}

std::string DeviceInfoGL::dumpExtensions() const
{
#if AX_GLES_PROFILE != 200
    std::string strExts;
    for (auto& extName : _glExtensions)
    {
        strExts += extName;
        strExts += ',';
    }
    return strExts;
#else
    return _glExtensions;
#endif
}

bool DeviceInfoGL::checkSupportsCompressedFormat(int compressedFormat)
{
    const int MAX_ALLOCA_SIZE = 512;

    GLint numFormats = 0;
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numFormats);
    GLint* formats = nullptr;
    int buffersize = numFormats * sizeof(GLint);
    if (buffersize <= MAX_ALLOCA_SIZE)
        formats = (GLint*)alloca(buffersize);
    else
        formats = (GLint*)malloc(buffersize);
    glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, formats);

    bool supported = false;
    for (GLint i = 0; i < numFormats; ++i)
    {
        if (formats[i] == compressedFormat)
        {
            supported = true;
            break;
        }
    }

    if (buffersize > MAX_ALLOCA_SIZE)
        free(formats);

    return supported;
}

NS_AX_BACKEND_END
