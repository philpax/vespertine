#include "vesp/script/Module.hpp"

#include <mruby/string.h>

namespace vesp { namespace script {

	Module::Module(StringView title)
	{
		this->title_ = title.CopyToVector();
		this->state_ = mrb_open();
	}

	Module::~Module()
	{
		mrb_close(this->state_);
	}

	mrb_state* Module::GetState()
	{
		return this->state_;
	}

	String Module::ToString(mrb_value const value) const
	{   
		auto obj = mrb_funcall(this->state_, value, "inspect", 0);
		return StringView(RSTRING_PTR(obj)).CopyToVector();
	}

} }