#pragma once
#ifndef BEARDSERVER_LOGGER_H
#define BEARDSERVER_LOGGER_H

#include <string>

namespace BeardServer
{
	namespace server
	{
		class Logger
		{
		public:
			enum class Severity
			{
				Critical,
				Error,
				Warning,
				Info,
				Debug
			};

			static std::string Log(Severity severity, const char* message, ...);

			static inline std::string FormatLogMessage(const char* message, const char* arg)
			{
				size_t bufferLength = 0;
				{
					bufferLength = std::snprintf(nullptr, 0, message, arg);
				}

				char* buffer = new char[bufferLength + 1];
				memset(buffer, 0, bufferLength + 1);

				std::snprintf(buffer, bufferLength + 1, message, arg);
				std::string retValue(buffer);

				delete[] buffer;
				buffer = nullptr;

				return retValue;
			}

			static inline std::string FormatLogMessage(const char* message, va_list args)
			{
				size_t bufferLength = 0;
				{
					va_list length_check_args;
					va_copy(length_check_args, args);
					bufferLength = std::vsnprintf(nullptr, 0, message, length_check_args);
					va_end(length_check_args);
				}

				char* buffer = new char[bufferLength + 1];
				memset(buffer, 0, bufferLength + 1);

				std::vsnprintf(buffer, bufferLength + 1, message, args);

				std::string retValue(buffer);

				delete[] buffer;
				buffer = nullptr;

				return retValue;
			}

			static inline std::string FormatLogMessage(const char* message, ...)
			{
				va_list args;
				va_start(args, message);

				std::string retValue = FormatLogMessage(message, args);

				va_end(args);

				return retValue;
			}
		};
	}
}

#endif // !BEARDSERVER_LOGGER_H
