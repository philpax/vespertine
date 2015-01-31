#pragma once

#include "vesp/Types.hpp"

#include "vesp/math/Matrix.hpp"
#include "vesp/math/Vector.hpp"
#include "vesp/math/Quaternion.hpp"

#include "vesp/graphics/Buffer.hpp"

namespace vesp { namespace graphics {

	class Camera
	{
	public:
		Camera();
		virtual ~Camera();

		void Update();

	protected:
		struct Constants
		{
			Mat4 viewProjection;
		};

		virtual void CalculateMatrices();
		Constants MakeConstants();

		float fov_;
		float aspectRatio_;
		float nearPlane_;
		float farPlane_;

		Mat4 view_;
		Mat4 projection_;
		Mat4 viewProjection_;

		ConstantBuffer<Constants> constantBuffer_;
	};

} }