#include "vesp/Profiler.hpp"
#include "vesp/Console.hpp"
#include "vesp/Log.hpp"
#include "vesp/EventManager.hpp"

#include "vesp/graphics/imgui.h"

namespace vesp
{
	Profiler::Profiler()
	{
		Console::Get()->AddCommand("profiler.nextFrame", [&] {
			this->printNextFrame_ = true;
		});

		Console::Get()->AddCommand("profiler.window", [&] {
			this->drawGui_ = true;
		});

		EventManager::Get()->Subscribe("Render.Gui", [&] (void const*) {
			this->Draw();
			return true;
		});
	}

	void Profiler::BeginSection(RawStringPtr title)
	{
		auto section = std::make_unique<Section>();
		auto sectionPtr = section.get();
		section->title = title;
		section->parent = this->currentSection_;
		section->parent->children.push_back(std::move(section));
		this->currentSection_ = sectionPtr;
	}

	void Profiler::EndSection()
	{
		this->currentSection_->duration = this->currentSection_->timer.GetSeconds();
		this->currentSection_ = this->currentSection_->parent;
	}

	void Profiler::BeginFrame()
	{
		if (!this->frozen_)
			this->root_.swap(this->savedRoot_);

		this->root_.reset(new Section());
		this->root_->title = "Frame";
		this->root_->parent = nullptr;
		this->currentSection_ = this->root_.get();
	}

	void Profiler::EndFrame()
	{
		this->EndSection();

		if (this->printNextFrame_)
		{
			this->PrintSection(this->root_.get(), 0);
			this->printNextFrame_ = false;
		}
	}

	void Profiler::Draw()
	{
		if (!this->drawGui_)
			return;

		VESP_PROFILE_FN();

		ImGui::Begin("Profiler", &this->drawGui_, ImGuiWindowFlags_MenuBar);
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::MenuItem(this->frozen_ ? "Unfreeze" : "Freeze", nullptr, &this->frozen_);
				ImGui::EndMenuBar();
			}
			this->DrawSection(this->savedRoot_.get());
		}
		ImGui::End();
	}
	
	void Profiler::DrawSection(Section* section)
	{
		auto makeTreeNode = [](Section* section, float duration, RawStringPtr title = nullptr, RawStringPtr id = nullptr) {
			auto parentDuration = section->parent ? section->parent->duration : section->duration;
			auto fraction = duration / parentDuration;  
			auto percentage = fraction * 100.0f;
			auto isLeafNode = section->children.size() == 0 || id != nullptr;

			if (!title)
				title = section->title;

			if (!id)
				id = title;

			ImVec4 col;
			if (fraction < 0.25f)
				col = ImVec4(fraction / 0.25f, 1.0f, 0.0f, 1.0f);
			else if (fraction > 0.25f && fraction < 0.75f)
				col = ImVec4(1.0f, 1.0f, 0.0f, 1.0f);
			else
				col = ImVec4(1.0f, (1.0f - fraction) / 0.25f, 0.0f, 1.0f);

			ImGui::PushStyleColor(ImGuiCol_Text, col);
			auto ret = ImGui::TreeNodeEx(
				id, isLeafNode ? ImGuiTreeNodeFlags_Leaf : 0,
				"%s (%f ms, %.01f%%)", title, duration * 1000.0f, percentage);
			ImGui::PopStyleColor();

			return ret;
		};

		if (makeTreeNode(section, section->duration))
		{
			auto unaccountedFor = section->duration;
			for (auto& child : section->children)
			{
				this->DrawSection(child.get());
				unaccountedFor -= child->duration;
			}

			auto treeId = Concat(section->title, " unaccounted");
			treeId.push_back('\0');
			
			if (unaccountedFor > 0.0f && section->children.size())
				if (makeTreeNode(section, unaccountedFor, "Unaccounted", treeId.data()))
					ImGui::TreePop();

			ImGui::TreePop();
		}
	}

	void Profiler::PrintSection(Section* section, size_t level)
	{
		auto printLine = 
		[](size_t level, RawStringPtr title, F32 duration, F32 parentDuration, LogType type = LogType::Info)
		{
			String line;
			for (size_t i = 0; i < level * 2; i++)
				line.push_back(' ');

			line.insert(
				line.end(),
				title,
				title + strlen(title));

			F32 percentage = duration / parentDuration * 100.0f;

			Log(type, "%.*s (%f ms, %.01f%%)", line.size(), line.data(), 
				duration * 1000.0f, percentage);
		};

		printLine(
			level, 
			section->title, 
			section->duration, 
			section->parent ? section->parent->duration : section->duration);
		
		auto accountedFor = 0.0f;
		for (auto& child : section->children)
		{
			this->PrintSection(child.get(), level + 1);
			accountedFor += child->duration;
		}

		auto unaccountedFor = section->duration - accountedFor;
		if (unaccountedFor > 0.0f && section->children.size())
			printLine(level + 1, "Unaccounted", unaccountedFor, section->duration, LogType::Warn);
	}
}