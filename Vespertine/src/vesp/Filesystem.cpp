#include "vesp/Filesystem.hpp"

#include <Windows.h>

namespace vesp
{
	// Temporary implementation based on the standard library
	// Will switch to more appropriate measures when necessary

	void Filesystem::File::Write(U8 const* data, U32 const count)
	{
		fwrite(data, 1, count, this->file_);
	}

	U32 Filesystem::File::Read(U8* data, U32 const count)
	{
		return fread(data, 1, count, this->file_);
	}

	Filesystem::File Filesystem::Open(StringPtr fileName, StringPtr mode)
	{
		File file;
		fopen_s(&file.file_, fileName, mode);

		return file;
	}

	void Filesystem::Close(Filesystem::File const& file)
	{
		fclose(file.file_);
	}

	bool Filesystem::Exists(StringPtr fileName)
	{
		return GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES;
	}
}