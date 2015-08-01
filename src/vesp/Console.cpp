#include "vesp/Console.hpp"
#include "vesp/Containers.hpp"
#include "vesp/Log.hpp"

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

	void Console::AddCommand(StringView command, CommandType fn)
	{
		auto cmd = command.CopyToVector();
		VESP_ASSERT(this->commands_.find(cmd) == this->commands_.end());
		this->commands_[cmd] = fn;
	}

	void Console::AddEmptyCommand(StringView command, EmptyCommandType fn)
	{
		this->AddCommand(command, [=](ArrayView<String>) { fn(); });
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
				this->AddMessage(view, graphics::Colour::CornflowerBlue);
				this->ProcessInput(view);
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

	void Console::ProcessInput(StringView input)
	{
		Vector<String> tokens;
		String currentToken;

		bool parsingQuote = false;
		size_t semicolonIndex = 0;
		for (size_t i = 0; i < input.size; ++i)
		{
			auto c = input[i];

			if (isspace(c) && !parsingQuote)
			{
				if (!currentToken.empty())
				{
					tokens.push_back(currentToken);
					currentToken.clear();
				}
			}
			else if (c == '"')
			{
				parsingQuote = !parsingQuote;
			}
			else if (c == ';' && !parsingQuote)
			{
				semicolonIndex = i;
				break;
			}
			else
			{
				currentToken.push_back(c);
			}
		}

		if (!currentToken.empty())
			tokens.push_back(currentToken);

		if (tokens.empty())
			return;

		auto it = this->commands_.find(tokens.front());

		ArrayView<String> args;
		args.size = tokens.size() - 1;

		if (args.size > 0)
			args.data = &tokens[1];

		if (it != this->commands_.end())
			it->second(args);
		else
			LogError("Unrecognized console command: `%s`", input);

		if (semicolonIndex)
		{
			auto offset = semicolonIndex + 1;
			auto view = StringView(input.data + offset, input.size - offset);
			this->ProcessInput(view);
		}
	}

}