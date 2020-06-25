#include "hzpch.h"
#include "RenderPasses.h"

#include "Platform/Vulkan/VulkanContext.h"

namespace Hazel {

	RenderPasses::RenderPasses(VulkanContext& ctx)
		: m_ctx(ctx)
	{
	}

	void RenderPasses::Cleanup()
	{
		vkDestroyRenderPass(m_ctx.GetDevice(), m_RenderPass, nullptr);
		m_RenderPass = nullptr;
	}

	void RenderPasses::Bind()
	{
		if (m_RenderPass)
			Cleanup();

		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = m_ctx.GetswapChainImageFormat(); //m_swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // 1 mean no sampling
		// what to do with the data in the attachment before rendering and after
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		// Stencil
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		// layout of the pixels in memory
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; //  Images to be presented in the swap chain

		//-VkAttachmentDescription depthAttachment{};
		//-depthAttachment.format = m_ctx.GetDepthFormat(); // findDepthFormat();
		//-depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		//-depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//-depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//-depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		//-depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//-depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//-depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Subpasses and attachment references
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//Vulkan will automatically transition the attachment to this layout when the subpass is started

		//-VkAttachmentReference depthAttachmentRef{};
		//-depthAttachmentRef.attachment = 1;
		//-depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		// Subpass dependencies
		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1; //  is directly referenced from the fragment shade with the layout(location = 0) out vec4 outColor directive!
		subpass.pColorAttachments = &colorAttachmentRef;
		//-subpass.pDepthStencilAttachment = &depthAttachmentRef;
		//-
		// Subpass dependencies
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; // wait for the swap chain to finish reading from the image
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		// Render pass
		//-std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1; // static_cast<uint32_t>(attachments.size()); ; // 1;
		renderPassInfo.pAttachments = &colorAttachment; // attachments.data(); ; // &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(m_ctx.GetDevice(), &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "Failed to create render pass!");
		}
	}

}
