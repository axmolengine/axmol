/****************************************************************************
 Copyright (c) 2018-2019 Xiamen Yaji Software Co., Ltd.
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

#include "DriverGL.h"
#include "RenderPipelineGL.h"
#include "BufferGL.h"
#include "ShaderModuleGL.h"
#include "CommandBufferGL.h"
#include "TextureGL.h"
#include "DepthStencilStateGL.h"
#include "ProgramGL.h"
#include "DriverGL.h"
#include "RenderTargetGL.h"
#include "MacrosGL.h"
#include "renderer/backend/ProgramManager.h"
#if !defined(__APPLE__) && AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
#    include "CommandBufferGLES2.h"
#endif

#include "base/axstd.h"
#include "xxhash/xxhash.h"

#if !defined(GL_COMPRESSED_RGBA8_ETC2_EAC)
#    define GL_COMPRESSED_RGBA8_ETC2_EAC 0x9278
#endif

#if !defined(GL_COMPRESSED_RGBA_ASTC_4x4)
#    define GL_COMPRESSED_RGBA_ASTC_4x4 0x93B0
#endif

NS_AX_BACKEND_BEGIN

static inline uint32_t hashString(std::string_view str)
{
    return !str.empty() ? XXH32(str.data(), str.length(), 0) : 0;
}

template <typename _Fty>
static void GL_EnumAllExtensions(_Fty&& func)
{
#if AX_GLES_PROFILE != 200  // NOT GLES2.0
    GLint NumberOfExtensions{0};
    glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
    for (GLint i = 0; i < NumberOfExtensions; ++i)
    {
        auto extName = (const char*)glGetStringi(GL_EXTENSIONS, i);
        if (extName)
            func(std::string_view{extName});
    }
#else
    auto extensions = (const char*)glGetString(GL_EXTENSIONS);
    if (extensions)
    {
        axstd::split_of_cb(extensions, " \f\n\r\t\v", [&func](const char* start, const char* end, char /*delim*/) {
            if (start != end)
                func(std::string_view{start, static_cast<size_t>(end - start)});
        });
    }
#endif
}

DriverBase* DriverBase::getInstance()
{
    if (!_instance)
        _instance = new DriverGL();

    return _instance;
}

void DriverBase::destroyInstance()
{
    AX_SAFE_DELETE(_instance);
}

DriverGL::DriverGL()
{
    /// driver info
    // These queries work with all GL/GLES versions!
    _vendor    = (char const*)glGetString(GL_VENDOR);
    _renderer  = (char const*)glGetString(GL_RENDERER);
    _version   = (char const*)glGetString(GL_VERSION);
    _shaderVer = (char const*)glGetString(GL_SHADING_LANGUAGE_VERSION);

    if (_version)
    {
        auto hint   = strstr(_version, "OpenGL ES");
        _verInfo.es = !!hint;
        if (_verInfo.es)
        {
            _verInfo.major = hint[10] - '0';
            _verInfo.minor = hint[12] - '0';
        }
        else
        {
            _verInfo.major = (_version[0] - '0');
            _verInfo.minor = (_version[2] - '0');
        }
    }

    // check OpenGL version at first
    constexpr int REQUIRED_GLES_MAJOR = (AX_GLES_PROFILE / AX_GLES_PROFILE_DEN);
    if ((!_verInfo.es && (_verInfo.major < 3 || (_verInfo.major == 3 && _verInfo.minor < 3))) ||
        (_verInfo.es && _verInfo.major < REQUIRED_GLES_MAJOR))
    {
        char strComplain[256] = {0};
#if AX_GLES_PROFILE == 0
        sprintf(strComplain,
                "OpeGL 3.3+ is required (your version is %s). Please upgrade the driver of your video card.", _version);
#else
        sprintf(strComplain,
                "OpeGL ES %d.%d+ is required (your version is %s). Please upgrade the driver of your video card.",
                REQUIRED_GLES_MAJOR, AX_GLES_PROFILE % AX_GLES_PROFILE, _version);
#endif
        messageBox(strComplain, "OpenGL version too old");
        utils::killCurrentProcess();  // kill current process, don't cause crash when driver issue.
        return;
    }

    // caps
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &_maxAttributes);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize);
    glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &_maxTextureUnits);

    // exts
    GL_EnumAllExtensions([this](const std::string_view& ext) {
        const auto key = hashString(ext);
        _glExtensions.insert(key);
    });

    // texture compressions
    axstd::pod_vector<GLint> formats;
    GLint numFormats{0};
    glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &numFormats);
    if (numFormats > 0)
    {
        formats.resize(numFormats);
        glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, formats.data());

        const auto textureCompressions = std::set<int>{formats.begin(), formats.end()};

        if (textureCompressions.find(GL_COMPRESSED_RGBA_ASTC_4x4_KHR) != textureCompressions.end())
            _textureCompressionAstc = true;

        if (textureCompressions.find(GL_COMPRESSED_RGBA8_ETC2_EAC) != textureCompressions.end())
            _textureCompressionEtc2 = true;
    }

    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_defaultFBO);

