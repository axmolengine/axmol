#ifndef _GLFW3EXT_H_
#define _GLFW3EXT_H_
#include "glfw3.h"

#ifndef GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#ifndef GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WGL
#endif
#include "glfw3native.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef GLFW_ALPHA_MASK
#define GLFW_ALPHA_MASK 0x00021011
#endif

/*! @brief The function signature for mouse button callbacks extension.
*   @Added by halx99
*
*  This is the function signature for mouse button callback functions.
*
*  @param[in] window The window that received the event.
*  @param[in] button The [mouse button](@ref buttons) that was pressed or
*  released.
*  @param[in] action One of `GLFW_PRESS` or `GLFW_RELEASE`.
*  @param[in] mods Bit field describing which [modifier keys](@ref mods) were
*  held down.
*  @param[in] cursor X
*  @param[in] cursor Y
*
*  @sa glfwSetMouseButtonCallback
*
*  @ingroup input
*/
typedef void(*GLFWXmousebuttonfun)(GLFWwindow*, int, int, int, double, double);
typedef int(*GLFWXloadImagefun)(void** ppvImage);
typedef void(*GLFWXunloadImagefun)(void* pvImage);
typedef void(*GLFWXdrawImagefun)(HDC hdc, void* pvImage);
typedef void* (*GLFWXEGLGetDisplayFun)(void* pUnknownAPI, GLFWwindow*);

GLFWAPI int glfwxInit(void);

GLFWAPI void glfwxTerminate(void);

GLFWAPI void glfwxSetParent(HWND hwndParent);


// unused?
// GLFWAPI void glfwxSetWindowPos(GLFWwindow* handle, int xpos, int ypos, int width, int height);

GLFWAPI void glfwxSetBackgroundDriver(GLFWXloadImagefun imageLoader, GLFWXdrawImagefun imageDrawer, GLFWXunloadImagefun imageUnloader); // TODO: rename function name

#ifdef __cplusplus
}
#endif

#endif

