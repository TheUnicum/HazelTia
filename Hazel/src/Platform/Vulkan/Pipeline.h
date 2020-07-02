#pragma once

#include "vulkan/vulkan.h"

#include "Hazel/Renderer/PipelineSpecification.h"
#include "Hazel/Renderer/Shader.h"
#include "Hazel/Renderer/VertexLayout.h"

namespace Hazel {

	class VulkanContext;

	class Pipeline
	{
	public:
		Pipeline(VulkanContext& ctx);
		virtual ~Pipeline();

		void Cleanup();

		void Create(const PipelineCreateInfo& spec);
		void CreateDescriptorPoolandSets(const PipelineCreateInfo& spec);

		void Bind();

		VkPipeline& Get() { return m_Pipeline; }
		VkPipelineLayout& GetLayout() { return m_PipelineLayout; }
		VkDescriptorSet& GetDescriptorSet() { return m_descriptorSets; }
	private:
		VulkanContext& m_ctx;
		//PipelineCreateInfo m_spec;

		VkPipelineLayout m_PipelineLayout = nullptr;
		VkPipeline m_Pipeline = nullptr;

		VkDescriptorPool m_descriptorPool = nullptr;
		VkDescriptorSet m_descriptorSets = nullptr;
	};

}
