#include "vesp/Console.hpp"
#include "vesp/Containers.hpp"
#include "vesp/Log.hpp"
#include "vesp/Main.hpp"

#include "vesp/graphics/imgui.h"
#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"

#include "vesp/math/Vector.hpp"

#include <mruby/compile.h>

namespace vesp {

	Console::Console()
	{
		InputManager::Get()->Subscribe(
			Action::Console, this, &Console::ConsolePress);

		this->module_ = std::make_unique<script::Module>("console");
		auto state = this->module_->GetState();
		mrb_define_method(state, state->kernel_module, 
			"quit", &Console::CommandQuit, MRB_ARGS_NONE());
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

		this->scrollToBottom_ = true;
	}

	void Console::AddMacro(StringView command, MacroType fn)
	{
	}

	void Console::AddCommand(StringView command, CommandType fn)
	{
	}

	void Console::AddEmptyCommand(StringView command, EmptyCommandType fn)
	{
		this->AddCommand(command, [=](ArrayView<String>) { fn(); });
	}

	void Console::WriteOutput(StringView output)
	{
	}

	void Console::Draw()
	{
		if (!this->active_)
			return;

		Array<StringByte, 256> inputBuffer;	
		inputBuffer.assign(0);

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
					ImGui::TextUnformatted(text.text.data(), text.text.data() + text.text.size());
					ImGui::PopStyleColor();
				}

				if (this->scrollToBottom_)
				{
					ImGui::SetScrollHere();
					this->scrollToBottom_ = false;
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
					this->Execute(view);
				}
			}
			ImGui::SetKeyboardFocusHere();
			ImGui::PopItemWidth();
		}
		ImGui::End();
	}

	void Console::Execute(StringView code)
	{
		auto state = this->module_->GetState();
		auto obj = mrb_load_nstring(state, code.data, code.size);
		if (state->exc)
			this->AddMessage(this->module_->ToString(mrb_obj_value(state->exc)));
		else
			this->AddMessage(this->module_->ToString(obj));

		state->exc = nullptr;
	}

	void Console::ConsolePress(float state)
	{
		if (state == 1.0f)
			this->SetActive(!this->GetActive());
	}

	mrb_value Console::CommandQuit(mrb_state* mrb, mrb_value self)
	{
		Quit();
		return self;
	}
}