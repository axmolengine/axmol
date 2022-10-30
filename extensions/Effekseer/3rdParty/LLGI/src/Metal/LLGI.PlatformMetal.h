
#pragma once

#include "../LLGI.Platform.h"

namespace LLGI
{

struct PlatformMetal_Impl;

class TextureMetal;
class RenderPassMetal;
class WindowMac;

class PlatformMetal : public Platform
{
	PlatformMetal_Impl* impl = nullptr;

	Vec2I windowSize_;

	struct RingBuffer
	{
		std::shared_ptr<TextureMetal> renderTexture = nullptr;
		std::shared_ptr<RenderPassMetal> renderPass = nullptr;
	};

	int32_t ringIndex_ = 0;
	std::vector<RingBuffer> ringBuffers_;

public:
	PlatformMetal(Window* window, bool waitVSync);
	~PlatformMetal() override;
	bool NewFrame() override;
	void Present() override;
	Graphics* CreateGraphics() override;

	void SetWindowSize(const Vec2I& windowSize) override;

	RenderPass* GetCurrentScreen(const Color8& clearColor, bool isColorCleared, bool isDepthCleared) override;

	DeviceType GetDeviceType() const override { return DeviceType::Metal; }
};

} // namespace LLGI
