#include "vesp/Console.hpp"
#include "vesp/Containers.hpp"
#include "vesp/Log.hpp"
#include "vesp/Main.hpp"

#include "vesp/graphics/ImGui.hpp"
#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"

#include "vesp/math/Vector.hpp"

namespace vesp {

	Console::Console()
	{
		InputManager::Get()->Subscribe(
			Action::Console, this, &Console::ConsolePress);

		this->AddCommand("parser-test",
			[](ArrayView<String> args)
			{
				LogInfo("parser-test: %d args", args.size);
				for (auto& s : args)
					LogInfo("%.*s", s.size(), s.data()); 
			}
		);

		this->AddCommand("echo",
			[](ArrayView<String> args)
			{
				Console::Get()->WriteOutput(Join(args));
			}
		);

		this->AddMacro("for",
			[](ArrayView<String> args, bool topLevel)
			{
				if (args.size < 3)
				{
					LogError("Expected at least three arguments");
					return;
				}

				auto startIndex = ToS32(args[0]);
				auto endIndex = ToS32(args[1]);

				if (startIndex > endIndex)
				{
					LogError("Start index must be lower than end index");
					return;
				}

				for (auto i = startIndex; i < endIndex; ++i)
				{
					auto indexString = ToString(i);
					auto executionStr = Replace(args[2], "$i", indexString);
					Console::Get()->Execute(executionStr, topLevel);
				}
			}
		);

		this->AddCommand("add",
			[](ArrayView<String> args)
			{
				if (args.size < 2)
				{
					LogError("Expected at least two arguments");
					return;
				}

				float ret = 0.0f;
				for (size_t i = 0; i < args.size; ++i)
				{
					auto value = ToF32(args[i]);

					if (i == 0)
						ret = value;
					else
						ret += value;
				}

				Console::Get()->WriteOutput(ToString(ret));
			}
		);

		this->AddCommand("sub",
			[](ArrayView<String> args)
			{
				if (args.size < 2)
				{
					LogError("Expected at least two arguments");
					return;
				}

				float ret = 0.0f;
				for (size_t i = 0; i < args.size; ++i)
				{
					auto value = ToF32(args[i]);

					if (i == 0)
						ret = value;
					else
						ret -= value;
				}

				Console::Get()->WriteOutput(ToString(ret));
			}
		);

		this->AddCommand("mul",
			[](ArrayView<String> args)
			{
				if (args.size < 2)
				{
					LogError("Expected at least two arguments");
					return;
				}

				float ret = 0.0f;
				for (size_t i = 0; i < args.size; ++i)
				{
					auto value = ToF32(args[i]);

					if (i == 0)
						ret = value;
					else
						ret *= value;
				}

				Console::Get()->WriteOutput(ToString(ret));
			}
		);

		this->AddCommand("div",
			[](ArrayView<String> args)
			{
				if (args.size < 2)
				{
					LogError("Expected at least two arguments");
					return;
				}

				float ret = 0.0f;
				for (size_t i = 0; i < args.size; ++i)
				{
					auto value = ToF32(args[i]);

					if (i == 0)
						ret = value;
					else
						ret /= value;
				}

				Console::Get()->WriteOutput(ToString(ret));
			}
		);

		this->AddCommand("sin",
			[](ArrayView<String> args)
			{
				if (args.size != 1)
				{
					LogError("Expected one argument");
					return;
				}

				Console::Get()->WriteOutput(ToString(sin(ToF32(args[0]))));
			}
		);

		this->AddCommand("cos",
			[](ArrayView<String> args)
			{
				if (args.size != 1)
				{
					LogError("Expected one argument");
					return;
				}

				Console::Get()->WriteOutput(ToString(cos(ToF32(args[0]))));
			}
		);

		this->AddCommand("index",
			[](ArrayView<String> args)
			{
				if (args.size < 2)
				{
					LogError("Expected at least two arguments");
					return;
				}

				auto index = ToS32(args[0]);
				
				if (index < 0 || size_t(index) >= args.size - 1)
				{
					LogError("Invalid index");
					return;
				}

				Console::Get()->WriteOutput(args[index + 1]);
			}
		);

		this->AddCommand("string-replace",
			[](ArrayView<String> args)
			{
				if (args.size < 3)
				{
					LogError("Expected at least two arguments");
					return;
				}

				Console::Get()->WriteOutput(Replace(args[0], args[1], args[2]));
			}
		);

		this->AddEmptyCommand("global-time",
			[]
			{
				Console::Get()->WriteOutput(
					ToString(GetGlobalTimer().GetSeconds()));
			}
		);
	}

	Console::~Console()
	{
	}

	void Console::SetActive(bool active)
	{
		this->active_ = active;
		
		if (this->active_)
			InputManager::Get()->AddGuiLock();
		else
			InputManager::Get()->RemoveGuiLock();
	}

	bool Console::GetActive() const
	{
		return this->active_;
	}

	void Console::AddMessage(StringView text, graphics::Colour colour)
	{
		this->messages_.push_back({text.CopyToVector(), colour});

		if (this->messages_.size() >= 1024)
			this->messages_.pop_front();
	}

