#pragma once

#include "vesp/util/GlobalSystem.hpp"

#include "vesp/Types.hpp"
#include "vesp/Containers.hpp"
#include "vesp/String.hpp"
#include "vesp/Assert.hpp"

namespace vesp
{
	class FileSystem : public util::GlobalSystem<FileSystem>
	{
	public:
		struct Mode
		{
			enum Enum
			{
				Read	= (1 << 0),
				Write	= (1 << 1),
				Append	= (1 << 2),
				Binary	= (1 << 3)
			};
		};

		struct File
		{
		public:
			friend class FileSystem;
		private: 
			File(FILE* file);
			File() = delete;
		public:
			File(File&& rhs);
			~File();

			File(File const&) = delete;

			File& operator=(File&& rhs);

			void Write(ArrayView<U8> const array);
			U32 Read(ArrayView<U8> array);

			template <typename T>
			Vector<T> Read()
			{
				VESP_ASSERT(this->Exists());
				VESP_ASSERT(this->Size() % sizeof(T) == 0);

				Vector<T> ret(this->Size() / sizeof(T));
				this->Read(ArrayView<T>(ret));

				return ret;
			}

			U32 Size() const;
			bool Exists() const;
			void Flush() const;

		private:
			FILE* file_;
		};

		File Open(StringView fileName, Mode::Enum mode);

		void Close(File& file);
		bool Exists(StringView fileName) const;
	};
}
