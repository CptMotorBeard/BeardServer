#pragma once
#ifndef BEARDSERVER_CLIENT_H
#define BEARDSERVER_CLIENT_H

#include <string>
#include <chrono>
#include <optional>

namespace BeardServer
{
	namespace server
	{
		class Client
		{
		public:
			Client();
			Client(int socket);
			~Client();

			void Update(long dt);
			int SendTransmission(const std::string& action, Result actionResult, int transmissionId);
			int SendTransmission(const std::string& action, Result actionResult, int transmissionId, const nlohmann::json& data);
			Result Receive();
			bool IsConnected() const;
			void CloseConnection(bool reconnect = false);

		private:
			int SendTransmissionImpl(const std::string& action, Result actionResult, int transmissionId, std::optional<const nlohmann::json *> data);
			
			int Send(const std::string& message);
			int Send(const std::string& message, int transmissionId);

			std::chrono::system_clock::time_point m_LastRecvDataTime;
			std::chrono::system_clock::time_point m_LastSentDataTime;

			bool b_isConnected;
			int m_Socket;
		};
	}
}

#endif // !BEARDSERVER_CLIENT_H
