#pragma once

#include "vesp/util/GlobalSystem.hpp"
#include "vesp/Types.hpp"

namespace vesp
{
	class Filesystem : public util::GlobalSystem<Filesystem>
	{
	public:
		struct File
		{
		public:
			friend class Filesystem;

			void Write(U8 const* data, U32 const count);
			U32 Read(U8* data, U32 const count);

		private:
			FILE* file_;
		};

		File Open(StringPtr fileName, StringPtr mode);
		void Close(File const& file);
		bool Exists(StringPtr fileName);
	};
}