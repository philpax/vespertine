#pragma once

#include <glm/fwd.hpp>

namespace vesp
{
	// Integer types
	typedef unsigned char		U8;
	typedef signed char			S8;

	typedef unsigned short		U16;
	typedef signed short		S16;

	typedef unsigned int		U32;
	typedef signed int			S32;

	typedef unsigned long long	U64;
	typedef signed long long	S64;

	// Floating point types
	typedef float				F32;
	typedef double				F64;

	// String types (assume UTF-8)
	typedef char				StringByte;
	typedef const StringByte*	StringPtr;

	// Vector types
	typedef glm::vec2			Vec2;
	typedef glm::vec3			Vec3;
	typedef glm::vec4			Vec4;

	typedef glm::ivec2			IVec2;
	typedef glm::ivec3			IVec3;
	typedef glm::ivec4			IVec4;

	// Matrix types
	typedef glm::mat2			Mat2;
	typedef glm::mat3			Mat3;
	typedef glm::mat4			Mat4;

	// Quaternion types
	typedef glm::quat			Quat;
}