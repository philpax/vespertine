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
		// std::function is used here because we don't particularly care about the
		// performance impact of the indirection. This is not the case in other
		// parts of the codebase, such as InputManager, in which we have callbacks
		// being called potentially many times a frame.
		typedef std::function<void (ArrayView<String>, bool)> MacroType;
		typedef std::function<void (ArrayView<String>)> CommandType;
		typedef std::function<void ()> EmptyCommandType;

		Console();
		~Console();

		void SetActive(bool active);
		bool GetActive() const;

		void AddMessage(StringView text, graphics::Colour colour = graphics::Colour::White);
		void AddMacro(StringView command, MacroType fn);
		void AddCommand(StringView command, CommandType fn);
		void AddEmptyCommand(StringView command, EmptyCommandType fn);

		void WriteOutput(StringView output);
		void Execute(StringView code);

		void Draw();

	private:
		void ConsolePress(float state);

		static mrb_value CommandQuit(mrb_state* mrb, mrb_value self);
		
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