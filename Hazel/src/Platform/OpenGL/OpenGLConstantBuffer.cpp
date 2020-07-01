#include "hzpch.h"
#include "Platform/OpenGL/OpenGLConstantBuffer.h"

#include <glad/glad.h>

namespace Hazel {

	OpenGLConstantBuffer::OpenGLConstantBuffer(uint32_t size, const void* data)
		: ConstantBuffer(GraphicsContext::Get_Active()), m_size(size)
	{
		glGenBuffers(1, &m_RendererID);
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBufferData(GL_UNIFORM_BUFFER, m_size, data, GL_DYNAMIC_DRAW);
	}

	void OpenGLConstantBuffer::Bind() const
	{
		glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
		glBindBufferBase(GL_UNIFORM_BUFFER, m_target, m_RendererID);
	}

	void OpenGLConstantBuffer::Update(const void* data, uint32_t size)
	{
		size = std::min(size, m_size);
		size = size ? size : m_size;
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	void OpenGLConstantBuffer::SetSlot(uint32_t slot, uint32_t target)
	{
		m_slot = slot;
		m_target = target;
	}

}
