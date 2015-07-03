#pragma once

// Implemented as standard library containers until otherwise needed

#pragma warning(push)
#pragma warning(disable: 4530)
#include <vector>
#include <unordered_map>
#include <array>
#pragma warning(pop)

#include "vesp/Types.hpp"

namespace vesp
{
	template <typename T>
	using Vector = std::vector<T>;

	template <typename K, typename T>
	using UnorderedMap = std::unordered_map<K, T>;

	template <typename T, int N>
	using Array = std::array<T, N>;

	template <typename T>
	struct ArrayView
	{
		T* data = nullptr;
		U32 size = 0;

		ArrayView()
		{
		}

		ArrayView(T& data)
		{
			this->data = &data;
			this->size = 1;
		}

		ArrayView(T* data, U32 size)
		{
			this->data = data;
			this->size = size;
		}

		ArrayView(Vector<T>& v)
		{
			this->data = v.data();
			this->size = v.size();
		}

		template <int N>
		ArrayView(Array<T, N>& a)
		{
			this->data = a.data();
			this->size = N;
		}

		template <int N>
		ArrayView(T (&a)[N])
		{
			this->data = a;
			this->size = N;
		}

		template <typename Y>
		ArrayView(ArrayView<Y> array)
		{
			static_assert(std::is_convertible<
				std::remove_pointer<Y>::type, 
				std::remove_pointer<T>::type>::value, "Type must be convertible");
			this->data = reinterpret_cast<T*>(array.data);
			this->size = array.size * (sizeof(T) / sizeof(Y));
		}
	};
}