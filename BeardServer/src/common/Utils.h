#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <string>

namespace BeardServer
{
	namespace common
	{
		void GetTimeInMilliseconds(time_t& curr_time);
		std::string CreateUniqueIdentifier(const std::string& cpu_id, const std::string& hard_disk_id, const std::string& motherboard_id);
	}
}

#endif // !UTILS_H
