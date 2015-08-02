#pragma once

#include <Windows.h>

#include "vesp/Containers.hpp"
#include "vesp/String.hpp"

namespace vesp { namespace util {
	inline String WideToMulti(wchar_t const* str, uint32_t cp = CP_ACP)
	{
		int len = WideCharToMultiByte(cp, 0, str, -1, 0, 0, 0, 0);

		String ret;
		if (len == 0)
			return ret;

		ret.resize(len);

		WideCharToMultiByte(cp, 0, str, -1, &ret[0], len, 0, 0);

		// Hack to prevent the last character being '\0'. We'll add unicode
		// support eventually, anyway.
		if (ret.back() == '\0')
			ret.pop_back();

		return ret;
	}

	inline WideString MultiToWide(char const* str, uint32_t cp = CP_ACP)
	{
		int len = MultiByteToWideChar(cp, 0, str, -1, 0, 0);

		WideString ret;
		if (len == 0)
			return ret;

		ret.resize(len);

		MultiByteToWideChar(cp, 0, str, -1, &ret[0], len);

		return ret;
	}
}}