#pragma warning(disable: 4005)
#include "vesp/graphics/FreeCamera.hpp"

#include "vesp/Math/Util.hpp"

#include "vesp/InputManager.hpp"

namespace vesp { namespace graphics {

	FreeCamera::FreeCamera(Vec3 const& position, Quat const& angle)
		: pitch_(0.0f), yaw_(0.0f)
	{
		this->SetPosition(position);
		this->SetAngle(angle);
	}

	FreeCamera::~FreeCamera()
	{
	}

	void FreeCamera::Update()
	{
		auto inputManager	= InputManager::Get();

		// Get all relevant input states
		auto forwardState	= inputManager->GetState(Action::Forward);
		auto backwardState	= inputManager->GetState(Action::Backward);
		auto leftState		= inputManager->GetState(Action::Left);
		auto rightState		= inputManager->GetState(Action::Right);

		auto camUpState		= inputManager->GetState(Action::CameraUp);
		auto camDownState	= inputManager->GetState(Action::CameraDown);
		auto camLeftState	= inputManager->GetState(Action::CameraLeft);
		auto camRightState	= inputManager->GetState(Action::CameraRight);

		auto boostMult		= inputManager->GetState(Action::Boost) * 4.0f + 1.0f;

		auto timeDelta		= this->frameDeltaTimer_.GetSeconds();

		// Calculate absolute translational delta
		auto speed			= 1.0f * boostMult * timeDelta; // ms^-1
		auto forwardDelta	= (forwardState - backwardState) * speed;
		auto sideDelta		= (rightState - leftState) * speed;

		// Calculate absolute rotational delta
		auto camUpDelta		= (camUpState - camDownState) * 0.25f;
		auto camSideDelta	= (camLeftState - camRightState) * 0.25f;

		// Adjust pitch/yaw of camera and clamp
		this->pitch_ += camUpDelta;
		this->yaw_ += camSideDelta;

		auto pi = glm::pi<float>();
		this->pitch_ = math::Clamp(this->pitch_, -pi / 2.0f, pi / 2.0f);
		this->yaw_ = fmod(this->yaw_, 2 * pi);

		// Calculate final quaternion angle
		Quat angle;
		angle *= Quat(Vec3(this->pitch_, 0.0f, 0.0f));
		angle *= Quat(Vec3(0.0f, this->yaw_, 0.0f));
		this->angle_ = angle;

		// Move camera by translational delta
		this->position_ += Vec3(sideDelta, 0.0f, forwardDelta) * angle;

		// Update internal matrices
		this->CalculateMatrices();
		Camera::Update();

		this->frameDeltaTimer_.Restart();
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
		this->view_ = Mat4();
		// Negate values as view matrix is *inverse* of camera world matrix
		this->view_ *= glm::mat4_cast(-this->angle_);
		this->view_ = glm::translate(this->view_, -this->position_);
	}

} }