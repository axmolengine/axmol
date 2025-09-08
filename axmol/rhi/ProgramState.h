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
#include <span>
#include "axmol/platform/PlatformMacros.h"
#include "axmol/base/Object.h"
#include "axmol/base/EventListenerCustom.h"
#include "axmol/rhi/RHITypes.h"
#include "axmol/rhi/Program.h"
#include "axmol/renderer/VertexLayoutManager.h"
#include "yasio/byte_buffer.hpp"

namespace ax::rhi
{

class Texture;
class VertexLayout;

/**
 * @addtogroup _rhi
 * @{
 */

struct TextureBinding
{
    int slot{-1};
    rhi::Texture* tex{nullptr};

    void reset()
    {
        slot = -1;
        tex  = nullptr;
    }
};

/**
 * Store texture binding information.
 * OpenGL can use 1 location (logic var location) to store multi texture units, in shader is 'uniform sampler2D texs[4]'
 * Metal/D3D11: location is bindingIndex reflected from shader
 */
struct AX_DLL TextureBindingSet
{
    TextureBindingSet() = default;
    TextureBindingSet(const TextureBindingSet&);
    TextureBindingSet(TextureBindingSet&& rhs) noexcept;
    ~TextureBindingSet();

    TextureBindingSet& operator=(const TextureBindingSet& other) noexcept;
    TextureBindingSet& operator=(TextureBindingSet&& other) noexcept;

    void assign(const TextureBindingSet& other);
    void swap(TextureBindingSet& other);

    void setTexture(int location, int slot, rhi::Texture* tex);
    void setTextureArray(int location, std::span<const TextureBinding> units);
    void setTextureArray(int location, std::span<const int> slots, std::span<rhi::Texture*> texs);

    void releaseTextures();

    axstd::pod_vector<int> slots;
    mutable axstd::pod_vector<rhi::Texture*> texs;
#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    int loc = -1;
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
    rhi::Program* getProgram() const { return _program; }

    /**
     * Specify the value of a uniform variable for the current program state object.
     * @param uniformLocation Specifies the uniform location.
     * @param data Specifies the new values to be used for the specified uniform variable.
     * @param size Specifies the uniform data size.
     */
    void setUniform(const rhi::UniformLocation& uniformLocation, const void* data, std::size_t size);

    /**
     * Get uniform location in given uniform name.
     * @param uniform Specifies the uniform name.
     * @return Uniform location.
     * @see `rhi::UniformLocation getUniformLocation(rhi::Uniform name) const`
     */
    rhi::UniformLocation getUniformLocation(std::string_view uniform) const;

    /**
     * Get uniform location in a more efficient way by the given built-in uniform name.
     * @param uniform Specifies the engin built-in uniform name.
     * @return Uniform location.
     * @see `rhi::UniformLocation getUniformLocation(rhi::Uniform name) const`
     */
    rhi::UniformLocation getUniformLocation(rhi::Uniform name) const;

    /**
     * Get an attribute location by the actual attribute name.
     * @param name Specifies the attribute name.
     * @return Attribute location.
     * @see `int getAttributeLocation(std::string_view name) const`
     */
    inline const VertexInputDesc* getVertexInputDesc(std::string_view name) const
    {
        return _program->getVertexInputDesc(name);
    }

    /**
     * Get an attribute location by the engine built-in attribute name.
     * @param name Specifies the built-in attribute name.
     * @return Attribute location.
     * @see `int getAttributeLocation(std::string_view name) const`
     */
    inline const VertexInputDesc* getVertexInputDesc(VertexInputKind name) const
    {
        return _program->getVertexInputDesc(name);
    }

    const axstd::string_map<VertexInputDesc>& getActiveVertexInputs() const
    {
        return _program->getActiveVertexInputs();
    }

    /*
     * Gets the inmutable vertex layout from _program
     * @returns nullable
     */
    VertexLayout* getVertexLayout() const { return _program->getVertexLayout(); }

