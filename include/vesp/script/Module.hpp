#pragma once

#include <Windows.h>

#include "vesp/String.hpp"
#include "vesp/script/Sol.hpp"

namespace vesp { namespace script {

	class Module
	{
	public:
		Module(StringView title);
		~Module();

		sol::state& GetState();
		void RunString(StringView code);
		
	private:
		static int PanicHandler(lua_State* state);

		String title_;
		sol::state state_;
	};

} }