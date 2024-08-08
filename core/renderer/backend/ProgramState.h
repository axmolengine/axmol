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

#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>
#include <functional>
#include "platform/PlatformMacros.h"
#include "base/Object.h"
#include "base/EventListenerCustom.h"
#include "renderer/backend/Types.h"
#include "renderer/backend/Program.h"
#include "renderer/backend/VertexLayout.h"
#include "yasio/byte_buffer.hpp"

NS_AX_BACKEND_BEGIN

class TextureBackend;
class VertexLayout;

/**
 * @addtogroup _backend
 * @{
 */

/**
 * Store texture information.
 */
struct AX_DLL TextureInfo
{
    TextureInfo(std::vector<int>&& _slots, std::vector<backend::TextureBackend*>&& _textures);
    TextureInfo(std::vector<int>&& _slots,
                std::vector<int>&& _indexs,
                std::vector<backend::TextureBackend*>&& _textures);
    TextureInfo() = default;
    TextureInfo(const TextureInfo&);
    TextureInfo(TextureInfo&& rhs);

    ~TextureInfo();

    TextureInfo& operator=(const TextureInfo& other) noexcept;
    TextureInfo& operator=(TextureInfo&& other) noexcept;

    void assign(const TextureInfo& other);
    void assign(TextureInfo&& other);
    void assign(int slot, int index, backend::TextureBackend* texture);

    void retainTextures();
    void releaseTextures();

    std::vector<int> slots;
    std::vector<int> indexs;
    std::vector<backend::TextureBackend*> textures;
#if AX_ENABLE_CACHE_TEXTURE_DATA
    int location = -1;
#endif
};

/**
 * A program state object can create or reuse a program.
 * Each program state object keep its own unifroms and textures data.
 */
class AX_DLL ProgramState : public Object
{
public:
    using UniformCallback = std::function<void(ProgramState*, const UniformLocation&)>;

    /**
     * @param program Specifies the program.
     */
    ProgramState(Program* program);

    /// destructor
    virtual ~ProgramState();

    /**
     * Deep clone ProgramState
     */
    ProgramState* clone() const;

    /**
     * Get the program object.
     */
    backend::Program* getProgram() const { return _program; }

    /**
     * Specify the value of a uniform variable for the current program state object.
     * @param uniformLocation Specifies the uniform location.
     * @param data Specifies the new values to be used for the specified uniform variable.
     * @param size Specifies the uniform data size.
     */
    void setUniform(const backend::UniformLocation& uniformLocation, const void* data, std::size_t size);

    /**
     * Get uniform location in given uniform name.
     * @param uniform Specifies the uniform name.
     * @return Uniform location.
     * @see `backend::UniformLocation getUniformLocation(backend::Uniform name) const`
     */
    backend::UniformLocation getUniformLocation(std::string_view uniform) const;

    /**
     * Get uniform location in a more efficient way by the given built-in uniform name.
     * @param uniform Specifies the engin built-in uniform name.
     * @return Uniform location.
     * @see `backend::UniformLocation getUniformLocation(backend::Uniform name) const`
     */
    backend::UniformLocation getUniformLocation(backend::Uniform name) const;

    /**
     * Get an attribute location by the actual attribute name.
     * @param name Specifies the attribute name.
     * @return Attribute location.
     * @see `int getAttributeLocation(std::string_view name) const`
     */
    inline int getAttributeLocation(std::string_view name) const { return _program->getAttributeLocation(name); }

    /**
     * Get an attribute location by the engine built-in attribute name.
     * @param name Specifies the built-in attribute name.
     * @return Attribute location.
     * @see `int getAttributeLocation(std::string_view name) const`
     */
    inline int getAttributeLocation(Attribute name) const { return _program->getAttributeLocation(name); }

    /**
     * A callback to update unifrom.
     * @param uniformLocation Specifies the uniform location.
     * @param unifromCallback Specifies a callback function to update the uniform.
     */
    void setCallbackUniform(const backend::UniformLocation&, const UniformCallback&);

    /**
     * Get the uniform callback function.
     * @return Uniform callback funciton.
     */
    inline const std::unordered_map<UniformLocation, UniformCallback, UniformLocation>& getCallbackUniforms() const
    {
        return _callbackUniforms;
    }

    /**
     * Set texture.
     * @param uniformLocation Specifies texture location.
     * @param texture Specifies a pointer to backend texture.
     */
    void setTexture(backend::TextureBackend* texture);

