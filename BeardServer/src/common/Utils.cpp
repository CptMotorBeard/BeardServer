#include "pch.h"
#include "Utils.h"

namespace BeardServer
{
	namespace common
	{
		void GetTimeInMilliseconds(time_t& curr_time)
		{
			LARGE_INTEGER ticks, ticksPerSecond;
			QueryPerformanceCounter(&ticks);
			QueryPerformanceFrequency(&ticksPerSecond);

			curr_time = time_t(double(ticks.QuadPart) / double(ticksPerSecond.QuadPart) * 1000);
		}
	}
}