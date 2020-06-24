#pragma once

#include "vulkan/vulkan.h"

namespace Hazel {

	class VulkanContext;

	struct PipelineSpecification
	{
		//std::shared_ptr<Shader> shader;
		//std::shared_ptr<BufferLayout> bufferLayout;
		//std::shared_ptr<UniformBuffer> uniformBuffer;
	};

	class Pipeline
	{
	public:
		Pipeline(VulkanContext& ctx);
		virtual ~Pipeline();

		void Cleanup();

		void SetSpec(const PipelineSpecification& spec);
		void Bind();

		VkPipeline& Get() { return m_Pipeline; }
		VkPipelineLayout& GetLayout() { return m_PipelineLayout; }
	private:
		VulkanContext& m_ctx;
		PipelineSpecification m_spec;

		VkPipelineLayout m_PipelineLayout = nullptr;
		VkPipeline m_Pipeline = nullptr;
	};

}
