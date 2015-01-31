#pragma once

#include "vesp/graphics/Camera.hpp"

namespace vesp { namespace graphics {

	class FreeCamera : public Camera
	{
	public:
		virtual ~FreeCamera();

		void SetPosition(Vec3 const& position);
		void SetAngle(Quat const& angle);

		Vec3 GetPosition() const;
		Quat GetAngle() const;

	protected:
		virtual void CalculateMatrices();

		Vec3 position_;
		Quat angle_;
	};

} }