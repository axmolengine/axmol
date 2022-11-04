
#include "LLGI.ShaderDX12.h"
#include "../LLGI.Shader.h"
#include "LLGI.CompilerDX12.h"

namespace LLGI
{

bool ShaderDX12::Initialize(DataStructure* data, int32_t count)
{
	if (data == nullptr || count == 0)
	{
		return false;
	}

	auto p = static_cast<const uint8_t*>(data->Data);
	data_.resize(data->Size);
	memcpy(data_.data(), p, data_.size());
	return true;
}

} // namespace LLGI
