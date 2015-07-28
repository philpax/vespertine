#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/graphics/Colour.hpp"

#include "vesp/InputManager.hpp"
#include "vesp/Types.hpp"

namespace vesp {

	class Console : 
		public InputHandler,
		public util::GlobalSystem<Console>
	{
	public:
		Console();
		~Console();

		void SetActive(bool active);
		bool GetActive() const;

		void AddMessage(ArrayView<StringByte> text, graphics::Colour colour);

		void Draw();

	private:
		void ConsolePress(float state);
		void ProcessInput(ArrayView<StringByte> input);
		
		struct Message
		{
			String text;
			graphics::Colour colour;
		};

		Deque<Message> messages_;

		bool active_ = false;
	};

}