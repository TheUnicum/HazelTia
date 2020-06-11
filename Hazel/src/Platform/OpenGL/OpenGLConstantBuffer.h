#pragma once

#include "Hazel/Renderer/ConstantBuffer.h"

#include <glad/glad.h>

namespace Hazel {

	template<typename C>
	class OpenGLConstantBuffer : public ConstanBuffer
	{
	public:
		OpenGLConstantBuffer(const C& consts)
			: ConstanBuffer(GraphicsContext::Get_Active())
		{
			glGenBuffers(1, &m_RendererID);
			glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
			glBufferData(GL_UNIFORM_BUFFER, sizeof(C), &consts, GL_STATIC_DRAW); // allocate 152 bytes of memory//INFOMAN(gfx);

			//glBufferSubData(GL_UNIFORM_BUFFER, 144, 4, &b);

		}
		virtual ~OpenGLConstantBuffer() = default;

		virtual void Bind() const override
		{
			glBindBuffer(GL_UNIFORM_BUFFER, m_RendererID);
			glBindBufferBase(GL_UNIFORM_BUFFER, m_target, m_RendererID);
		}

		virtual void SetSlot(uint32_t slot, uint32_t target) // target 0 VS & PS, 1 only VS, 2 only PS
		{
			m_slot = slot;
			m_target = target;
		}

		virtual void Unbind() const override {}
	private:
		uint32_t m_RendererID;
		uint32_t m_slot = 0;
		uint32_t m_target = 0;

	};

}
