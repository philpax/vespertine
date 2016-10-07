#include "vesp/String.hpp"

#include <string>

namespace vesp
{
	String Concat(StringView lhs, StringView rhs)
	{
		String ret = lhs.CopyToVector();
		ret.insert(ret.end(), rhs.begin(), rhs.end());
		return ret;
	}

	String Join(ArrayView<String> strings, StringByte sep)
	{
		String ret;

		bool first = true;
		for (auto string : strings)
		{
			if (!first)
				ret.push_back(sep);
			ret.insert(ret.end(), string.begin(), string.end());
			first = false;
		}

		return ret;
	}

	String Replace(StringView str, StringView toReplace, StringView replacement)
	{
		String ret;

		auto maxSize = str.size - toReplace.size;
		for (size_t i = 0; i < str.size;)
		{
			if (i < maxSize && memcmp(&str[i], toReplace.data, toReplace.size) == 0)
			{
				ret.insert(ret.end(), replacement.begin(), replacement.end());
				i += toReplace.size;
			}
			else
			{
				ret.push_back(str[i]);
				++i;
			}
		}
		return ret;
	}

	String Repeat(StringByte ch, size_t count)
	{
		String line;

		for (size_t i = 0; i < count * 2; i++)
			line.push_back(ch);

		return line;
	}

	Vector<StringView> Split(StringView str, StringByte sep)
	{
		Vector<StringView> ret;

		size_t startIndex = 0;
		size_t endIndex = 0;
		size_t index = 0;

		for (auto c : str)
		{
			if (c == sep || index == str.size - 1)
			{
				if (index == str.size - 1)
					endIndex = str.size;

				if (startIndex != endIndex)
					ret.push_back(StringView(str.data + startIndex, endIndex - startIndex));

				startIndex = endIndex = index + 1;
			}
			else
			{
				++endIndex;
			}

			++index;
		}

		return ret;
	}

	bool StartsWith(StringView str, StringView prefix)
	{
		if (prefix.size > str.size)
			return false;

		return memcmp(str.data, prefix.data, prefix.size) == 0;
	}

	String ToString(size_t value)
	{
		String ret;
		auto s = std::to_string(value);
		ret.insert(ret.begin(), s.begin(), s.end());
		return ret;
	}

	String ToString(S32 value)
	{
		String ret;
		auto s = std::to_string(value);
		ret.insert(ret.begin(), s.begin(), s.end());
		return ret;
	}

	String ToString(F32 value)
	{
		String ret;
		auto s = std::to_string(value);
		ret.insert(ret.begin(), s.begin(), s.end());
		return ret;
	}

	S32 ToS32(StringView string)
	{
		auto cString = ToCString(string);
		return std::atoi(cString.get());
	}

	F64 ToF64(StringView string)
	{
		auto cString = ToCString(string);
		return std::atof(cString.get());
	}

	F32 ToF32(StringView string)
	{
		return float(ToF64(string));
	}

	std::unique_ptr<StringByte[]> ToCString(StringView string)
	{
		std::unique_ptr<StringByte[]> ret(new StringByte[string.size + 1]);
		memcpy(ret.get(), string.data, string.size);
		ret[string.size] = '\0';
		return ret;
	}

	bool operator==(StringView const lhs, StringView const rhs)
	{
		return memcmp(lhs.data, rhs.data, lhs.size) == 0;
	}

	bool operator!=(StringView const lhs, StringView const rhs)
	{
		return !(lhs == rhs);
	}
}