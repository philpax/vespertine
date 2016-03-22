#pragma once

#include "vesp/String.hpp"
#include "vesp/script/Class.hpp"

namespace vesp { namespace script {

	class Module
	{
	public:
		Module(StringView title);
		~Module();

		UniquePtr<Class> kernel;

		String ToString(mrb_value const value) const;
		std::tuple<mrb_value, mrb_value> Execute(StringView code);

	private:
		friend Class;

		mrb_state* GetState();

		String title_;
		mrb_state* state_;
	};

} }