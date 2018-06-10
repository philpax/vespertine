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
	public:
		ArrayView()
		{
		}

		ArrayView(T& data)
		{
			this->data_ = &data;
			this->size_ = 1;
		}

		ArrayView(T* data, size_t size)
		{
			this->data_ = data;
			this->size_ = size;
		}

		ArrayView(Vector<T>& v)
		{
			this->data_ = v.data();
			this->size_ = v.size();
		}

		template <int N>
		ArrayView(Array<T, N>& a)
		{
			this->data_ = a.data();
			this->size_ = N;
		}

		template <int N>
		ArrayView(T (&a)[N])
		{
			this->data_ = a;
			this->size_ = N;
		}

		template <typename Y>
		ArrayView(ArrayView<Y> array)
		{
			this->data_ = reinterpret_cast<T*>(array.data());

			size_t scaleFactor = 0;
			if (sizeof(T) > sizeof(Y))
				scaleFactor = sizeof(T) / sizeof(Y);
			else
				scaleFactor = sizeof(Y) / sizeof(T);

			this->size_ = array.size() * scaleFactor;
		}

		T* data()
		{
			return this->data_;
		}

		const T* data() const
		{
			return this->data_;
		}

		size_t& size()
		{
			return this->size_;
		}

		size_t const& size() const
		{
			return this->size_;
		}

		T* begin()
		{
			return this->data_;
		}

		T* end()
		{
			return this->begin() + this->size_;
		}

		T const* cbegin() const
		{
			return this->data_;
		}

		T const* cend() const
		{
			return this->cbegin() + this->size_;
		}

		T& operator[](size_t index)
		{
			return this->data_[index];
		}

		T const& operator[](size_t index) const
		{
			return this->data_[index];
		}

		T& front()
		{
			return this->data_[0];
		}

		T& back()
		{
			return this->data_[this->size_ - 1];
		}

		Vector<T> CopyToVector() const
		{
			return Vector<T>(this->cbegin(), this->cend());
		}
	
	private:
		T* data_ = nullptr;
		size_t size_ = 0;
	};
}