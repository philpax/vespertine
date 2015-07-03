#pragma once

#pragma warning(push)
#pragma warning(disable: 4201)

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vesp/Types.hpp"

#pragma warning(pop)

namespace vesp { namespace math {

	Mat4 DXPerspective(F32 fovYInRadians, F32 aspect, F32 zNear, F32 zFar);
	Mat4 Transform(Vec3 const& position, Quat const& angle, Vec3 const& scale = Vec3(1,1,1));

} }