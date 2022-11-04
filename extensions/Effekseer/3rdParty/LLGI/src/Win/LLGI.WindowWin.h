#pragma once

#include "../LLGI.Base.h"
#include <Windows.h>
#include <string>

namespace LLGI
{

class WindowWin : public Window
{
private:
	HWND hwnd_ = nullptr;
	HINSTANCE hInstance_ = nullptr;
	std::string title_;
	Vec2I windowSize_;

public:
	WindowWin() = default;

	~WindowWin() override;

	bool Initialize(const char* title, const Vec2I& windowSize);

	bool DoEvent();

	void Terminate();

	bool OnNewFrame() override;

	void* GetNativePtr(int32_t index) override;

	Vec2I GetWindowSize() const;

	HWND GetHandle() const { return hwnd_; }

	HINSTANCE GetInstance() const { return hInstance_; }
};

} // namespace LLGI
