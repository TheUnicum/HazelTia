#include "hzpch.h"
#include "VulkanVertexLayout.h"

#include "Hazel/Renderer/GraphicsContext.h"
#include "VulkanBuffer.h"

namespace Hazel {

	//----------- utiliy buffer Functions---------------------------------
	uint32_t findMemoryType(VulkanContext& m_ctx, uint32_t typeFilter, VkMemoryPropertyFlags properties)
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(m_ctx.GetPhysicalDevice(), &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void createBuffer(VulkanContext& m_ctx, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(m_ctx.GetDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(m_ctx.GetDevice(), buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(m_ctx, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_ctx.GetDevice(), &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(m_ctx.GetDevice(), buffer, bufferMemory, 0);
	}
	void copyBuffer(VulkanContext& m_ctx, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) // In Image is splitted in 3 (begin/action/end)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_ctx.GetCommandPool(); //commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_ctx.GetDevice(), &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_ctx.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_ctx.GetGraphicsQueue());

		vkFreeCommandBuffers(m_ctx.GetDevice(), m_ctx.GetCommandPool(), 1, &commandBuffer);
	}
	void CreateImage(VulkanContext& m_ctx, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
	{
		VkDevice& device = m_ctx.GetDevice();

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format; // VK_FORMAT_R8G8B8A8_SRGB
		imageInfo.tiling = tiling; // VK_IMAGE_TILING_OPTIMAL
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage; // VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT; // The samples flag is related to multisampling.
		imageInfo.flags = 0; // Optional
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // The image will only be used by one queue family

		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements); // for Image

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(m_ctx, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);
	}
	VkCommandBuffer beginSingleTimeCommands(VulkanContext& m_ctx)
	{
		VkDevice& device = m_ctx.GetDevice();

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_ctx.GetCommandPool(); // commandPool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}
	void endSingleTimeCommands(VulkanContext& m_ctx, VkCommandBuffer commandBuffer)
	{
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(m_ctx.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(m_ctx.GetGraphicsQueue());

		vkFreeCommandBuffers(m_ctx.GetDevice(), m_ctx.GetCommandPool(), 1, &commandBuffer);
	}
	void copyBufferToImage(VulkanContext& m_ctx, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands(m_ctx);

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0; // Specifying 0 for both indicates that the pixels are simply tightly packed like they are in our case.
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endSingleTimeCommands(m_ctx, commandBuffer);
	}
	void transitionImageLayout(VulkanContext& m_ctx, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
	{
		VkCommandBuffer commandBuffer = beginSingleTimeCommands(m_ctx);

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		// If you are using the barrier to transfer queue family ownership, then these two fields should be the indices of the queue families
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			//0,0,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endSingleTimeCommands(m_ctx, commandBuffer);
	}
	VkImageView createImageView(VulkanContext& m_ctx, VkImage image, VkFormat format)
	{
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(m_ctx.GetDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create texture image view!");
		}
	}
	//----------- utiliy buffer Functions---------------------------------



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

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, uint32_t count)
		: IndexBuffer(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx), m_Count(count)
	{
		VkDeviceSize bufferSize = count * sizeof(uint32_t);

		// Temporary Buffer
		VkBuffer m_stagingBuffer;
		VkDeviceMemory m_stagingBufferMemory;

		createBuffer(_c, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_stagingBuffer, m_stagingBufferMemory);

		void* data;
		vkMapMemory(_c.GetDevice(), m_stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, static_cast<void*>(indices), (size_t)bufferSize);
		vkUnmapMemory(_c.GetDevice(), m_stagingBufferMemory);

		createBuffer(_c, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

		copyBuffer(_c, m_stagingBuffer, m_indexBuffer, bufferSize);

		vkDestroyBuffer(_c.GetDevice(), m_stagingBuffer, nullptr);
		vkFreeMemory(_c.GetDevice(), m_stagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
	}

}