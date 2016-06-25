#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/graphics/Colour.hpp"

#include "vesp/script/Module.hpp"

#include "vesp/InputManager.hpp"
#include "vesp/Types.hpp"
#include "vesp/String.hpp"

#include <functional>

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

		void AddMessage(StringView text, graphics::Colour colour = graphics::Colour::White);
		void Execute(StringView code);

		void Draw();

	private:
		void ConsolePress(float state);

	
		struct Message
		{
			String text;
			graphics::Colour colour;
		};

		Deque<Message> messages_;
		String output_;

		UniquePtr<script::Module> module_;		

		bool active_ = false;
		bool scrollToBottom_ = false;
	};

}