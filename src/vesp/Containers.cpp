#include "vesp/Containers.hpp"

namespace vesp
{
	ArrayView<StringByte> ArrayViewBase<StringByte>::From(StringByte const* str)
	{
		// This const_cast is *very* bad practice, but we trust our users
		// to not do bad things with the resulting ArrayView
		return ArrayView<StringByte>(
			const_cast<StringByte*>(str), strlen(str));
	}
}