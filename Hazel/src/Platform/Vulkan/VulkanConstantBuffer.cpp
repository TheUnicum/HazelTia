#include "hzpch.h"
#include "Platform/Vulkan/VulkanConstantBuffer.h"

#include "Platform/Vulkan/VulkanBuffer.h"

namespace Hazel {

	VulkanConstantBuffer::VulkanConstantBuffer(uint32_t size, const void* data)
		: ConstantBuffer(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx), m_size(size)
	{
		createBuffer(_c, m_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffer, m_uniformBufferMemory);

		Update(data, m_size);
	}

	VulkanConstantBuffer::~VulkanConstantBuffer()
	{
		VkDevice& device = _c.GetDevice();

		vkDestroyBuffer(device, m_uniformBuffer, nullptr);
		vkFreeMemory(device, m_uniformBufferMemory, nullptr);
	}


	void VulkanConstantBuffer::Update(const void* data, uint32_t size)
	{
		VkDevice& device = _c.GetDevice();
		void* mapData;
		vkMapMemory(device, m_uniformBufferMemory, 0, size, 0, &mapData);
		memcpy(mapData, data, size);
		vkUnmapMemory(device, m_uniformBufferMemory);
	}

	void VulkanConstantBuffer::SetSlot(uint32_t slot, uint32_t target)
	{
		m_slot = slot;
		m_target = target;
		SetDescriptorSetLayout();		// For Pipeline Information
	}

	void VulkanConstantBuffer::SetDescriptorSetLayout()
	{
	
	}

}