#if AX_GLES_PROFILE != 200
    glGenVertexArrays(1, &_defaultVAO);
    glBindVertexArray(_defaultVAO);
    CHECK_GL_ERROR_DEBUG();
#endif
}

DriverGL::~DriverGL()
{
    ProgramManager::destroyInstance();
}

GLint DriverGL::getDefaultFBO() const
{
    return _defaultFBO;
}

CommandBuffer* DriverGL::newCommandBuffer()
{
#if !defined(__APPLE__) && AX_TARGET_PLATFORM != AX_PLATFORM_WINRT
    return !isGLES2Only() ? new CommandBufferGL() : new CommandBufferGLES2();
#else
    return new CommandBufferGL();
#endif
}

Buffer* DriverGL::newBuffer(std::size_t size, BufferType type, BufferUsage usage)
{
    return new BufferGL(size, type, usage);
}

TextureBackend* DriverGL::newTexture(const TextureDescriptor& descriptor)
{
    switch (descriptor.textureType)
    {
    case TextureType::TEXTURE_2D:
        return new Texture2DGL(descriptor);
    case TextureType::TEXTURE_CUBE:
        return new TextureCubeGL(descriptor);
    default:
        return nullptr;
    }
}

RenderTarget* DriverGL::newDefaultRenderTarget()
{
    auto rtGL = new RenderTargetGL(true, this);
    return rtGL;
}

RenderTarget* DriverGL::newRenderTarget(TextureBackend* colorAttachment,
                                        TextureBackend* depthAttachment,
                                        TextureBackend* stencilAttachhment)
{
    auto rtGL = new RenderTargetGL(false, this);
    rtGL->bindFrameBuffer();
    RenderTarget::ColorAttachment colors{{colorAttachment, 0}};
    rtGL->setColorAttachment(colors);
    rtGL->setDepthAttachment(depthAttachment);
    rtGL->setStencilAttachment(stencilAttachhment);
    return rtGL;
}

ShaderModule* DriverGL::newShaderModule(ShaderStage stage, std::string_view source)
{
    return new ShaderModuleGL(stage, source);
}

DepthStencilState* DriverGL::newDepthStencilState()
{
    return new DepthStencilStateGL();
}

RenderPipeline* DriverGL::newRenderPipeline()
{
    return new RenderPipelineGL();
}

Program* DriverGL::newProgram(std::string_view vertexShader, std::string_view fragmentShader)
{
    return new ProgramGL(vertexShader, fragmentShader);
}

void DriverGL::resetState()
{
    OpenGLState::reset();
}

/// below is driver info APIs

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
        AXLOGE("axmol:ERROR: Failed to compile shader, detail: {}\n{}", errorLog.get(), source);
    }
    else
    {
        AXLOGE("axmol:ERROR: Failed to compile shader without errors.");
    }

    glDeleteShader(shader);
    return 0;
}

