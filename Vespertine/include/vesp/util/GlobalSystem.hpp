#pragma once

namespace vesp { namespace util {

	template <typename T>
	class GlobalSystem
	{
	public:
		template <typename... Values>
		static void Create(Values&&... values)
		{
			ptr_ = new T(values...);
		}

		static void Destroy()
		{
			delete ptr_;
		}

		static T* Get()
		{
			return ptr_;
		}

	private:
		static T* ptr_;
	};

	template <typename T>
	T* GlobalSystem<T>::ptr_;

} }