    /**
     * A callback to update unifrom.
     * @param uniformLocation Specifies the uniform location.
     * @param unifromCallback Specifies a callback function to update the uniform.
     */
    void setCallbackUniform(const rhi::UniformLocation&, const UniformCallback&);

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
    void setTexture(rhi::Texture* texture);
    void setTexture(const rhi::UniformLocation& uniformLocation, int slot, rhi::Texture* texture);

    /**
     * Set texture array in shader 'uniform sampler2D xxx[4];' unlikely sampoler2DArray, the array will
     * use multi texture units
     */
    void setTextureArray(const rhi::UniformLocation& uniformLocation, std::span<TextureBinding> units);
    void setTextureArray(const rhi::UniformLocation& uniformLocation,
                         std::span<int> slot,
                         std::span<rhi::Texture*> texture);

    /**
     * Get vertex texture informations
     * @return Vertex texture informations. Key is the texture location, Value store the texture informations
     */
    inline const std::unordered_map<int, TextureBindingSet>& getTextureBindingSets() const
    {
        return _textureBindingSets;
    }

#if AX_RENDER_API == AX_RENDER_API_GL
    std::span<const char> getUniformBuffer() { return std::span{_uniformBuffer}; }
#else

    /**
     * Get vertex uniform buffer. The buffer store all the vertex uniform's data.
     * @return vertex uniform buffer, not nullable
     */
    std::span<const char> getVertexUniformBuffer() const
    {
        return std::span{_uniformBuffer.begin() + _vertexUniformBufferStart, _uniformBuffer.end()};
    }

    /**
     * Get fragment uniform buffer. The buffer store all the fragment uniform's data for metal.
     * @return fragment uniform buffer, not nullable
     */
    std::span<const char> getFragmentUniformBuffer() const
    {
        return std::span{_uniformBuffer.begin(), _uniformBuffer.begin() + _vertexUniformBufferStart};
    }
#endif

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
     * Note that the custom resolver is called only once for a ProgramState object when its
     * node binding is initially set. This occurs when a material is initially bound to a
     * Node. The resolver is NOT called each frame or each time the ProgramState is bound.
     *
     * If no registered resolvers explicitly handle an auto binding, the binding will attempt
     * to be resolved using the internal/built-in resolver, which is able to handle any
     * auto bindings found in the ProgramState::AutoBinding enumeration.
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

    inline const VertexLayout* getBuiltinVertexLayout() const { return _program->getVertexLayout(); }

    /*
     * Gets batch id of current program state, part of batch draw materialID
     */
    uint64_t getBatchId() const { return _batchId; };

    /*
     * Gets the state of the batch ID. If true, then batch ID is valid
     */
    bool isBatchable() const { return _isBatchable; };

    /*
     * Update batchID of current program state, by default, custom program was traits with mutable uniforms
     * so batch ID was set to -1 indicate batch was disabled
     */
    void updateBatchId();

protected:
    /**
     * Set the vertex uniform data.
     * @param location Specifies the uniform location.
     * @param data Specifies the new values to be used for the specified uniform variable.
     * @param size Specifies the uniform data size.
     * @param offset Specifies the offset of the uniform in bytes.
     * @param start Specifies the start offset of the uniform in bytes.
     */
    void setUniform(int location, const void* data, std::size_t size, int offset, int start);

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

    rhi::Program* _program = nullptr;
    std::unordered_map<UniformLocation, UniformCallback, UniformLocation> _callbackUniforms;

    yasio::sbyte_buffer _uniformBuffer;
#if AX_RENDER_API != AX_RENDER_API_GL
    std::size_t _vertexUniformBufferStart = 0;
#endif

    std::unordered_map<int, TextureBindingSet> _textureBindingSets;

    std::unordered_map<std::string, std::string> _autoBindings;

    static std::vector<AutoBindingResolver*> _customAutoBindingResolvers;

    uint64_t _batchId = -1;
    bool _isBatchable = false;

#if AX_ENABLE_CONTEXT_LOSS_RECOVERY
    EventListenerCustom* _backToForegroundListener = nullptr;
#endif
};

// end of _rhi group
/// @}
}  // namespace ax::rhi
