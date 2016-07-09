#include "vesp/InputManager.hpp"
#include "vesp/EventManager.hpp"
#include "vesp/Console.hpp"

#include "vesp/math/Util.hpp"

#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"
#include "vesp/graphics/imgui.h"

#include "vesp/Log.hpp"
#include "vesp/Assert.hpp"

#include <Windowsx.h>

namespace vesp
{
	InputManager::InputManager()
	{
		this->state_.assign(0);

		EventManager::Get()->Subscribe("Console.ReadyForBinding", [&](const void*)
		{
			this->BindConsole();
			return true;
		});

		EventManager::Get()->Subscribe(
			"Render.Gui", [&](const void*) { this->Draw(); return true; });
	}

	InputManager::~InputManager()
	{
	}

	void InputManager::FeedEvent(MSG const* event)
	{
		if (!graphics::Engine::Get()->GetWindow()->HasFocus())
			return;

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
			case VK_F1:
				this->SetState(Action::Console, 1.0f);
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
			case VK_F1:
				this->SetState(Action::Console, 0.0f);
				break;
			}
		}
		else if (event->message == WM_MOUSEMOVE)
		{
			S16 currentCursorX = GET_X_LPARAM(event->lParam);
			S16 currentCursorY = GET_Y_LPARAM(event->lParam);
		}
	}

	F32 InputManager::GetState(Action action)
	{
		return	this->state_[static_cast<U32>(action)] / 
				static_cast<float>(std::numeric_limits<U16>::max());
	}

	void InputManager::SetState(Action action, F32 state)
	{
		Array<Action, 1> guiActions = { Action::Console };

		// If this is not a GUI action, and we have input lock, drop it
		if (this->HasGuiLock() && 
			std::find(guiActions.begin(), guiActions.end(), action) == guiActions.end())
			return;

		auto oldValue = this->state_[static_cast<U32>(action)];
		auto newValue = static_cast<U16>(state * std::numeric_limits<U16>::max());

		this->state_[static_cast<U32>(action)] = newValue;

		if (oldValue != newValue)
		{
			for (auto& v : this->callbacks_[static_cast<U32>(action)])
			{
				auto handler = v.handler;
				(v.instance->*handler)(state);
			}
		}
	}

	void InputManager::Pulse()
	{
		// Reset inputs to 0
		this->SetState(Action::CameraRight, 0.0f);
		this->SetState(Action::CameraLeft, 0.0f);

		this->SetState(Action::CameraUp, 0.0f);
		this->SetState(Action::CameraDown, 0.0f);

		if (!graphics::Engine::Get()->GetWindow()->HasFocus())
			return;

		// Get the centre point of the window; if we're already there, no need to set inputs
		auto centre = graphics::Engine::Get()->GetWindow()->GetCentre();
		POINT currentCursorPoint;
		GetCursorPos(&currentCursorPoint);

		auto currentCursor = IVec2(currentCursorPoint.x, currentCursorPoint.y);

		if (currentCursor != centre && !this->HasGuiLock())
		{
			// Calculate the new mouse inputs based on deltas
			auto frameTime = this->lastFrameTimer_.GetSeconds();
			Vec2 deltaCursor = (currentCursor - centre);
			deltaCursor *= frameTime;
			deltaCursor *= 4.0f;

			auto x = math::Clamp(deltaCursor.x, -1.0f, 1.0f);
			auto y = -math::Clamp(deltaCursor.y, -1.0f, 1.0f);

			if (x > 0)
				this->SetState(Action::CameraRight, x);
			else
				this->SetState(Action::CameraLeft, -x);

			if (y > 0)
				this->SetState(Action::CameraUp, y);
			else
				this->SetState(Action::CameraDown, -y);

			// Reset cursor to centre of window
			SetCursorPos(centre.x, centre.y);
		}

		this->lastFrameTimer_.Restart();
	}

	void InputManager::AddGuiLock()
	{
		this->guiLockCount_++;
	}

	void InputManager::RemoveGuiLock()
	{
		VESP_ASSERT(this->guiLockCount_ > 0);
		this->guiLockCount_--;
		
		// If we're exiting GUI lock, we want to reset the cursor position to 0
		// so that we don't jerk the camera on the next tick (as the cursor's
		// position will not be at the centre as a result of GUI use)
		if (this->guiLockCount_ == 0)
			this->ResetCursorToCentre();
	}

	void InputManager::BindConsole()
	{
		auto& state = Console::Get()->GetModule()->GetState();
		state["input"] = state.create_table();
		state["input"]["dump"] = [&]()
		{
			for (U32 actionIndex = 0u; actionIndex < U32(Action::EndOfEnum); ++actionIndex)
				LogInfo("%s: %f", ActionNames[actionIndex], this->GetState(Action(actionIndex)));
		};
		state["input"]["window"] = [&]()
		{
			this->windowActive_ = !this->windowActive_;
		};
	}

	void InputManager::ResetCursorToCentre()
	{
		auto centre = graphics::Engine::Get()->GetWindow()->GetCentre();
		// Reset cursor to centre of window
		SetCursorPos(centre.x, centre.y);
	}

	bool InputManager::HasGuiLock()
	{
		return this->guiLockCount_ > 0;
	}

	void InputManager::SubscribeInternal(Action action, InputHandler* instance, InputHandler::Function handler)
	{
		this->callbacks_[static_cast<U32>(action)].push_back({instance, handler});
	}

	void InputManager::Draw()
	{
		if (!this->windowActive_)
			return;

		ImGui::Begin("Input", &this->windowActive_);
		{
			for (U32 actionIndex = 0u; actionIndex < U32(Action::EndOfEnum); ++actionIndex)
				ImGui::ProgressBar(this->GetState(Action(actionIndex)), ImVec2(-1, 0), ActionNames[actionIndex]);
		}
		ImGui::End();
	}
}
