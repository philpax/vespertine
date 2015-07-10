#include "vesp/FileSystem.hpp"
#include "vesp/Assert.hpp"

#include <Windows.h>

namespace vesp
{
	// Temporary implementation based on the standard library
	// Will switch to more appropriate measures when necessary

	void FileSystem::File::Write(ArrayView<U8> const array)
	{
		fwrite(array.data, 1, array.size, this->file_);
	}

	U32 FileSystem::File::Read(ArrayView<U8> array)
	{
		return fread(array.data, 1, array.size, this->file_);
	}

	U32 FileSystem::File::Size()
	{
		auto currentPosition = ftell(this->file_);

		fseek(this->file_, 0, SEEK_END);
		auto count = ftell(this->file_);
		fseek(this->file_, currentPosition, SEEK_SET);

		return count;
	}
	
	bool FileSystem::File::Exists()
	{
		return this->file_ != nullptr;
	}

	void FileSystem::File::Flush()
	{
		fflush(this->file_);
	}

	FileSystem::File FileSystem::Open(RawStringPtr fileName, RawStringPtr mode)
	{
		File file;
		fopen_s(&file.file_, fileName, mode);

		return file;
	}

	void FileSystem::Close(FileSystem::File const& file)
	{
		fclose(file.file_);
	}

	bool FileSystem::Exists(RawStringPtr fileName)
	{
		return GetFileAttributes(fileName) != INVALID_FILE_ATTRIBUTES;
	}

	void FileSystem::Read(RawStringPtr fileName, String& output)
	{
		auto file = this->Open(fileName, "r");

		VESP_ENFORCE(file.Exists());
		
		auto size = file.Size();
		output.clear();
		output.resize(size);
		file.Read(ArrayView<StringByte>(output));

		this->Close(file);
	}
}
