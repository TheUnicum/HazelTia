#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

//#include "VertexBuffer.h"

namespace Hazel {

	class VulkanContext;

	class CommandBuffer
	{
	public:
		CommandBuffer(VulkanContext& ctx);
		virtual ~CommandBuffer();

		//void Rec(VkFramebuffer& framebuffer, VertexBuffer& vb, IndexBuffer& ib, UniformBuffer& ubo);
		void Rec(VkFramebuffer& framebuffer);
		void Submit();

		void Clenaup();

		VkCommandBuffer& Get() { return m_cmdBuffer; }
	private:
		VulkanContext& m_ctx;
		VkCommandBuffer m_cmdBuffer;
	};

}
