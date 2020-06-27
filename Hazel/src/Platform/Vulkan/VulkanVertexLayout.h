#pragma once
#include "Hazel/Renderer/VertexLayout.h"
#include <vector>

#include "vulkan/vulkan.h"

namespace Hazel
{

	struct VertexX {
		glm::vec2 pos;
		glm::vec3 color;
	};
	
	class VulkanVertexLayout : public VertexLayout
	{
	public:
		VkFormat get_format(const ElementType& type)
		{
			switch (type)
			{
				case VertexLayout::AP_FLOAT2:	return VK_FORMAT_R32G32_SFLOAT;
				case VertexLayout::AP_FLOAT3:	return VK_FORMAT_R32G32B32_SFLOAT;
				case VertexLayout::AP_FLOAT4:	return VK_FORMAT_R32G32B32A32_SFLOAT;
			};
			HZ_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		}
		
		void* GetDescriptorData() override
		{
			m_descriptorData.InputBindingDescription.binding = 0;
			m_descriptorData.InputBindingDescription.stride = m_stride;
			m_descriptorData.InputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

			m_descriptorData.VertexInputAttributeDescription_list.clear();
			m_descriptorData.VertexInputAttributeDescription_list.resize(GetElementCount());
			
			int i = 0;
			for (const auto& ele : m_elements)
			{
				m_descriptorData.VertexInputAttributeDescription_list[i].binding = 0;
				m_descriptorData.VertexInputAttributeDescription_list[i].location = i;
				m_descriptorData.VertexInputAttributeDescription_list[i].format = get_format(ele.GetType());
				m_descriptorData.VertexInputAttributeDescription_list[i].offset = ele.GetOffset();
				i++;
			}

			return static_cast<void*>(&m_descriptorData);
		};
	public:
		struct DescriptorData
		{
			VkVertexInputBindingDescription InputBindingDescription;
			std::vector<VkVertexInputAttributeDescription> VertexInputAttributeDescription_list;
		};
	private:
		DescriptorData m_descriptorData;
	};



	


}
