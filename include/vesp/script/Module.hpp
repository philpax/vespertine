#pragma once

#include <Windows.h>

#include "vesp/String.hpp"
#include "vesp/script/Sol.hpp"

namespace vesp { namespace script {

	class Module
	{
	public:
		typedef sol::protected_function_result FnResult;
		typedef sol::load_result ParseResult;
		typedef sol::object Object;
		
		Module(StringView title);
		~Module();

		sol::state& GetState();

		ParseResult ParseString(StringView code);
		FnResult RunParseResult(ParseResult& result);
		void RunString(StringView code);

		String ToString(Object object);
				
	private:
		static int PanicHandler(lua_State* state);

		String title_;
		sol::state state_;
	};

} }

// Add overload for vesp::String
namespace sol { namespace stack {
	template <>
	struct pusher<vesp::String>
	{
		static int push(lua_State* L, vesp::String const& s)
		{
			lua_pushlstring(L, s.data(), s.size());
			return 1;
		}
	};

	template <>
	struct getter<vesp::String>
	{
		static vesp::String get(lua_State* L, int index)
		{
			auto str = stack::get<vesp::RawStringPtr>(L, index);
			return vesp::StringView(str).CopyToVector();
		}
	};
}

template <>
struct lua_type_of<::vesp::String> : std::integral_constant<type, type::string> {};
}