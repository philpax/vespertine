#pragma once

namespace vesp { namespace util {

	template <typename T, int N>
	const int SizeOfArray(T(&arr)[N])
	{
		return N;
	}

} }