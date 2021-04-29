#ifndef _GLFWEXT_INTERNAL_H_
#define _GLFWEXT_INTERNAL_H_
#include <assert.h>
#include "internal.h"
#include "../include/GLFW/glfw3ext.h"

typedef BOOL (WINAPI *GLFWXalphaBlendFunc)(
	_In_ HDC hdcDest,
	_In_ int xoriginDest,
	_In_ int yoriginDest,
	_In_ int wDest,
	_In_ int hDest,
	_In_ HDC hdcSrc,
	_In_ int xoriginSrc,
	_In_ int yoriginSrc,
	_In_ int wSrc,
	_In_ int hSrc,
	_In_ BLENDFUNCTION ftn);

typedef struct _glfwx
{
	/// support create GL surface as child window of exist window
	HWND     hwndGLParent;
	
	/// support irregular window
	/// GL Render Target is compatible DIB dc
	struct {
		HMODULE   msimg32;
		GLFWXalphaBlendFunc alphaBlend;
		int       screenW;
		int       screenH;
		void*     pvImage; // when alpha mask enabled
		GLFWXloadImagefun loadImage; // end use can use gdiplus to load the image
		GLFWXdrawImagefun drawImage;
		GLFWXunloadImagefun unloadImage;
		HDC       dcBackground;

		HANDLE    hSectionGLDIB;
	} wglvi;

    // EGL extensions
    // GLFWXEGLGetDisplayFun glfwxEGLGetDisplay;
} GLFWXconfig;

extern GLFWXconfig _glfwx;

extern GLFWbool _glfwxInitialized;

/*! @brief create a compatible DIB DC as GLSurface.
*  @param[in] width: the width of DIB.
*  @param[in] height: the height of DIB.
*  @param[in] bits: 32 is ok.
*  of the client area of the window.
*  @ingroup event x-studio spec
*/
HBITMAP _glfwxCreateDIB(HDC hdc, float width, float height, void** ppvBits, HANDLE hSection); // bits = 32

#endif

 