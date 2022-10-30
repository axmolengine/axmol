#include "LLGI.ShaderVulkan.h"

namespace LLGI
{

ShaderVulkan::ShaderVulkan() {}

ShaderVulkan::~ShaderVulkan()
{
	if (shaderModule_)
	{
		graphics_->GetDevice().destroyShaderModule(shaderModule_);
		shaderModule_ = nullptr;
	}

	SafeRelease(graphics_);
}

bool ShaderVulkan::Initialize(GraphicsVulkan* graphics, DataStructure* data, int count)
{
	if (count != 1)
		return false;
	if (data[0].Size == 0)
		return false;

	buffer.resize(data[0].Size);
	memcpy(buffer.data(), data[0].Data, data[0].Size);

	SafeAddRef(graphics);
	SafeRelease(graphics_);
	graphics_ = graphics;

	vk::ShaderModuleCreateInfo info;
	info.pCode = reinterpret_cast<const uint32_t*>(buffer.data());
	info.codeSize = buffer.size();

	shaderModule_ = graphics_->GetDevice().createShaderModule(info);

	return true;
}

vk::ShaderModule ShaderVulkan::GetShaderModule() const { return shaderModule_; }

} // namespace LLGI