    /**
     * Set texture.
     * @param uniformLocation Specifies texture location.
     * @param slot Specifies texture slot selector.
     * @param texture Specifies a pointer to backend texture.
     */
    void setTexture(const backend::UniformLocation& uniformLocation, int slot, backend::TextureBackend* texture);

    /**
     * Set texture.
     * @param uniformLocation Specifies texture location.
     * @param slot Specifies texture slot selector.
     * @param index Specifies texture index selector
     * @param texture Specifies a pointer to backend texture.
     */
    void setTexture(const backend::UniformLocation& uniformLocation,
                    int slot,
                    int index,
                    backend::TextureBackend* texture);

    /**
     * Set textures in array.
     * @param uniformLocation Specifies texture location.
     * @param slots Specifies texture slot selector.
     * @param textures Specifies a vector of backend texture object.
     */
    void setTextureArray(const backend::UniformLocation& uniformLocation,
                         std::vector<int> slots,
                         std::vector<backend::TextureBackend*> textures);

    /**
     * Get vertex texture informations
     * @return Vertex texture informations. Key is the texture location, Value store the texture informations
     */
    inline const std::unordered_map<int, TextureInfo>& getVertexTextureInfos() const { return _vertexTextureInfos; }

    /**
     * Get fragment texture informations
     * @return Fragment texture informations. Key is the texture location, Value store the texture informations
     */
    inline const std::unordered_map<int, TextureInfo>& getFragmentTextureInfos() const { return _fragmentTextureInfos; }

    /**
     * Get vertex uniform buffer. The buffer store all the vertex uniform's data.
     * @param[out] size Specifies the size of the buffer in bytes.
     * @return vertex uniform buffer, not nullable
     */
    const char* getVertexUniformBuffer(std::size_t& size) const;

    /**
     * Get fragment uniform buffer. The buffer store all the fragment uniform's data for metal.
     * @param[out] size Specifies the size of the buffer in bytes.
     * @return fragment uniform buffer, not nullable
     */
    const char* getFragmentUniformBuffer(std::size_t& size) const;

    /**
     * An abstract base class that can be extended to support custom material auto bindings.
     *
     * Implementing a custom auto binding resolver allows the set of built-in parameter auto
     * bindings to be extended or overridden. Any parameter auto binding that is set on a
     * material will be forwarded to any custom auto binding resolvers, in the order in which
     * they are registered. If a registered resolver returns true (specifying that it handles
     * the specified autoBinding), no further code will be executed for that autoBinding.
     * This allows auto binding resolvers to not only implement new/custom binding strings,
     * but it also lets them override existing/built-in ones. For this reason, you should
     * ensure that you ONLY return true if you explicitly handle a custom auto binding; return
     * false otherwise.
     *
     * Note that the custom resolver is called only once for a GLProgramState object when its
     * node binding is initially set. This occurs when a material is initially bound to a
     * Node. The resolver is NOT called each frame or each time the GLProgramState is bound.
     *
     * If no registered resolvers explicitly handle an auto binding, the binding will attempt
     * to be resolved using the internal/built-in resolver, which is able to handle any
     * auto bindings found in the GLProgramState::AutoBinding enumeration.
     *
     * When an instance of a class that extends AutoBindingResolver is created, it is automatically
     * registered as a custom auto binding handler. Likewise, it is automatically unregistered
     * on destruction.
     *
     * @script{ignore}
     */
    class AX_DLL AutoBindingResolver
    {
    public:
        AutoBindingResolver();
        virtual ~AutoBindingResolver();
        /**
         * Called when an unrecognized uniform variable is encountered
         * during material loading.
         *
         * Implementations of this method should do a string comparison on the passed
         * in name parameter and decide whether or not they should handle the
         * parameter. If the parameter is not handled, false should be returned so
         * that other auto binding resolvers get a chance to handle the parameter.
         * Otherwise, the parameter should be set or bound and true should be returned.
         *
         * @param programState The ProgramState
         * @param uniformName Name of the uniform
         * @param autoBinding Name of the auto binding to be resolved.
         *
         * @return True if the auto binding is handled and the associated parameter is
         *      bound, false otherwise.
         */
        virtual bool resolveAutoBinding(ProgramState*, std::string_view uniformName, std::string_view autoBinding) = 0;
    };
    /**
     * Sets a uniform auto-binding.
     *
     * This method parses the passed in autoBinding string and attempts to convert it
     * to an enumeration value. If it matches to one of the predefined strings, it will create a
     * callback to get the correct value at runtime.
     *
     * @param uniformName The name of the material parameter to store an auto-binding for.
     * @param autoBinding A string matching one of the built-in AutoBinding enum constants.
     */
    void setParameterAutoBinding(std::string_view uniformName, std::string_view autoBinding);

