#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/graphics/Colour.hpp"

#include "vesp/InputManager.hpp"
#include "vesp/Types.hpp"

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
		typedef std::function<void (ArrayView<String>)> CommandType;
		typedef std::function<void ()> EmptyCommandType;

		Console();
		~Console();

		void SetActive(bool active);
		bool GetActive() const;

		void AddMessage(StringView text, graphics::Colour colour);
		void AddCommand(StringView command, CommandType fn);
		void AddEmptyCommand(StringView command, EmptyCommandType fn);

		void WriteOutput(StringView output);

		void Draw();

	private:
		void ConsolePress(float state);
		void Execute(StringView input, bool topLevel);
		
		struct Message
		{
			String text;
			graphics::Colour colour;
		};

		Deque<Message> messages_;
		UnorderedMap<String, CommandType> commands_;
		String output_;

		bool active_ = false;
	};

}