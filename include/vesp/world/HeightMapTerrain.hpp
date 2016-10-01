#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/graphics/Mesh.hpp"
#include "vesp/graphics/Image.hpp"

#include "vesp/String.hpp"

namespace vesp { namespace world {

	class HeightMapTerrain : public util::GlobalSystem<HeightMapTerrain>
	{
	public:
		HeightMapTerrain();

		void Load();
		void Draw();

	private:
		graphics::Image heightMap_;
		graphics::Mesh terrainMesh_;
	};

} }