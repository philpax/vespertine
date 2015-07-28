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

	void Console::AddMessage(ArrayView<StringByte> text, graphics::Colour colour)
	{
		this->messages_.push_back({String(text.data, text.data + text.size), colour});

		if (this->messages_.size() >= 1024)
			this->messages_.pop_front();
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
				this->ProcessInput(inputBuffer);
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

	void Console::ProcessInput(ArrayView<StringByte> input)
	{
		if (strcmp(input.data, "quit") == 0)
			vesp::Quit();
		else
			LogError("Unrecognized console command: `%s`", input);
	}

}