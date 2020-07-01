#pragma once
#include "Hazel/Renderer/ConstantBuffer.h"

namespace Hazel {

	class OpenGLConstantBuffer : public ConstantBuffer
	{
	public:
		OpenGLConstantBuffer(uint32_t size, const void* data);
		virtual ~OpenGLConstantBuffer() = default;

		virtual void Bind() const override;
		virtual void Unbind() const override {}

		virtual void Update(const void* data, uint32_t size = 0) override;
		virtual void SetSlot(uint32_t slot, uint32_t target) override; // target 0 VS & PS, 1 only VS, 2 only PS
	private:
		uint32_t m_RendererID;
		uint32_t m_size = 0;
		uint32_t m_slot = 0;
		uint32_t m_target = 0;
	};

}
