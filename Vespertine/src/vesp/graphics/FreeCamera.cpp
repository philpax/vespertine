#pragma warning(disable: 4005)
#include "vesp/graphics/FreeCamera.hpp"

namespace vesp { namespace graphics {

	FreeCamera::~FreeCamera()
	{
	}

	void FreeCamera::SetPosition(Vec3 const& position)
	{
		this->position_ = position;
	}

	void FreeCamera::SetAngle(Quat const& angle)
	{
		this->angle_ = angle;
	}

	Vec3 FreeCamera::GetPosition() const
	{
		return this->position_;
	}

	Quat FreeCamera::GetAngle() const
	{
		return this->angle_;
	}

	void FreeCamera::CalculateMatrices()
	{
		this->view_ = glm::translate(Mat4(), this->position_);
		this->view_ *= glm::mat4_cast(this->angle_);

		Camera::CalculateMatrices();
	}

} }