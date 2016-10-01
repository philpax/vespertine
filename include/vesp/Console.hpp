#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/graphics/Colour.hpp"
#include "vesp/graphics/imgui.h"

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
		static const size_t MaxHistoryLength = 100;

		Console();
		~Console();

		void PostInitialisation();

		void SetActive(bool active);
		bool GetActive() const;

		void AddMessage(StringView text, graphics::Colour colour = graphics::Colour::White);
		void Execute(StringView code);

		template <typename Fn>
		void AddCommand(StringView command, Fn&& fn)
		{
			auto& state = this->module_->GetState();
			auto segments = Split(command, '.');

			size_t index = 0;
			sol::table table = state.globals();

			for (auto segment : segments)
			{
				auto segmentCStrPtr = ToCString(segment);
				auto segmentCStr = segmentCStrPtr.get();

				if (index != segments.size() - 1)
					table = table[segmentCStr] ? table[segmentCStr] : table.create_named(segmentCStr);
				else
					table[segmentCStr] = fn;

				++index;
			}
		}

	private:
		void Draw();
		void ConsolePress(float state);
		int TextboxCallback(ImGuiTextEditCallbackData* data);
	
		struct Message
		{
			String text;
			graphics::Colour colour;
		};

		Deque<Message> messages_;

		Deque<String> history_;
		S32 historyIndex_;

		UniquePtr<script::Module> module_;

		bool active_ = false;
		bool inputNeedsFocus_ = false;
		bool scrollToBottom_ = false;
	};

}