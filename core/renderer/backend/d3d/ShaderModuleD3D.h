// ShaderModuleImpl.h
#pragma once
#include "renderer/backend/ShaderModule.h"
#include <d3d11.h>
#include <d3dcompiler.h>

namespace ax::backend::d3d
{
struct SLCReflectContext;

class ShaderModuleImpl : public ShaderModule
{
public:
    ShaderModuleImpl(ShaderStage stage, std::string_view source, ID3D11Device* device);
    ~ShaderModuleImpl();

    IUnknown* internalHandle() const { return _shader; }
    ID3DBlob* getShaderBlob() const { return _blob; }

    /**
     * Get uniform info by engine built-in uniform enum name.
     * @param name Specifies the engine built-in uniform enum name.
     * @return The uniform location.
     */
    const UniformInfo& getUniformInfo(Uniform name) const;

    /**
     * Get uniform info by name.
     * @param uniform Specifies the uniform name.
     * @return The uniform location.
     */
    const UniformInfo& getUniformInfo(std::string_view name) const;

    /**
     * Get attribute location by engine built-in attribute enum name.
     * @param name Specifies the engine built-in attribute enum name.
     * @return The attribute location.
     */
    const VertexInputDesc* getVertexInputDesc(VertexInputKind name) const;

    /**
     * Get attribute location by attribute name.
     * @param name Specifies the attribute name.
     * @return The attribute location.
     */
    const VertexInputDesc* getVertexInputDesc(std::string_view name) const;

    /**
     * Get uniform buffer size in bytes that holds all the uniforms.
     * @return The uniform buffer size.
     */
    inline std::size_t getUniformBufferSize() const { return _uniformBufferSize; }

    /**
     * Get all uniformInfos.
     * @return The uniformInfos.
     */
    inline const hlookup::string_map<UniformInfo>& getActiveUniformInfos() const { return _activeUniformInfos; }

    /**
     * Get active attribute informations.
     * @return Active attribute informations. key is attribute name and Value is corresponding attribute info.
     */
    inline const hlookup::string_map<VertexInputDesc>& getActiveVertexInputs() const { return _allVertexInputs; }

    /**
     * Get maximum uniform location.
     * @return Maximum uniform location.
     */
    inline const int getMaxLocation() const { return _maxLocation; }

private:
    void compileShader(ShaderStage stage, std::string_view source, ID3D11Device* device);
    void releaseShader();
    void parseAttibute(SLCReflectContext* context);
    void parseUniform(SLCReflectContext* context);
    void parseTexture(SLCReflectContext* context);
    void setBuiltinLocations();

    IUnknown* _shader = nullptr;
    ID3DBlob* _blob   = nullptr;

    hlookup::string_map<VertexInputDesc> _allVertexInputs;
    hlookup::string_map<UniformInfo> _activeUniformInfos;
    
    const VertexInputDesc* _builtinVertexInputs[VIK_COUNT];

    int _maxLocation = -1;
    UniformInfo _builtinUniforms[UNIFORM_MAX];

    std::size_t _uniformBufferSize = 0;
};

}  // namespace ax::backend::d3d
