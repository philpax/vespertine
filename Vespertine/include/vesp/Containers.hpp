#pragma once

// Implemented as standard library containers until otherwise needed

#include <vector>
#include <unordered_map>
#include <array>

namespace vesp
{
	template <typename T>
	using Vector = std::vector<T>;

	template <typename K, typename T>
	using UnorderedMap = std::unordered_map<K, T>;

	template <typename T, int N>
	using Array = std::array<T, N>;
}