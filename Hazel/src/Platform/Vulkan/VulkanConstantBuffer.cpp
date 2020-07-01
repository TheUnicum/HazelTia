#include "hzpch.h"
#include "VulkanConstantBuffer.h"

namespace Hazel {

	VulkanConstantBuffer::VulkanConstantBuffer(uint32_t size, const void* data)
		: ConstantBuffer(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx)
	{
	}

	void VulkanConstantBuffer::Bind() const
	{
	}


	void VulkanConstantBuffer::Update(const void* data, uint32_t size)
	{
	}

	void VulkanConstantBuffer::SetSlot(uint32_t slot, uint32_t target)
	{
		m_slot = slot;
		m_target = target;
	}

}
