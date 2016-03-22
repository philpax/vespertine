#include "vesp/script/Module.hpp"

#include <mruby/string.h>
#include <mruby/compile.h>

namespace vesp { namespace script {

	Module::Module(StringView title)
	{
		this->title_ = title.CopyToVector();
		this->state_ = mrb_open();

		this->kernel = std::make_unique<Class>(this, this->state_->kernel_module);
	}

	Module::~Module()
	{
		mrb_close(this->state_);
	}

	String Module::ToString(mrb_value const value) const
	{   
		auto obj = mrb_funcall(this->state_, value, "inspect", 0);
		return StringView(RSTRING_PTR(obj)).CopyToVector();
	}

	std::tuple<mrb_value, mrb_value> Module::Execute(StringView code)
	{
		auto obj = mrb_load_nstring(this->state_, code.data, code.size);

		auto exception = this->state_->exc;
		this->state_->exc = nullptr;

		if (exception)
			return std::make_tuple(obj, mrb_obj_value(exception));
		else
			return std::make_tuple(obj, mrb_undef_value());
	}

	mrb_state* Module::GetState()
	{
		return this->state_;
	}

} }