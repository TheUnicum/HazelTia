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
	private:
		VulkanContext& m_ctx;
		VkImage m_DepthImage;
		VkImageView m_DepthImageView;
		VkDeviceMemory m_DepthImageMemory;
	};

}
