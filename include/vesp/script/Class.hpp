#pragma once

#pragma warning(push)
#pragma warning(disable: 4200)
#include <mruby.h>
#pragma warning(pop)

namespace vesp { namespace script {

	class Module;

	class Class
	{
	public:
		Class(Module* module, RClass* rubyClass);

		void DefineStatic(char const* name, mrb_func_t func, mrb_aspec aspec);

	private:
		Module* module_;
		RClass* class_;
	};

} }