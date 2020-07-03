#pragma once

#include "Platform/Vulkan/VulkanContext.h"

namespace Hazel {

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
