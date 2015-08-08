#pragma once

#include "vesp/Containers.hpp"
#include "vesp/util/MurmurHash.hpp"

namespace vesp 
{
	using String = Vector<StringByte>;
	using WideString = Vector<wchar_t>;

	class StringView : public ArrayView<StringByte>
	{
	public:
		StringView(RawStringPtr ptr)
			: ArrayView(const_cast<StringByte*>(ptr), strlen(ptr))
		{
		}

		using ArrayView::ArrayView;
	};

	bool operator==(StringView const lhs, StringView const rhs);
	
	String Concat(StringView lhs, StringView rhs);
	String Join(ArrayView<String> strings, StringByte sep = ' ');
	String Replace(StringView str, StringView toReplace, StringView replacement);

	String ToString(size_t value);
	String ToString(S32 value);
	String ToString(F32 value);

	S32 ToS32(StringView string);
	F64 ToF64(StringView string);
	F32 ToF32(StringView string);

	std::unique_ptr<StringByte[]> ToCString(StringView string);
}

namespace std 
{
	template <> struct hash<vesp::String>
	{
		size_t operator()(vesp::String const& s) const
		{
			return vesp::util::MurmurHash(
				reinterpret_cast<void const*>(s.data()), s.size());
		}
	};
}