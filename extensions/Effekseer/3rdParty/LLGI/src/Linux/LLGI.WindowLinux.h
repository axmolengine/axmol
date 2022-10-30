#pragma once

#include "../LLGI.Base.h"
#include <X11/Xlib-xcb.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <locale.h>
#include <string>

namespace LLGI
{

class WindowLinux : public Window
{
private:
	Display* display_ = nullptr;
	::Window window_;
	GC gc_;
	bool closed_ = false;
	Atom wm_delete_window_;
	std::string title_;
	Vec2I windowSize_;

public:
	WindowLinux() = default;

	~WindowLinux() override;

	bool Initialize(const char* title, const Vec2I& windowSize);

	bool DoEvent();

	void Terminate();

	Display*& GetDisplay() { return display_; }
	::Window& GetWindow() { return window_; }

	bool OnNewFrame() override;

	void* GetNativePtr(int32_t index) override;

	Vec2I GetWindowSize() const override;
};

} // namespace LLGI
