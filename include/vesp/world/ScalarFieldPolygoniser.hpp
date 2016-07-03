#pragma once

#include "vesp/graphics/Mesh.hpp"

#include "vesp/String.hpp"

namespace vesp { namespace world {

	class ScalarFieldPolygoniser
	{
	public:
		ScalarFieldPolygoniser();

		void Load(StringView const path);
		void Draw();

	private:
		graphics::Mesh mesh_;

		typedef struct
		{
			Vec3 p[3];
		} TRIANGLE;

		typedef struct
		{
			Vec3 p[8];
			double val[8];
		} GRIDCELL;

		void Polygonise(GRIDCELL grid, double isolevel, std::vector<graphics::Vertex>& vertices);
	};
} }