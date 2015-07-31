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

		File Open(StringView fileName, RawStringPtr mode);
		// TEMPORARY SHIM: Need to upgrade compiler so we can implicitly
		// convert from RawStringPtr to StringView
		File Open(RawStringPtr fileName, RawStringPtr mode)
		{
			return this->Open(StringView::From(fileName), mode);
		}

		void Close(File const& file);
		bool Exists(StringView fileName);

		void Read(StringView fileName, String& output);
		// TEMPORARY SHIM: Need to upgrade compiler so we can implicitly
		// convert from RawStringPtr to StringView
		void Read(RawStringPtr fileName, String& output)
		{
			this->Read(StringView::From(fileName), output);
		}
	};
}
