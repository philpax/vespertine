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

		template <typename... Args>
		std::tuple<mrb_value, mrb_value> Call(mrb_value object, char const* functionName, Args... args) const
		{
			static const int argumentCount = sizeof...(args);
			auto ret = mrb_funcall(this->state_, object, functionName, argumentCount, args...);

			auto exc = this->state_->exc;
			this->state_->exc = nullptr;
			if (exc)
				return std::make_tuple(mrb_undef_value(), mrb_obj_value(exc));
			else
				return std::make_tuple(ret, mrb_undef_value());
		}

		template <typename T>
		mrb_value Convert(T const& value)
		{
			static_assert(false, "Type not supported for conversion");
		}

		template <>
		mrb_value Convert<F32>(F32 const& value)
		{
			return mrb_float_value(this->state_, value);
		}

		template <>
		mrb_value Convert<F64>(F64 const& value)
		{
			return mrb_float_value(this->state_, value);
		}
		
		template <>
		mrb_value Convert<S32>(S32 const& value)
		{
			return mrb_fixnum_value(value);
		}

		template <>
		mrb_value Convert<bool>(bool const& value)
		{
			return mrb_bool_value(value);
		}

		template <>
		mrb_value Convert<StringView>(StringView const& value)
		{
			return mrb_str_new(this->state_, value.data, value.size);
		}
		
	private:
		friend Class;

		mrb_state* GetState();

		String title_;
		mrb_state* state_;
	};

} }