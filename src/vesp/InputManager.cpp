#include "vesp/InputManager.hpp"
#include "vesp/Math/Util.hpp"
#include "vesp/Log.hpp"

#include <Windowsx.h>

namespace vesp
{
	InputManager::InputManager()
	{
		this->state_.assign(0);
	}

	InputManager::~InputManager()
	{
	}

	void InputManager::FeedEvent(MSG const* event)
	{
		if (event->message == WM_KEYDOWN)
		{
			// TODO: Settings manager
			switch (static_cast<StringByte>(event->wParam))
			{
			case 'W':
				this->SetState(Action::Forward, 1.0f);
				break;
			case 'S':
				this->SetState(Action::Backward, 1.0f);
				break;
			case 'A':
				this->SetState(Action::Left, 1.0f);
				break;
			case 'D':
				this->SetState(Action::Right, 1.0f);
				break;
			case VK_SHIFT:
				this->SetState(Action::Boost, 1.0f);
				break;
			}
		}
		else if (event->message == WM_KEYUP)
		{
			// TODO: Settings manager
			switch (static_cast<StringByte>(event->wParam))
			{
			case 'W':
				this->SetState(Action::Forward, 0.0f);
				break;
			case 'S':
				this->SetState(Action::Backward, 0.0f);
				break;
			case 'A':
				this->SetState(Action::Left, 0.0f);
				break;
			case 'D':
				this->SetState(Action::Right, 0.0f);
				break;
			case VK_SHIFT:
				this->SetState(Action::Boost, 0.0f);
				break;
			}
		}
		else if (event->message == WM_MOUSEMOVE)
		{
			S16 currentCursorX = GET_X_LPARAM(event->lParam);
			S16 currentCursorY = GET_Y_LPARAM(event->lParam);

			S16 deltaCursorX = currentCursorX - lastCursorX_;
			S16 deltaCursorY = currentCursorY - lastCursorY_;

			lastCursorX_ = currentCursorX;
			lastCursorY_ = currentCursorY;

			auto x = math::Clamp(deltaCursorX / 16.0f, -1.0f, 1.0f);
			auto y = -math::Clamp(deltaCursorY / 16.0f, -1.0f, 1.0f);
			
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