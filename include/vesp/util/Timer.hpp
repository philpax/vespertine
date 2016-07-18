#pragma once

#include <chrono>

namespace vesp { namespace util {

	struct Timer
	{
	public:
		Timer()
		{
			this->Restart();
		}

		void Restart()
		{
			this->startTime_ = std::chrono::high_resolution_clock::now();
		}
			
		template <typename Ratio, typename T = float>
		T GetElapsed() const
		{
			namespace ch = std::chrono;

			typedef ch::duration<T, Ratio> DurationType;

			auto duration = ch::duration_cast<DurationType>(
				ch::high_resolution_clock::now() - this->startTime_);

			return duration.count();
		}

	#define TIMER_ALIAS(name, ...)\
		template <typename T = float> \
		T Get##name() const { return this->GetElapsed<__VA_ARGS__, T>(); }

		TIMER_ALIAS(Hours, std::ratio<3600, 1>)
		TIMER_ALIAS(Minutes, std::ratio<60, 1>)
		TIMER_ALIAS(Seconds, std::ratio<1, 1>)
		TIMER_ALIAS(Milliseconds, std::milli)
		TIMER_ALIAS(Microseconds, std::micro)

	#undef TIMER_ALIAS

	private:
		std::chrono::high_resolution_clock::time_point startTime_;
	};

} }