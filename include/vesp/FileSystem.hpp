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
		struct File
		{
		public:
			friend class FileSystem;
			File();
			File(File&& rhs);
			~File();

			File(File const&) = delete;

			File& operator=(File&& rhs);

			void Write(ArrayView<U8> const array);
			U32 Read(ArrayView<U8> array);

			template <typename T>
			Vector<T> Read()
			{
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

		File Open(StringView fileName, RawStringPtr mode);

		void Close(File& file);
		bool Exists(StringView fileName) const;
	};
}
