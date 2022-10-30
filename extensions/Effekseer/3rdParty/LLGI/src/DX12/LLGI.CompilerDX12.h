
#pragma once

#include "../LLGI.Compiler.h"
#include "LLGI.BaseDX12.h"

namespace LLGI
{

enum class CompilerDX12Option : int32_t
{
	None = 0,
	RowMajor = (1 << 0),
	ColumnMajor = (2 << 0),
};

class CompilerDX12 : public Compiler
{
private:
	struct CompileShaderResultDX12
	{
		ID3DBlob* shader = nullptr;
		std::string error;
	};

	CompileShaderResultDX12 CompileShader(const char* text,
										  const char* fileName,
										  const char* target,
										  const std::vector<D3D_SHADER_MACRO>& macro,
										  const CompilerDX12Option& option = LLGI::CompilerDX12Option::ColumnMajor);

	CompilerDX12Option option_;

public:
	CompilerDX12(const CompilerDX12Option& option = LLGI::CompilerDX12Option::ColumnMajor);
	~CompilerDX12() override = default;

	void Initialize() override;
	void Compile(CompilerResult& result, const char* code, ShaderStageType shaderStage) override;

	DeviceType GetDeviceType() const override { return DeviceType::DirectX12; }
};

} // namespace LLGI
