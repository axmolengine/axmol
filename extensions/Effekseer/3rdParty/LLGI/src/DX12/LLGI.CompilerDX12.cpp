#include "LLGI.CompilerDX12.h"

#include <d3dcompiler.h>

namespace LLGI
{

CompilerDX12::CompileShaderResultDX12 CompilerDX12::CompileShader(const char* text,
																  const char* fileName,
																  const char* target,
																  const std::vector<D3D_SHADER_MACRO>& macro,
																  const CompilerDX12Option& option)
{
	ID3DBlob* shader = nullptr;
	ID3DBlob* error = nullptr;

	UINT flag = 0;

	if ((static_cast<int32_t>(option) & static_cast<int32_t>(CompilerDX12Option::ColumnMajor)) != 0)
	{
		flag |= D3DCOMPILE_PACK_MATRIX_COLUMN_MAJOR;
	}
	else
	{
		flag |= D3DCOMPILE_PACK_MATRIX_ROW_MAJOR;
	}

#if !_DEBUG
	flag = flag | D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	HRESULT hr;

	hr = D3DCompile(text,
					strlen(text),
					fileName,
					macro.size() > 0 ? (D3D_SHADER_MACRO*)&macro[0] : NULL,
					NULL,
					"main",
					target,
					flag,
					0,
					&shader,
					&error);

	CompileShaderResultDX12 result;

	if (FAILED(hr))
	{
		if (hr == E_OUTOFMEMORY)
		{
			result.error += "Out of memory\n";
		}
		else
		{
			result.error += "Unknown error\n";
		}

		if (error != NULL)
		{
			result.error += (const char*)error->GetBufferPointer();
			error->Release();
		}

		return result;
	}

	result.shader = shader;
	return result;
}

CompilerDX12::CompilerDX12(const CompilerDX12Option& option) : option_(option) {}

void CompilerDX12::Initialize() {}

void CompilerDX12::Compile(CompilerResult& result, const char* code, ShaderStageType shaderStage)
{
	char* vs_target = "vs_5_0";
	char* ps_target = "ps_5_0";
	char* cs_target = "cs_5_0";
	char* target = nullptr;

	if (shaderStage == ShaderStageType::Vertex)
	{
		target = vs_target;
	}
	else if (shaderStage == ShaderStageType::Pixel)
	{
		target = ps_target;
	}
	else if (shaderStage == ShaderStageType::Compute)
	{
		target = cs_target;
	}

	std::vector<D3D_SHADER_MACRO> macro;
	auto compileResult = CompileShader(code, "dx12_code", target, macro, option_);

	result.Message = compileResult.error;

	if (compileResult.shader != nullptr)
	{
		auto size = compileResult.shader->GetBufferSize();
		result.Binary.resize(1);
		result.Binary[0].resize(size);
		memcpy(result.Binary[0].data(), compileResult.shader->GetBufferPointer(), result.Binary[0].size());
		SafeRelease(compileResult.shader);
	}
}

} // namespace LLGI
