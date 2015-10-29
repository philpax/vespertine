#pragma once

#include "vesp/Util.hpp"
#include "vesp/Containers.hpp"
#include "vesp/Types.hpp"

#include "vesp/math/Vector.hpp"

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
		Console,
		EndOfEnum
	};

	class InputHandler 
	{
	public:
		typedef void (InputHandler::*Function)(float state);
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

		template <typename T>
		void Subscribe(Action action, InputHandler* instance, void (T::*f)(float))
		{
			this->SubscribeInternal(action, instance, static_cast<InputHandler::Function>(f));
		}

		void AddGuiLock();
		void RemoveGuiLock();
		bool HasGuiLock();
		
	private:
		void SubscribeInternal(Action action, InputHandler* instance, InputHandler::Function handler);

		Array<U16, static_cast<U32>(Action::EndOfEnum)> state_;

		struct Callback
		{
			InputHandler* instance;
			InputHandler::Function handler;
		};
		Array<Vector<Callback>, static_cast<U32>(Action::EndOfEnum)> callbacks_;

		U32 guiLockCount_ = 0;
	};
}