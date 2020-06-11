#pragma once
#include "IndexedTriangleList.h"
#include "TiaMath.h"

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective

namespace Hazel {

	class Sphere
	{
	public:
		template<class V>
		static IndexedTriangleList<V> MakeTesselated(int latDiv, int longDiv)
		{
			HZ_CORE_ASSERT((latDiv >= 3), "latDiv must be at least 3!");
			HZ_CORE_ASSERT((longDiv >= 3), "longDiv must be at least 3!");

			constexpr float radius = 0.5f;
			auto base = glm::vec4(0.0f, radius, 0.0f, 1.0f);
			const float lattitudeAngle = PI / latDiv;
			const float longitudeAngle = 2.0f * PI / longDiv;

			std::vector<V> vertices;
			for (int iLat = 1; iLat < latDiv; iLat++)
			{
				glm::vec4 latBase = glm::rotate(glm::mat4(1.0f), lattitudeAngle * iLat, glm::vec3(0.0f, 0.0f, 1.0f)) * base;
				for (int iLong = 0; iLong < longDiv; iLong++)
				{
					vertices.emplace_back();
					vertices.back().pos = glm::vec3(glm::rotate(glm::mat4(1.0f), longitudeAngle * iLong, glm::vec3(0.0f, 1.0f, 0.0f)) * latBase);
				}
			}

			// add the cap vertices
			const auto iNorthPole = (uint32_t)vertices.size();
			vertices.emplace_back();
			vertices.back().pos = base;
			const auto iSouthPole = (uint32_t)vertices.size();
			vertices.emplace_back();
			vertices.back().pos = -base;

			const auto calcIdx = [latDiv, longDiv](unsigned short iLat, unsigned short iLong)
			{ return iLat * longDiv + iLong; };
			std::vector<uint32_t> indices;
			for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
			{
				for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
				{
					indices.push_back(calcIdx(iLat, iLong));
					indices.push_back(calcIdx(iLat + 1, iLong));
					indices.push_back(calcIdx(iLat, iLong + 1));
					indices.push_back(calcIdx(iLat, iLong + 1));
					indices.push_back(calcIdx(iLat + 1, iLong));
					indices.push_back(calcIdx(iLat + 1, iLong + 1));
				}
				// wrap band
				indices.push_back(calcIdx(iLat, longDiv - 1));
				indices.push_back(calcIdx(iLat + 1, longDiv - 1));
				indices.push_back(calcIdx(iLat, 0));
				indices.push_back(calcIdx(iLat, 0));
				indices.push_back(calcIdx(iLat + 1, longDiv - 1));
				indices.push_back(calcIdx(iLat + 1, 0));
			};
			// cap fans
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				// north
				indices.push_back(iNorthPole);
				indices.push_back(calcIdx(0, iLong));
				indices.push_back(calcIdx(0, iLong + 1));
				// south
				indices.push_back(calcIdx(latDiv - 2, iLong + 1));
				indices.push_back(calcIdx(latDiv - 2, iLong));
				indices.push_back(iSouthPole);
			}
			// wrap triangles
			// north
			indices.push_back(iNorthPole);
			indices.push_back(calcIdx(0, longDiv - 1));
			indices.push_back(calcIdx(0, 0));
			// south
			indices.push_back(iSouthPole);
			indices.push_back(calcIdx(latDiv - 2, 0));
			indices.push_back(calcIdx(latDiv - 2, longDiv - 1));

			return { std::move(vertices),std::move(indices) };
		};
		template<class V>
		static IndexedTriangleList<V> Make()
		{
			return MakeTesselated<V>(12, 24);
		}
	};
}
