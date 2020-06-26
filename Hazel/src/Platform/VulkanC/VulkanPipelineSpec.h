#pragma once

#include "Hazel/Renderer/PipelineSpecification.h"
#include "Platform/VulkanC/VulkanContext.h"

#include "Platform/VulkanC/Pipeline.h"


namespace Hazel {

	class VulkanPipelineSpec : public PipelineSpecification
	{
	public:
		VulkanPipelineSpec(const PipelineCreateInfo& createInfo);
		virtual ~VulkanPipelineSpec();

		virtual void Bind() const override;
		virtual void Unbind() const override {};

		void ReCreate() const;

	private:
		std::string m_Name;
		mutable std::shared_ptr<Pipeline> m_Pipeline;
	public:
		VulkanContext& _c;
	};

}