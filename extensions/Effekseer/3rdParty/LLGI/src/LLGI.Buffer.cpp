#include "LLGI.Buffer.h"

namespace LLGI
{

bool Buffer::VerifyUsage(BufferUsageType usage)
{
	if (BitwiseContains(usage, BufferUsageType::MapWrite) && BitwiseContains(usage, BufferUsageType::MapRead))
	{
		Log(LogType::Error, "It cannot specify MapWrite and MapRead simultaniously.");
		return false;
	}

	if ((BitwiseContains(usage, BufferUsageType::MapWrite) || BitwiseContains(usage, BufferUsageType::MapRead)) &&
		BitwiseContains(usage, BufferUsageType::Compute))
	{
		Log(LogType::Error, "It cannot specify Map(Read/Write) and Compute simultaniously.");
		return false;
	}

	return true;
}

void* Buffer::Lock() { return nullptr; }

void* Buffer::Lock(int32_t offset, int32_t size) { return nullptr; }

void Buffer::Unlock() {}

// void* const Buffer::Read() { return nullptr; }

int32_t Buffer::GetSize() { return int32_t(); }

} // namespace LLGI
