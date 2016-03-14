#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/graphics/Mesh.hpp"

#include "vesp/String.hpp"

namespace vesp { namespace world {

	class TerrainManager : public util::GlobalSystem<TerrainManager>
	{
	public:
		TerrainManager();

		void Load(StringView const path);
		void Draw();

	private:
		struct OctreeNode
		{
			struct Corner
			{
				bool isValue = false;
				union
				{
					float value;
					OctreeNode* node;
				};
			};

			Corner corners[8];

			float SampleAtCorner(size_t i);

			void Polygonise();
			graphics::Mesh nodeMesh_;
		};
	};
} }