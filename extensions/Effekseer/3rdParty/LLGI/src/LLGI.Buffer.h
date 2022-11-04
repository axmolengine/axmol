#pragma once

#include "LLGI.Base.h"

namespace LLGI
{

class Buffer : public ReferenceObject
{
protected:
	BufferUsageType usage_ = BufferUsageType::Index;

	static bool VerifyUsage(BufferUsageType usage);

public:
	Buffer() = default;
	~Buffer() override = default;

	virtual void* Lock();
	virtual void* Lock(int32_t offset, int32_t size);
	virtual void Unlock();

	virtual int32_t GetSize();

	BufferUsageType GetBufferUsage() { return usage_; }
};

} // namespace LLGI
