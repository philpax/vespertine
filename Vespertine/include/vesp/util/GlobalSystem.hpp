#pragma once

#include <memory>

namespace vesp { namespace util {

	template <typename T>
	class GlobalSystem
	{
	public:
		template <typename... Values>
		static void Create(Values&&... values)
		{
			ptr_ = std::make_unique<T>(values...);
		}

		static void Destroy()
		{
			ptr_.reset();
		}

		static T* Get()
		{
			return ptr_.get();
		}

	private:
		static std::unique_ptr<T> ptr_;
	};

	template <typename T>
	std::unique_ptr<T> GlobalSystem<T>::ptr_;

} }