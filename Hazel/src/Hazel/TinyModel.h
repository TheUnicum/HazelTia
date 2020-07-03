#pragma once
#include <glm/glm.hpp>

namespace Hazel {

	struct VertexM
	{
		glm::vec3 pos;
		glm::vec3 color;
		glm::vec2 texCoord;


		bool operator==(const VertexM& other) const {
			return pos == other.pos && color == other.color && texCoord == other.texCoord;
		}
	};

	class TinyModel
	{

	public:
		TinyModel(const std::string& filepath);

		std::vector<VertexM> GetV() { return m_vertices; }
		std::vector<uint32_t> GetI() { return m_indices; }
		uint32_t GetsizeVertex() { return (uint32_t)(sizeof(VertexM) * m_vertices.size()); }
		uint32_t GetsizeICount() { return (uint32_t)(m_indices.size()); }
	public:
		std::vector<VertexM> m_vertices;
		std::vector<uint32_t> m_indices;
	};

}
