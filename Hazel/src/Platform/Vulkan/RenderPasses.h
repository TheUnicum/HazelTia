#pragma once

#include "vulkan/vulkan.h"

namespace Hazel {

	class VulkanContext;

	class RenderPasses
	{
	public:
		RenderPasses(VulkanContext& ctx);
		virtual ~RenderPasses() = default;

		void Cleanup();

		void Bind();
		VkRenderPass& Get() { return m_RenderPass; }
	private:
		VulkanContext& m_ctx;
		VkRenderPass m_RenderPass = nullptr;
	};

}
