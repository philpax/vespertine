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
	// vesp::String
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
		static vesp::String get(lua_State* L, int index, record& tracking)
		{
			auto str = stack::get<vesp::RawStringPtr>(L, index);
			tracking.use(1);
			return vesp::StringView(str).CopyToVector();
		}
	};

	template <>
	struct checker<vesp::String>
	{
		template <typename Handler>
		static bool check(lua_State* L, int index, Handler&& handler, record& tracking)
		{
			int absIndex = lua_absindex(L, index);
			auto success = stack::check<bool>(L, absIndex, handler);
			tracking.use(1);
			return success;
		}
	};

	// vesp::StringView
	template <>
	struct pusher<vesp::StringView>
	{
		static int push(lua_State* L, vesp::StringView const& s)
		{
			lua_pushlstring(L, s.data, s.size);
			return 1;
		}
	};
} }