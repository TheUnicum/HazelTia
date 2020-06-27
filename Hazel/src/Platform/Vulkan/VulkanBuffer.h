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

}