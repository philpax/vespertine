#pragma once

namespace vesp { namespace math {

	template <typename T>
	T Clamp(T const in, T const min, T const max)
	{
		if (in > max)
			return max;
		else if (in < min)
			return min;

		return in;
	}

} }