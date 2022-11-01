#pragma once

#include "LLGI.Base.h"

namespace LLGI
{

Compiler* CreateCompiler(DeviceType device);

struct CompilerResult
{
	std::string Message;
	std::vector<std::vector<uint8_t>> Binary;
};

class Compiler : public ReferenceObject
{
private:
public:
	Compiler() = default;
	~Compiler() override = default;

	virtual void Initialize();
	virtual void Compile(CompilerResult& result, const char* code, ShaderStageType shaderStage);

	virtual DeviceType GetDeviceType() const { return DeviceType::Default; }
};

} // namespace LLGI
