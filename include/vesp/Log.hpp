#pragma once

#include "vesp/Types.hpp"
#include "vesp/FileSystem.hpp"

#include "vesp/util/GlobalSystem.hpp"

#define VESP_LOG_TYPES \
	LOG_TYPE(Info), \
	LOG_TYPE(Warn), \
	LOG_TYPE(Error), \
	LOG_TYPE(Fatal)

namespace vesp
{

#define LOG_TYPE(type) type
	enum class LogType : U8
	{
		VESP_LOG_TYPES
	};
#undef LOG_TYPE

	class Logger : public util::GlobalSystem<Logger>
	{
	public:
		Logger(RawStringPtr path);
		~Logger();

		void WriteLog(LogType type, RawStringPtr fmt, ...);

	private:
		FileSystem::File logFile_;
	};

#define Log(type, fmt, ...) vesp::Logger::Get()->WriteLog(type, fmt, __VA_ARGS__)
#define LogInfo(fmt, ...) Log(vesp::LogType::Info, fmt, __VA_ARGS__)
#define LogWarn(fmt, ...) Log(vesp::LogType::Warn, fmt, __VA_ARGS__)
#define LogError(fmt, ...) Log(vesp::LogType::Error, fmt, __VA_ARGS__)
#define LogFatal(fmt, ...) Log(vesp::LogType::Fatal, fmt, __VA_ARGS__)

}