#pragma once

#include "vulkan/vulkan.h"

namespace Hazel {

	class VulkanContext;

	class RenderPasses
	{
	public:
		RenderPasses(VulkanContext& ctx);
		virtual ~RenderPasses() = default;

		void CleanUp();

		void Set();
		VkRenderPass& Get() { return m_RenderPass; }
	private:
		VulkanContext& m_ctx;
		VkRenderPass m_RenderPass;
	};

}
