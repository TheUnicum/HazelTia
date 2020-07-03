#pragma once

#include "vulkan/vulkan.h"

namespace Hazel {

	class VulkanContext;

	class DepthResources
	{
	public:
		DepthResources(VulkanContext& ctx);
		virtual ~DepthResources();

		void Cleanup();
		VkFormat findDepthFormat();
		VkFormat& GetDepthFormat() { return m_depthFormat; }
		VkImageView& GetimageView() { return m_DepthImageView; }
	private:
		VulkanContext& m_ctx;
		VkImage m_DepthImage;
		VkImageView m_DepthImageView;
		VkDeviceMemory m_DepthImageMemory;

		VkFormat m_depthFormat;
	};

}
