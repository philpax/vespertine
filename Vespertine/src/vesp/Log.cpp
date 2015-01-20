#include "vesp/Log.hpp"
#include "vesp/Util.hpp"

#include <cstdio>
#include <cstdarg>
#include <ctime>

namespace vesp
{
	namespace logger
	{
		FILE* LogFile;

		void Initialise(StringPtr path)
		{
			fopen_s(&LogFile, path, "a");
		}

		void Shutdown()
		{
			fclose(LogFile);
		}
	}

#define LOG_TYPE(type) #type
	const char* LogTypeStrings[] = { VESP_LOG_TYPES };
#undef LOG_TYPE

	void Log(LogType type, StringPtr fmt, ...)
	{
		StringByte tempBuffer[512];
		StringByte finalBuffer[1024];
		StringByte timeBuffer[32];

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

		printf("%s", finalBuffer);
		fprintf(logger::LogFile, "%s", finalBuffer);
	}
}