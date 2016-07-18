#pragma once

#include "vesp/graphics/Mesh.hpp"

#include "vesp/String.hpp"

#include <functional>

namespace vesp { namespace world {

	class ScalarField
	{
	public:
		typedef F32 Scalar;

		ScalarField();

		void Load(Scalar const* data, U32 xSize, U32 ySize, U32 zSize);
		
		template <typename Functor>
		void LoadFromFunction(U32 xSize, U32 ySize, U32 zSize, Functor&& f)
		{
			auto data = std::make_unique<world::ScalarField::Scalar[]>(xSize*ySize*zSize);
			auto dataPtr = data.get();

			for (auto z = 0u; z < zSize; z++)
			{
				for (auto y = 0u; y < ySize; y++)
				{
					for (auto x = 0u; x < xSize; x++)
					{
						auto point = Vec3(x - (xSize/2.0f), y - (ySize / 2.0f), z - (zSize / 2.0f));
						auto index = z * (ySize * xSize) + y * (xSize) + x;
						dataPtr[index] = f(point);
					}
				}
			}
			this->Load(data.get(), xSize, ySize, zSize);
		}

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