#pragma once
#ifndef BEARDSERVER_DEFINES_H
#define BEARDSERVER_DEFINES_H

#include <exception>
#include <server/common/Logger.h>
#include <server/Result.h>

#define SERVER_ASSERT(value, message, ...)					\
do															\
{															\
	if (!(value))											\
	{														\
		BeardServer::server::Logger::Log(BeardServer::server::Logger::Severity::Error, "Assert failed at: %s:%d, Msg:%s", __FILE__, __LINE__, BeardServer::server::Logger::FormatLogMessage(message, ##__VA_ARGS__).c_str());	\
		DEBUG_BREAK();										\
	}														\
} while(false)

// This verify should only be used when a result should be returned
#define SERVER_VERIFY(value, responseCode, message, ...)	\
do															\
{															\
	if (!(value))		\
	{					\
		std::string m = BeardServer::server::Logger::Log(BeardServer::server::Logger::Severity::Error, "Verify failed at: %s:%d, Msg:%s", __FILE__, __LINE__, BeardServer::server::Logger::FormatLogMessage(message, ##__VA_ARGS__).c_str());	\
		DEBUG_BREAK();	\
		return BeardServer::server::Result(responseCode, m); \
	}					\
} while (false)

#if defined(BS_DEBUG) || defined(BS_OPTDEBUG)
#define DEBUG_BREAK()	\
do						\
{						\
	__debugbreak();		\
} while (false)
#else
#define DEBUG_BREAK()	\
do						\
{						\
						\
} while (false)
#endif
#endif // !BEARDSERVER_DEFINES_H