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