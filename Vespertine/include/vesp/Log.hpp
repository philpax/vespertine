#pragma once

#include "vesp/Types.hpp"
#include "vesp/Filesystem.hpp"

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
		Logger(StringPtr path);
		~Logger();

		void WriteLog(LogType type, StringPtr fmt, ...);

	private:
		Filesystem::File logFile_;
	};

#define Log(type, fmt, ...) Logger::Get()->WriteLog(type, fmt, __VA_ARGS__)
#define LogInfo(fmt, ...) Log(LogType::Info, fmt, __VA_ARGS__)
#define LogWarn(fmt, ...) Log(LogType::Warn, fmt, __VA_ARGS__)
#define LogError(fmt, ...) Log(LogType::Error, fmt, __VA_ARGS__)
#define LogFatal(fmt, ...) Log(LogType::Fatal, fmt, __VA_ARGS__)

}