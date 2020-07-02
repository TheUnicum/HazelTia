#include "hzpch.h"
#include "Platform/Vulkan/VulkanConstantBuffer.h"

#include "Platform/Vulkan/VulkanBuffer.h"

namespace Hazel {

	VulkanConstantBuffer::VulkanConstantBuffer(uint32_t size, const void* data)
		: ConstantBuffer(GraphicsContext::Get_Active()), _c((VulkanContext&)this->_ctx), m_size(size)
	{
		createBuffer(_c, m_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffer, m_uniformBufferMemory);

		Update(data, m_size);
	}

	VulkanConstantBuffer::~VulkanConstantBuffer()
	{
		VkDevice& device = _c.GetDevice();

		vkDestroyDescriptorPool(device, m_descriptorPool, nullptr);

		vkDestroyDescriptorSetLayout(device, m_descriptorSetLayout, nullptr);

		vkDestroyBuffer(device, m_uniformBuffer, nullptr);
		vkFreeMemory(device, m_uniformBufferMemory, nullptr);
	}


	void VulkanConstantBuffer::Update(const void* data, uint32_t size)
	{
		VkDevice& device = _c.GetDevice();
		void* mapData;
		vkMapMemory(device, m_uniformBufferMemory, 0, size, 0, &mapData);
		memcpy(mapData, data, size);
		vkUnmapMemory(device, m_uniformBufferMemory);
	}

	void VulkanConstantBuffer::SetSlot(uint32_t slot, uint32_t target)
	{
		m_slot = slot;
		m_target = target;
		SetDescriptorSetLayout();		// For Pipeline Information
	}

	void VulkanConstantBuffer::SetDescriptorSetLayout()
	{
		// Cleanup
		if (m_descriptorSetLayout)
			vkDestroyDescriptorSetLayout(_c.GetDevice(), m_descriptorSetLayout, nullptr);

		std::vector<VkDescriptorSetLayoutBinding> uboLayoutBindings_vector;

		VkDescriptorSetLayoutBinding uboLayoutBinding;
		if (m_target != 2)
		{
			uboLayoutBinding.binding = m_slot;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.pImmutableSamplers = nullptr;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			uboLayoutBindings_vector.push_back(uboLayoutBinding);
		}
		if (m_target != 1)
		{
			uboLayoutBinding.binding = m_slot;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.pImmutableSamplers = nullptr;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			uboLayoutBindings_vector.push_back(uboLayoutBinding);
		}

		// This must be made in SAMPLER!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! TODO!!!
		// This is new for layout binding sampler!!!!
		//VkDescriptorSetLayoutBinding samplerLayoutBinding{};
		//samplerLayoutBinding.binding = 1;
		//samplerLayoutBinding.descriptorCount = 1;
		//samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		//samplerLayoutBinding.pImmutableSamplers = nullptr;
		//samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		//std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
		//VkDescriptorSetLayoutCreateInfo layoutInfo{};
		//layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		//layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
		//layoutInfo.pBindings = bindings.data();

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = uboLayoutBindings_vector.size();
		layoutInfo.pBindings = uboLayoutBindings_vector.data();

		if (vkCreateDescriptorSetLayout(_c.GetDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "failed to create descriptor set layout!")
		}
	}

}
