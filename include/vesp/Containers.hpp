#pragma once

// Implemented as standard library containers until otherwise needed
#pragma warning(push)
#pragma warning(disable: 4530)
#include <vector>
#include <unordered_map>
#include <array>
#include <deque>
#include <memory>
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
	using Deque = std::deque<T>;

	template <typename T>
	struct ArrayView
	{
		T* data = nullptr;
		size_t size = 0;

		ArrayView()
		{
		}

		ArrayView(T& data)
		{
			this->data = &data;
			this->size = 1;
		}

		ArrayView(T* data, size_t size)
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
			this->data = reinterpret_cast<T*>(array.data);

			size_t scaleFactor = 0;
			if (sizeof(T) > sizeof(Y))
				scaleFactor = sizeof(T) / sizeof(Y);
			else
				scaleFactor = sizeof(Y) / sizeof(T);

			this->size = array.size * scaleFactor;
		}

		T* begin()
		{
			return this->data;
		}

		T* end()
		{
			return this->begin() + this->size;
		}

		T const* cbegin() const
		{
			return this->data;
		}

		T const* cend() const
		{
			return this->cbegin() + this->size;
		}

		T& operator[](size_t index)
		{
			return this->data[index];
		}

		T const& operator[](size_t index) const
		{
			return this->data[index];
		}

		Vector<T> CopyToVector()
		{
			return Vector<T>(this->begin(), this->end());
		}
	};
}