#include "vesp/Console.hpp"
#include "vesp/Containers.hpp"
#include "vesp/Log.hpp"

#include "vesp/graphics/imgui.h"
#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Window.hpp"

#include "vesp/math/Vector.hpp"
#include <vesp/EventManager.hpp>

namespace vesp {

	Console::Console()
	{
		this->module_ = std::make_unique<script::Module>("console");

		InputManager::Get()->Subscribe(
			Action::Console, this, &Console::ConsolePress);

		EventManager::Get()->Subscribe(
			"Render.Gui", [&](const void*) { this->Draw(); return true; });

		// TODO: Autoexec
	}

	Console::~Console()
	{
	}

	void Console::PostInitialisation()
	{
		EventManager::Get()->Fire("Console.ReadyForBinding");
	}

	void Console::SetActive(bool active)
	{
		if (!this->active_ && active)
			this->inputNeedsFocus_ = true;

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

				this->inputNeedsFocus_ = true;
			}
			
			if (this->inputNeedsFocus_)
				ImGui::SetKeyboardFocusHere();
		
			ImGui::PopItemWidth();
		}
		ImGui::End();

		this->inputNeedsFocus_ = false;
	}

	script::Module* Console::GetModule()
	{
		return this->module_.get();
	}

	void Console::Execute(StringView code)
	{
		// Try loading with return prefixed
		auto codeWithReturn = Concat("return ", code);
		auto parseResult = this->GetModule()->ParseString(codeWithReturn);

		// If that doesn't work, try loading without return
		if (!parseResult.valid())
			parseResult = this->GetModule()->ParseString(code);

		// If it still doesn't work, log and error
		if (!parseResult.valid())
		{
			auto errorStr = parseResult.get<std::string>();
			LogError("Failed to parse console input: %s", errorStr.c_str());
			return;
		}

		auto runResult = this->GetModule()->RunParseResult(parseResult);
		auto runResultObj = sol::object(runResult);

		if (runResult.valid())
		{
			// If it's a function, run it and use its result
			if (runResultObj.is<sol::protected_function>())
				runResultObj = runResult.get<sol::protected_function>()();

			auto resultStr = this->GetModule()->ToString(runResultObj);
			LogInfo("%.*s", resultStr.size(), resultStr.data());
		}
	}

	void Console::ConsolePress(float state)
	{
		if (state == 1.0f)
			this->SetActive(!this->GetActive());
	}
}
