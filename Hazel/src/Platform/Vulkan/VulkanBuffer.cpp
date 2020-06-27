#include "hzpch.h"
#include "VulkanVertexLayout.h"

#include "Hazel/Renderer/GraphicsContext.h"
#include "VulkanBuffer.h"

namespace Hazel {

	VulkanVertexBuffer::VulkanVertexBuffer(float* vertices, uint32_t size)
		: VertexBuffer(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx)
	{
		// Buffer creation
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size; //sizeof(vertices[0]) * vertices.size();
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; //The buffer will only be used from the graphics queue.
		
		if (vkCreateBuffer(_c.GetDevice(), &bufferInfo, nullptr, &m_vertexBuffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create vertex buffer!");
		}
		
		// Memory requirements
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(_c.GetDevice(), m_vertexBuffer, &memRequirements);
		
		// Memory allocation
		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		//  like being able to map it so we can write to it from the CPU. This property is indicated with VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
		
		if (vkAllocateMemory(_c.GetDevice(), &allocInfo, nullptr, &m_vertexBufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate vertex buffer memory!");
		}
		
		// If memory allocation was successful, then we can now associate this memory with the buffer using vkBindBufferMemory:
		vkBindBufferMemory(_c.GetDevice(), m_vertexBuffer, m_vertexBufferMemory, 0); // the fourth parameter is the offset within the region of memory.
		
		// Filling the vertex buffer
		// Warn: It is also possible that writes to the buffer are not visible in the mapped memory yet
		// here we use VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		void* data;
		vkMapMemory(_c.GetDevice(), m_vertexBufferMemory, 0, bufferInfo.size, 0, &data);
		memcpy(data, vertices, (size_t)bufferInfo.size);
		vkUnmapMemory(_c.GetDevice(), m_vertexBufferMemory);
	}


	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		//vkDestroyBuffer(m_ctx.GetDevice(), m_vertexBuffer, nullptr);
		//vkFreeMemory(m_ctx.GetDevice(), m_vertexBufferMemory, nullptr);
	}

	//void VulkanVertexBuffer::Cleanup()
	//{
	//	vkDestroyBuffer(m_ctx.GetDevice(), m_vertexBuffer, nullptr);
	//	vkFreeMemory(m_ctx.GetDevice(), m_vertexBufferMemory, nullptr);
	//}


	uint32_t VulkanVertexBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(_c.GetPhysicalDevice(), &memProperties);
	
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}
	
		throw std::runtime_error("failed to find suitable memory type!");
	
	}

}