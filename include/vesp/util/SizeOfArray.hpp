#pragma once

namespace vesp { namespace util {

	template <typename T, int N>
	__forceinline const size_t SizeOfArray(T (&)[N])
	{
		return N;
	}

} }