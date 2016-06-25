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

	U32 FileSystem::File::Size() const
	{
		auto currentPosition = ftell(this->file_);

		fseek(this->file_, 0, SEEK_END);
		auto count = ftell(this->file_);
		fseek(this->file_, currentPosition, SEEK_SET);

		return count;
	}
	
	bool FileSystem::File::Exists() const
	{
		return this->file_ != nullptr;
	}

	void FileSystem::File::Flush() const
	{
		fflush(this->file_);
	}

	FileSystem::File FileSystem::Open(StringView fileName, Mode::Enum mode)
	{
		File file;

		char modeString[3] = { '\0' };
		if (mode & Mode::Read)
			modeString[0] = 'r';
		else if (mode & Mode::Write)
			modeString[0] = 'w';
		else if (mode & Mode::Append)
			modeString[0] = 'a';
		else
			VESP_ASSERT(false && "Expected a valid mode for file opening!");

		if (mode & Mode::Binary)
			modeString[1] = 'b';

		auto cString = ToCString(fileName);
		fopen_s(&file.file_, cString.get(), modeString);

		return file;
	}

	void FileSystem::Close(FileSystem::File& file)
	{
		fclose(file.file_);
		file.file_ = nullptr;
	}

	bool FileSystem::Exists(StringView fileName) const
	{
		auto cString = ToCString(fileName);
		auto wideString = util::MultiToWide(cString.get());
		return GetFileAttributesW(wideString.data()) != INVALID_FILE_ATTRIBUTES;
	}
}
