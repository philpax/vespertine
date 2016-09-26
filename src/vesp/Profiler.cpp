#include "vesp/Profiler.hpp"
#include "vesp/Console.hpp"
#include "vesp/Log.hpp"

namespace vesp
{
	Profiler::Profiler()
	{
		Console::Get()->AddCommand("profiler.nextFrame", [&] {
			this->printNextFrame_ = true;
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