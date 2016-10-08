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
		                            sol::lib::io,
									sol::lib::ffi,
									sol::lib::package,
									sol::lib::os);

		this->state_.registry()["Module"] = this;
		this->state_.registry()["ModuleErrorHandler"] = [&](char const* msg)
		{
			LogError("%.*s: %s", this->title_.size(), this->title_.data(), msg);
		};
		this->state_["print"] = [&](sol::variadic_args args)
		{
			vesp::String output;
			bool first = true;
			for (sol::object object : args)
			{
				if (!first)
					output.push_back('\t');

				auto msg = this->ToString(object);
				output.insert(output.end(), msg.begin(), msg.end());

				first = false;
			}
			
			LogInfo("%.*s: %.*s", this->title_.size(), this->title_.data(), output.size(), output.data());
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

	Module::ParseResult Module::ParseString(StringView code)
	{
		auto nameCString = ToCString(this->title_);
		return this->state_.load_buffer(code.data(), code.size(), nameCString.get());
	}

	Module::FnResult Module::RunParseResult(ParseResult& result)
	{
		auto fn = result.get<sol::protected_function>();
		fn.error_handler = this->state_.registry()["ModuleErrorHandler"];

		return fn();
	}

	void Module::RunString(StringView code)
	{
		auto parseResult = this->ParseString(code);

		// If that didn't work, error
		if (!parseResult.valid())
		{
			auto errorStr = parseResult.get<std::string>();
			LogError("Failed to load module %.*s: %s", this->title_.size(), this->title_.data(), errorStr.c_str());
			return;
		}

		auto runResult = this->RunParseResult(parseResult);

		if (runResult.status() != sol::call_status::ok)
		{
			auto errorStr = runResult.get<std::string>();
			LogError("Failed to run module %.*s: %s", this->title_.size(), this->title_.data(), errorStr.c_str());
			return;
		}
	}

	String Module::ToString(Object object)
	{
		std::string result = this->state_["tostring"](object);
		return StringView(result).CopyToVector();
	}

} }