#include "pch.h"
#include "Utils.h"
#include <iomanip>

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

		std::string CreateUniqueIdentifier(const std::string& cpu_id, const std::string& hard_disk_id, const std::string& motherboard_id)
		{
			size_t cpu_hash = std::hash<std::string>{}(cpu_id);
			size_t har_hash = std::hash<std::string>{}(hard_disk_id);
			size_t mob_hash = std::hash<std::string>{}(motherboard_id);

			size_t fin_hash = cpu_hash ^ ((har_hash ^ (mob_hash << 1)) << 1);

			std::stringstream ss;
			ss << std::hex << fin_hash;

			return ss.str();
		}
	}
}