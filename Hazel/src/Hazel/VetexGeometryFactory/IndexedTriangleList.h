#pragma once

#include "Hazel/Core/Base.h"

#include <vector>
#include <glm/glm.hpp>

namespace Hazel {

	template<class T>
	class IndexedTriangleList
	{
	public:
		IndexedTriangleList() = default;
		IndexedTriangleList(std::vector<T> verts_in, std::vector<uint32_t> indices_in)
			:
			vertices(std::move(verts_in)),
			indices(std::move(indices_in))
		{
			HZ_CORE_ASSERT((vertices.size() > 2), "IndexedTriangleList must contein at least 3 vertices!");
			HZ_CORE_ASSERT((indices.size() % 3 == 0), "IndexedTriangleList, indices number must be MOD 3!");
		}
		void Transform(glm::mat4 matrix)
		{
			for (auto& v : vertices)
			{
				v.pos = glm::vec3(matrix * glm::vec4(v.pos, 1.0f));
			}
		}

	public:
		std::vector<T> vertices;
		std::vector<uint32_t> indices;
	};

}
