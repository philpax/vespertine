#include "vesp/FileSystem.hpp"
#include "vesp/Assert.hpp"

#include "vesp/util/StringConversion.hpp"

#include <Windows.h>

namespace vesp
{
	// Temporary implementation based on the standard library
	// Will switch to more appropriate measures when necessary
	FileSystem::File::File()
	{
	}

	FileSystem::File::File(File&& rhs)
	{
		this->file_ = rhs.file_;
		rhs.file_ = nullptr;
	}

	FileSystem::File::~File()
	{
		if (this->file_)
			FileSystem::Get()->Close(*this);
	}

	FileSystem::File& FileSystem::File::operator=(File&& rhs)
	{
		this->file_ = rhs.file_;
		rhs.file_ = nullptr;
		return *this;
	}

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

	FileSystem::File FileSystem::Open(StringView fileName, RawStringPtr mode)
	{
		File file;
		auto cString = ToCString(fileName);
		fopen_s(&file.file_, cString.get(), mode);

		return file;
	}

	void FileSystem::Close(FileSystem::File& file)
	{
		fclose(file.file_);
		file.file_ = nullptr;
	}

	bool FileSystem::Exists(StringView fileName)
	{
		auto cString = ToCString(fileName);
		auto wideString = util::MultiToWide(cString.get());
		return GetFileAttributesW(wideString.data()) != INVALID_FILE_ATTRIBUTES;
	}

	void FileSystem::Read(StringView fileName, String& output)
	{
		auto file = this->Open(fileName, "r");

		VESP_ENFORCE(file.Exists());
		
		auto size = file.Size();
		output.clear();
		output.resize(size);
		file.Read(ArrayView<StringByte>(output));
	}
}
