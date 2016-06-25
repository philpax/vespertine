#include "vesp/Log.hpp"
#include "vesp/Util.hpp"

#include "vesp/Console.hpp"

#include <cstdarg>
#include <ctime>

namespace vesp
{
#define LOG_TYPE(type) #type
	const char* LogTypeStrings[] = { VESP_LOG_TYPES };
#undef LOG_TYPE

	Logger::Logger(RawStringPtr path) :
		logFile_(FileSystem::Get()->Open(path, FileSystem::Mode::Append))
	{
	}

	Logger::~Logger()
	{
		FileSystem::Get()->Close(this->logFile_);
	}

	void Logger::WriteLog(LogType type, RawStringPtr fmt, ...)
	{
		static StringByte tempBuffer[4096];
		static StringByte finalBuffer[4500];
		static StringByte timeBuffer[32];

		time_t rawTime;
		tm timeInfo;

		time(&rawTime);
		localtime_s(&timeInfo, &rawTime);

		va_list args;
		va_start(args, fmt);

		std::strftime(timeBuffer, util::SizeOfArray(timeBuffer), 
			"%H:%M:%S", &timeInfo);

		vsnprintf_s(tempBuffer, util::SizeOfArray(tempBuffer), fmt, args);

		sprintf_s(finalBuffer, "%s | %s | %s\n", 
			timeBuffer, LogTypeStrings[static_cast<U8>(type)], tempBuffer);

		auto len = strlen(finalBuffer);
		auto col = graphics::Colour::White;

		if (type == LogType::Warn)
			col = graphics::Colour::Orange;
		else if (type == LogType::Error)
			col = graphics::Colour::OrangeRed;
		else if (type == LogType::Fatal)
			col = graphics::Colour::Red;

		Console::Get()->AddMessage(StringView(finalBuffer, len), col);

		this->logFile_.Write(ArrayView<U8>(reinterpret_cast<U8*>(finalBuffer), len));
		this->logFile_.Flush();
	}
}