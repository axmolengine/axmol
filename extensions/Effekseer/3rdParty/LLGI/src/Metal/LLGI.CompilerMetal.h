
#pragma once

#include "../LLGI.Compiler.h"

namespace LLGI
{

class CompilerMetal : public Compiler
{
private:
public:
	void Initialize() override;
	void Compile(CompilerResult& result, const char* code, ShaderStageType shaderStage) override;

	DeviceType GetDeviceType() const override { return DeviceType::Metal; }
};

} // namespace LLGI
