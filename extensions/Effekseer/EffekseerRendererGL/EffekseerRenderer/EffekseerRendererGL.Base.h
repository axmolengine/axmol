
#ifndef __EFFEKSEERRENDERER_GL_BASE_H__
#define __EFFEKSEERRENDERER_GL_BASE_H__

//----------------------------------------------------------------------------------
// Include
//----------------------------------------------------------------------------------
#include "EffekseerRendererGL.Base.Pre.h"

#include <Effekseer.h>

#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <list>
#include <map>
#include <queue>
#include <set>
#include <string>
#include <vector>

#include <memory>

// #if _WIN32
// #include "GL/gl.h"
// #elif EMSCRIPTEN
// #include <emscripten.h>
// #endif
#include "platform/CCGL.h"

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
class RendererImplemented;

class RenderStateBase;

class DeviceObject;
class Texture;
class TargetTexture;
class DepthTexture;
class VertexBuffer;
class IndexBuffer;
class VertexArray;
class Shader;

class SpriteRenderer;
class RibbonRenderer;

class TextureLoader;

#if _WIN32
#pragma comment(lib, "glu32.lib")
#ifndef NDEBUG
#define GLCheckError()                                                                                                          \
	{                                                                                                                           \
		int __code = glGetError();                                                                                              \
		if (__code != GL_NO_ERROR)                                                                                              \
		{                                                                                                                       \
			printf("GLError filename = %s , line = %d, error = %d\n", __FILE__, __LINE__, (int)(__code)); \
		}                                                                                                                       \
	}
#else
#define GLCheckError()
#endif
#elif EMSCRIPTEN
#ifndef NDEBUG
#define GLCheckError()                                                                                                       \
	{                                                                                                                        \
		int __code = glGetError();                                                                                           \
		if (__code != GL_NO_ERROR)                                                                                           \
		{                                                                                                                    \
			EM_ASM_ARGS({ console.log("GLError filename = " + UTF8ToString($0) + " , line = " + $1); }, __FILE__, __LINE__); \
		}                                                                                                                    \
	}
#else
#define GLCheckError()
#endif
#else
#define GLCheckError()
#endif

//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
//----------------------------------------------------------------------------------
//
//----------------------------------------------------------------------------------
#endif // __EFFEKSEERRENDERER_GL_BASE_H__
