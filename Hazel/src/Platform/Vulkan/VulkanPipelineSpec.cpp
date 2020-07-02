#include "hzpch.h"
#include "VulkanPipelineSpec.h"

namespace Hazel {

	VulkanPipelineSpec::VulkanPipelineSpec(const PipelineCreateInfo& createInfo)
		: PipelineSpecification(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx)
	{
		m_spec = createInfo;

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

		//std::vector<VkDescriptorSetLayoutBinding> uboLayoutBindings_vector;
		//
		//VkDescriptorSetLayoutBinding uboLayoutBinding;
		//if (m_target != 2)
		//{
		//	uboLayoutBinding.binding = m_slot;
		//	uboLayoutBinding.descriptorCount = 1;
		//	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//	uboLayoutBinding.pImmutableSamplers = nullptr;
		//	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		//	uboLayoutBindings_vector.push_back(uboLayoutBinding);
		//}
		//if (m_target != 1)
		//{
		//	uboLayoutBinding.binding = m_slot;
		//	uboLayoutBinding.descriptorCount = 1;
		//	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		//	uboLayoutBinding.pImmutableSamplers = nullptr;
		//	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		//	uboLayoutBindings_vector.push_back(uboLayoutBinding);
		//}


		VkDescriptorSetLayoutBinding uboLayoutBinding{};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.pImmutableSamplers = nullptr;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

		// This must be made in SAMPLER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! TODO!!!
		// This is new for layout binding sampler!!!!
		VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = nullptr;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		layoutInfo.pBindings = bindings.data();

		//VkDescriptorSetLayoutCreateInfo layoutInfo{};
		//layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		//layoutInfo.bindingCount = uboLayoutBindings_vector.size();
		//layoutInfo.pBindings = uboLayoutBindings_vector.data();

		if (vkCreateDescriptorSetLayout(_c.GetDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "failed to create descriptor set layout!")
		}
	}

}