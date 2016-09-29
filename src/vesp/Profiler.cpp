#include "vesp/Profiler.hpp"
#include "vesp/Console.hpp"
#include "vesp/Log.hpp"
#include "vesp/EventManager.hpp"

#include "vesp/graphics/imgui.h"

namespace vesp
{
	Profiler::Profiler()
	{
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

			float r, g, b;
			ImGui::ColorConvertHSVtoRGB((1.0f - fraction) * 0.3f, 0.9f, 0.9f, r, g, b);

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(r, g, b, 1.0f));
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
}