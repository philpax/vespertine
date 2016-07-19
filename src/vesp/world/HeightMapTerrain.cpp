#include "vesp/world/HeightMapTerrain.hpp"

#include "vesp/graphics/stb_image.h"
#include "vesp/graphics/ShaderManager.hpp"

#include "vesp/math/Util.hpp"

#include "vesp/FileSystem.hpp"

namespace vesp { namespace world {

HeightMapTerrain::HeightMapTerrain()
{
	this->Load("data/heightmap.png");
}

void HeightMapTerrain::Load(StringView const path)
{
	auto file = FileSystem::Get()->Open(path, FileSystem::Mode::ReadBinary);
	auto imageData = file.Read<U8>();

	S32 xSize = 0, ySize = 0, comp = 0;
	auto data = UniquePtrWithDeleter(stbi_uc, 
		stbi_load_from_memory(imageData.data(), imageData.size(), &xSize, &ySize, &comp, 1), 
		stbi_image_free);

	VESP_ASSERT(data);

	LogInfo("Loading heightmap (%d %d)", xSize, ySize);

	Vector<graphics::Vertex> vertices;
	Vector<U32> indices;

	auto GetIndex = [&](S32 x, S32 y) -> U32
	{
		x = math::Clamp(x, 0, xSize-1);
		y = math::Clamp(y, 0, ySize-1);

		return y * xSize + x;
	};
	
	auto Sample = [&](S32 x, S32 y) -> graphics::Vertex
	{
		auto height = data.get()[GetIndex(x, y)];

		graphics::Vertex v;
		v.position = Vec3(x, height / 2.0f, y);
		v.colour = graphics::Colour(height, 0, 255-height);

		return v;
	};

	vertices.reserve(xSize * ySize);
	indices.reserve(xSize * ySize * 6);

	for (S32 y = 0; y < ySize; y += 1)
		for (S32 x = 0; x < xSize; x += 1)
			vertices.push_back(Sample(x, y));

	const S32 SampleRate = 4;
	for (S32 y = 0; y < ySize; y += SampleRate)
	{
		for (S32 x = 0; x < xSize; x += SampleRate)
		{
			indices.push_back(GetIndex(x + SampleRate, y + SampleRate));
			indices.push_back(GetIndex(x + SampleRate, y));
			indices.push_back(GetIndex(x, y));

			indices.push_back(GetIndex(x, y));
			indices.push_back(GetIndex(x, y + SampleRate));
			indices.push_back(GetIndex(x + SampleRate, y + SampleRate));
		}
	}

	auto shaderManager = graphics::ShaderManager::Get();
	VESP_ENFORCE(this->terrainMesh_.Create(vertices, indices));
	this->terrainMesh_.SetVertexShader(shaderManager->GetVertexShader("default"));
	this->terrainMesh_.SetPixelShader(shaderManager->GetPixelShader("grid"));
}

void HeightMapTerrain::Draw()
{
	this->terrainMesh_.Draw();
}

} }