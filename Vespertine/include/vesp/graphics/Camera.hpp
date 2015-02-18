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

		virtual void Update();

		Mat4 const& GetView();
		Mat4 const& GetProjection();

		void* operator new(size_t i);
		void operator delete(void* p);

	protected:
		struct PerFrameConstants
		{
			Mat4 viewProjection;
		};

		void CalculateMatrices();
		PerFrameConstants MakeConstants();

		float fov_;
		float aspectRatio_;
		float nearPlane_;
		float farPlane_;

		Mat4 view_;
		Mat4 projection_;
		Mat4 viewProjection_;

		ConstantBuffer<PerFrameConstants> constantBuffer_;
	};

} }