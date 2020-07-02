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

	void VulkanConstantBuffer::Bind() const
	{
		_c.BindConstantBuffer(*this);
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
		CreateDescriptorPoolandSets();	// For Bind Operation
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

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = uboLayoutBindings_vector.size();
		layoutInfo.pBindings = uboLayoutBindings_vector.data();

		if (vkCreateDescriptorSetLayout(_c.GetDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "failed to create descriptor set layout!")
		}
	}

	void VulkanConstantBuffer::CreateDescriptorPoolandSets()
	{
		VkDevice& device = _c.GetDevice();

		// ------CreateDescriptorPool
		VkDescriptorPoolSize poolSize{};
		poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSize.descriptorCount = 1;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = 1;
		poolInfo.pPoolSizes = &poolSize;
		poolInfo.maxSets = 1;// static_cast<uint32_t>(m_swapChainImages.size());

		if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "failed to create descriptor pool!")
		}

		// ------CreateDescriptorSets
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_descriptorSetLayout;

		//m_descriptorSets.resize(m_swapChainImages.size());
		if (vkAllocateDescriptorSets(device, &allocInfo, &m_descriptorSets) != VK_SUCCESS)
		{
			HZ_CORE_ASSERT(false, "failed to allocate descriptor sets!")
		}

		//for (size_t i = 0; i < m_swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = m_uniformBuffer;
		bufferInfo.offset = 0;
		bufferInfo.range = m_size; // sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite{};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets;
		descriptorWrite.dstBinding = m_slot;  //<----------------
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional

		vkUpdateDescriptorSets(device, 1, &descriptorWrite, 0, nullptr);
	}

}
