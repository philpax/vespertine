#include "vesp/graphics/Vertex.hpp"
#include "vesp/math/Util.hpp"
#include "vesp/Log.hpp"

#include <glm/gtc/constants.hpp>

namespace vesp { namespace graphics {

	Vertex::Vertex()
	{
	}

	Vertex::Vertex(Vec3 position, Vec3 normal, Vec2 texcoord)
	{
		this->position = position;
		this->SetNormal(normal);
		this->SetTexcoord(texcoord);
	}

	Vertex::Vertex(Vec3 position, Vec3 normal, Colour colour)
	{
		this->position = position;
		this->SetNormal(normal);
		this->colour = colour;
	}

	void Vertex::SetTexcoord(Vec2 texcoord)
	{
		this->texcoord[0] = math::PackFloat<U16>(texcoord.x);
		this->texcoord[1] = math::PackFloat<U16>(texcoord.y);
	}

	Vec2 Vertex::GetTexcoord() const
	{
		auto x = math::UnpackFloat<U16>(this->texcoord[0]);
		auto y = math::UnpackFloat<U16>(this->texcoord[1]);

		return Vec2(x, y);
	}

	void Vertex::SetNormal(Vec3 normal)
	{
		float inclination = acos(normal.z);
		float azimuth = atan2(normal.y, normal.x);

		auto pi = glm::pi<float>();
		this->normal[0] = math::PackFloat<U16>(inclination, 0, pi);
		this->normal[1] = math::PackFloat<U16>(azimuth, -pi, pi);
	}

	Vec3 Vertex::GetNormal() const
	{
		auto pi = glm::pi<float>();
		float inclination = math::UnpackFloat<U16>(this->normal[0], 0, pi);
		float azimuth = math::UnpackFloat<U16>(this->normal[1], -pi, pi);

		float si = sin(inclination);
		float ci = cos(inclination);
		float sa = sin(azimuth);
		float ca = cos(azimuth);
		
		return Vec3(si * ca, si * sa, ci);
	}

} }