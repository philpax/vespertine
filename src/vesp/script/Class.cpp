#include "vesp/script/Class.hpp"
#include "vesp/script/Module.hpp"

namespace vesp { namespace script {

	Class::Class(Module* module, RClass* rubyClass)
	{
		this->module_ = module;
		this->class_ = rubyClass;
	}

	void Class::DefineStatic(char const* name, mrb_func_t func, mrb_aspec aspec)
	{
		mrb_define_method(this->module_->GetState(), this->class_, name, func, aspec);
	}

} }