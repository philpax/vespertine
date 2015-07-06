#pragma once

#include "vesp/Types.hpp"
#include "vesp/math/Vector.hpp"
#include "vesp/graphics/Colour.hpp"

namespace vesp { namespace graphics {
	
	struct Vertex
	{
		Vertex();
		Vertex(Vec3 position, Vec3 normal, Vec2 texcoord = Vec2(0.0f, 0.0f));
		Vertex(Vec3 position, Vec3 normal, Colour colour);

		void SetNormal(Vec3 normal);
		Vec3 GetNormal() const;

		void SetTexcoord(Vec2 texcoord);
		Vec2 GetTexcoord() const;

		Vec3 position;
		U16 normal[2];
		U16 texcoord[2];
		Colour colour = Colour::White;
	};

	static_assert(sizeof(Vertex) == 24, "Vertex size is wrong");

} }