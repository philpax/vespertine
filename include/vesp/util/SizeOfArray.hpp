#pragma once

namespace vesp { namespace util {

	template <typename T, int N>
	const size_t SizeOfArray(T (&)[N])
	{
		return N;
	}

} }