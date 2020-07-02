#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>

//#include "VertexBuffer.h"
#include "Platform/Vulkan/Pipeline.h"

namespace Hazel {

	class VulkanContext;

	class CommandBuffer
	{
	public:
		CommandBuffer(VulkanContext& ctx);
		virtual ~CommandBuffer();

		//void Rec(VkFramebuffer& framebuffer, VertexBuffer& vb, IndexBuffer& ib, UniformBuffer& ubo);
		void Bind(const VkFramebuffer& framebuffer);

		
		void BindVertexBuffer(const VkBuffer& vb);
		void BindIndexBuffer(const VkBuffer& ib);
		void BindPipeline(const Ref<Pipeline>& pipeline);
		void BindConstantBuffer(const ConstantBuffer&cb);
		void Draw(uint32_t vertexCount, uint32_t instanceCount = 1);


		void Flush(const VkFramebuffer& framebuffer);

		void Clenaup();

		VkCommandBuffer& Get() { return m_cmdBuffer; }
		std::vector<std::function<void(const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)>>& GetQueue() {
			return m_Queue;
		};
	private:
		VulkanContext& m_ctx;
		VkCommandBuffer m_cmdBuffer;

		std::vector<std::function<void(const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)>> m_Queue;
	};

}