/// <summary>
///  render 1x1 to back framebuffer to detect whether GPU support astc compressed texture really
/// </summary>
/// <returns>true: support, false: not support</returns>
static bool checkASTCRenderability()
{
    constexpr int TEX_DIM      = 1;
    constexpr int TEX_DATA_LEN = TEX_DIM * TEX_DIM * 4;

    // 1x1/2x2/4x4 astc 4x4 compressed texels srgb present solid orange color: rgba(255, 128, 0, 255)
    uint8_t astctexels[] = {0xfc, 0xfd, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0xff, 0xff, 0x80, 0x80, 0x00, 0x00, 0xff, 0xff};

    GLuint texID = 0;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glCompressedTexImage2D(GL_TEXTURE_2D,
                           0,                                // level
                           GL_COMPRESSED_RGBA_ASTC_4x4_KHR,  // format
                           1, 1,                             // texture dim
                           0,                                // border
                           sizeof(astctexels),               // dataLen,
                           astctexels);

    bool supported = false;
    auto error     = glGetError();
    if (!error)
    {
        uint8_t pixels[TEX_DATA_LEN] = {0};

#if !AX_GLES_PROFILE
        // read 1 pixel RGB: should be: 255, 128, 0
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        error     = glGetError();
        supported = !error && pixels[0] == 255 && pixels[1] == 128;
#else  // GLES no API: glGetTexImage
       // prepare a back frame buffer
        GLuint defaultFBO{0};
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&defaultFBO));

        GLuint fbo{0};
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);

        /// color attachment0
        GLuint colorAttachment{0};
        glGenRenderbuffers(1, &colorAttachment);
        glBindRenderbuffer(GL_RENDERBUFFER, colorAttachment);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, TEX_DIM, TEX_DIM);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorAttachment);

        // GLenum buffers[1] = {GL_COLOR_ATTACHMENT0};
        // glDrawBuffers(1, buffers);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
        {  // does back framebuffer ready for rendering
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

            // set up vertex data (and buffer(s)) and configure vertex attributes
            // ------------------------------------------------------------------
            float vertices[] = {
                // positions        // texture coords
                1.0f,  1.0f,  0.0f, 1.0f, 1.0f,  // top right
                1.0f,  -1.0f, 0.0f, 1.0f, 0.0f,  // bottom right
                -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,  // bottom left
                -1.0f, 1.0f,  0.0f, 0.0f, 1.0f   // top left
            };
            unsigned short indices[] = {
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
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                                  reinterpret_cast<const void*>(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            // create shader program
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

            // Some device can't draw our test texture to back buffer due to viewport is 0, i.e. iOS
            GLint viewport[4] = {0};
            glGetIntegerv(GL_VIEWPORT, viewport);
            glViewport(0, 0, TEX_DIM, TEX_DIM);

            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

            // read pixel RGB: should be: 255, 128, 0
            glReadPixels(0, 0, TEX_DIM, TEX_DIM, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

            // restore viewport after read rendered pixels
            glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

            supported = pixels[0] == 255 && pixels[1] == 128 && pixels[2] == 0;

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
        glDeleteRenderbuffers(1, &colorAttachment);
        glDeleteFramebuffers(1, &fbo);

        // restore binding to defaultFBO
        glBindFramebuffer(GL_FRAMEBUFFER, defaultFBO);
#endif
    }

    // clean test astc texture
    glDeleteTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, 0);

    return supported;
}

bool DriverGL::isGLES2Only() const
{
    return _verInfo.es && _verInfo.major == 2;
}

const char* DriverGL::getVendor() const
{
    return _vendor;
}
const char* DriverGL::getRenderer() const
{
    return _renderer;
}

const char* DriverGL::getVersion() const
{
    return _version;
}

const char* DriverGL::getShaderVersion() const
{
    return _shaderVer;
}

bool DriverGL::checkForFeatureSupported(FeatureType feature)
{
    bool featureSupported = false;
    switch (feature)
    {
    case FeatureType::ETC1:
        featureSupported = hasExtension("GL_OES_compressed_ETC1_RGB8_texture"sv);
        break;
    case FeatureType::ETC2:
        featureSupported = _textureCompressionEtc2;
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
        featureSupported = checkASTCRenderability();
        break;
    default:
        break;
    }
    return featureSupported;
}

bool DriverGL::hasExtension(std::string_view searchName) const
{
    const auto key = hashString(searchName);
    return _glExtensions.find(key) != _glExtensions.end();
}

std::string DriverGL::dumpExtensions() const
{
    std::string strExts;

    GL_EnumAllExtensions([&strExts](const std::string_view& ext) {
        strExts += ext;
        strExts += ',';
    });

    return strExts;
}

NS_AX_BACKEND_END
