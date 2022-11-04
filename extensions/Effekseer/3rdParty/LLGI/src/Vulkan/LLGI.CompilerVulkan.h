
#pragma once

#include "../LLGI.Compiler.h"
#include "LLGI.BaseVulkan.h"

namespace LLGI
{

class CompilerVulkan : public Compiler
{
private:
public:
	CompilerVulkan();
	~CompilerVulkan() override;

	void Initialize() override;
	void Compile(CompilerResult& result, const char* code, ShaderStageType shaderStage) override;

	DeviceType GetDeviceType() const override { return DeviceType::Vulkan; }
};

} // namespace LLGI
