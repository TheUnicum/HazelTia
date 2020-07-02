#include "hzpch.h"
#include "VulkanPipelineSpec.h"
#include "VulkanConstantBuffer.h"
#include "VulkanTexture.h"

namespace Hazel {

	VulkanPipelineSpec::VulkanPipelineSpec(const PipelineCreateInfo& createInfo)
		: PipelineSpecification(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx)
	{
		m_spec = createInfo;

		m_spec.texture->SetSlot(1);

		SetDescriptorSetLayout();

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
	}

	void VulkanPipelineSpec::ReCreate() const
	{
		// clean up???
		m_Pipeline->Create(m_spec, m_descriptorSetLayout); // layout inside constant buffer if exist!
		if (m_spec.constantBuffer)
			m_Pipeline->CreateDescriptorPoolandSets(m_spec, m_descriptorSetLayout);
	}

	void VulkanPipelineSpec::SetDescriptorSetLayout()
	{	// Cleanup
		if (m_descriptorSetLayout)
			vkDestroyDescriptorSetLayout(_c.GetDevice(), m_descriptorSetLayout, nullptr);

		std::vector<VkDescriptorSetLayoutBinding> bindings;
		if (m_spec.constantBuffer)
			bindings.push_back(std::dynamic_pointer_cast<VulkanConstantBuffer>(m_spec.constantBuffer)->GetLayoutBinding());
		if (m_spec.texture)
			bindings.push_back(std::dynamic_pointer_cast<VulkanTexture2D>(m_spec.texture)->GetLayoutBinding());
		
		
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		if (vkCreateDescriptorSetLayout(_c.GetDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "failed to create descriptor set layout!")
		}
	}

}