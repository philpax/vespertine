#pragma once

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "vesp/Types.hpp"

namespace vesp { namespace math {

	Mat4 DXPerspective(F32 fovY, F32 aspect, F32 zNear, F32 zFar);

} }