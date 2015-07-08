#pragma once

#include <Windows.h>

#include "vesp/Containers.hpp"

namespace vesp { namespace util {
	inline String WideToMulti(wchar_t const* str, uint32_t cp =
		CP_ACP)
	{
		int len = WideCharToMultiByte(cp, 0, str, -1, 0, 0, 0,
			0);

		if (len == 0)
		{
			static String empty;
			return empty;
		}

		Vector<StringByte> ansistr;
		ansistr.resize(len);

		WideCharToMultiByte(cp, 0, str, -1,
			&ansistr[0], len, 0, 0);

		// Hack to prevent the last character being '\0'. We'll add unicode
		// support eventually, anyway.
		if (ansistr.back() == '\0')
			ansistr.pop_back();

		return ansistr;
	}

	inline WideString MultiToWide(char const* str, uint32_t cp =
		CP_ACP)
	{
		int len = MultiByteToWideChar(cp, 0, str, -1, 0, 0);

		if (len == 0)
		{
			static WideString empty;
			return empty;
		}

		WideString ansistr;
		ansistr.resize(len);

		MultiByteToWideChar(cp, 0, str, -1,
			&ansistr[0], len);

		return ansistr;
	}
}}