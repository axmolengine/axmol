
#include "../LLGI.Compiler.h"
#include "../LLGI.Platform.h"

#ifdef ENABLE_VULKAN
#include "../Vulkan/LLGI.PlatformVulkan.h"
#endif

#ifdef _WIN32
#include "../DX12/LLGI.CompilerDX12.h"
#include "../DX12/LLGI.PlatformDX12.h"
#include "../Win/LLGI.WindowWin.h"
#endif

#ifdef __APPLE__
#include "../Mac/LLGI.WindowMac.h"
#include "../Metal/LLGI.CompilerMetal.h"
#include "../Metal/LLGI.PlatformMetal.h"
#endif

#if defined(ENABLE_VULKAN)
#include "../Vulkan/LLGI.CompilerVulkan.h"
#endif

#ifdef __linux__
#include "../Linux/LLGI.WindowLinux.h"
#endif

#ifdef _WIN32
#undef CreateWindow
#endif

namespace LLGI
{

Window* CreateWindow(const char* title, Vec2I windowSize)
{
#ifdef _WIN32
	auto window = new WindowWin();
	if (window->Initialize(title, windowSize))
	{
		return window;
	}
#elif __APPLE__
	auto window = new WindowMac();
	if (window->Initialize(title, windowSize))
	{
		return window;
	}
#elif __linux__
	auto window = new WindowLinux();
	if (window->Initialize(title, windowSize))
	{
		return window;
	}
#endif

	Log(LogType::Error, "Failed to create window.");
	return nullptr;
}

Platform* CreatePlatform(const PlatformParameter& parameter, Window* window)
{
	Vec2I windowSize;
	windowSize.X = 1280;
	windowSize.Y = 720;

#ifdef ENABLE_VULKAN
#if defined(__linux__)
	if (parameter.Device == DeviceType::Vulkan || parameter.Device == DeviceType::Default)
#else
	if (parameter.Device == DeviceType::Vulkan)
#endif
	{
		auto platform = new PlatformVulkan();
		if (!platform->Initialize(window, parameter.WaitVSync))
		{
			SafeRelease(platform);
			return nullptr;
		}
		return platform;
	}
#endif

#ifdef _WIN32

	if (parameter.Device == DeviceType::Default || parameter.Device == DeviceType::DirectX12)
	{
		auto platform = new PlatformDX12();
		if (!platform->Initialize(window, parameter.WaitVSync))
		{
			SafeRelease(platform);
			return nullptr;
		}
		return platform;
	}

#elif defined(__APPLE__)
	auto obj = new PlatformMetal(window, parameter.WaitVSync);
	return obj;

#endif

	Log(LogType::Error, "Specified device is not valid.");
	return nullptr;
}

Compiler* CreateCompiler(DeviceType device)
{
#ifdef ENABLE_CREATE_COMPILER

#ifdef _WIN32
	if (device == DeviceType::Default || device == DeviceType::DirectX12)
	{
		auto obj = new CompilerDX12();
		return obj;
	}
#endif

#ifdef ENABLE_VULKAN
	if (device == DeviceType::Vulkan)
	{
#if defined(ENABLE_VULKAN_COMPILER)
		auto obj = new CompilerVulkan();
		return obj;
#else
		return nullptr;
#endif
	}
#endif

#ifdef __APPLE__
	auto obj = new CompilerMetal();
	return obj;
#endif

#endif
	return nullptr;
}

} // namespace LLGI
