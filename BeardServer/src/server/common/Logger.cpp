#include "pch.h"
#include "Logger.h"

namespace BeardServer
{
	namespace server
	{
		std::string Logger::Log(Severity severity, const char* message, ...)
		{
			va_list args;
			va_start(args, message);
			
			std::string formattedMessage = FormatLogMessage(message, args);

			switch (severity)
			{
			case Severity::Critical:
				std::cerr << "[BeardServer::Critical] ";
				break;
			case Severity::Error:
				std::cerr << "[BeardServer::Error] ";
				break;
			case Severity::Warning:
				std::cerr << "[BeardServer::Warning] ";
				break;
			case Severity::Info:
				std::cerr << "[BeardServer::Info] ";
				break;
			case Severity::Debug:
				std::cerr << "[BeardServer::Debug] ";
				break;
			}

			std::cerr << formattedMessage << std::endl;

			va_end(args);

			return formattedMessage;
		}
	}
}