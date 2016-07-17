#pragma once

#include "vesp/graphics/Mesh.hpp"

#include "vesp/String.hpp"

namespace vesp { namespace world {

	class ScalarField
	{
	public:
		typedef F32 Scalar;

		ScalarField();

		void Load(Scalar const* data, U32 xSize, U32 ySize, U32 zSize);
		Vector<graphics::Vertex> Polygonise(Scalar isolevel);

	private:
		typedef struct
		{
			Vec3 p[3];
		} TRIANGLE;

		typedef struct
		{
			Vec3 p[8];
			Scalar val[8];
		} GRIDCELL;

		U32 xSize_;
		U32 ySize_;
		U32 zSize_;

		UniquePtr<Scalar[]> data_;
		void PolygoniseCell(GRIDCELL grid, Scalar isolevel, Vector<graphics::Vertex>& vertices);
	};
} }