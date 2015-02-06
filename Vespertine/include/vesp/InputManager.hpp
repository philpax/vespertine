#pragma once

#include "vesp/Util.hpp"
#include "vesp/Containers.hpp"

union SDL_Event;

namespace vesp
{
	enum class Action : U8
	{
		Forward,
		Backward,
		Left,
		Right,
		CameraUp,
		CameraDown,
		CameraLeft,
		CameraRight,
		EndOfEnum
	};

	class InputManager : public util::GlobalSystem<InputManager>
	{
	public:
		InputManager();
		~InputManager();

		void FeedEvent(SDL_Event const* event);
		
		F32 GetState(Action action);
		void SetState(Action action, F32 state);

		void Pulse();

	private:
		Array<U16, static_cast<U32>(Action::EndOfEnum)> state_;
	};
}