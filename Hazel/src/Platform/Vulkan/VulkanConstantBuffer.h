#pragma once
#include "Hazel/Renderer/ConstantBuffer.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace Hazel {

	class VulkanConstantBuffer : public ConstantBuffer
	{
	public:
		VulkanConstantBuffer(uint32_t size, const void* data);
		virtual ~VulkanConstantBuffer();
			
		virtual void Bind() const override {};
		virtual void Unbind() const override {}

		virtual void Update(const void* data, uint32_t size = 0) override;
		virtual void SetSlot(uint32_t slot, uint32_t target); // target 0 VS & PS, 1 only VS, 2 only PS

		VkDescriptorSetLayoutBinding& GetLayoutBinding() { return m_LayoutBinding; }

	private:
		void SetDescriptorSetLayout();
	private:
	public:
		uint32_t m_size = 0;
		uint32_t m_slot = 0;
		uint32_t m_target = 0;

	public:
		VulkanContext& _c;

		VkBuffer m_uniformBuffer;
		VkDeviceMemory m_uniformBufferMemory;

		VkDescriptorSetLayoutBinding m_LayoutBinding{};
	};

}
