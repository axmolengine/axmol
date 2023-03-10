#include "../EffekseerForCocos2d-x.h"
#ifndef AX_USE_METAL

#include "../../EffekseerRendererCommon/ModelLoader.h"
#include "../../EffekseerRendererGL/EffekseerRendererGL.h"
#include "../../EffekseerRendererGL/EffekseerRenderer/EffekseerRendererGL.MaterialLoader.h"
#include "renderer/backend/opengl/TextureGL.h"

namespace efk {

#pragma region DistortingCallbackGL
class DistortingCallbackGL
	: public EffekseerRenderer::DistortingCallback
{
	GLuint framebufferForCopy = 0;
	GLuint backGroundTexture = 0;
	uint32_t backGroundTextureWidth = 0;
	uint32_t backGroundTextureHeight = 0;
	GLuint backGroundTextureInternalFormat = 0;

public:
	DistortingCallbackGL();

	~DistortingCallbackGL() override;

	void ReleaseTexture();

	// prepare a taget
	void PrepareTexture(uint32_t width, uint32_t height, GLint internalFormat);

	virtual bool OnDistorting(EffekseerRenderer::Renderer* renderer) override;
};

DistortingCallbackGL::DistortingCallbackGL()
{
	glGenTextures(1, &backGroundTexture);
#ifndef _WIN32
	glGenFramebuffers(1, &framebufferForCopy);
#endif
}

DistortingCallbackGL::~DistortingCallbackGL()
{
	ReleaseTexture();
}

void DistortingCallbackGL::ReleaseTexture()
{
#ifndef _WIN32
	glDeleteFramebuffers(1, &framebufferForCopy);
#endif
	glDeleteTextures(1, &backGroundTexture);
}

void DistortingCallbackGL::PrepareTexture(uint32_t width, uint32_t height, GLint internalFormat)
{
	glBindTexture(GL_TEXTURE_2D, backGroundTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

	backGroundTextureWidth = width;
	backGroundTextureHeight = height;
	backGroundTextureInternalFormat = internalFormat;
}

bool DistortingCallbackGL::OnDistorting(EffekseerRenderer::Renderer* renderer)
{
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	uint32_t width = viewport[2];
	uint32_t height = viewport[3];

	if (backGroundTextureWidth != width ||
		backGroundTextureHeight != height)
	{
		PrepareTexture(width, height, GL_RGBA);
	}

	glBindTexture(GL_TEXTURE_2D, backGroundTexture);
	//glCopyTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height );
	glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, viewport[0], viewport[1], width, height);//bad performace.(Iphone - OpenGL ES3)
	glBindTexture(GL_TEXTURE_2D, 0);

	auto r = static_cast<EffekseerRendererGL::Renderer*>(renderer);
	r->SetBackground(backGroundTexture);

	return true;
}
#pragma endregion

static ::Effekseer::Backend::GraphicsDeviceRef g_deviceObjectCollection = nullptr;

class EffekseerGraphicsDevice : public ::EffekseerRendererGL::Backend::GraphicsDevice
{
private:

public:
	EffekseerGraphicsDevice(::EffekseerRendererGL::OpenGLDeviceType deviceType)
		: ::EffekseerRendererGL::Backend::GraphicsDevice(deviceType)
	{
	}

	virtual ~EffekseerGraphicsDevice()
	{

	}

	static ::Effekseer::Backend::GraphicsDeviceRef create()
	{
		if (g_deviceObjectCollection == nullptr)
		{
#if (AX_TARGET_PLATFORM == AX_PLATFORM_IOS || AX_TARGET_PLATFORM == AX_PLATFORM_ANDROID || defined(AX_USE_GLES))
			g_deviceObjectCollection = Effekseer::MakeRefPtr<EffekseerGraphicsDevice>(EffekseerRendererGL::OpenGLDeviceType::OpenGLES2);
#else
			g_deviceObjectCollection = Effekseer::MakeRefPtr<EffekseerGraphicsDevice>(EffekseerRendererGL::OpenGLDeviceType::OpenGL2);
#endif
		}

		return g_deviceObjectCollection;
	}

	int Release() override
	{
		auto ret = ::EffekseerRendererGL::Backend::GraphicsDevice::Release();
		if (ret == 1)
		{
			g_deviceObjectCollection = nullptr;
		}

		return ret;
	}
};

Effekseer::ModelLoaderRef CreateModelLoader(Effekseer::FileInterfaceRef effectFile)
{
	return Effekseer::MakeRefPtr<::EffekseerRenderer::ModelLoader>(EffekseerGraphicsDevice::create(), effectFile);
}

::Effekseer::MaterialLoaderRef CreateMaterialLoader(Effekseer::FileInterfaceRef effectFile)
{
	return Effekseer::MakeRefPtr<::EffekseerRendererGL::MaterialLoader>(EffekseerGraphicsDevice::create().DownCast<::EffekseerRendererGL::Backend::GraphicsDevice>(), effectFile);
}

void UpdateTextureData(::Effekseer::TextureRef textureData, cocos2d::Texture2D* texture)
{
	auto textureGL = static_cast<cocos2d::backend::Texture2DGL*>(texture->getBackendTexture());

	auto device = EffekseerGraphicsDevice::create().DownCast<::EffekseerRendererGL::Backend::GraphicsDevice>();

	auto backend = device->CreateTexture(textureGL->getHandler(), texture->hasMipmaps(), []() -> void {});
	textureData->SetBackend(backend);
}

void CleanupTextureData(::Effekseer::TextureRef textureData) {}

::EffekseerRenderer::DistortingCallback* CreateDistortingCallback(::EffekseerRenderer::RendererRef renderer, Effekseer::RefPtr<::EffekseerRenderer::CommandList> commandList)
{
	return new DistortingCallbackGL();
}

void EffectEmitter::beforeRender(EffekseerRenderer::RendererRef renderer, ::Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
}

void EffectEmitter::afterRender(EffekseerRenderer::RendererRef renderer, ::Effekseer::RefPtr<EffekseerRenderer::CommandList> commandList)
{
}

void EffectManager::onDestructor()
{
}

void EffectManager::CreateRenderer(int32_t spriteSize)
{
	renderer2d = ::EffekseerRendererGL::Renderer::Create(EffekseerGraphicsDevice::create(), spriteSize);
}

void EffectManager::newFrame() {}

void ResetBackground(::EffekseerRenderer::RendererRef renderer)
{
    auto r = static_cast<::EffekseerRendererGL::Renderer*>(renderer.Get());
    r->SetBackground(0);
}

}

#endif
