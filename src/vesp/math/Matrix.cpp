#include "vesp/math/Matrix.hpp"
#include "vesp/Assert.hpp"

#include <glm/gtc/quaternion.hpp>

namespace vesp { namespace math {

	Mat4 DXPerspective(F32 fovYInRadians, F32 aspect, F32 zNear, F32 zFar)
	{
		VESP_ASSERT(abs(aspect - std::numeric_limits<F32>::epsilon()) > 0.0f);
		VESP_ASSERT(zFar > zNear);

		F32 const tanHalfFovY = tan(fovYInRadians / 2.0f);

		Mat4 result(0.0f);
		result[0][0] = 1.0f / (aspect * tanHalfFovY);
		result[1][1] = 1.0f / tanHalfFovY;
		result[2][2] = (zFar) / (zFar - zNear);
		result[2][3] = 1.0f;
		result[3][2] = - (zFar * zNear) / (zFar - zNear);
		return result;
	}

	Mat4 Transform(Vec3 const& position, Quat const& angle, Vec3 const& scale)
	{
		Mat4 result;

		result = glm::scale(result, scale);
		result = glm::translate(result, position);
		result *= glm::mat4_cast(angle);

		return result;
	}

} }