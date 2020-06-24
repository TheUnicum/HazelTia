#include "hzpch.h"
#include "Pipeline.h"

#include "Platform/Vulkan/VulkanContext.h"

namespace Hazel {

	Pipeline::Pipeline(VulkanContext& ctx)
		: m_ctx(ctx)
	{
	}

	Pipeline::~Pipeline()
	{
	}

	void Pipeline::Cleanup()
	{
		vkDestroyPipeline(m_ctx.GetDevice(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(m_ctx.GetDevice(), m_PipelineLayout, nullptr);
	}

	void Pipeline::SetSpec(const PipelineSpecification& spec)
	{
		m_spec = spec;
	}

	void Pipeline::Bind()
	{
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		//vertShaderStageInfo.module = m_spec.shader->GetV(); //vert;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		//fragShaderStageInfo.module = m_spec.shader->GetF(); // frag;
		fragShaderStageInfo.pName = "main";
	}


}