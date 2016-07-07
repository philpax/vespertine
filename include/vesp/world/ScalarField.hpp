#pragma once

#include "vesp/graphics/Mesh.hpp"

#include "vesp/String.hpp"

namespace vesp { namespace world {

	class ScalarField
	{
	public:
		ScalarField();

		Vector<graphics::Vertex> Polygonise(float isolevel);

	private:
		typedef struct
		{
			Vec3 p[3];
		} TRIANGLE;

		typedef struct
		{
			Vec3 p[8];
			float val[8];
		} GRIDCELL;

		U32 xSize_;
		U32 ySize_;
		U32 zSize_;

		UniquePtr<float[]> data_;
		void PolygoniseCell(GRIDCELL grid, float isolevel, Vector<graphics::Vertex>& vertices);
	};
} }