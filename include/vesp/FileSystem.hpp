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

		File Open(StringPtr fileName, StringPtr mode);
		void Close(File const& file);
		bool Exists(StringPtr fileName);

		void Read(StringPtr fileName, String& output);
	};
}
