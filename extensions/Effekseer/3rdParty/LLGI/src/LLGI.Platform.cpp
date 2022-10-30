#include "LLGI.Platform.h"

namespace LLGI
{

bool Platform::NewFrame() { return false; }

void Platform::Present() {}

Graphics* Platform::CreateGraphics() { return nullptr; }

int Platform::GetCurrentFrameIndex() const
{
	assert(0); // TODO: Not implemented.
	return 0;
}

int Platform::GetMaxFrameCount() const
{
	assert(0); // TODO: Not implemented.
	return 0;
}

void Platform::SetWindowSize(const Vec2I& windowSize)
{
	assert(0); // TODO: Not implemented.
}

RenderPass* Platform::GetCurrentScreen(const Color8& clearColor, bool isColorCleared, bool isDepthCleared)
{
	Log(LogType::Warning, "GetCurrentScreen is not implemented.");
	return nullptr;
}

} // namespace LLGI