	void Console::AddMacro(StringView command, MacroType fn)
	{
		auto cmd = command.CopyToVector();
		VESP_ASSERT(this->macros_.find(cmd) == this->macros_.end());
		VESP_ASSERT(this->commands_.find(cmd) == this->commands_.end());
		this->macros_[cmd] = fn;
	}

	void Console::AddCommand(StringView command, CommandType fn)
	{
		auto cmd = command.CopyToVector();
		VESP_ASSERT(this->macros_.find(cmd) == this->macros_.end());
		VESP_ASSERT(this->commands_.find(cmd) == this->commands_.end());
		this->commands_[cmd] = fn;
	}

	void Console::AddEmptyCommand(StringView command, EmptyCommandType fn)
	{
		this->AddCommand(command, [=](ArrayView<String>) { fn(); });
	}

	void Console::WriteOutput(StringView output)
	{
		this->output_ = output.CopyToVector();
	}

	void Console::Draw()
	{
		if (!this->active_)
			return;

		Array<StringByte, 256> inputBuffer;		

		auto size = graphics::Engine::Get()->GetWindow()->GetSize() / 2;
		ImGui::SetNextWindowSize(
			ImVec2(float(size.x), float(size.y)), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("Console");
		{
			ImGui::BeginChild("ScrollingRegion", ImVec2(0, -ImGui::GetItemsLineHeightWithSpacing()));
			{
				for (auto& text : this->messages_)
				{
					auto c = text.colour;
					ImGui::PushStyleColor(ImGuiCol_Text, ImColor(c.r, c.g, c.b, c.a));
					ImGui::TextUnformatted(text.text.data());
					ImGui::PopStyleColor();
				}
			}
			ImGui::EndChild();
			ImGui::Separator();

			ImGui::PushItemWidth(-1.0f);
			if (ImGui::InputText("Input", inputBuffer.data(),
				inputBuffer.size(), ImGuiInputTextFlags_EnterReturnsTrue))
			{
				StringView view = inputBuffer.data();

				if (view.size)
				{
					this->AddMessage(view, graphics::Colour::CornflowerBlue);
					this->Execute(view, true);
				}
			}
			ImGui::PopItemWidth();
		}
		ImGui::End();
	}

	void Console::ConsolePress(float state)
	{
		if (state == 1.0f)
			this->SetActive(!this->GetActive());
	}

	void Console::Execute(StringView input, bool topLevel)
	{
		Vector<String> tokens;
		String currentToken;

		bool parsingQuote = false;
		size_t semicolonIndex = 0;
		size_t nestingLevel = 0;
		size_t nestingStartIndex = 0;
		size_t quasiquoteLevel = 0;

		auto lexToken = [&](StringByte c)
		{
			if (isspace(c) && !parsingQuote && !quasiquoteLevel)
			{
				if (!currentToken.empty())
				{
					tokens.push_back(currentToken);
					currentToken.clear();
				}
			}
			else if (c == '"' && !quasiquoteLevel)
			{
				parsingQuote = !parsingQuote;
			}
			else
			{
				if (c == ']')
					quasiquoteLevel--;

				// If quasiquote level > 0, emit everything.
				// Otherwise, emit everything that isn't a quasiquote if we're not in a quote
				if (quasiquoteLevel > 0 || parsingQuote || (c != '[' && c != ']'))
					currentToken.push_back(c);

				if (c == '[')
					quasiquoteLevel++;
			}
		};

		for (size_t i = 0; i < input.size; ++i)
		{
			auto c = input[i];

			if (c == '(')
			{
				nestingLevel++;

				if (nestingLevel == 1)
					nestingStartIndex = i;
				continue;
			}
			else if (c == ')')
			{
				if (nestingLevel == 1)
				{
					auto offset = nestingStartIndex + 1;
					auto commandView = StringView(input.data + offset, i - offset);

					this->Execute(commandView, false);
					for (auto c : this->output_)
						lexToken(c);
				}
				else if (nestingLevel == 0)
				{
					LogError("Too many right parenthesis in command");
					return;
				}

				nestingLevel--;				
				continue;
			}

			if (nestingLevel == 0)
			{
				if (c == ';' && !parsingQuote)
				{
					semicolonIndex = i;
					break;
				}
				else
				{
					lexToken(c);
				}
			}
		}

		if (!currentToken.empty())
			tokens.push_back(currentToken);

		if (nestingLevel > 0)
		{
			LogError("Unmatched parenthesis in command");
			return;
		}

		if (quasiquoteLevel > 0)
		{
			LogError("Unmatched quasiquote in command");
			return;
		}

		if (tokens.empty())
			return;

		ArrayView<String> args;
		args.size = tokens.size() - 1;

		if (args.size > 0)
			args.data = &tokens[1];

		this->output_.clear();

		auto commandIt = this->commands_.find(tokens.front());
		auto macroIt = this->macros_.find(tokens.front());
		if (commandIt != this->commands_.end())
			commandIt->second(args);
		else if (macroIt != this->macros_.end())
			macroIt->second(args, topLevel);
		else
			LogError("Unrecognized console command: `%s`", input);

		if (topLevel && this->output_.size())
			LogInfo("%.*s", this->output_.size(), this->output_.data());

		if (semicolonIndex)
		{
			auto offset = semicolonIndex + 1;
			auto view = StringView(input.data + offset, input.size - offset);
			this->Execute(view, topLevel);
		}
	}

}