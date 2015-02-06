#include "vesp/InputManager.hpp"
#include "vesp/Math/Util.hpp"
#include "vesp/Log.hpp"

#include <SDL.h>

namespace vesp
{
	InputManager::InputManager()
	{
		this->state_.assign(0);
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}

	InputManager::~InputManager()
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	void InputManager::FeedEvent(SDL_Event const* event)
	{
		if (event->type == SDL_KEYDOWN)
		{
			// TODO: Settings manager
			switch (event->key.keysym.sym)
			{
			case 'w':
				this->SetState(Action::Forward, 1.0f);
				break;
			case 's':
				this->SetState(Action::Backward, 1.0f);
				break;
			case 'a':
				this->SetState(Action::Left, 1.0f);
				break;
			case 'd':
				this->SetState(Action::Right, 1.0f);
				break;
			}
		}
		else if (event->type == SDL_KEYUP)
		{
			// TODO: Settings manager
			switch (event->key.keysym.sym)
			{
			case 'w':
				this->SetState(Action::Forward, 0.0f);
				break;
			case 's':
				this->SetState(Action::Backward, 0.0f);
				break;
			case 'a':
				this->SetState(Action::Left, 0.0f);
				break;
			case 'd':
				this->SetState(Action::Right, 0.0f);
				break;
			}
		}
		else if (event->type == SDL_MOUSEMOTION)
		{
			auto x = math::Clamp(event->motion.xrel / 16.0f, -1.0f, 1.0f);
			auto y = -math::Clamp(event->motion.yrel / 16.0f, -1.0f, 1.0f);
			
			if (x > 0)
				this->SetState(Action::CameraRight, x);
			else
				this->SetState(Action::CameraLeft, -x);

			if (y > 0)
				this->SetState(Action::CameraUp, y);
			else
				this->SetState(Action::CameraDown, -y);
		}
	}

	F32 InputManager::GetState(Action action)
	{
		return	this->state_[static_cast<U32>(action)] / 
				static_cast<float>(std::numeric_limits<U16>::max());
	}

	void InputManager::SetState(Action action, F32 state)
	{
		this->state_[static_cast<U32>(action)] =
			static_cast<U16>(state * std::numeric_limits<U16>::max());
	}

	void InputManager::Pulse()
	{
		this->SetState(Action::CameraLeft, 0.0f);
		this->SetState(Action::CameraRight, 0.0f);
		this->SetState(Action::CameraUp, 0.0f);
		this->SetState(Action::CameraDown, 0.0f);
	}
}