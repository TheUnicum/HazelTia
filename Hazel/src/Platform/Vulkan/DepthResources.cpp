#include "hzpch.h"
#include "DepthResources.h"

#include "Platform/Vulkan/VulkanContext.h"

#include "Platform/Vulkan/VulkanBuffer.h"

namespace Hazel
{

	DepthResources::DepthResources(VulkanContext& ctx)
		: m_ctx(ctx)
	{
		VkFormat depthFormat = findDepthFormat();

		CreateImage(m_ctx, m_ctx.GeSwapchainExtent().width, m_ctx.GeSwapchainExtent().height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_DepthImage, m_DepthImageMemory);
		createImageViewFromImage(m_ctx, m_DepthImageView, m_DepthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	DepthResources::~DepthResources()
	{
	}

	void DepthResources::Cleanup()
	{
		//VkDevice& device = m_ctx.GetDevice();
		//
		//vkDestroyImageView(device, m_DepthImageView, nullptr);
		//
		//vkDestroyImage(device, m_DepthImage, nullptr);
		//vkFreeMemory(device, m_DepthImageMemory, nullptr);
	}

	VkFormat DepthResources::findDepthFormat()
	{
		return findSupportedFormat(m_ctx,
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

}
