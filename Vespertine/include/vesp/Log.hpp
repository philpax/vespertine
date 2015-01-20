#pragma once

#include "vesp/Types.hpp"

#define VESP_LOG_TYPES \
	LOG_TYPE(Info), \
	LOG_TYPE(Warn), \
	LOG_TYPE(Error), \
	LOG_TYPE(Fatal)

namespace vesp
{
	namespace logger
	{
		void Initialise(StringPtr path);
		void Shutdown();
	}

#define LOG_TYPE(type) type
	enum class LogType : U8
	{
		VESP_LOG_TYPES
	};
#undef LOG_TYPE

	void Log(LogType type, StringPtr fmt, ...);
}