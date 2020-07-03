#include "hzpch.h"
#include "CommandBuffer.h"

#include "Platform/Vulkan/VulkanContext.h"
#include "Platform/Vulkan/VulkanBuffer.h"
#include "Platform/Vulkan/Pipeline.h"
#include "Platform/Vulkan/VulkanConstantBuffer.h"

#include <array>

namespace Hazel {

	CommandBuffer::CommandBuffer(VulkanContext& ctx)
		: m_ctx(ctx)
	{
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_ctx.GetCommandPool(); // m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_ctx.GetDevice(), &allocInfo, &m_cmdBuffer) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to allocate command buffers!");
		}
	}

	CommandBuffer::~CommandBuffer()
	{
	}

	void CommandBuffer::Bind(const VkFramebuffer& framebuffer)
	{
		/*
		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT:
			The command buffer will be rerecorded right after executing it once.
		VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT :T
				his is a secondary command buffer that will be entirely within a single render pass.
		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT :
				The command buffer can be resubmitted while it is also already pending execution.
		*/

		// We begin recording a command buffe
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; // Optional
		beginInfo.pInheritanceInfo = nullptr; // Optional

		if (vkBeginCommandBuffer(m_cmdBuffer, &beginInfo) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to begin recording command buffer!");
		}

		// Starting a render pass
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		// The first parameters are the render pass itself and the attachments to bind
		renderPassInfo.renderPass = m_ctx.GetRenderPass();// m_RenderPasses->Get(); //m_renderPass;
		renderPassInfo.framebuffer = framebuffer; //m_swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_ctx.GeSwapchainExtent();// m_swapChainExtent;

		// value used for VK_ATTACHMENT_LOAD_OP_CLEAR
		
		//*VkClearValue clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		//*renderPassInfo.clearValueCount = 1;
		//*renderPassInfo.pClearValues = &clearColor;
		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };
		
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		// "The render pass can now begin!!!"
		vkCmdBeginRenderPass(m_cmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ctx.GetPipeline());

		if (m_ctx.GetPipelineHasDesctriptorSet())
			vkCmdBindDescriptorSets(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ctx.GetPipelineLayout(), 0, 1, &m_ctx.GetPipelineDescSet(), 0, nullptr);

		

		//if (false)
		//{
		//	// Basic drawing commands
		//	//vkCmdBindPipeline(m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);
		//	vkCmdBindPipeline(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ctx.GetPipeline());
		//
		//	//--// Binding the vertex buffer
		//	//--VkBuffer vertexBuffers[] = { vb.Get() };
		//	//--VkDeviceSize offsets[] = { 0 };
		//	//--vkCmdBindVertexBuffers(m_cmdBuffer, 0, 1, vertexBuffers, offsets);
		//	//--
		//	//--vkCmdBindIndexBuffer(m_cmdBuffer, ib.Get(), 0, VK_INDEX_TYPE_UINT32);
		//	//--
		//	//--// UBO
		//	//--//vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);
		//	//--vkCmdBindDescriptorSets(m_cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_ctx.GetPipelineLayout(), 0, 1, &m_ctx.GetDescriptorSets(), 0, nullptr);
		//	//--
		//	//--
		//	//--vkCmdDrawIndexed(m_cmdBuffer, ib.Count(), 1, 0, 0, 0);
		//
		//	//vkCmdDraw(m_cmdBuffer, 3, 1, 0, 0);
		//	vkCmdDraw(m_cmdBuffer, 3, 1, 0, 0);
		//}

		Flush(framebuffer);

		// Finishing up
		vkCmdEndRenderPass(m_cmdBuffer);

		if (vkEndCommandBuffer(m_cmdBuffer) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to record command buffer!");
		}

	}



	void CommandBuffer::BindVertexBuffer(const VkBuffer& vb)
	{
		m_Queue.push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
		{
			// Binding the vertex buffer
			VkBuffer vertexBuffers[] = { vb };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(m_cmdBuffer, 0, 1, vertexBuffers, offsets);
		});
	}

	void CommandBuffer::BindIndexBuffer(const VkBuffer& ib)
	{
		m_Queue.push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
		{
			vkCmdBindIndexBuffer(m_cmdBuffer, ib, 0, VK_INDEX_TYPE_UINT32);
		});
	}


	void CommandBuffer::BindPipeline(const Ref<Pipeline>& pipeline)
	{
		m_Queue.push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
		{
			vkCmdBindPipeline(drawCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline->Get());

		});
	}


	void CommandBuffer::Draw(uint32_t vertexCount, uint32_t instanceCount)
	{
		m_Queue.push_back([=](const VkCommandBuffer& drawCommandBuffer, const VkFramebuffer& framebuffer)
		{
			vkCmdDraw(drawCommandBuffer, vertexCount, instanceCount, 0, 0);
		});
	}


	void CommandBuffer::Flush(const VkFramebuffer& framebuffer)
	{
		for (auto& cmd : m_Queue)
		{
			cmd(m_cmdBuffer, framebuffer);
		}
		m_Queue.clear();
	}

	void CommandBuffer::Clenaup()
	{
	}

}
