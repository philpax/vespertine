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

			void Write(U8 const* data, U32 const count);
			U32 Read(U8* data, U32 const count);
			U32 Size();
			bool Exists();

		private:
			FILE* file_;
		};

		File Open(StringPtr fileName, StringPtr mode);
		void Close(File const& file);
		bool Exists(StringPtr fileName);

		void Read(StringPtr fileName, Vector<StringByte>& output);
	};
}
