#include "vesp/script/Module.hpp"

#include "vesp/Log.hpp"

namespace vesp { namespace script {

	Module::Module(StringView title)
	{
		this->title_ = title.CopyToVector();
		
		this->state_.open_libraries(sol::lib::base,
		                            sol::lib::coroutine,
		                            sol::lib::string,
		                            sol::lib::math,
		                            sol::lib::table,
		                            sol::lib::bit32,
		                            sol::lib::io);

		this->state_.registry()["Module"] = this;
		this->state_.registry()["ModuleErrorHandler"] = [&](char const* msg)
		{
			LogError("%.*s: %s", this->title_.size(), this->title_.data(), msg);
		};
		this->state_.set_panic(&Module::PanicHandler);
	}

	Module::~Module()
	{
	}

	sol::state& Module::GetState()
	{
		return this->state_;
	}

	int Module::PanicHandler(lua_State* state)
	{
		auto msg = lua_tostring(state, -1);

		auto view = sol::state_view(state);
		auto this_ = view.registry()["Module"].get<Module*>();

		if (this_ == nullptr)
			return 0;
		
		LogError("%.*s: %s", this_->title_.size(), this_->title_.data(), msg);

		return 0;
	}

	void Module::RunString(StringView code)
	{
		auto nameCString = ToCString(this->title_);
		auto loadResult = this->state_.load_buffer(code.data, code.size, nameCString.get());

		if (!loadResult.valid())
		{
			auto errorStr = loadResult.get<std::string>();
			LogError("Failed to load module %.*s: %s", this->title_.size(), this->title_.data(), errorStr.c_str());
			return;
		}

		auto fn = loadResult.get<sol::protected_function>();
		fn.error_handler = this->state_.registry()["ModuleErrorHandler"];

		fn();
	}

} }