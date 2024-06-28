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
#pragma once

#include "../DriverBase.h"
#include "platform/GL.h"
#include "OpenGLState.h"
#include "base/hlookup.h"

NS_AX_BACKEND_BEGIN
/**
 * @addtogroup _opengl
 * @{
 */

/**
 * Use to create resoureces.
 */
class DriverGL : public DriverBase
{
public:
    DriverGL();
    ~DriverGL();

    GLint getDefaultFBO() const;

    /**
     * New a CommandBuffer object, not auto released.
     * @return A CommandBuffer object.
     */
    CommandBuffer* newCommandBuffer() override;

    /**
     * New a Buffer object, not auto released.
     * @param size Specifies the size in bytes of the buffer object's new data store.
     * @param type Specifies the target buffer object. The symbolic constant must be BufferType::VERTEX or
     * BufferType::INDEX.
     * @param usage Specifies the expected usage pattern of the data store. The symbolic constant must be
     * BufferUsage::STATIC, BufferUsage::DYNAMIC.
     * @return A Buffer object.
     */
    Buffer* newBuffer(std::size_t size, BufferType type, BufferUsage usage) override;

    /**
     * New a TextureBackend object, not auto released.
     * @param descriptor Specifies texture description.
     * @return A TextureBackend object.
     */
    TextureBackend* newTexture(const TextureDescriptor& descriptor) override;

    RenderTarget* newDefaultRenderTarget() override;
    RenderTarget* newRenderTarget(TextureBackend* colorAttachment,
                                  TextureBackend* depthAttachment,
                                  TextureBackend* stencilAttachhment) override;

    DepthStencilState* newDepthStencilState() override;

    /**
     * New a RenderPipeline object, not auto released.
     * @param descriptor Specifies render pipeline description.
     * @return A RenderPipeline object.
     */
    RenderPipeline* newRenderPipeline() override;

    /**
     * Design for metal.
     */
    void setFrameBufferOnly(bool frameBufferOnly) override {}

    /**
     * New a Program, not auto released.
     * @param vertexShader Specifes this is a vertex shader source.
     * @param fragmentShader Specifes this is a fragment shader source.
     * @return A Program instance.
     */
    Program* newProgram(std::string_view vertexShader, std::string_view fragmentShader) override;

    void resetState() override;

    /// below is driver info API

    /**
     * Get vendor device name.
     * @return Vendor device name.
     */
    const char* getVendor() const override;

    /**
     * Get the full name of the vendor device.
     * @return The full name of the vendor device.
     */
    const char* getRenderer() const override;

    /**
     * Get version name.
     * @return Version name.
     */
    const char* getVersion() const override;

    const char* getShaderVersion() const override;

    /**
     * Check does device has extension.
     */
    bool hasExtension(std::string_view /*extName*/) const override;

    /**
    * Dump all extensions to string
    */
    std::string dumpExtensions() const override;

    /**
     * Check if feature supported by OpenGL ES.
     * @param feature Specify feature to be query.
     * @return true if the feature is supported, false otherwise.
     */
    bool checkForFeatureSupported(FeatureType feature) override;

    /*
    * Check does the device only support GLES2.0
    */
    bool isGLES2Only() const;

protected:
    /**
     * New a shaderModule, not auto released.
     * @param stage Specifies whether is vertex shader or fragment shader.
     * @param source Specifies shader source.
     * @return A ShaderModule object.
     */
    ShaderModule* newShaderModule(ShaderStage stage, std::string_view source) override;

    GLint _defaultFBO = 0;  // The value gets from glGetIntegerv, so need to use GLint
    GLuint _defaultVAO = 0;

private:
    std::set<uint32_t> _glExtensions;

    const char* _vendor{nullptr};
    const char* _renderer{nullptr};
    const char* _version{nullptr};
    const char* _shaderVer{nullptr};

    struct VersionInfo
    {
        bool es{false};     // is GLES?
        uint16_t major{0};  // major version
        uint16_t minor{0};  // minor version
    } _verInfo;

    bool _textureCompressionAstc = false;
    bool _textureCompressionEtc2 = false;
};
// end of _opengl group
/// @}
NS_AX_BACKEND_END
