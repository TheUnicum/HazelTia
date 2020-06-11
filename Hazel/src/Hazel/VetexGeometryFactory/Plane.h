#pragma once
#include "IndexedTriangleList.h"
#include "TiaMath.h"

namespace Hazel {

	class Plane
	{
	public:
		template<class V>
		static IndexedTriangleList<V> MakeTesselated(int divisions_x, int divisions_y)
		{
			HZ_CORE_ASSERT((divisions_x >= 1), "Plane division x must be at least 1!");
			HZ_CORE_ASSERT((divisions_y >= 1), "Plane division y must be at least 1!");

			constexpr float width = 1.0f;
			constexpr float height = 1.0f;
			const int nVertices_x = divisions_x + 1;
			const int nVertices_y = divisions_y + 1;
			std::vector<V> vertices(nVertices_x * nVertices_y);

			// Make Vertices
			{
				const float side_x = width / 2.0f;
				const float side_y = height / 2.0f;
				const float tileSize_x = width / float(divisions_x);
				const float tileSize_y = height / float(divisions_y);

				for (int y = 0, i = 0; y < nVertices_y; y++)
				{
					const float y_pos = (float(y) * tileSize_y) - side_y;
					const float y_pos_tc = 1.0f - float(y) * tileSize_y;
					for (int x = 0; x < nVertices_x; x++, i++)
					{
						vertices[i].pos = glm::vec3((float(x) * tileSize_x) - side_x, y_pos, 0.0f);
						vertices[i].tex = glm::vec2((float(x) * tileSize_x), y_pos_tc);
					}
				}
			}

			std::vector<uint32_t> indices;
			indices.reserve(divisions_x * divisions_y * 6);
			{
				const auto vxy2i = [nVertices_x](size_t x, size_t y) {return (unsigned short)(y * nVertices_x + x); };
				for (int y = 0; y < divisions_y; y++)
				{
					for (int x = 0; x < divisions_x; x++)
					{
						const std::array<unsigned short, 4> indexArray = 
						{ vxy2i(x,y),vxy2i(x + 1,y),vxy2i(x,y + 1),vxy2i(x + 1,y + 1) };
						indices.push_back(indexArray[0]);
						indices.push_back(indexArray[2]);
						indices.push_back(indexArray[1]);
						indices.push_back(indexArray[1]);
						indices.push_back(indexArray[2]);
						indices.push_back(indexArray[3]);
					}
				}
			}

			return { std::move(vertices),std::move(indices) };
		}

		template<class V>
		static IndexedTriangleList<V> Make()
		{
			return MakeTesselated<V>(1, 1);
		}
	};

}
