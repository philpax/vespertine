#pragma once

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vesp { namespace math {

	template <typename T>
	Mat4 DXPerspective
	(
		T fovy,
		T aspect,
		T zNear,
		T zFar
	)
	{
		assert(abs(aspect - std::numeric_limits<T>::epsilon()) > static_cast<T>(0));
		assert(zFar > zNear);

		T const tanHalfFovy = -tan(fovy / static_cast<T>(2));

		Mat4 Result(static_cast<T>(0));
		Result[0][0] = static_cast<T>(1) / (aspect * tanHalfFovy);
		Result[1][1] = static_cast<T>(1) / (tanHalfFovy);
		Result[2][2] = (zFar) / (zFar - zNear);
		Result[2][3] = static_cast<T>(1);
		Result[3][2] = - (zFar * zNear) / (zFar - zNear);
		return Result;
	}

} }