    inline const VertexLayout* getVertexLayout() const { return _vertexLayout; }

    VertexLayout* getMutableVertexLayout();

    void setSharedVertexLayout(VertexLayout* vertexLayout);

    /*
    * Gets batch id of current program state, part of batch draw materialID
    */
    uint64_t getBatchId() const { return _batchId; };

    /*
    * Update batchID of current program state, by default, custom program was traits with mutable uniforms
    * so batch ID was set to -1 indicate batch was disabled
    */
    void updateBatchId();

    /*
     * Follow API is deprecated, use getMutableVertexLayout instead
     */
    AX_DEPRECATED_ATTRIBUTE void setVertexAttrib(std::string_view name,
                                                 std::size_t index,
                                                 VertexFormat format,
                                                 std::size_t offset,
                                                 bool needToBeNormallized);
    AX_DEPRECATED_ATTRIBUTE void setVertexStride(uint32_t stride);

    /** Custom shader program's vertex layout maybe not setup
     * so engine specific render node(such as Sprite) should invoke this API when ProgramState changed
     */
    void validateSharedVertexLayout(VertexLayoutType);

protected:
    void ensureVertexLayoutMutable();

    /**
     * Set the vertex uniform data.
     * @param location Specifies the uniform location.
     * @param data Specifies the new values to be used for the specified uniform variable.
     * @param size Specifies the uniform data size.
     */
    void setVertexUniform(int location, const void* data, std::size_t size, std::size_t offset);

#ifdef AX_USE_METAL
    /**
     * Set the fargment uniform data.
     * @param location Specifies the uniform location.
     * @param data Specifies the new values to be used for the specified uniform variable.
     * @param size Specifies the uniform data size.
     */
    void setFragmentUniform(int location, const void* data, std::size_t size, std::size_t offset);
#endif

    /**
     * Set texture.
     * @param location Specifies the location of texture.
     * @param slot Specifies slot selector of texture.
     * @param texture Specifies the texture to set in given location.
     * @param textureInfo Specifies the texture information to update.
     */
    void setTexture(int location,
                    int slot,
                    int index,
                    backend::TextureBackend* texture,
                    std::unordered_map<int, TextureInfo>& textureInfo);

    /**
     * Set textures in array.
     * @param location Specifies the location of texture.
     * @param slots Specifies slot selector of texture.
     * @param textures Specifies the texture to set in given location.
     * @param textureInfo Specifies the texture information to update.
     */
    void setTextureArray(int location,
                         std::vector<int> slots,
                         std::vector<backend::TextureBackend*> textures,
                         std::unordered_map<int, TextureInfo>& textureInfo);

    /**
     * Reset uniform informations when EGL context lost
     */
    void resetUniforms();

    /// Initialize.
    bool init(Program* program);

    /**
     * Applies the specified custom auto-binding.
     *
     * @param uniformName Name of the shader uniform.
     * @param autoBinding Name of the auto binding.
     */
    void applyAutoBinding(std::string_view, std::string_view);

    backend::Program* _program = nullptr;
    std::unordered_map<UniformLocation, UniformCallback, UniformLocation> _callbackUniforms;
    yasio::sbyte_buffer _uniformBuffers;
    std::size_t _vertexUniformBufferSize   = 0;
    std::size_t _fragmentUniformBufferSize = 0;

    std::unordered_map<int, TextureInfo> _vertexTextureInfos;
    std::unordered_map<int, TextureInfo> _fragmentTextureInfos;

    std::unordered_map<std::string, std::string> _autoBindings;

    static std::vector<AutoBindingResolver*> _customAutoBindingResolvers;
    VertexLayout* _vertexLayout = nullptr;
    bool _ownVertexLayout       = false;

    uint64_t _batchId = -1;

#if AX_ENABLE_CACHE_TEXTURE_DATA
    EventListenerCustom* _backToForegroundListener = nullptr;
#endif
};

// end of _backend group
/// @}
NS_AX_BACKEND_END
