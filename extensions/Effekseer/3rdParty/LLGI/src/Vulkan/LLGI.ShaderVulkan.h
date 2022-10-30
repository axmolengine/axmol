
#pragma once

#include "../LLGI.Shader.h"
#include "LLGI.BaseVulkan.h"
#include "LLGI.GraphicsVulkan.h"

namespace LLGI
{

class ShaderVulkan : public Shader
{
private:
	GraphicsVulkan* graphics_ = nullptr;
	std::vector<uint8_t> buffer;
	vk::ShaderModule shaderModule_;

public:
	ShaderVulkan();
	~ShaderVulkan() override;

	bool Initialize(GraphicsVulkan* graphics, DataStructure* data, int count);

	vk::ShaderModule GetShaderModule() const;
};

} // namespace LLGI
