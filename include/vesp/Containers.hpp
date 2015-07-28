#pragma once

// Implemented as standard library containers until otherwise needed
#pragma warning(push)
#pragma warning(disable: 4530)
#include <vector>
#include <unordered_map>
#include <array>
#include <deque>
#pragma warning(pop)

#include "vesp/Types.hpp"
#include "vesp/util/MurmurHash.hpp"

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

	using String = Vector<StringByte>;
	using WideString = Vector<wchar_t>;

	template <typename T>
	struct ArrayView;

	template <typename T>
	struct ArrayViewBase
	{
	};

	template <>
	struct ArrayViewBase<StringByte>
	{
		static ArrayView<StringByte> From(StringByte const* str);
	};

	template <typename T>
	struct ArrayView : public ArrayViewBase<T>
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

		std::vector<T> CopyToVec()
		{
			return std::vector<T>(this->begin(), this->end());
		}
	};

	typedef ArrayView<StringByte> StringView;
}

namespace std 
{
	template <> struct hash<vesp::String>
	{
		size_t operator()(vesp::String const& s) const
		{
			return vesp::util::MurmurHash(
				reinterpret_cast<void const*>(s.data()), s.size());
		}
	};
}