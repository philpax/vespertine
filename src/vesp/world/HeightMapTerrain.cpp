#include "vesp/world/HeightMapTerrain.hpp"

#include "vesp/graphics/ShaderManager.hpp"

#include "vesp/math/Util.hpp"

#include "vesp/Profiler.hpp"

namespace vesp { namespace world {

HeightMapTerrain::HeightMapTerrain()
{
	this->heightMap_ = graphics::Image::FromPath("data/heightmap.png");
	this->Load();
}

void HeightMapTerrain::Load()
{
	auto data = this->heightMap_.data.get();
	S32 sizeX = this->heightMap_.sizeX;
	S32 sizeY = this->heightMap_.sizeY;

	LogInfo("Loading heightmap (%d %d)", sizeX, sizeY);

	Vector<graphics::Vertex> vertices;
	Vector<U32> indices;

	auto GetIndex = [&](S32 x, S32 y) -> U32
	{
		x = math::Clamp(x, 0, sizeX-1);
		y = math::Clamp(y, 0, sizeY-1);

		return y * sizeX + x;
	};
	
	auto Sample = [&](S32 x, S32 y) -> graphics::Vertex
	{
		auto height = data[GetIndex(x, y)];

		graphics::Vertex v;
		v.position = Vec3(x, height / 2.0f, y);
		v.colour = graphics::Colour(height, 0, 255-height);

		// Use Sobel filter to calculate normals
		F32 s[9];
		s[0] = data[GetIndex(x-1, y+1)];
		s[1] = data[GetIndex(x+0, y+1)];
		s[2] = data[GetIndex(x+1, y+1)];

		s[3] = data[GetIndex(x-1, y+0)];
		s[4] = data[GetIndex(x+0, y+0)];
		s[5] = data[GetIndex(x+1, y+0)];

		s[6] = data[GetIndex(x-1, y-1)];
		s[7] = data[GetIndex(x+0, y-1)];
		s[8] = data[GetIndex(x+1, y-1)];

		Vec3 normal;
		normal.x = -(s[2] - s[0] + 2*(s[5] - s[3]) + s[8] - s[6]);
		normal.y = -(s[6] - s[0] + 2*(s[7] - s[1]) + s[8] - s[2]);
		normal.z = 1.0f;
		normal = glm::normalize(normal);

		v.SetNormal(normal);

		return v;
	};

	vertices.reserve(sizeX * sizeY);
	indices.reserve(sizeX * sizeY * 6);

	for (S32 y = 0; y < sizeY; y += 1)
		for (S32 x = 0; x < sizeX; x += 1)
			vertices.push_back(Sample(x, y));

	const S32 SampleRate = 4;
	for (S32 y = 0; y < sizeY; y += SampleRate)
	{
		for (S32 x = 0; x < sizeX; x += SampleRate)
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
	this->terrainMesh_.SetVertexShader("default");
	this->terrainMesh_.SetPixelShader("grid");
}

void HeightMapTerrain::Draw()
{
	VESP_PROFILE_FN();
	this->terrainMesh_.Draw();
}

} }