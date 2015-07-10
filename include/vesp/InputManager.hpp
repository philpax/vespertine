#pragma once

#include "vesp/Util.hpp"
#include "vesp/Containers.hpp"
#include "vesp/Types.hpp"

struct tagMSG;

namespace vesp
{
	enum class Action : U8
	{
		Forward,
		Backward,
		Left,
		Right,
		Boost,
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

		void FeedEvent(MSG const* event);
		
		F32 GetState(Action action);
		void SetState(Action action, F32 state);

		void Pulse();

	private:
		Array<U16, static_cast<U32>(Action::EndOfEnum)> state_;

		S16 lastCursorX_ = 0;
		S16 lastCursorY_ = 0;
	};
}