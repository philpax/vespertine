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

		std::vector<T> CopyToVec()
		{
			return std::vector<T>(this->begin(), this->end());
		}
	};

	class StringView : public ArrayView<StringByte>
	{
	public:
		StringView(RawStringPtr ptr)
			: ArrayView(const_cast<StringByte*>(ptr), strlen(ptr))
		{
		}

		StringView(Vector<StringByte>& v)
			: ArrayView(v)
		{
		}

		StringView(StringByte* data, size_t size)
			: ArrayView(data, size)
		{
		}
	};
	
	String MakeString(RawStringPtr ptr);
	String Concat(String const& string, StringView rhs);
	std::unique_ptr<StringByte[]> ToCString(StringView string);
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