#pragma once
#include "IndexedTriangleList.h"
#include "TiaMath.h"

namespace Hazel {

	class Cone
	{
	public:
		template<class V>
		static IndexedTriangleList<V> MakeTesselated(int circlePoints)
		{
			HZ_CORE_ASSERT((circlePoints >= 3), "circlePoints must be at least 3!");

			const float stepAngle = 2.0f * PI / circlePoints;

			//// base vertices
			std::vector<V> vertices;
			vertices.emplace_back();
			vertices.back().pos = { 0.5f, 0.0f, -0.5f };
			for (int iLong = 1; iLong < circlePoints; iLong++)
			{
				vertices.emplace_back();
				vertices.back().pos = { cos(stepAngle * iLong) * 0.5f, sin(stepAngle * iLong) * 0.5f, -0.5f };
			}
			// the center
			vertices.emplace_back();
			vertices.back().pos = { 0.0f,0.0f,-0.5f };
			const auto iCenter = (uint32_t)(vertices.size() - 1);
			//// the tip :darkness:
			vertices.emplace_back();
			vertices.back().pos = { 0.0f, 0.0f, +0.5f };
			const auto iTip = (uint32_t)(vertices.size() - 1);

			// base indices
			std::vector<uint32_t> indices;
			for (unsigned short iLong = 0; iLong < circlePoints; iLong++)
			{
				indices.push_back(iCenter);
				indices.push_back((iLong + 1) % circlePoints);
				indices.push_back(iLong);
			}

			// cone indices
			for (unsigned short iLong = 0; iLong < circlePoints; iLong++)
			{
				indices.push_back(iLong);
				indices.push_back((iLong + 1) % circlePoints);
				indices.push_back(iTip);
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
