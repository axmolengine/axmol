
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
#include "EffekseerRendererGL.VertexArray.h"
#include "EffekseerRendererGL.GLExtension.h"
#include "EffekseerRendererGL.IndexBuffer.h"
#include "EffekseerRendererGL.Shader.h"
#include "EffekseerRendererGL.VertexBuffer.h"

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
namespace EffekseerRendererGL
{
//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexArray::VertexArray(
	const Backend::GraphicsDeviceRef& graphicsDevice, Shader* shader, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
	: DeviceObject(graphicsDevice.Get())
	, m_shader(shader)
	, m_vertexBuffer(vertexBuffer)
	, m_indexBuffer(indexBuffer)
{
	Init();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexArray::~VertexArray()
{
	Release();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
VertexArray*
VertexArray::Create(const Backend::GraphicsDeviceRef& graphicsDevice, Shader* shader, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
{
	if (GLExt::IsSupportedVertexArray())
	{
		return new VertexArray(graphicsDevice, shader, vertexBuffer, indexBuffer);
	}
	return nullptr;
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexArray::OnLostDevice()
{
	Release();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexArray::OnResetDevice()
{
	Init();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexArray::Init()
{
	GLCheckError();

	if (!m_shader->IsValid())
	{
		m_shader->OnResetDevice();
	}

	GLCheckError();

	glGenVertexArrays(1, &m_vertexArray);

	glBindVertexArray(m_vertexArray);

	if (m_vertexBuffer != nullptr)
	{
		if (!m_vertexBuffer->IsValid())
		{
			m_vertexBuffer->OnResetDevice();
		}

		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer->GetInterface());
	}

	if (m_indexBuffer != nullptr)
	{
		if (!m_indexBuffer->IsValid())
		{
			m_indexBuffer->OnResetDevice();
		}

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer->GetInterface());
	}

	if (m_vertexBuffer != nullptr)
	{
		m_shader->EnableAttribs();
	}

	glBindVertexArray(0);

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
void VertexArray::Release()
{
	GLCheckError();

	if (m_vertexArray != 0)
	{
		glDeleteVertexArrays(1, &m_vertexArray);
		m_vertexArray = 0;
	}

	GLCheckError();
}

//-----------------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------------
} // namespace EffekseerRendererGL
  //-----------------------------------------------------------------------------------
  //
  //-----------------------------------------------------------------------------------