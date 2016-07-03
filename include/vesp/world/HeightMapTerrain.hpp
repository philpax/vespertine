#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/graphics/Mesh.hpp"

#include "vesp/String.hpp"

namespace vesp { namespace world {

	class HeightMapTerrain : public util::GlobalSystem<HeightMapTerrain>
	{
	public:
		HeightMapTerrain();

		void Load(StringView const path);
		void Draw();

	private:
		graphics::Mesh terrainMesh_;
	};

} }