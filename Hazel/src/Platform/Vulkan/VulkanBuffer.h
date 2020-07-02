#pragma once
#include <cstdint>
#include <string>
#include <vector>

#include <Vulkan/vulkan.h>
#include <glm/glm.hpp>

#define GLM_FORCE_RADIANS
// We need to configure it to use the Vulkan range of 0.0 to 1.0 using the GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include "Hazel/Renderer/Buffer.h"

#include "VulkanContext.h"

namespace Hazel {

	class VulkanContext;


	uint32_t findMemoryType(VulkanContext& m_ctx, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VulkanContext& m_ctx, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VulkanContext& m_ctx, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	void CreateImage(VulkanContext& m_ctx, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkCommandBuffer beginSingleTimeCommands(VulkanContext& m_ctx);
	void endSingleTimeCommands(VulkanContext& m_ctx, VkCommandBuffer commandBuffer);
	//void copyBuffer(VulkanContext& m_ctx, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void copyBufferToImage(VulkanContext& m_ctx, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void transitionImageLayout(VulkanContext& m_ctx, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	VkImageView createImageView(VulkanContext& m_ctx, VkImage image, VkFormat format);

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(float* vertices, uint32_t size);
		~VulkanVertexBuffer();

		virtual void Bind() const override
		{
			_c.BindVertexBuffer(m_vertexBuffer);
		};
		virtual void Unbind() const override {};

		virtual void SetData(const void* data, uint32_t size) override {};

		virtual const BufferLayout& GetLayout() const override
		{
			return BufferLayout{};
		};
		virtual void SetLayout(const BufferLayout& layout) override {};
		//void Cleanup();

		uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		VkBuffer& Get() { return m_vertexBuffer; }
	private:
		VulkanContext& _c;
		VkBuffer m_vertexBuffer;
		VkDeviceMemory m_vertexBufferMemory;

	};



	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* indices, uint32_t count);
		~VulkanIndexBuffer();

		virtual void Bind() const override
		{
			_c.BindIndexBuffer(m_indexBuffer);
		};
		virtual void Unbind() const override {};

		virtual uint32_t GetCount() const { return m_Count; };
		//void Cleanup();

		VkBuffer& Get() { return m_indexBuffer; }
	private:
		VulkanContext& _c;

		VkBuffer m_indexBuffer;
		VkDeviceMemory m_indexBufferMemory;
		uint32_t m_Count;
	};

}
