#include "hzpch.h"
#include "TinyModel.h"

#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

namespace std {
	template<> struct hash<Hazel::VertexM> {
		size_t operator()(Hazel::VertexM const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.texCoord) << 1);
		}
	};
}

namespace Hazel {

	TinyModel::TinyModel(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		{
			HZ_CORE_ASSERT(false, warn + err);
		}
		
		std::unordered_map<VertexM, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices) {
				VertexM vertex{};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					//1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					attrib.texcoords[2 * index.texcoord_index + 1]
				};

				vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
					m_vertices.push_back(vertex);
				}

				m_indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

}
