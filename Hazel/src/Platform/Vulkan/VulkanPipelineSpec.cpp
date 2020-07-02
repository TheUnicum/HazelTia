#include "hzpch.h"
#include "VulkanPipelineSpec.h"

namespace Hazel {

	VulkanPipelineSpec::VulkanPipelineSpec(const PipelineCreateInfo& createInfo)
		: PipelineSpecification(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx)
	{
		m_spec = createInfo;
		m_Pipeline = CreateRef<Pipeline>(_c);
		ReCreate();
	}

	VulkanPipelineSpec::~VulkanPipelineSpec()
	{
	}

	void VulkanPipelineSpec::Bind() const
	{
		ReCreate();
		m_spec.shader->Bind(); // Nothing in vulkan
		m_Pipeline->Bind();

		if (m_spec.constantBuffer)
			m_spec.constantBuffer->Bind();


	}

	void VulkanPipelineSpec::ReCreate() const
	{
		// clean up???
		m_Pipeline->Create(m_spec);
	}

}