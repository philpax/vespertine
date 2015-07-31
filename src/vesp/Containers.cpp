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

	String MakeString(RawStringPtr ptr)
	{
		String ret;
		ret.insert(ret.begin(), ptr, ptr + strlen(ptr));
		return ret;
	}
	
	String Concat(String const& string, StringView rhs)
	{
		String ret = string;
		ret.insert(ret.end(), rhs.begin(), rhs.end());
		return ret;
	}

	std::unique_ptr<StringByte[]> ToCString(StringView string)
	{
		std::unique_ptr<StringByte[]> ret(new StringByte[string.size + 1]);
		memcpy(ret.get(), string.data, string.size);
		ret[string.size] = '\0';
		return ret;
	}
}