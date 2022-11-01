#pragma once

#include "../LLGI.Base.h"
#include <string>

namespace LLGI
{

struct WindowMac_Impl;

class WindowMac : public Window
{
private:
	std::shared_ptr<WindowMac_Impl> impl_ = nullptr;
	Vec2I windowSize_;

public:
	WindowMac() = default;

	~WindowMac() override = default;

	bool Initialize(const char* title, const Vec2I& windowSize);

	bool DoEvent();

	void Terminate();

	void* GetNSWindowAsVoidPtr();

	bool OnNewFrame() override;

	void* GetNativePtr(int32_t index) override;

	Vec2I GetWindowSize() const override;

	Vec2I GetFrameBufferSize() const override;
};

} // namespace LLGI
