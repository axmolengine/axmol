
#include "glfw3ext_internal.h"

#include <string.h>
#include <stdlib.h>
#if defined(_MSC_VER)
#include <malloc.h>
#endif

GLFWXconfig _glfwx;
GLFWbool _glfwxInitialized;

static HANDLE internalCreateFileMapping(uint64_t size)
{
	return CreateFileMappingW(
				NULL,
				NULL,
				PAGE_READWRITE,
				size >> 32,
				size & 0x00000000ffffffff,
				NULL);
}

GLFWAPI int glfwxInit(void)
{
	if (_glfwxInitialized) 
		return GLFW_TRUE;

	memset(&_glfwx, 0, sizeof(_glfwx));

	_glfwx.wglvi.msimg32 = LoadLibrary(TEXT("msimg32.dll"));
	if (_glfwx.wglvi.msimg32)
	{
		_glfwx.wglvi.alphaBlend = (GLFWXalphaBlendFunc)GetProcAddress(_glfwx.wglvi.msimg32, "AlphaBlend");
	}

	_glfwx.wglvi.screenW = GetSystemMetrics(SM_CXSCREEN);
	_glfwx.wglvi.screenH = GetSystemMetrics(SM_CYSCREEN);

	uint64_t size = _glfwx.wglvi.screenW * _glfwx.wglvi.screenH * 4;
	double KB = (double)size / (1024);
	double MB = KB / (1024);
	// _glfwx.wglvi.hSectionBackDIB = internalCreateFileMapping(size);
	_glfwx.wglvi.hSectionGLDIB = internalCreateFileMapping(size);
	
	_glfwxInitialized = GLFW_TRUE;

	if (!glfwInit())
		glfwxTerminate();

	return _glfwxInitialized;
}

GLFWAPI void glfwxTerminate(void)
{
	if (!_glfwxInitialized)
		return;

	if (_glfwx.wglvi.hSectionGLDIB)
		CloseHandle(_glfwx.wglvi.hSectionGLDIB);

	if (_glfwx.wglvi.dcBackground)
		DeleteDC(_glfwx.wglvi.dcBackground);

	if (_glfwx.wglvi.unloadImage){
		_glfwx.wglvi.unloadImage(_glfwx.wglvi.pvImage);
	}

	if (_glfwx.wglvi.msimg32)
		FreeLibrary(_glfwx.wglvi.msimg32);

	memset(&_glfwx, 0, sizeof(_glfwx));

	glfwTerminate();

	_glfwxInitialized = GLFW_FALSE;
}

// Return the maxiumum of the specified values
//
static int Max(int a, int b)
{
	return (a > b) ? a : b;
}

//////////////////////////////////////////////////////////////////////////
//////                        GLFW public API                       //////
//////////////////////////////////////////////////////////////////////////

GLFWAPI void glfwxSetParent(HWND hwndParent)
{
	_glfwx.hwndGLParent = hwndParent;
}

GLFWAPI void glfwxSetBackgroundDriver(GLFWXloadImagefun imageLoader, GLFWXdrawImagefun imageDrawer, GLFWXunloadImagefun imageUnloader)
{
	if (!_glfwxInitialized)
		return;

	_glfwx.wglvi.loadImage = imageLoader;
	_glfwx.wglvi.drawImage = imageDrawer;
	_glfwx.wglvi.unloadImage = imageUnloader;

	/*if (!_glfwx.wglvi.irregularEnabled) {
		int bits = _glfw.hints.redBits + _glfw.hints.blueBits + _glfw.hints.greenBits + _glfw.hints.alphaBits;
		HDC hdcGL = _glfwxCreateGLSurfaceDIB(_glfwx.wglvi.screenW, _glfwx.wglvi.screenH, bits);
		assert(hdcGL);
		_glfwx.wglvi.hdcGL = hdcGL;
	}*/
}

// unused ?
// GLFWAPI void glfwxSetWindowPos(GLFWwindow* handle, int xpos, int ypos, int width, int height)
// {
//     extern DWORD getWindowStyle(const _GLFWwindow* window);
//     extern DWORD getWindowExStyle(const _GLFWwindow* window);
// 
//     _GLFWwindow* window = (_GLFWwindow*)handle;
//     assert(window != NULL);
// 
//     RECT rect = { xpos, ypos, width, height };
//     AdjustWindowRectEx(&rect, getWindowStyle(window),
//         FALSE, getWindowExStyle(window));
//     SetWindowPos(window->win32.handle, NULL, rect.left, rect.top,
//         rect.right - rect.left, rect.bottom - rect.top,
//         SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOOWNERZORDER);
// }
