#pragma once
#include "IndexedTriangleList.h"
#include "TiaMath.h"

namespace Hazel {

	class Prism
	{
	public:
		template<class V>
		static IndexedTriangleList<V> MakeTesselated(int circlePoints)
		{
			HZ_CORE_ASSERT((circlePoints >= 3), "circlePoints must be at least 3!");

			const auto base = glm::vec3(+0.5f, 0.0f, -0.5f);
			const float stepAngle = 2.0f * PI / circlePoints;

			// vertices
			std::vector<V> vertices;
			for (int iLong = 0; iLong < circlePoints; iLong++)
			{
				// near base
				vertices.emplace_back();
				vertices.back().pos = { cos(stepAngle * iLong) * 0.5f, sin(stepAngle * iLong) * 0.5f, -0.5f };
				// far base
				vertices.emplace_back();
				vertices.back().pos = { cos(stepAngle * iLong) * 0.5f, sin(stepAngle * iLong) * 0.5f, +0.5f };
			}

			// near center
			vertices.emplace_back();
			vertices.back().pos = { 0.0f, 0.0f,-0.5f };
			const auto iCenterNear = (uint32_t)(vertices.size() - 1);
			// far center
			vertices.emplace_back();
			vertices.back().pos = { 0.0f, 0.0f,+0.5f };
			const auto iCenterFar = (uint32_t)(vertices.size() - 1);

			// side indices
			std::vector<uint32_t> indices;
			for (int iLong = 0; iLong < circlePoints; iLong++)
			{
				const auto i = iLong * 2;
				const auto mod = (circlePoints) * 2;
				indices.push_back(i + 0);
				indices.push_back((i + 3) % mod);
				indices.push_back(i + 1);

				indices.push_back(i + 0);
				indices.push_back((i + 2) % mod);
				indices.push_back((i + 3) % mod);
			}

			// base2 indices
			for (int iLong = 0; iLong < circlePoints; iLong++)
			{
				const auto i = iLong * 2;
				const auto mod = (circlePoints) * 2;
				indices.push_back(i + 0);
				indices.push_back(iCenterNear);
				indices.push_back((i + 2) % mod);

				indices.push_back(i + 1);
				indices.push_back((i + 3) % mod);
				indices.push_back(iCenterFar);
			}

			return { std::move(vertices),std::move(indices) };
		}
		template<class V>
		static IndexedTriangleList<V> Make()
		{
			return MakeTesselated<V>(24);
		}
	};

}
