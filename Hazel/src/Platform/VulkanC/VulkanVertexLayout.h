#pragma once

#include "Hazel/Renderer/VertexLayout.h"
//#include "Platform/VulkanC/VulkanContext.h"

namespace Hazel {

	struct Vertex1
	{
		glm::vec2 pos;
		glm::vec3 color;
	};

	class VulkanVertexLayout : public VertexLayout
	{
	public:
		//void test();
		//std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions()
		//{
		//	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
		//	attributeDescriptions.resize(3);
		//
		//	attributeDescriptions[0].binding = 0;
		//	attributeDescriptions[0].location = 0;
		//	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		//	attributeDescriptions[0].offset = offsetof(Vertex1, pos);
		//
		//	attributeDescriptions[1].binding = 0;
		//	attributeDescriptions[1].location = 1;
		//	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		//	attributeDescriptions[1].offset = offsetof(Vertex1, color);
		//
		//
		//	return attributeDescriptions;
		//}

	};



}
