#pragma once

#include "vesp/util/GlobalSystem.hpp"
#include "vesp/Types.hpp"
#include "vesp/Containers.hpp"

namespace vesp
{
	class FileSystem : public util::GlobalSystem<FileSystem>
	{
	public:
		struct File
		{
		public:
			friend class FileSystem;

			void Write(ArrayView<U8> const array);
			U32 Read(ArrayView<U8> array);
			U32 Size();
			bool Exists();
			void Flush();

		private:
			FILE* file_;
		};

		File Open(RawStringPtr fileName, RawStringPtr mode);
		void Close(File const& file);
		bool Exists(RawStringPtr fileName);

		void Read(RawStringPtr fileName, String& output);
	};
}
